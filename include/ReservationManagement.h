#ifndef RESERVATIONMANAGER
#define RESERVATIONMANAGER

#include <string>
#include <queue>
#include <stack>
#include "include/Reservation.h"

using namespace std;

class ReservationManagement{
public:
    void CreateReservation(Reservation r);
    void CancelReservation(Reservation r);
    bool validateReservation(Reservation r);
    Reservation findReservation(int ID);

    void GenerateReport();

private:
    Reservations ReservationsList;
    Waitlist waitlist;
    CancellationHistory cancellations;

};

/* Waitlist & CancellationHistory Classes*/
class Waitlist{
public:
    Waitlist();
    ~Waitlist();

    void Insert(Reservation r);

    //Allows to grab Reservation and remove it at the same time
    Reservation Pop();

    Reservation Peek();

    void Display();

    int getSize() const;

private:
    ReservationNode *head;
    ReservationNode *tail;
    int size;
};

class CancellationHistory{
public:
    CancellationHistory();
    ~CancellationHistory();

    void Insert(Reservation r);

    //Allows to grab Reservation and remove it at the same time
    Reservation Pop();

    Reservation Peek();

    void Display();

    int getSize() const;

private:
    ReservationNode *head;
    int size;
};

#endif