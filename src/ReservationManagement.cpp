#include "ReservationManagement.h"
#include "Reservation.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

//helpers
namespace {

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";   //line was entirely whitespace
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

}

//namespace

ReservationManagement::ReservationManagement(){
    this->nextID = 1;
}

ReservationManagement::~ReservationManagement(){
}

//Reads "ResvID|StudentID|StudentName|ResourceID|Date" records from file
void ReservationManagement::LoadReservations(const string &filename){
    ifstream in(filename);
    if (!in.is_open()){
        cerr << "Error: could not open reservation file '" << filename << "'.\n";
        return;
    }

    string line;
    int loaded = 0;
    while (getline(in, line)){
        line = trim(line);
        if (line.empty()) continue;

        // Split on '|'
        string fields[5];
        int fieldCount = 0;
        stringstream ss(line);
        string field;
        while (fieldCount < 5 && getline(ss, field, '|')){
            fields[fieldCount++] = trim(field);
        }
        if (fieldCount != 5){
            cerr << "Warning: skipping malformed reservation line: " << line << "\n";
            continue;
        }

        Reservation r;
        try {
            r.ID        = stoi(fields[0]);
            r.StudentID = stoi(fields[1]);
        } catch (const exception &) {
            cerr << "Warning: skipping reservation line with a non-numeric ID: " << line << "\n";
            continue;
        }
        r.StudentName = fields[2];
        r.ResourceID  = fields[3];
        r.Date        = fields[4];

        //Duplicate reservation IDs are rejected
        if (reservationIDExists(r.ID)){
            cerr << "Warning: skipping duplicate reservation ID " << r.ID << ".\n";
            continue;
        }

        ReservationsList.Insert(r);
        if (r.ID >= this->nextID) this->nextID = r.ID + 1;
        ++loaded;
    }
    in.close();
    cout << "Loaded " << loaded << " reservations from " << filename << ".\n";
}

//A resource may not be booked twice on the same date
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
}

bool ReservationManagement::reservationIDExists(int ID){
    return findReservation(ID) != nullptr;
}

void ReservationManagement::CreateReservation(Reservation r){
    //An ID of 0 means "assign the next free one"
    if (r.ID <= 0){
        r.ID = this->nextID;
    }

    if (reservationIDExists(r.ID)){
        cout << "ERROR: Reservation ID " << r.ID << " is already in use!" << endl;
        return;
    }

    if (r.ID >= this->nextID) this->nextID = r.ID + 1;

    if (validateReservation(r)){
        ReservationsList.Insert(r);
        cout << "Reservation Created Successfully. (ID " << r.ID << ")" << endl;
        return;
    }

    //Resource is taken on that date, so the request queues up instead
    waitlist.Insert(r);
    cout << "Resource unavailable on " << r.Date
         << ". Added to the waiting list. (ID " << r.ID << ")" << endl;
}

void ReservationManagement::CancelReservation(int ID){
    ReservationNode* node = findReservation(ID);

    if (node == nullptr){
        cout << "ERROR: Reservation not found!" << endl;
        return;
    }

    //Copy Reservation into CancelledReservations
    Reservation cancelled = node->reservation;
    cancellations.Insert(cancelled);

    ReservationsList.Remove(cancelled.ID);
    cout << "Reservation Cancelled." << endl;
    cout << "Added to cancellation history." << endl;

    //Waiting List Check: hand the freed slot to the next student in line
    if (!waitlist.isEmpty() && validateReservation(waitlist.Peek())){
        Reservation next = waitlist.Peek();
        waitlist.Pop();
        ReservationsList.Insert(next);
        cout << "Waiting list: reservation " << next.ID << " for "
             << next.StudentName << " is now active." << endl;
    }
    
}

//Restores the most recently cancelled reservation (top of the stack)
void ReservationManagement::UndoCancellation(){
    if (cancellations.isEmpty()){
        cout << "ERROR: No cancellations to undo!" << endl;
        return;
    }

    Reservation r = cancellations.Peek();
    cancellations.Pop();

    if (reservationIDExists(r.ID)){
        cout << "ERROR: Reservation " << r.ID << " is already active!" << endl;
        return;
    }

    ReservationsList.Insert(r);
    cout << "Reservation Restored Successfully." << endl;
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

void ReservationManagement::DisplayCancellationHistory(){
    cancellations.Display();
}

//Generate report of reservations
void ReservationManagement::GenerateReport(){
    cout << "------Reservation Report-----" << endl;

    cout << "Active Reservations: " << ReservationsList.Size() << endl;

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

//New arrivals go on at the head, so the oldest request sits at the tail
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

//First In, First Out: the oldest request leaves first
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

//The next thing to pop is always the last, peek the tail
Reservation Waitlist::Peek(){
    if (this->tail == nullptr) return Reservation();
    return this->tail->reservation;
}

//First In, First Out, start from the Tail
void Waitlist::Display(){
    
    cout << "------     Waitlist     -----" << endl;
    if (this->head == nullptr){
        cout << "The waiting list is empty." << endl;
        cout << "-----------------------------" << endl;
        return;
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

bool Waitlist::isEmpty() const{
    return this->size == 0;
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

//Last In, First Out: the newest cancellation is removed first
void CancellationHistory::Pop(){
    if (this->head == nullptr) return;

    ReservationNode *old = this->head;
    this->head = this->head->next;
    delete old;

    this->size--;
}

//The next thing to pop is always the first, peek the head
Reservation CancellationHistory::Peek(){
    if (this->head == nullptr) return Reservation();
    return this->head->reservation;
}

//Last In, First Out, start from the head
void CancellationHistory::Display(){

    cout << "--- Cancellation History ---" << endl;
    if (this->head == nullptr){
        cout << "No cancellations recorded." << endl;
        cout << "----------------------------" << endl;
        return;
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

bool CancellationHistory::isEmpty() const{
    return this->size == 0;
}

int CancellationHistory::getSize() const{
    return this->size;
}
