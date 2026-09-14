#include "Resource.h"

#include <iomanip>
#include <sstream>

//Student

Student :: Student() : id(0), name("") {}
Student :: Student(int id, const std::string& name) : id(id), name(name) {}

int Student :: getId() const {return id; }
const std::string& Student::getName() const {return name; }
void Student::setId(int newId) {id = newId; }
void Student::setName(const std::string& n) {name = n; }

//Resource

Resource::Resource() : id(""), name(""), type(""), available(false) {}
Resource::Resource(const std::string& id, const std::string& name, const std::string& type, bool available)
    : id(id), name(name), type(type), available (available) {}

const std::string& Resource::getId() const {return id; }
const std::string& Resource::getName() const {return name; }
const std::string& Resource::getType() const {return type; }

bool Resource::isAvailable() const {return available; }

void Resource::setAvailable(bool value) {available = value; }

std::string Resource::availabilityText() const {return available ? "Available" : "Unavailable" ; }

std::string Resource::toDisplayString() const { std::ostringstream out;
    out << std::left
        << std::setw(8) << id
        << std::setw(24) << name
        << std::setw(24) << type
        << availabilityText();
    return out str();
}
