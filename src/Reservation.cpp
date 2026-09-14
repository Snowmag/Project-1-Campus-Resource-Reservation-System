#include "Reservation.h"

#include <iomanip>
#include <sstream>

using namespace std;

Reservations::Reservations () : head(nullptr), tail(nullptr), size(0) {}

Reservations::~Reservations () {
    ReservationNode *current = head;
    while (current != nullptr) {
        ReservationNode *next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    size = 0;
}

//Append at the tail
void Reservations::Insert(Reservation r) {
    ReservationNode *node = new ReservationNode;
    node->reservation = r;
    node->next = nullptr;
    node->previous = tail;

    if (tail != nullptr) {
        tail->next = node;
    } else {
        head = node;
    }
    tail = node;

    size++;
}

//Remove by ID
void Reservations::Remove(int ID) {
    ReservationNode *current = head;
    while (current != nullptr) {
        if (current->reservation.ID == ID) {
            if (current->previous != nullptr) {
                current->previous->next = current->next;
            } else {
                head = current->next;
            }

            if (current->next != nullptr) {
                current->next->previous = current->previous;
            } else {
                tail = current->previous;
            }

            delete current;
            size--;
            return;
        }
        current = current->next;
    }
}

void Reservations::Traverse() {
    ReservationNode *current = head;
    while (current != nullptr) {
        cout << "ID: " << current->reservation.ID << (current->next != nullptr ? " -> " : "\n");
        current = current->next;
    }
}

void Reservations::Display() {
    cout << "\n-----Active Reservations (" << size << ")-----\n";
    if (head == nullptr) {
        cout << "No active reservations.\n";
        return;
    }
}

cout << "ResvID | StudentID | StudentName | ResourceID | Date\n"
cout << "----------------------------------------------------\n";
ReservationNode *current = head;
while (current != nullptr) {
    const Reservation &r = current->reservation;
    cout << left
         << setw(8) << r.ID
         << setw(10) << r.StudentID
         << setw(22) << r.StudentName
         << setw(8) << r.ResourceID
         << r.Date << "\n";
    current = current->next;
}

int Reservations::Size() const {return size; }