#ifndef RESERVATION
#define RESERVATION

#include <string>
#include <vector>

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
    Reservations();
    ~Reservations();

    void Insert(Reservation r);
    void Remove(int ID);
    void Traverse();

    void Display();

    vector<Reservation> ToVector();

    //Linked List Implimentation
    ReservationNode *head;
    ReservationNode *tail;
};

void SortReservationsByID(vector<Reservation>& v);
void SortReservationsByStudent(vector<Reservation>& v);

int BinarySearchReservationID(const vector<Reservation>& sortedByID, int id);

int LowerBoundStudentID(const vector<Reservation>& sortedByStudent, int studentID);

#endif