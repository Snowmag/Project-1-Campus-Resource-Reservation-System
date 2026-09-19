#ifndef RESOURCE
#define RESOURCE

#include <string>
#include "Reservation.h"

using namespace std;

struct Student{
    int ID;
    string Name;
};

class Resources{
public:
    Resources();

    bool LoadResources(string filename);
    void DisplayResources() const;

private:
    string ResourceList;

};


#endif