#include "ReservationManagement.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Strip surrounding whitespace and a trailing '\r' (Windows-edited files).
static string trimField(string s){
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

ReservationManagement::ReservationManagement(){

}

ReservationManagement::~ReservationManagement(){

}

ReservationStatus ReservationManagement::CreateReservation(Reservation r){
    // Prevent duplicate reservation IDs among active reservations.
    if (findReservation(r.ID) != nullptr){
        return ReservationStatus::DuplicateID;
    }

    if (validateReservation(r)){
        ReservationsList.Insert(r);
        return ReservationStatus::Created;
    }

    waitlist.Insert(r);
    return ReservationStatus::Waitlisted;
}


// Load reservations from a '|' delimited file:
//   ID|StudentID|StudentName|ResourceID|Date
// Each record runs through CreateReservation, so duplicate IDs are rejected and
// resource/date clashes are pushed to the waiting list automatically.
// Returns the number of records placed into the active reservation list.
int ReservationManagement::LoadReservations(const string& filename){
    ifstream file(filename);
    if (!file.is_open()){
        cout << "ERROR: Could not open reservation file: " << filename << endl;
        return 0;
    }

    int active = 0, waitlisted = 0, skipped = 0, lineNo = 0;
    string line;
    while (getline(file, line)){
        lineNo++;
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string id, sid, name, res, date;
        if (!getline(ss, id,   '|') ||
            !getline(ss, sid,  '|') ||
            !getline(ss, name, '|') ||
            !getline(ss, res,  '|') ||
            !getline(ss, date, '|')){
            cout << "WARNING: Skipping malformed reservation on line " << lineNo << endl;
            skipped++;
            continue;
        }

        Reservation r;
        try {
            r.ID        = stoi(trimField(id));
            r.StudentID = stoi(trimField(sid));
        } catch (...) {
            cout << "WARNING: Bad numeric field on line " << lineNo << endl;
            skipped++;
            continue;
        }
        r.StudentName = trimField(name);
        r.ResourceID  = trimField(res);
        r.Date        = trimField(date);

        switch (this->CreateReservation(r)){
            case ReservationStatus::Created:     active++;     break;
            case ReservationStatus::Waitlisted:  waitlisted++; break;
            case ReservationStatus::DuplicateID: skipped++;    break;
        }
    }
    file.close();

    if (waitlisted > 0 || skipped > 0){
        cout << "  (" << waitlisted << " waitlisted, " << skipped << " skipped)" << endl;
    }
    return active;
}

bool ReservationManagement::validateReservation(Reservation r){
    ReservationNode* current = ReservationsList.head;
    while(current != nullptr){
        if (r.Date == current->reservation.Date &&
            r.ResourceID == current->reservation.ResourceID){
            return false;
        }
        current = current->next;
    }
    return true;
};

void ReservationManagement::CancelReservation(int ID){
    ReservationNode* node = findReservation(ID);

    if (node == nullptr){
        cout << "ERROR: Reservation not found!" << endl;
        return;
    }

    //Copy Reservation into CancelledReservations
    cancellations.Insert(node->reservation);

    ReservationsList.Remove(node->reservation.ID);
    cout << "Reservation Cancelled. Added to cancellation history." << endl;

    //Waiting List Check - promote the oldest waiting request if the freed
    //resource/date now satisfies it.
    if (waitlist.getSize() > 0 && validateReservation(waitlist.Peek())){
        Reservation promoted = waitlist.Peek();
        ReservationsList.Insert(promoted);
        waitlist.Pop();
        cout << "Waitlisted reservation [" << promoted.ID
             << "] has been assigned the freed resource." << endl;
    }
}

// Restore the most recently cancelled reservation (stack / undo behaviour).
void ReservationManagement::UndoCancellation(){
    if (cancellations.getSize() == 0){
        cout << "ERROR: No cancellations to undo!" << endl;
        return;
    }

    Reservation r = cancellations.Peek();

    // Only restore if the resource is still free for that date.
    if (validateReservation(r)){
        ReservationsList.Insert(r);
        cancellations.Pop();
        cout << "Reservation Restored Successfully." << endl;
    } else {
        cout << "ERROR: Cannot restore reservation [" << r.ID
             << "] - resource is already booked for " << r.Date << "." << endl;
    }
}

//Linear search through linked list by Reservation ID
ReservationNode* ReservationManagement::findReservation(int ID){
    ReservationNode* current = ReservationsList.head;

    while(current != nullptr){
        if(current->reservation.ID == ID){
            return current;
        }
        current = current->next;
    }

    //No match found
    return nullptr;
}

void ReservationManagement::DisplayReservations(){
    ReservationsList.Display();
}

void ReservationManagement::DisplayWaitlist(){
    waitlist.Display();
}

void ReservationManagement::DisplayCancellations(){
    cancellations.Display();
}

/*-------Reports-------*/

//Merge step: combines the sorted halves [left, mid] and [mid + 1, right]
template <typename T>
static void Merge(vector<T> &items, int left, int mid, int right, bool (*precedes)(const T &, const T &)) {
    vector<T> merged;
    merged.reserve(static_cast<size_t>(right - left + 1));

    int i = left;
    int j = mid + 1;
    while (i <= mid && j <= right) {
        if (precedes(items[j], items[i])) {
            merged.push_back(items[j++]);
        } else {
            merged.push_back(items[i++]);   //keeps the sort stable
        }
    }
    while (i <= mid)   merged.push_back(items[i++]);
    while (j <= right) merged.push_back(items[j++]);
    for (size_t k = 0; k < merged.size(); ++k) {
        items[left + static_cast<int>(k)] = merged[k];
    }
}

//Merge sort: O(n log n), used to order report rows
template <typename T>
static void MergeSort(vector<T> &items, int left, int right, bool (*precedes)(const T &, const T &)) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    MergeSort(items, left, mid, precedes);
    MergeSort(items, mid + 1, right, precedes);
    Merge(items, left, mid, right, precedes);
}

