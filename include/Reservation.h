#ifndef RESERVATION
#define RESERVATION

#include <string>

using namespace std;

//Reservation Struct
struct Reservation{
    int ID;
    int StudentID;
    string StudentName;
    string ResourceID;
    string Date;
};

//Linked List Implimentation for Reservations (Double Linked)
class ReservationNode{
public:
    Reservation reservation;
    ReservationNode *next = nullptr;
    ReservationNode *previous = nullptr;
};


//List
class Reservations{
public:
    void Insert(Reservation r);
    void Remove(int ID);
    void Traverse();

    void Display();

    //Linked List Implimentation
    ReservationNode *head;
    ReservationNode *tail;
};

#endif