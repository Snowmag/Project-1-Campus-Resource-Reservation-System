#ifndef RESERVATIONMANAGER
#define RESERVATIONMANAGER

#include <string>
#include <queue>
#include <stack>
#include "include/Reservation.h"

using namespace std;

class ReservationManagement{
public:
    ReservationManagement();
    ~ReservationManagement();

    void CreateReservation(Reservation r);
    void CancelReservation(int ID);
    bool validateReservation(Reservation r);
    ReservationNode* findReservation(int ID);

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

    void Pop();

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

    void Pop();

    Reservation Peek();

    void Display();

    int getSize() const;

private:
    ReservationNode *head;
    int size;
};

#endif