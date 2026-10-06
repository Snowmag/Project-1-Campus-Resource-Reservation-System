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

class Resources{
public:
    Resources();

    bool LoadResources(string filename);
    void DisplayResources() const;

    bool HasResource(const string& id) const;
    const vector<Resource>& GetAll() const;

private:
    string ResourceList;
    vector<Resource> inventory;

};


#endif