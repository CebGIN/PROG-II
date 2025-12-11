#ifndef APPOIMENT_HPP
#define APPOINMENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/UIPrefabs.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp"
#include "../Doctor/doctor.hpp"
#include <cstdint>

namespace app{
    class Appointment {
        uint64_t id;
        uint64_t patientID;
        uint64_t doctorID;

        char date[11] = "XX/XX/XXXX";
        char time[6] = "XX:XX";
        char reason[150] = "Default";
        char status[20] = "Default";
        char notes[200] = "Default";

        std::weak_ptr<Node2D> card;

        
        public :
        
        Appointment() = default;
        Appointment(uint64_t ID, uint64_t p, uint64_t d): id(ID), patientID(p), doctorID(d) {}
        
        int getID() const {return id;}
        int getPatientID() const {return patientID;}
        int getdoctorID() const {return doctorID;}
        
        std::string getDate() const { return std::string(date); }
        void setDate(const std::string& d) {std::strncpy(date, d.c_str(), sizeof(date) - 1); date[sizeof(date) - 1] = '\0';}
        
        std::string getTime() const { return std::string(time); }
        void setTime(const std::string& t) {std::strncpy(time, t.c_str(), sizeof(time) - 1); time[sizeof(time) - 1] = '\0';}
        
        std::string getReason() const { return std::string(reason); }
        void setReason(const std::string& r) {std::strncpy(reason, r.c_str(), sizeof(reason) - 1); reason[sizeof(reason) - 1] = '\0';}
        
        std::string getStatus() const { return std::string(status); }
        void setStatus(const std::string& s) {std::strncpy(status, s.c_str(), sizeof(status) - 1); status[sizeof(status) - 1] = '\0';}
        
        std::string getNotes() const { return std::string(notes); }
        void setNotes(const std::string& n) {std::strncpy(notes, n.c_str(), sizeof(notes) - 1); notes[sizeof(notes) - 1] = '\0';}
        
        std::weak_ptr<Node2D> getCard() {return card;}
        void eraseCard(){card.reset();}
        
        bool saveAtIn(uint64_t idx, cfm::IndexedFile<Appointment> &list){
            Appointment copy = *this;
            copy.eraseCard();
            list.modifyAtIdx(idx, copy);
            // try{
            // }catch(...){
            //     return false;
            // }
            return true;
        }
        void erasefrom(cfm::IndexedFile<app::Appointment> &list) {list.eraseAtIdx(id);}
        
        std:: shared_ptr<Node2D> createCard(cfm::IndexedFile<Appointment> &list, cfm::IndexedFile<ptn::Patient> &patients, cfm::IndexedFile<doc::Doctor> &doctors, int contextFlag){
            auto root = std::make_shared<Node2D>("pivot");
            
            std::shared_ptr<ptn::Patient> patient = patients.getAtIDX(patientID);
            std::shared_ptr<doc::Doctor> doctor = doctors.getAtIDX(doctorID);

            if(!patient || !doctor) return nullptr;
            
            auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
                "Cita N:" + std::to_string(getID()),
                "Paciente: " + (patient ? std::string(patient->getFirstName()) + ", " + std::string(patient->getLastName()) : "N/A"),
                "Doctor: " + (doctor ? std::string(doctor->getLastName()) : "N/A"),
                "Fecha: "  + getDate(),
                "Hora: "   + getTime(),
                "Costo: "  + std::to_string(doctor ? doctor->getConsultationFee() : 0.0f), // Display Doctor's fee
                "Motivo: " + getReason().substr(0, 30) + "...",
                "Notas: "  + getNotes().substr(0, 30) + "..."
            });
            
            auto update_appt_label = [this, label](){
                std::vector<std::string> labelText = label->getText();
                labelText[3] = "Fecha: "   + std::string(date);
                labelText[4] = "Hora: "   + std::string(time);
                labelText[6] = "Motivo: " + std::string(reason).substr(0, 30) + "...";
                labelText[7] = "Notas: "  + std::string(notes).substr(0, 30) + "...";
                label->set_text(labelText);
            };
        
            // Date (row 3)
            auto editDate = std::make_shared<NodeButton>("editDate", COORD{-6, 3}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editDate->setOnClick([this, &list, editDate, update_appt_label](){
                setDate(Input::getLineInput(editDate->getGlobalPosition() + COORD{15, 0}));
                update_appt_label();
                saveAtIn(id, list);
            });
        
            // Time (row 4)
            auto editTime = std::make_shared<NodeButton>("editTime", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editTime->setOnClick([this, &list, editTime, update_appt_label](){
                setTime(Input::getLineInput(editTime->getGlobalPosition() + COORD{15, 0}));
                update_appt_label();
                saveAtIn(id, list);
            });
        
            // Reason (row 6)
            auto editReason = std::make_shared<NodeButton>("editReason", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editReason->setOnClick([this, &list, editReason, update_appt_label](){
                setReason(Input::getLineInput(editReason->getGlobalPosition() + COORD{15, 0}));
                update_appt_label();
                saveAtIn(id, list);
            });
        
            // Notes (row 7)
            auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editNotes->setOnClick([this, &list, editNotes, update_appt_label](){
                setNotes(Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0}));
                update_appt_label();
                saveAtIn(id, list);
            });
        
            // --- Delete Confirmation Dialog (REPURPOSED as ARCHIVE) ---
            std::shared_ptr<NodeSQ> confirmDelete = pui::confirmDialog([this, root, contextFlag](){
                // SceneManager::getInstance().changeScene(createMedicalRecordMenu(this, contextFlag));
                
                // We do NOT delete the pointers or remove from the lists here.
                // Cleanup happens AFTER the user saves the record in createMedicalRecordMenu.
            });
        
            // --- Card Structure ---
            auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_GREEN);
        
            root->addChild(square);
            root->addChild(label);
                label->addChild(editDate);
                label->addChild(editTime);
                label->addChild(editReason);
                label->addChild(editNotes);
        
            return root;
        }
    };
}
#endif