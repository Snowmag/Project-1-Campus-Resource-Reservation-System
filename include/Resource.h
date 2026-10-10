#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <vector>
#include "Reservation.h"

using namespace std;

struct Student{
    int ID;
    string Name;
};

struct Resource{
    string ID;
    string Name;
    string Type;
    string Status;
};

void SortResourcesByID(vector<Resource>& v);

int BinarySearchResourceID(const vector<Resource>& sortedByID, const string& id);

class Resources{
public:
    Resources();

    bool LoadResources(string filename);
    void DisplayResources() const;

    bool HasResource(const string& id) const;
    const vector<Resource>& GetAll() const;

    bool SearchByID(const string& id, Resource& found) const;

private:
    string ResourceList;
    vector<Resource> inventory;

};


#endif