// MedicalRecord.cpp
#include "MedicalRecord.hpp"
#include <cstring>
#include <string>

using namespace app;

// Implementación del constructor con IDs
MedicalRecord::MedicalRecord(uint64_t id, uint64_t doctor) : Id(static_cast<int>(id)), doctorId(doctor) {
    std::strncpy(date, "XX/XX/XXXX", sizeof(date)); date[sizeof(date)-1] = '\0';
    std::strncpy(time, "XX:XX", sizeof(time)); time[sizeof(time)-1] = '\0';
}

// --- Getters y Setters ---

// Triviales
int MedicalRecord::getId() const { return Id; }
int MedicalRecord::getDoctorId() const { return static_cast<int>(doctorId); }

// Getters de buffer
std::string MedicalRecord::getDate() const { return std::string(date); }
std::string MedicalRecord::getTime() const { return std::string(time); }
std::string MedicalRecord::getDiagnosis() const { return std::string(diagnosis); }
std::string MedicalRecord::getTreatment() const { return std::string(treatment); }
std::string MedicalRecord::getMedications() const { return std::string(medications); }

// Setters de buffer
void MedicalRecord::setDate(const std::string& value) { 
    std::strncpy(date, value.c_str(), sizeof(date)-1); 
    date[sizeof(date)-1] = '\0'; 
}
void MedicalRecord::setTime(const std::string& value) { 
    std::strncpy(time, value.c_str(), sizeof(time)-1); 
    time[sizeof(time)-1] = '\0'; 
}
void MedicalRecord::setDiagnosis(const std::string& value) { 
    std::strncpy(diagnosis, value.c_str(), sizeof(diagnosis)-1); 
    diagnosis[sizeof(diagnosis)-1] = '\0'; 
}
void MedicalRecord::setTreatment(const std::string& value) { 
    std::strncpy(treatment, value.c_str(), sizeof(treatment)-1); 
    treatment[sizeof(treatment)-1] = '\0'; 
}
void MedicalRecord::setMedications(const std::string& value) { 
    std::strncpy(medications, value.c_str(), sizeof(medications)-1); 
    medications[sizeof(medications)-1] = '\0'; 
}

// Getters y Setters especiales
float MedicalRecord::getCost() const { return cost; }
void MedicalRecord::setCost(float value) { 
    cost = std::max(value, 0.0f); 
}