template <typename T>
static void Sort(vector<T> &items, bool (*precedes)(const T &, const T &)) {
    if (items.size() > 1) MergeSort(items, 0, static_cast<int>(items.size()) - 1, precedes);
}

//Turns "MM/DD/YYYY" into "YYYYMMDD" so dates compare correctly as text.
//Anything else (e.g. "N/A") sorts after every real date.
static string DateKey(const string &date) {
    if (date.size() == 10 && date[2] == '/' && date[5] == '/') {
        return date.substr(6, 4) + date.substr(0, 2) + date.substr(3, 2);
    }
    return "~" + date;
}

static bool EarlierDate(const Reservation &a, const Reservation &b) {
    string ka = DateKey(a.Date);
    string kb = DateKey(b.Date);
    if (ka != kb) return ka < kb;
    return a.ID < b.ID;
}

//Most requests first, ties broken by Resource ID
static bool MoreRequested(const ResourceStats &a, const ResourceStats &b) {
    if (a.Requests() != b.Requests()) return a.Requests() > b.Requests();
    return a.ResourceID < b.ResourceID;
}

//Linear search for a resource's row; -1 when the ID is not present
static int FindRow(const vector<ResourceStats> &rows, const string &ID) {
    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].ResourceID == ID) return static_cast<int>(i);
    }
    return -1;
}

//Row for a reservation whose resource is missing from the inventory
static ResourceStats UnknownRow(const string &ID) {
    ResourceStats row;
    row.ResourceID = ID;
    row.Name = "(not in inventory)";
    row.Type = "-";
    row.Status = "-";
    return row;
}

//Linear search of the inventory for a resource's name
static string ResourceName(const Resources &resources, const string &ID) {
    for (const Resource &r : resources.GetAll()) {
        if (r.ID == ID) return r.Name;
    }
    return "(not in inventory)";
}

