#include "ReservationManagement.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

// Strip surrounding whitespace and a trailing '\r' (Windows-edited files).
static string trimField(string s){
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

ReservationManagement::ReservationManagement(){

}

ReservationManagement::~ReservationManagement(){

}

ReservationStatus ReservationManagement::CreateReservation(Reservation r){
    // Prevent duplicate reservation IDs among active reservations.
    if (findReservation(r.ID) != nullptr){
        return ReservationStatus::DuplicateID;
    }

    if (validateReservation(r)){
        ReservationsList.Insert(r);
        return ReservationStatus::Created;
    }

    waitlist.Insert(r);
    return ReservationStatus::Waitlisted;
}


// Load reservations from a '|' delimited file:
//   ID|StudentID|StudentName|ResourceID|Date
// Each record runs through CreateReservation, so duplicate IDs are rejected and
// resource/date clashes are pushed to the waiting list automatically.
// Returns the number of records placed into the active reservation list.
int ReservationManagement::LoadReservations(const string& filename){
    ifstream file(filename);
    if (!file.is_open()){
        cout << "ERROR: Could not open reservation file: " << filename << endl;
        return 0;
    }

    int active = 0, waitlisted = 0, skipped = 0, lineNo = 0;
    string line;
    while (getline(file, line)){
        lineNo++;
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string id, sid, name, res, date;
        if (!getline(ss, id,   '|') ||
            !getline(ss, sid,  '|') ||
            !getline(ss, name, '|') ||
            !getline(ss, res,  '|') ||
            !getline(ss, date, '|')){
            cout << "WARNING: Skipping malformed reservation on line " << lineNo << endl;
            skipped++;
            continue;
        }

        Reservation r;
        try {
            r.ID        = stoi(trimField(id));
            r.StudentID = stoi(trimField(sid));
        } catch (...) {
            cout << "WARNING: Bad numeric field on line " << lineNo << endl;
            skipped++;
            continue;
        }
        r.StudentName = trimField(name);
        r.ResourceID  = trimField(res);
        r.Date        = trimField(date);

        switch (this->CreateReservation(r)){
            case ReservationStatus::Created:     active++;     break;
            case ReservationStatus::Waitlisted:  waitlisted++; break;
            case ReservationStatus::DuplicateID: skipped++;    break;
        }
    }
    file.close();

    if (waitlisted > 0 || skipped > 0){
        cout << "  (" << waitlisted << " waitlisted, " << skipped << " skipped)" << endl;
    }
    return active;
}

bool ReservationManagement::validateReservation(Reservation r){
    ReservationNode* current = ReservationsList.head;
    while(current != nullptr){
        if (r.Date == current->reservation.Date &&
            r.ResourceID == current->reservation.ResourceID){
            return false;
        }
        current = current->next;
    }
    return true;
};

void ReservationManagement::CancelReservation(int ID){
    ReservationNode* node = findReservation(ID);

    if (node == nullptr){
        cout << "ERROR: Reservation not found!" << endl;
        return;
    }

    //Copy Reservation into CancelledReservations
    cancellations.Insert(node->reservation);

    ReservationsList.Remove(node->reservation.ID);
    cout << "Reservation Cancelled. Added to cancellation history." << endl;

    //Waiting List Check - promote the oldest waiting request if the freed
    //resource/date now satisfies it.
    if (waitlist.getSize() > 0 && validateReservation(waitlist.Peek())){
        Reservation promoted = waitlist.Peek();
        ReservationsList.Insert(promoted);
        waitlist.Pop();
        cout << "Waitlisted reservation [" << promoted.ID
             << "] has been assigned the freed resource." << endl;
    }
}

// Restore the most recently cancelled reservation (stack / undo behaviour).
void ReservationManagement::UndoCancellation(){
    if (cancellations.getSize() == 0){
        cout << "ERROR: No cancellations to undo!" << endl;
        return;
    }

    Reservation r = cancellations.Peek();

    // Only restore if the resource is still free for that date.
    if (validateReservation(r)){
        ReservationsList.Insert(r);
        cancellations.Pop();
        cout << "Reservation Restored Successfully." << endl;
    } else {
        cout << "ERROR: Cannot restore reservation [" << r.ID
             << "] - resource is already booked for " << r.Date << "." << endl;
    }
}

//Linear search through linked list by Reservation ID
ReservationNode* ReservationManagement::findReservation(int ID){
    ReservationNode* current = ReservationsList.head;

    while(current != nullptr){
        if(current->reservation.ID == ID){
            return current;
        }
        current = current->next;
    }

    //No match found
    return nullptr;
}

void ReservationManagement::DisplayReservations(){
    ReservationsList.Display();
}

void ReservationManagement::DisplayWaitlist(){
    waitlist.Display();
}

void ReservationManagement::DisplayCancellations(){
    cancellations.Display();
}

//Generate report of reservations
void ReservationManagement::GenerateReport(){
    cout << "------Reservation Report-----" << endl;

    int count = 0;
    ReservationNode* current = ReservationsList.head;
    while(current != nullptr){
        count++;
        current = current->next;
    }
    cout << "Active Reservations: " << count << endl;

    cout << "Waitlist Size: " << this->waitlist.getSize() << endl;
    cout << "Cancellation History Size: " << cancellations.getSize() << endl;
}


/*-------Waitlist Queue-------*/

Waitlist::Waitlist(){
    this->head = nullptr;
    this->tail = nullptr;
    this->size = 0;
}

Waitlist::~Waitlist(){

    ReservationNode* current = this->head;
    while(current != nullptr){
        this->head = current->next;
        delete current;
        current = this->head;
    }

}

void Waitlist::Insert(Reservation r){
    ReservationNode *node = new ReservationNode;

    node->reservation = r;
    node->next = this->head;
    node->previous = nullptr;

    if (this->head != nullptr){
        this->head->previous = node;
    } else {
        this->tail = node;
    }

    this->head = node;

    this->size++;
}

//Oldest request lives at the tail, so remove from the tail (FIFO).
void Waitlist::Pop(){
    if (this->tail == nullptr) return;

    ReservationNode *old = this->tail;
    this->tail = this->tail->previous;

    if (this->tail != nullptr){
        this->tail->next = nullptr;
    } else {
        this->head = nullptr;
    }

    delete old;

    this->size--;
}

//The next thing to serve is always the oldest, peek the tail
Reservation Waitlist::Peek(){
    return this->tail->reservation;
}

//FIFO order, start from the Tail (oldest) toward the head (newest)
void Waitlist::Display(){

    cout << "------     Waitlist     -----" << endl;

    if (this->tail == nullptr){
        cout << "(empty)" << endl;
    }

    ReservationNode* current = this->tail;
    while(current != nullptr){

        cout
        //Reservation ID
        << "[" << current->reservation.ID << "] | "
        //Student Name & ID
        << current->reservation.StudentName << "(" << current->reservation.StudentID << ") | "
        //Resource ID
        << current->reservation.ResourceID << " | "
        //Date
        << current->reservation.Date << endl;

        current = current->previous;
    }

    cout << "-----------------------------" << endl;

}

int Waitlist::getSize() const{
    return this->size;
}

/*-------Cancelation History Stack-------*/

CancellationHistory::CancellationHistory(){
    this->head = nullptr;
    this->size = 0;
}

CancellationHistory::~CancellationHistory(){

    ReservationNode* current = this->head;
    while(current != nullptr){
        this->head = current->next;
        delete current;
        current = this->head;
    }

}

//Doesn't use Previous Pointer as its not needed
void CancellationHistory::Insert(Reservation r){
    ReservationNode *node = new ReservationNode;

    node->reservation = r;
    node->next = this->head;
    node->previous = nullptr;

    this->head = node;

    this->size++;
}

//Most recent cancellation sits at the head, remove from the head (LIFO).
void CancellationHistory::Pop(){
    if (this->head == nullptr) return;

    ReservationNode *old = this->head;
    this->head = this->head->next;
    delete old;

    this->size--;
}

//The next thing to restore is always the most recent, peek the head
Reservation CancellationHistory::Peek(){
    return this->head->reservation;
}

//LIFO order, start from the head (most recent) toward the oldest
void CancellationHistory::Display(){

    cout << "--- Cancellation History ---" << endl;

    if (this->head == nullptr){
        cout << "(empty)" << endl;
    }

    ReservationNode* current = this->head;
    while(current != nullptr){

        cout
        //Reservation ID
        << "[" << current->reservation.ID << "] | "
        //Student Name & ID
        << current->reservation.StudentName << "(" << current->reservation.StudentID << ") | "
        //Resource ID
        << current->reservation.ResourceID << " | "
        //Date
        << current->reservation.Date << endl;

        current = current->next;
    }

    cout << "----------------------------" << endl;

}

int CancellationHistory::getSize() const{
    return this->size;
}
