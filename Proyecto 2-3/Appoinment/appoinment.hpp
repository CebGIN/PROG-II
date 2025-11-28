#ifndef APPOIMENT_HPP
#define APPOINMENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../Doctor/doctor.hpp"
#include "../Patient/patient.hpp"
#include <cstdint>


class Appointment {
    uint64_t id;
    uint64_t doctorID;
    uint64_t patientID;

    char date[11];
    char time[6];
    char reason[150];
    char status[20];
    char notes[200];

    bool wasAttended;

    public :

    std:: shared_ptr<Node2D> createAppoinmentCard(COORD position, int contextFlag){
        auto root = std::make_shared<Node2D>("pivot", position);
    
        auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
            "Cita N:" + std::to_string(id),
            // "Paciente: " + (patientPTR ? std::string(patientPTR->firstName) + " " + std::string(patientPTR->lastName) : "N/A"),
            // "Doctor: " + (doctorPTR ? std::string(doctorPTR->lastName) : "N/A"),
            "Fecha: "  + std::string(date),
            "Hora: "   + std::string(time),
            // "Costo: "  + std::to_string(doctorPTR ? doctorPTR->consultationFee : 0.0f), // Display Doctor's fee
            "Motivo: " + std::string(reason).substr(0, 30) + "...",
            "Notas: "  + std::string(notes).substr(0, 30) + "..."
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
        editDate->setOnClick([this, editDate, update_appt_label](){
            strncpy(date, (Input::getLineInput(editDate->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
            update_appt_label();
        });
    
        // Time (row 4)
        auto editTime = std::make_shared<NodeButton>("editTime", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editTime->setOnClick([this, editTime, update_appt_label](){
            strncpy(time, (Input::getLineInput(editTime->getGlobalPosition() + COORD{15, 0})).c_str(), 6);
            update_appt_label();
        });
    
        // Reason (row 6)
        auto editReason = std::make_shared<NodeButton>("editReason", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editReason->setOnClick([this, editReason, update_appt_label](){
            strncpy(reason, (Input::getLineInput(editReason->getGlobalPosition() + COORD{15, 0})).c_str(), 100);
            update_appt_label();
        });
    
        // Notes (row 7)
        auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editNotes->setOnClick([this, editNotes, update_appt_label](){
            strncpy(notes, (Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0})).c_str(), 200);
            update_appt_label();
        });
    
        // --- Delete Confirmation Dialog (REPURPOSED as ARCHIVE) ---
        std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([this, root, &contextFlag](){
            // Pass the appointment data and the ListElement pointer (for cleanup after saving the record)
            // The current scene (RootAppoinments) will act as the backScene
            SceneManager::getInstance().changeScene(createMedicalRecordMenu(this, contextFlag));
            
            // We do NOT delete the pointers or remove from the lists here.
            // Cleanup happens AFTER the user saves the record in createMedicalRecordMenu.
        });
        
        std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{50, 6}, Color::GREEN, Color::BLACK, std::vector<std::string>{
            "[ Archivar ]"
        });
    
        // Update the position of the dialog call
        finalizeAppointment->setOnClick([confirmDelete](){
            confirmDelete->setGlobalPosition(COORD{25, 8});
        });
    
        // --- Card Structure ---
        auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_GREEN);
    
        root->addChild(square);
        root->addChild(label);
            label->addChild(editDate);
            label->addChild(editTime);
            label->addChild(editReason);
            label->addChild(editNotes);
        root->addChild(finalizeAppointment);
        root->addChild(confirmDelete);
    
        return root;
    }std:: shared_ptr<Node2D> createAppoinmentCard(COORD position, Appointment *appoinmentPTR, int contextFlag){
        auto root = std::make_shared<Node2D>("pivot", position);
    
        auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
            "Cita N:" + std::to_string(id),
            // "Paciente: " + (patientPTR ? std::string(patientPTR->firstName) + " " + std::string(patientPTR->lastName) : "N/A"),
            // "Doctor: " + (doctorPTR ? std::string(doctorPTR->lastName) : "N/A"),
            "Fecha: "  + std::string(date),
            "Hora: "   + std::string(time),
            // "Costo: "  + std::to_string(doctorPTR ? doctorPTR->consultationFee : 0.0f), // Display Doctor's fee
            "Motivo: " + std::string(reason).substr(0, 30) + "...",
            "Notas: "  + std::string(notes).substr(0, 30) + "..."
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
        editDate->setOnClick([this, editDate, update_appt_label](){
            strncpy(date, (Input::getLineInput(editDate->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
            update_appt_label();
        });
    
        // Time (row 4)
        auto editTime = std::make_shared<NodeButton>("editTime", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editTime->setOnClick([this, editTime, update_appt_label](){
            strncpy(time, (Input::getLineInput(editTime->getGlobalPosition() + COORD{15, 0})).c_str(), 6);
            update_appt_label();
        });
    
        // Reason (row 6)
        auto editReason = std::make_shared<NodeButton>("editReason", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editReason->setOnClick([this, editReason, update_appt_label](){
            strncpy(reason, (Input::getLineInput(editReason->getGlobalPosition() + COORD{15, 0})).c_str(), 100);
            update_appt_label();
        });
    
        // Notes (row 7)
        auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editNotes->setOnClick([this, editNotes, update_appt_label](){
            strncpy(notes, (Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0})).c_str(), 200);
            update_appt_label();
        });
    
        // --- Delete Confirmation Dialog (REPURPOSED as ARCHIVE) ---
        std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([appoinmentPTR, root, &contextFlag](){
            // Pass the appointment data and the ListElement pointer (for cleanup after saving the record)
            // The current scene (RootAppoinments) will act as the backScene
            SceneManager::getInstance().changeScene(createMedicalRecordMenu(appoinmentPTR, contextFlag));
            
            // We do NOT delete the pointers or remove from the lists here.
            // Cleanup happens AFTER the user saves the record in createMedicalRecordMenu.
        });
        
        std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{50, 6}, Color::GREEN, Color::BLACK, std::vector<std::string>{
            "[ Archivar ]"
        });
    
        // Update the position of the dialog call
        finalizeAppointment->setOnClick([confirmDelete](){
            confirmDelete->setGlobalPosition(COORD{25, 8});
        });
    
        // --- Card Structure ---
        auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_GREEN);
    
        root->addChild(square);
        root->addChild(label);
            label->addChild(editDate);
            label->addChild(editTime);
            label->addChild(editReason);
            label->addChild(editNotes);
        root->addChild(finalizeAppointment);
        root->addChild(confirmDelete);
    
        return root;
    }
};

#endif