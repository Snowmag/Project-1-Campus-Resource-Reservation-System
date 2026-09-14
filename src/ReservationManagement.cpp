#include "ReservationManagement.h"
#include "Reservation.h"

#include <string>
#include <iostream>

using namespace std;

ReservationManagement::ReservationManagement(){

}

ReservationManagement::~ReservationManagement(){
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

void ReservationManagement::CreateReservation(Reservation r){
    if (validateReservation(r)){
        ReservationsList.Insert(r);
        return;
    }
    waitlist.Insert(r);
}

void ReservationManagement::CancelReservation(int ID){
    ReservationNode* node = findReservation(ID);

    if (node == nullptr){
        cout << "ERROR: Reservation not found!" << endl;
        return;
    }

    //Copy Reservation into CancelledReservations
    cancellations.Insert(node->reservation);

    ReservationsList.Remove(node->reservation.ID);

    //Waiting List Check
    if (waitlist.getSize() > 0 && validateReservation(waitlist.Peek())){
        ReservationsList.Insert(waitlist.Peek());
        waitlist.Pop();
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

//First in, Last Out
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
    return this->tail->reservation;
}

//First In, Last Out, start from the Tail
void Waitlist::Display(){
    
    cout << "------     Waitlist     -----" << endl;
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

//First in, First Out
void CancellationHistory::Pop(){
    if (this->head == nullptr) return;

    ReservationNode *old = this->head;
    this->head = this->head->next;
    delete old;

    this->size--;
}

//The next thing to pop is always the first, peek the head
Reservation CancellationHistory::Peek(){
    return this->head->reservation;
}

//First in, First out, start from the head
void CancellationHistory::Display(){

    cout << "--- Cancellation History ---" << endl;
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