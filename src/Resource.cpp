#include "Resource.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

// Strip surrounding whitespace and a trailing '\r' (Windows-edited files).
static string trim(string s){
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

Resources::Resources(){
    
}

// Load resources from a '|' delimited file:  ID|Name|Type|Availability
// Availability: 1 = available, 0 = unavailable. Lines starting with # are ignored.
// Returns false if the file cannot be opened.
bool Resources::LoadResources(string filename){
    ifstream file(filename);
    if (!file.is_open()){
        cout << "ERROR: Could not open resource file: " << filename << endl;
        return false;
    }

    this->ResourceList.clear();

    string line;
    int lineNo = 0;
    while (getline(file, line)){
        lineNo++;

        // skip blank lines and comments
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string id, name, type, avail;

        if (!getline(ss, id, '|') ||
            !getline(ss, name, '|') ||
            !getline(ss, type, '|') ||
            !getline(ss, avail, '|')){
            cout << "WARNING: Skipping malformed line " << lineNo << endl;
            continue;
        }

        Resource r;
        r.ID   = trim(id);
        r.Name = trim(name);
        r.Type = trim(type);

        // Accept the word "Available" (any case) as well as 1 / true.
        string a = trim(avail);
        for (char& c : a) c = (char)tolower(c);
        r.Availability = (a == "available" || a == "1" || a == "true");

        this->ResourceList.push_back(r);
    }

    file.close();
    return true;
}

// Display every resource in a simple aligned table.
void Resources::DisplayResources() const{
    cout << "========================  Campus Resources  ==========================" << endl;

    if (this->ResourceList.empty()){
        cout << "(no resources loaded)" << endl;
        cout << "======================================================================" << endl;
        return;
    }

    cout << left
         << setw(8)  << "ID"
         << setw(28) << "Name"
         << setw(24) << "Type"
         << "Availability" << endl;
    cout << "----------------------------------------------------------------------" << endl;

    for (const Resource& r : this->ResourceList){
        cout << left
             << setw(8)  << r.ID
             << setw(28) << r.Name
             << setw(24) << r.Type
             << (r.Availability ? "Available" : "Unavailable") << endl;
    }
    cout << "======================================================================" << endl;
}

// Linear search by resource ID. Returns nullptr if not found.
Resource* Resources::findResource(string ID){
    for (Resource& r : this->ResourceList){
        if (r.ID == ID){
            return &r;
        }
    }
    return nullptr;
}

// Flip a resource's availability flag. Returns false if the ID is unknown.
bool Resources::setAvailability(string ID, bool available){
    Resource* r = findResource(ID);
    if (r == nullptr) return false;
    r->Availability = true;
    return true;
}

int Resources::getCount() const{
    return (int)this->ResourceList.size();
}