static string Truncate(const string &s, size_t width) {
    if (s.size() < width) return s;
    return s.substr(0, width - 2) + "~";
}

static void PrintHeader(const string &title) {
    cout << "\n===== " << title << " =====\n";
}

static void PrintLine(int width) {
    cout << string(static_cast<size_t>(width), '-') << "\n";
}


vector<ResourceStats> ReservationManagement::BuildStats(const Resources &resources) const {
    vector<ResourceStats> rows;

    //One row per resource, in inventory order
    for (const Resource &r : resources.GetAll()) {
        ResourceStats row;
        row.ResourceID = r.ID;
        row.Name = r.Name;
        row.Type = r.Type;
        row.Status = r.Status;
        rows.push_back(row);
    }

    //Traverse the reservation linked list
    for (const ReservationNode *node = ReservationsList.head; node != nullptr; node = node->next) {
        int index = FindRow(rows, node->reservation.ResourceID);
        if (index == -1) {
            rows.push_back(UnknownRow(node->reservation.ResourceID));
            index = static_cast<int>(rows.size()) - 1;
        }
        rows[static_cast<size_t>(index)].ActiveReservations++;
    }

    //Traverse the waiting-list queue from front to back
    for (const ReservationNode *node = waitlist.Oldest(); node != nullptr; node = node->previous) {
        int index = FindRow(rows, node->reservation.ResourceID);
        if (index == -1) {
            rows.push_back(UnknownRow(node->reservation.ResourceID));
            index = static_cast<int>(rows.size()) - 1;
        }
        rows[static_cast<size_t>(index)].Waiting++;
    }
    return rows;
}

void ReservationManagement::ActiveReservationsReport(const Resources &resources) const {
    PrintHeader("Active Reservations Report");

    vector<Reservation> active;
    for (const ReservationNode *node = ReservationsList.head; node != nullptr; node = node->next) {
        active.push_back(node->reservation);
    }

    if (active.empty()) {
        cout << "There are no active reservations.\n";
        return;
    }

    Sort(active, EarlierDate);

    cout << left
    << setw(8)  << "ResvID"
    << setw(11) << "StudentID"
    << setw(22) << "Student Name"
    << setw(10) << "Resource"
    << setw(22) << "Resource Name"
    << "Date\n";
    PrintLine(83);
    for (const Reservation &r : active) {
        string resName = ResourceName(resources, r.ResourceID);
        cout << left
        << setw(8)  << r.ID
        << setw(11) << r.StudentID
        << setw(22) << Truncate(r.StudentName, 22)
        << setw(10) << r.ResourceID
        << setw(22) << Truncate(resName, 22)
        << r.Date << "\n";
    }
    PrintLine(83);
    cout << "Total active reservations: " << active.size() << " (sorted by date)\n";
}

void ReservationManagement::ResourceUtilizationReport(const Resources &resources) const {
    PrintHeader("Resource Utilization Report");

    vector<ResourceStats> rows = BuildStats(resources);
    if (rows.empty()) {
        cout << "No resources loaded.\n";
        return;
    }

    cout << left
    << setw(8)  << "ID"
    << setw(22) << "Name"
    << setw(22) << "Type"
    << setw(13) << "Status"
    << "Reservations\n";
    PrintLine(77);

    int total = 0;
    int used = 0;
    for (const ResourceStats &row : rows) {
        cout << left
        << setw(8)  << row.ResourceID
        << setw(22) << Truncate(row.Name, 22)
        << setw(22) << Truncate(row.Type, 22)
        << setw(13) << row.Status
        << setw(4)  << row.ActiveReservations
        << string(static_cast<size_t>(row.ActiveReservations), '#') << "\n";
        total += row.ActiveReservations;
        if (row.ActiveReservations > 0) used++;
    }
    PrintLine(77);

    int percent = static_cast<int>(rows.size()) > 0 ? (used * 100) / static_cast<int>(rows.size()) : 0;
    cout << "Resources listed:             " << rows.size() << "\n";
    cout << "Resources with reservations:  " << used << " (" << percent << "%)\n";
    cout << "Resources never reserved:     " << rows.size() - static_cast<size_t>(used) << "\n";
    cout << "Total reservations:           " << total << "\n";
}

