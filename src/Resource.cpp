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