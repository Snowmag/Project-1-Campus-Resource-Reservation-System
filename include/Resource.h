#ifndef RESOURCE
#define RESOURCE

#include <string>
#include <list>

using namespace std;

struct Student{
    int ID;
    string Name;
};

struct Resource{
    string ID;
    string Name;
    string Type;
    bool Availability;
};

class Resources{
public:
    Resources();

    bool LoadResources(string filename);
    void DisplayResources() const;
    bool setAvailability(string ID, bool available);
    Resource* findResource(string ID);
    int getCount() const;

private:
    list<Resource> SortList();
    list<Resource> ResourceList;
};


#endif