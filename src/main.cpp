#include <iostream>
#include <string>

#include "Resource.h"
#include "ReservationManagement.h"

using namespace std;

namespace {

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);

}

string readLine(const string &prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    return trim(line);
}

int readInt(const string &prompt) {
    while (true) {
        string line = readLine(prompt);
        try {
            size_t pos = 0;
            int value = stoi(line, &pos);
            if (pos == line.size()) return value;
        } catch (const exception &) { /* fall through */ }
        cout << " Please enter a valid whole number.\n";
    }
}

void printMenu() {
    cout << "\n=====Campus Resource Reservation System=====\n";
    cout << "1. View Resources\n";
    cout << "2. Create Reservation\n";
    cout << "3. Cancel Reservation\n";
    cout << "4. View Reservations\n";
    cout << "5. View Waitlist\n";
    cout << "6. Undo Cancellation\n";
    cout << "7. Search Reservations\n";
    cout << "8. Sort Resources\n";
    cout << "9. Generate Report\n";
    cout << "0. Exit\n";
    cout << "Enter Choice: ";
}

// Option 1: View Resources
void handleViewResources(Resources &resources) {
    cout << "\n1. List all 2. Find by ID\n";
    string c = readLine("Enter Choice: ");
    if (c == "1") {
        resources.DisplayResources();
    } else if (c == "2") {
        Resource *r = resources.findResource(readLine("Enter Resource ID: "));
        if (r == nullptr) {
            cout << "Resource not found.\n";
        } else {
            cout << "Found: " << r->toDisplayString() << "\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }
}

// Option 2: Create Reservation
void handleCreate(Resources &resources, ReservationManagement &manager) {
    cout << "\n---- Create Reservation ----\n";
    int studentId = readInt("Enter Student ID: ");
    string name = readLine("Enter Student Name: ");
    string resId = readLine("Enter Resource ID: ");
    string date = readLine("Enter Reservation Date (MM/DD/YYYY): ");

    if (name.empty() || resId.empty()) {
        cout << "Name and Resource ID cannot be empty.\n";
        return;
    }
    if (resources.findResource(resId) == nullptr) {
        cout << "Error: Invalid Resource ID '" << resId << "'.\n";
        return;
    }
    if (date.empty()) date = "N/A"; // Default date if not provided

    Reservation r;
    r.ID = 0;                       // 0 asks the manager for the next free ID
    r.StudentID = studentId;
    r.StudentName = name;
    r.ResourceID = resId;
    r.Date = date;
    manager.CreateReservation(r);
}

// Option 8: Sort Resources
void handleSort(Resources &resources) {
    cout << "\n1. By ID 2. By Name 3. By Type\n";
    string c = readLine("Enter Choice: ");
    if (c == "1")
        resources.SortList("id");
    else if (c == "2")
        resources.SortList("name");
    else if (c == "3")
        resources.SortList("type");
    else {
        cout << "Invalid choice.\n";
        return;
    }
    resources.DisplayResources();
}

}

// namespace

int main(int argc, char **argv) {
    string resourcesFile = (argc > 1) ? argv[1] : "data/resources.txt";
    string reservationsFile = (argc > 2) ? argv[2] : "data/reservations.txt";

    cout << "Initializing Campus Resource Reservation System...\n";

    Resources resources;
    resources.LoadResources(resourcesFile);

    ReservationManagement manager;
    manager.LoadReservations(reservationsFile);

    while(true) {
        printMenu();
        string choice = readLine("");

        if (choice == "1")  {
            handleViewResources(resources);
        } else if (choice == "2") {
            handleCreate(resources, manager);
        } else if (choice == "3") {
            manager.CancelReservation(readInt("Enter Reservation ID to cancel: "));
        } else if (choice == "4") {
            manager.DisplayReservations();
        } else if (choice == "5") {
            manager.DisplayWaitlist();
        } else if (choice == "6") {
            manager.UndoCancellation();
        } else if (choice == "7") {
            ReservationNode *node = manager.findReservation(readInt("Enter Reservation ID to search: "));
            if (node == nullptr) {
                cout << "Reservation not found.\n";
            } else {
                const Reservation &r = node->reservation;
                cout << "Found [" <<r.ID << "] " << r.StudentName
                     << " [" << r.StudentID << "] " << r.ResourceID
                     << " | " << r.Date << "\n";

            }
        } else if (choice == "8") {
            handleSort(resources);
        } else if (choice == "9") {
            manager.GenerateReport();
        } else if (choice == "0") {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}
