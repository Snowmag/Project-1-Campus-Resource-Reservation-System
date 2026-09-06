#ifndef RESERVATION
#define RESERVATION

#include <string>

using namespace std;

//Basic Reservation; Node
class Reservation{
    int ID;
    int StudentID;
    string StudentName;
    string ResourceID;
    string Date;

    //Linked List Implimentation
    Reservation *next;
};

//List
class Reservations{
    void Insert(Reservation r);
    void Remove(int ID);
    void Traverse();

    void Display();

    //Linked List Implimentation
    Reservation *head;
    Reservation *tail;
};

#endif