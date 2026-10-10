#include "Resource.h"
#include "Reservation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

static string trimField(const string& s){
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return s.substr(start, end - start + 1);
}

Resources::Resources(){
    
}

// Load resources file, storing it to print it out later
//Complexity: O(n) - n is the size of the file
bool Resources::LoadResources(string filename){
    ifstream file(filename);
    if (!file.is_open()){
        cout << "ERROR: Could not open resource file: " << filename << endl;
        return false;
    }

    stringstream buf;
    buf << file.rdbuf();

    ResourceList = buf.str();

    inventory.clear();
    stringstream lines(ResourceList);
    string line;
    while (getline(lines, line)){
        if (trimField(line).empty() || line[0] == '#') continue;

        stringstream ss(line);
        string id, name, type, status;
        if (!getline(ss, id,     '|') ||
            !getline(ss, name,   '|') ||
            !getline(ss, type,   '|') ||
            !getline(ss, status, '|')){
            continue;
        }

        Resource r;
        r.ID     = trimField(id);
        r.Name   = trimField(name);
        r.Type   = trimField(type);
        r.Status = trimField(status);
        inventory.push_back(r);
    }

    file.close();
    return true;
}

// Display the file we injested earlier
//Complexity: O(n) - n is the length of ResourceList
void Resources::DisplayResources() const{
    cout << ResourceList << endl;
}

bool Resources::HasResource(const string& id) const{
    for (size_t i = 0; i < inventory.size(); ++i){
        if (inventory[i].ID == id) return true;
    }
    return false;
}

const vector<Resource>& Resources::GetAll() const{
    return inventory;
}

//Replace with merge sort (sort by ID) vvvv
void SortResourcesByID(vector<Resource>& v){
    for (size_t i = 1; i < v.size(); i++){
        Resource key = v[i];
        size_t j = i;
        while (j > 0 && v[j - 1].ID > key.ID){
            v[j] = v[j - 1];
            j--;
        }
        v[j] = key;
    }
}
//Replace with merge sort sort by ID) ^^^^

int BinarySearchResourceID(const vector<Resource>& sortedByID, const string& id){
    int low  = 0;
    int high = (int)sortedByID.size() - 1;

    while (low <= high){
        int mid = low + (high - low) / 2;

        if (sortedByID[mid].ID == id){
            return mid;
        } else if (sortedByID[mid].ID < id){
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

bool Resources::SearchByID(const string& id, Resource& found) const{
    vector<Resource> sorted = inventory;
    SortResourcesByID(sorted);

    int index = BinarySearchResourceID(sorted, id);
    if (index < 0) return false;

    found = sorted[index];
    return true;
}