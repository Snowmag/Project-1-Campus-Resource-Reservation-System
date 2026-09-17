#include "Reservation.h"

#include <iostream>

using namespace std;

// The header does not initialise head/tail, so the constructor must.
Reservations::Reservations(){
    this->head = nullptr;
    this->tail = nullptr;
}

// Free every node so the list does not leak.
Reservations::~Reservations(){
    ReservationNode* current = this->head;
    while(current != nullptr){
        ReservationNode* next = current->next;
        delete current;
        current = next;
    }
    this->head = nullptr;
    this->tail = nullptr;
}

// Insert a reservation at the tail (keeps records in creation order). O(1)
void Reservations::Insert(Reservation r){
    ReservationNode* node = new ReservationNode;
    node->reservation = r;
    node->next = nullptr;
    node->previous = this->tail;

    if (this->tail != nullptr){
        this->tail->next = node;
    } else {
        this->head = node;      // list was empty
    }
    this->tail = node;
}

// Remove the first reservation whose ID matches. O(n)
void Reservations::Remove(int ID){
    ReservationNode* current = this->head;

    while(current != nullptr){
        if (current->reservation.ID == ID){
            // relink the previous neighbour
            if (current->previous != nullptr){
                current->previous->next = current->next;
            } else {
                this->head = current->next;   // removing the head
            }
            // relink the next neighbour
            if (current->next != nullptr){
                current->next->previous = current->previous;
            } else {
                this->tail = current->previous; // removing the tail
            }
            delete current;
            return;
        }
        current = current->next;
    }
}

// Walk the whole list front to back (compact view). O(n)
void Reservations::Traverse(){
    ReservationNode* current = this->head;
    if (current == nullptr){
        cout << "(empty)" << endl;
        return;
    }
    while(current != nullptr){
        cout << current->reservation.ID;
        if (current->next != nullptr) cout << " -> ";
        current = current->next;
    }
    cout << endl;
}

// Pretty-print every active reservation. O(n)
void Reservations::Display(){
    cout << "-----   Active Reservations   -----" << endl;

    if (this->head == nullptr){
        cout << "(none)" << endl;
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
    cout << "-----------------------------------" << endl;
}
