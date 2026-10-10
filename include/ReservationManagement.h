#ifndef RESERVATIONMANAGER
#define RESERVATIONMANAGER

#include <string>
#include <queue>
#include <stack>
#include <vector>
#include "Reservation.h"
#include "Resource.h"

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

//One row of report statistics for a single resource
struct ResourceStats{
    string ResourceID;
    string Name;
    string Type;
    string Status;
    int ActiveReservations = 0;   //reservation linked list
    int Waiting = 0;              //waiting-list queue
    int Requests() const { return ActiveReservations + Waiting; }
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

    //Reports (menu option 8)
    void ActiveReservationsReport(const Resources& resources) const;    //sorted by date
    void ResourceUtilizationReport(const Resources& resources) const;   //reservations per resource
    void MostRequestedReport(const Resources& resources, int top = 5) const;
    void WaitingListReport(const Resources& resources) const;           //students waiting per resource
    void GenerateReport();

    void DisplayReservations();
    void DisplayWaitlist();
    void DisplayCancellations();

private:
    Reservations ReservationsList;
    Waitlist waitlist;
    CancellationHistory cancellations;

    //Counts reservations and waiting requests per resource for the reports
    vector<ResourceStats> BuildStats(const Resources& resources) const;


};

#endif
