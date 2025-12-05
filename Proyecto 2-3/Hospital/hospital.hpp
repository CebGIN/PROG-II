#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINFU.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp"
#include "../Doctor/doctor.hpp"
#include "../Appoinment/appoinment.hpp"
#include <cstdint>

namespace hos{
    class Hospital {
        cfm::Folder folder;
        public :
        std::string folderName;
        cfm::IndexedFile<ptn::Patient> patients;
        cfm::IndexedFile<doc::Doctor> doctors;
        cfm::IndexedFile<app::Appointment> appoinments;
        char name[100];
        char address[150];
        char phone[15];
        
        std::string getName() {return std::string(name);}

        void setName(std::string hname){strncpy(name, hname.c_str(), 99);}

        std::string getAddres(){return std::string(name);}

        void setAddres(std::string addr){strncpy(address, addr.c_str(), 149);}

        std::string getPhone(){return std::string(phone);}

        void setPhone(std::string phon){strncpy(phone, phon.c_str(), 149);}


        Hospital(std::string foldrName, std::string hosName = "Default", std::string hosAddress = "Default", std::string hosPhone = "Default") :
        folder(foldrName),
        folderName(foldrName),
        patients(folderName +"/Patients"),
        doctors(folderName + "/Doctors"),
        appoinments(folderName + "/Appoinments"){
            setName(hosName); setAddres(hosAddress); setPhone(hosPhone);
        }
    };
}

#endif