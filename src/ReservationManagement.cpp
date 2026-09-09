#include "ReservationManagement.h"

//Linear search through linked list by Reservation ID
Reservation ReservationManagement::findReservation(int ID){
    Reservation* currrent = ReservationsList.head;

    while(currrent != nullptr){
        if(current->ID == ID){
            return *current;
        }
        current = current->next;
    }

    //No match found
    Reservation notFound;
    notFound.ID = -1; //Not found = -1
    return notFound;
}

//Generate report of reservations
void ReservationManagment::GenerateReport(){
    cout << "------Reservation Report-----" << endl;

    int count = 0;
    Reservation* current = ReservationsList.head;
    while(current != nullptr){
        count++;
        current = current->next;
    }
    cout << "Active Reservations: " << count << endl;

    cout << "Waitlist Size: " << Waitlist.size() << endl;
    cout << "Cancellation History Size: " << CancellationHistory.size() << endl;
}