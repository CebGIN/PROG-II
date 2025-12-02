#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINFU.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp"
#include <cstdint>

namespace hos{
    class Hospital {
        cfm::Folder folder;
        public :
        cfm::IndexedFile<ptn::Patient> patients;
        char name[100];
        char address[150];
        char phone[15];
        
        std::string getName(){
            return std::string(name);
        }

        void setName(std::string hname){
            strncpy(name, hname.c_str(), 99);
        }

        std::string getAddres(){
            return std::string(name);
        }

        void setAddres(std::string addr){
            strncpy(address, addr.c_str(), 149);
        }

        std::string getPhone(){
            return std::string(phone);
        }

        void setPhone(std::string phon){
            strncpy(phone, phon.c_str(), 149);
        }


        Hospital(std::string folderName, std::string hosName = "Default", std::string hosAddress = "Default", std::string hosPhone = "Default") : folder(folderName), patients(folderName +"/Patients") {
            setName(hosName); setAddres(hosAddress); setPhone(hosPhone);
        }
    };
}

#endif