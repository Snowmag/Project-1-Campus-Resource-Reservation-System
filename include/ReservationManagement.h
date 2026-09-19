#ifndef RESERVATIONMANAGER
#define RESERVATIONMANAGER

#include <string>
#include <queue>
#include <stack>
#include "Reservation.h"

using namespace std;

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

enum ReservationStatus{
    DuplicateID,
    Created,
    Waitlisted
};

class ReservationManagement{
public:
    ReservationManagement();
    ~ReservationManagement();

    ReservationStatus CreateReservation(Reservation r);
    void CancelReservation(int ID);
    void UndoCancellation();
    
    bool validateReservation(Reservation r);
    ReservationNode* findReservation(int ID);

    int LoadReservations(const string& filename);
    void GenerateReport();

    void DisplayReservations();
    void DisplayWaitlist();
    void DisplayCancellations();

private:
    Reservations ReservationsList;
    Waitlist waitlist;
    CancellationHistory cancellations;

};

#endif