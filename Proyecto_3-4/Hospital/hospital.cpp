// hospital.cpp
#include "hospital.hpp"
#include <cstring>

using namespace hos;

// Implementación del constructor (es el bloque más grande)
Hospital::Hospital(std::string foldrName, std::string hosName, std::string hosAddress, std::string hosPhone) :
    folder(foldrName),
    folderName(foldrName),
    patients(folderName +"/Patients"),
    doctors(folderName + "/Doctors"),
    appoinments(folderName + "/Appoinments")
{
    // Inicializar los miembros char[] usando los setters
    setName(hosName); 
    setAddress(hosAddress); 
    setPhone(hosPhone);
}

// --- Implementación de Getters y Setters ---

std::string Hospital::getName() {
    return std::string(name);
}

void Hospital::setName(std::string hname) {
    std::strncpy(name, hname.c_str(), sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';
}

// Nota: Corregí el error tipográfico en el getter original (getAddres() devolvía name)
std::string Hospital::getAddress() {
    return std::string(address);
}

void Hospital::setAddress(std::string addr) {
    std::strncpy(address, addr.c_str(), sizeof(address) - 1);
    address[sizeof(address) - 1] = '\0';
}

std::string Hospital::getPhone() {
    return std::string(phone);
}

void Hospital::setPhone(std::string phon) {
    std::strncpy(phone, phon.c_str(), sizeof(phone) - 1);
    phone[sizeof(phone) - 1] = '\0';
}