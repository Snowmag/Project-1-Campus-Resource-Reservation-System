#ifndef RESERVATIONMANAGER
#define RESERVATIONMANAGER

#include <string>
#include <queue>
#include <stack>
#include "include/Reservation.h"

using namespace std;

class ReservationManagement{
    void CreateReservation();
    void CancelReservation();
    void viewReservations();
    Reservation findReservation();

    void GenerateReport();

    Reservations ReservationsList;
    queue<Reservation> Waitlist;
    stack<Reservation> CancellationHistory;

};

#endif