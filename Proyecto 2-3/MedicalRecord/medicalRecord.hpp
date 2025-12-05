#ifndef RECORD_HPP
#define RECORD_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/UIPrefabs.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp"
#include "../Doctor/doctor.hpp"
#include <cstdint>

namespace app{

    class MedicalRecord {
        int Id;
        char date[11];
        char time[6];
        char diagnosis[200];
        char treatment[200];
        char medications[150];
        uint64_t doctorId;
        float cost;
        public:

        MedicalRecord() = default;
        MedicalRecord(uint64_t id, uint64_t doctor) : Id(id), doctorId(doctor) {}

        int getId() const { return Id; }

        std::string getDate() const { return std::string(date); }
        void setDate(const std::string& value) { std::strncpy(date, value.c_str(), sizeof(date)-1); date[sizeof(date)-1] = '\0'; }

        std::string getTime() const { return std::string(time); }
        void setTime(const std::string& value) { std::strncpy(time, value.c_str(), sizeof(time)-1); time[sizeof(time)-1] = '\0'; }

        std::string getDiagnosis() const { return std::string(diagnosis); }
        void setDiagnosis(const std::string& value) { std::strncpy(diagnosis, value.c_str(), sizeof(diagnosis)-1); diagnosis[sizeof(diagnosis)-1] = '\0'; }

        std::string getTreatment() const { return std::string(treatment); }
        void setTreatment(const std::string& value) { std::strncpy(treatment, value.c_str(), sizeof(treatment)-1); treatment[sizeof(treatment)-1] = '\0'; }

        std::string getMedications() const { return std::string(medications); }
        void setMedications(const std::string& value) { std::strncpy(medications, value.c_str(), sizeof(medications)-1); medications[sizeof(medications)-1] = '\0'; }

        int getDoctorId() const { return static_cast<int>(doctorId); }
        // void setDoctorId(uint64_t value) { doctorId = value; }

        float getCost() const { return cost; }
        void setCost(float value) { cost = std::max(value, 0.0f); }

    };
}

#endif