#include "Reservation.h"
#include "ReservationManagement.h"
#include "Resource.h"

#include <iostream>
#include <string>
#include <limits>

using namespace std;

/* ---------- small input helpers (basic validation) ---------- */

// Read an integer, re-prompting on bad input. On end-of-input, returns 9
// so a piped/closed stdin cleanly exits the menu.
static int readInt(const string& prompt){
    int value;
    while (true){
        cout << prompt;
        if (cin >> value){
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()){
            cout << endl;
            return 9;
        }
        cout << "Invalid input. Please enter a number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

static string readLine(const string& prompt){
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

/* ---------- menu actions ---------- */

static void createReservation(ReservationManagement& manager, Resources& resources){
    cout << "\n----- Create Reservation -----" << endl;

    int resID       = readInt("Reservation ID: ");
    int studentID   = readInt("Student ID: ");
    string name     = readLine("Student Name: ");
    string resource = readLine("Resource ID: ");

    if (!resources.HasResource(resource)){
        cout << "ERROR: Resource [" << resource << "] does not exist." << endl;
        return;
    }

    string date = readLine("Reservation Date (MM/DD/YYYY): ");

    Reservation r;
    r.ID          = resID;
    r.StudentID   = studentID;
    r.StudentName = name;
    r.ResourceID  = resource;
    r.Date        = date;

    switch (manager.CreateReservation(r)){
        case ReservationStatus::Created:
            cout << "Reservation Created Successfully." << endl;
            break;
        case ReservationStatus::Waitlisted:
            cout << "Resource is booked for that date. Added to Waiting List." << endl;
            break;
        case ReservationStatus::DuplicateID:
            cout << "ERROR: A reservation with ID " << resID << " already exists." << endl;
            break;
    }
}

static void cancelReservation(ReservationManagement& manager){
    cout << "\n----- Cancel Reservation -----" << endl;
    int id = readInt("Reservation ID: ");
    manager.CancelReservation(id);   // prints its own success / error message
}

//binary search
static void printReservation(const Reservation& r){
    cout << "[" << r.ID << "] | "
         << r.StudentName << "(" << r.StudentID << ") | "
         << r.ResourceID << " | "
         << r.Date << endl;
}

static void searchMenu(ReservationManagement& manager, Resources& resources){
    cout << "\n----- Search -----" << endl;
    cout << "1. Search Resource by ID"                  << endl;
    cout << "2. Search Reservation by ID"               << endl;
    cout << "3. Search Reservations by Student ID"      << endl;
    cout << "4. Back"                                   << endl;

    int choice = readInt("Enter Choice: ");

    switch (choice){
        case 1: {
            string id = readLine("Resource ID: ");
            Resource found;
            if (resources.SearchByID(id, found)){
                cout << "Found: " << found.ID << " | " << found.Name << " | "
                     << found.Type << " | " << found.Status << endl;
            } else {
                cout << "No resource found with ID '" << id << "'." << endl;
            }
            break;
        }
        case 2: {
            int id = readInt("Reservation ID: ");
            Reservation found;
            if (manager.SearchReservationByID(id, found)){
                cout << "Found: ";
                printReservation(found);
            } else {
                cout << "No active reservation found with ID " << id << "." << endl;
            }
            break;
        }
        case 3: {
            int studentID = readInt("Student ID: ");
            vector<Reservation> matches = manager.SearchReservationsByStudent(studentID);
            if (matches.empty()){
                cout << "No active reservations found for student " << studentID << "." << endl;
            } else {
                cout << matches.size() << " reservation(s) found:" << endl;
                for (size_t i = 0; i < matches.size(); i++){
                    printReservation(matches[i]);
                }
            }
            break;
        }
        case 4:
            break;
        default:
            cout << "Invalid choice. Please select 1-4." << endl;
    }
}

/* ---------- menu ---------- */

static void printMenu(){
    cout << "\n===== Campus Resource Reservation System =====" << endl;
    cout << "1. View Resources"            << endl;
    cout << "2. Create Reservation"        << endl;
    cout << "3. Cancel Reservation"        << endl;
    cout << "4. View Active Reservations"  << endl;
    cout << "5. View Waiting List"         << endl;
    cout << "6. View Cancellation History" << endl;
    cout << "7. Undo Cancellation"         << endl;
    cout << "8. Generate Report"           << endl;
    cout << "9. Exit"                      << endl;
    cout << "10. Search "                  << endl;
}

int main(){
    Resources resources;
    ReservationManagement manager;

    // Load the resource inventory. Path is relative to where you run the program;
    // run from the Project1/ folder so "data/resources.txt" resolves.
    if (!resources.LoadResources("data/resources.txt")){
        cout << "WARNING: Continuing with an empty resource list." << endl;
    } else {
        cout << "Loaded resources." << endl;
    }

    // Load any pre-existing reservations from file.
    int loadedReservations = manager.LoadReservations("data/reservations.txt");
    cout << "Loaded " << loadedReservations << " reservations." << endl;

    while (true){
        printMenu();
        int choice = readInt("Enter Choice: ");

        switch (choice){
            case 1: resources.DisplayResources();          break;
            case 2: createReservation(manager, resources); break;
            case 3: cancelReservation(manager);            break;
            case 4: manager.DisplayReservations();         break;
            case 5: manager.DisplayWaitlist();             break;
            case 6: manager.DisplayCancellations();        break;
            case 7: manager.UndoCancellation();            break;
            case 8: manager.GenerateReport();              break;
            case 10: searchMenu(manager, resources);       break;
            case 9:
                cout << "Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please select 1-10." << endl;
        }
    }
}
