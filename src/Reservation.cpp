#include "Reservation.h"

#include <iomanip>
#include <sstream>

Reservation::Reservation()
    :id(0), studentId(0), studentName(""), resourceId(""), date("") {}

Reservation::Reservation(int id, int studentId, const std::string& studentName, const std::string& resourceId, const std::string& date)
    :id(id), studentId(studentId), studentName(studentName), resourceId(resourceId), date(date) {}

int Reservation::getId() const { return id; }
int Reservation::getStudentId() const { return studentId; }

const std::string& Reservation::getStudentName() const { return studentName; }
const std::string& Reservation::getResourceId() const {return resourceId; }
const std::string& Reservation::getDate() const { return date }

std::string Reservation::toDisplayString() const
{
    std::osringstream out;
    out << std::left
        << std::setw(8) << id
        << std::setw(10) << studentId
        << std::setw(22) << studentName
        << std::setw(8) << resourceId
        << date;
    return out.str();
}
