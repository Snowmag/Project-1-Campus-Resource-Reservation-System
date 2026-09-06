#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <list>

using namespace std;

class Student{
    int ID;
    string Name;
};

class Resource{
    string ID;
    string Name;
    string Type;
    bool Availability;
};

class Resources{
    void LoadResources();
    void DisplayResources();
    Resource findResource(string ID);
    list<Resource> SortList();

    list<Resource> Resources;
};


#endif