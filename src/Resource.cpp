#include "Resource.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

//helpers
namespace {

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

}

//namespace

//Student
Student::Student() : ID(0), Name("") {}
Student::Student(int id, const string &name) : ID(id), Name(name) {}

int Student::getID() const { return ID; }
string Student::getName() const { return Name; }

//Resource

Resource::Resource() : ID(""), Name(""), Type(""), Availability(false) {}
Resource::Resource(const string &id, const string &name, const string &type, bool availability)
    : ID(id), Name(name), Type(type), Availability(availability) {}

string Resource::getID()           const { return ID; }
string Resource::getName()         const { return Name; }
string Resource::getType()         const { return Type; }
bool   Resource::getAvailability() const { return Availability; }
void   Resource::setAvailability(bool value) { Availability = value; }

string Resource::availabilityText() const {
    return Availability ? "Available" : "Unavailable";
}

string Resource::toDisplayString() const {
    ostringstream out;
    out << left
        << setw(8)  << ID
        << setw(24) << Name
        << setw(24) << Type
        << availabilityText();
    return out.str();
}


void Resources::LoadResources(const string &filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error: could not open resource file '" << filename << "'.\n";
        return;
    }
    string line;
    int loaded = 0;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;

        // Split on '|'
        string fields[4];
        int fieldCount = 0;
        stringstream ss(line);
        string field;
        while (fieldCount < 4 && getline(ss, field, '|')) {
            fields[fieldCount++] = trim(field);
        }
        if (fieldCount != 4) {
            cerr << "Warning: skipping malformed resource line: " << line << "\n";
            continue;
        }
        bool available = (fields[3] == "Available" || fields[3] == "available");
        resourceList.emplace_back(fields[0], fields[1], fields[2], available);
        ++loaded;
    }
    in.close();
    cout << "Loaded " << loaded << " resources from " << filename << ".\n";
}

void Resources::DisplayResources() const {
    cout << "\n----- Campus Resources (" << resourceList.size() << ") -----\n";
    cout << "ID      Name                    Type                    Status\n";
    cout << "-------------------------------------------------------------------\n";
    for (const Resource &r : resourceList) {
        cout << r.toDisplayString() << "\n";
    }
}

Resource *Resources::findResource(const string &ID) {
    for (Resource &r : resourceList) {
        if (r.getID() == ID) return &r;   // live pointer into the list
    }
    return nullptr;
}

void Resources::SortList(const string &criteria) {
    if (criteria == "id") {
        resourceList.sort([](const Resource &a, const Resource &b) {
            return a.getID() < b.getID();
        });
        cout << "Resources sorted by ID.\n";
    } else if (criteria == "name") {
        resourceList.sort([](const Resource &a, const Resource &b) {
            return a.getName() < b.getName();
        });
        cout << "Resources sorted by name.\n";
    } else if (criteria == "type") {
        resourceList.sort([](const Resource &a, const Resource &b) {
            return a.getType() < b.getType();
        });
        cout << "Resources sorted by type.\n";
    } else {
        cout << "Unknown sort criteria '" << criteria << "'.\n";
    }
}

int Resources::getSize() const { return static_cast<int>(resourceList.size()); }