void ReservationManagement::MostRequestedReport(const Resources &resources, int top) const {
    PrintHeader("Most Requested Resources");

    vector<ResourceStats> rows = BuildStats(resources);
    Sort(rows, MoreRequested);

    if (rows.empty() || rows[0].Requests() == 0) {
        cout << "No resources have been requested yet.\n";
        return;
    }

    cout << left
    << setw(6)  << "Rank"
    << setw(8)  << "ID"
    << setw(22) << "Name"
    << setw(9)  << "Active"
    << setw(9)  << "Waiting"
    << "Total Requests\n";
    PrintLine(68);

    //Resources tied with the last ranked one are listed too
    int rank = 0;
    int shown = 0;
    int previous = -1;
    for (size_t i = 0; i < rows.size(); ++i) {
        const ResourceStats &row = rows[i];
        if (row.Requests() == 0) break;
        if (row.Requests() != previous) {
            if (shown >= top) break;
            rank = shown + 1;
            previous = row.Requests();
        }
        cout << left
        << setw(6)  << rank
        << setw(8)  << row.ResourceID
        << setw(22) << Truncate(row.Name, 22)
        << setw(9)  << row.ActiveReservations
        << setw(9)  << row.Waiting
        << row.Requests() << "\n";
        shown++;
    }
    PrintLine(68);
    cout << "Requests = active reservations + students on the waiting list.\n";
}

void ReservationManagement::WaitingListReport(const Resources &resources) const {
    PrintHeader("Waiting-List Statistics");

    const Waitlist &queue = waitlist;
    if (queue.getSize() == 0) {
        cout << "No students are currently waiting for any resource.\n";
        return;
    }

    vector<ResourceStats> rows = BuildStats(resources);

    cout << left
    << setw(8)  << "ID"
    << setw(22) << "Name"
    << setw(9)  << "Waiting"
    << "Next in Line\n";
    PrintLine(70);

    int resourcesWithQueue = 0;
    int longest = 0;
    string longestID;
    for (const ResourceStats &row : rows) {
        if (row.Waiting == 0) continue;

        //First matching node from the front is the oldest request (FIFO)
        string next = "-";
        for (const ReservationNode *node = queue.Oldest(); node != nullptr; node = node->previous) {
            if (node->reservation.ResourceID == row.ResourceID) {
                ostringstream out;
                out << node->reservation.StudentName << " (" << node->reservation.StudentID
                << ", " << node->reservation.Date << ")";
                next = out.str();
                break;
            }
        }

        cout << left
        << setw(8)  << row.ResourceID
        << setw(22) << Truncate(row.Name, 22)
        << setw(9)  << row.Waiting
        << next << "\n";

        resourcesWithQueue++;
        if (row.Waiting > longest) {
            longest = row.Waiting;
            longestID = row.ResourceID;
        }
    }
    PrintLine(70);

    const ReservationNode *front = queue.Oldest();
    cout << "Students waiting (total):     " << queue.getSize() << "\n";
    cout << "Resources with a waiting list: " << resourcesWithQueue << "\n";
    cout << "Longest waiting list:         " << longestID << " (" << longest << " waiting)\n";
    cout << "Next request to be served:    [" << front->reservation.ID << "] "
    << front->reservation.StudentName << " for " << front->reservation.ResourceID << "\n";
}

void ReservationManagement::GenerateReport(const Resources &resources) const {
    cout << "\n######## Campus Resource Reservation System - Full Report ########\n";
    ActiveReservationsReport(resources);
    ResourceUtilizationReport(resources);
    MostRequestedReport(resources);
    WaitingListReport(resources);
    cout << "\nCancellation history entries: " << cancellations.getSize() << "\n";
}


