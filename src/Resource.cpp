#include "Resource.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>

using namespace std;

//helpers
namespace {

string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";   //line was entirely whitespace
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

string toLower(const string &s) {
    string out = s;
    for (char &c : out) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return out;
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
        bool available = (toLower(fields[3]) == "available");
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
    if (resourceList.empty()) {
        cout << "No resources loaded.\n";
        return;
    }
    for (const Resource &r : resourceList) {
        cout << r.toDisplayString() << "\n";
    }
}

//Linear search across the inventory
Resource *Resources::findResource(const string &ID) {
    for (Resource &r : resourceList) {
        if (r.getID() == ID) return &r;   // live pointer into the vector
    }
    return nullptr;
}

//Ordering rule used by the merge sort below
bool Resources::Precedes(const Resource &a, const Resource &b, const string &criteria) {
    if (criteria == "name") return a.getName() < b.getName();
    if (criteria == "type") return a.getType() < b.getType();
    return a.getID() < b.getID();
}

//Merges the two sorted halves
void Resources::Merge(int left, int mid, int right, const string &criteria) {
    vector<Resource> merged;
    merged.reserve(static_cast<size_t>(right - left + 1));

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (Precedes(resourceList[j], resourceList[i], criteria)) {
            merged.push_back(resourceList[j++]);
        } else {
            merged.push_back(resourceList[i++]);   //keeps the sort stable
        }
    }
    while (i <= mid)   merged.push_back(resourceList[i++]);
    while (j <= right) merged.push_back(resourceList[j++]);

    for (size_t k = 0; k < merged.size(); ++k) {
        resourceList[left + static_cast<int>(k)] = merged[k];
    }
}

//Merge sort: O(n log n) comparisons
void Resources::MergeSort(int left, int right, const string &criteria) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    MergeSort(left, mid, criteria);
    MergeSort(mid + 1, right, criteria);
    Merge(left, mid, right, criteria);
}

void Resources::SortList(const string &criteria) {
    string key = toLower(trim(criteria));

    if (key != "id" && key != "name" && key != "type") {
        cout << "Unknown sort criteria '" << criteria << "'.\n";
        return;
    }
    if (resourceList.size() > 1) {
        MergeSort(0, static_cast<int>(resourceList.size()) - 1, key);
    }
    cout << "Resources sorted by " << key << ".\n";
}

int Resources::getSize() const { return static_cast<int>(resourceList.size()); }
