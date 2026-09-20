#include "Resource.h"
#include "Reservation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

using namespace std;

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

    file.close();
    return true;
}

// Display the file we injested earlier
//Complexity: O(n) - n is the length of ResourceList
void Resources::DisplayResources() const{
    cout << ResourceList << endl;
}