/*-------Waitlist Queue-------*/

Waitlist::Waitlist(){
    this->head = nullptr;
    this->tail = nullptr;
    this->size = 0;
}

Waitlist::~Waitlist(){

    ReservationNode* current = this->head;
    while(current != nullptr){
        this->head = current->next;
        delete current;
        current = this->head;
    }

}

void Waitlist::Insert(Reservation r){
    ReservationNode *node = new ReservationNode;

    node->reservation = r;
    node->next = this->head;
    node->previous = nullptr;

    if (this->head != nullptr){
        this->head->previous = node;
    } else {
        this->tail = node;
    }

    this->head = node;

    this->size++;
}

//Oldest request lives at the tail, so remove from the tail (FIFO).
void Waitlist::Pop(){
    if (this->tail == nullptr) return;

    ReservationNode *old = this->tail;
    this->tail = this->tail->previous;

    if (this->tail != nullptr){
        this->tail->next = nullptr;
    } else {
        this->head = nullptr;
    }

    delete old;

    this->size--;
}


//Oldest request (next to be served) sits at the tail
ReservationNode* Waitlist::Oldest() const{
    return this->tail;
}


//The next thing to serve is always the oldest, peek the tail
Reservation Waitlist::Peek(){
    return this->tail->reservation;
}

//FIFO order, start from the Tail (oldest) toward the head (newest)
void Waitlist::Display(){

    cout << "------     Waitlist     -----" << endl;

    if (this->tail == nullptr){
        cout << "(empty)" << endl;
    }

    ReservationNode* current = this->tail;
    while(current != nullptr){

        cout
        //Reservation ID
        << "[" << current->reservation.ID << "] | "
        //Student Name & ID
        << current->reservation.StudentName << "(" << current->reservation.StudentID << ") | "
        //Resource ID
        << current->reservation.ResourceID << " | "
        //Date
        << current->reservation.Date << endl;

        current = current->previous;
    }

    cout << "-----------------------------" << endl;

}

int Waitlist::getSize() const{
    return this->size;
}

/*-------Cancelation History Stack-------*/

CancellationHistory::CancellationHistory(){
    this->head = nullptr;
    this->size = 0;
}

CancellationHistory::~CancellationHistory(){

    ReservationNode* current = this->head;
    while(current != nullptr){
        this->head = current->next;
        delete current;
        current = this->head;
    }

}

//Doesn't use Previous Pointer as its not needed
void CancellationHistory::Insert(Reservation r){
    ReservationNode *node = new ReservationNode;

    node->reservation = r;
    node->next = this->head;
    node->previous = nullptr;

    this->head = node;

    this->size++;
}

//Most recent cancellation sits at the head, remove from the head (LIFO).
void CancellationHistory::Pop(){
    if (this->head == nullptr) return;

    ReservationNode *old = this->head;
    this->head = this->head->next;
    delete old;

    this->size--;
}

//The next thing to restore is always the most recent, peek the head
Reservation CancellationHistory::Peek(){
    return this->head->reservation;
}

//LIFO order, start from the head (most recent) toward the oldest
void CancellationHistory::Display(){

    cout << "--- Cancellation History ---" << endl;

    if (this->head == nullptr){
        cout << "(empty)" << endl;
    }

    ReservationNode* current = this->head;
    while(current != nullptr){

        cout
        //Reservation ID
        << "[" << current->reservation.ID << "] | "
        //Student Name & ID
        << current->reservation.StudentName << "(" << current->reservation.StudentID << ") | "
        //Resource ID
        << current->reservation.ResourceID << " | "
        //Date
        << current->reservation.Date << endl;

        current = current->next;
    }

    cout << "----------------------------" << endl;

}

int CancellationHistory::getSize() const{
    return this->size;
}
