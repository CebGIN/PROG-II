#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include <cstdint>

class Doctor {
    uint64_t id;
    char firstName[50];
    char lastName[50];
    char nationalId[20];
    char specialty[50];
    int yearsExperience;
    float consultationFee;
    char workingHours[50];
    char phone[15];
    char email[50];
    
    char AppoinmentsRoute[20];
    
    bool isAvailable;

    public:
    
    int getID() const {
        return id;
    }

    auto getFirstName(){
        return firstName;
    }

    auto getLastName(){
        return lastName;
    }

    auto getNationalID(){
        return nationalId;
    }

    auto getSpecialty(){
        return specialty;
    }

    std::shared_ptr<Node2D> createDoctorCard(){
    
        auto root = std::make_shared<Node2D>("pivot", COORD{0, 0});
    
        auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
            "Doctor N:"         + std::to_string(id),
            "Nombre: "          + std::string(firstName),
            "Apellido: "        + std::string(lastName),
            "Cedula: "          + std::string(nationalId),
            "Especialidad: "    + std::string(specialty),
            "Experiencia(A): "  + std::to_string(yearsExperience),
            "Tarifa: "          + std::to_string(consultationFee),
            "Horario: "         + std::string(workingHours),
            "Telefono: "        + std::string(phone),
            "Email: "           + std::string(email)
        });
    
        std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{
            "[Guardar]",});
        
        saveChanges->setOnClick([saveChanges](){
            
            saveChanges->setLocalPosition(COORD{300, 300}); // Hide save button
        });
    
        auto update_doctor_label = [this, label](){
            std::vector<std::string> labelText = label->getText();
            labelText[1] = "Nombre: "       + std::string(firstName);
            labelText[2] = "Apellido: "     + std::string(lastName);
            labelText[3] = "Cedula: "       + std::string(nationalId);
            labelText[4] = "Especialidad: " + std::string(specialty);
            labelText[5] = "Experiencia(A): "  + std::to_string(yearsExperience);
            labelText[6] = "Tarifa: "       + std::to_string(consultationFee);
            labelText[7] = "Horario: "      + std::string(workingHours);
            labelText[8] = "Telefono: "     + std::string(phone);
            labelText[9] = "Email: "        + std::string(email);
            label->set_text(labelText);
        };
    
        std::shared_ptr<NodeSQ> confirmDelete;// = confirmDialog([doctorPTR, doctorLEPTR, root](){
            // --- 1. Cleanup Appointments (CRITICAL) ---
            // doctorPTR->scheduledAppointments.reset_iteration();
            // for (int i = 0; i < doctorPTR->scheduledAppointments.get_size(); ++i) {
            //     Appointment* appt_to_delete = doctorPTR->scheduledAppointments.get_iteration();
                
            //     // Remove from Patient's scheduled list using the new method
            //     if (appt_to_delete->patientPTR) {
            //         appt_to_delete->patientPTR->scheduledAppointments.remove_by_value(appt_to_delete);
            //     }
                
            //     // Remove from the Hospital's global list using the new method
            //     hospital.appointments.remove_by_value(appt_to_delete);
                
            //     // Deleting the appointment entity itself
            //     delete appt_to_delete; 
            //     doctorPTR->scheduledAppointments.continue_iteration();
            // }
            
            // // --- 2. Cleanup Doctor Entity ---
            // delete doctorPTR;
            // hospital.doctors.remove_LE(doctorLEPTR);
        //     root->setLocalPosition({300, 300});
        // });
        
        std::shared_ptr<NodeButton> deleteDoctor = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_CYAN, std::vector<std::string>{" X "});
        deleteDoctor->setOnClick([confirmDelete](){
            confirmDelete->setGlobalPosition(COORD{25, 8});
        });
    
        auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 12}, Color::BRIGHT_CYAN, Color::BRIGHT_CYAN);
        
        // First Name (row 1)
        auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editName->setOnClick([this, editName, update_doctor_label, saveChanges](){
            strncpy(firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
        
        // Last Name (row 2)
        auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editLastname->setOnClick([this, editLastname, update_doctor_label, saveChanges](){
            strncpy(lastName, (Input::getLineInput(editLastname->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Last Name (row 2)
        auto editNationalId = std::make_shared<NodeButton>("editNationalId", COORD{-6, 3}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editNationalId->setOnClick([this, editNationalId, update_doctor_label, saveChanges](){
            strncpy(nationalId, (Input::getLineInput(editNationalId->getGlobalPosition() + COORD{15, 0})).c_str(), 20);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Specialty (row 3)
        auto editSpecialty = std::make_shared<NodeButton>("editSpecialty", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editSpecialty->setOnClick([this, editSpecialty, update_doctor_label, saveChanges](){
            strncpy(specialty, (Input::getLineInput(editSpecialty->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Years Experience (row 4)
        auto editExp = std::make_shared<NodeButton>("editExp", COORD{-6, 5}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editExp->setOnClick([this, editExp, update_doctor_label, saveChanges](){
            std::string input = Input::getLineInput(editExp->getGlobalPosition() + COORD{15, 0});
            try {
                yearsExperience = std::stoi(input);
            } catch (...) { /* Handle conversion error silently */ }
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Consultation Fee (row 5)
        auto editFee = std::make_shared<NodeButton>("editFee", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editFee->setOnClick([this, editFee, update_doctor_label, saveChanges](){
            std::string input = Input::getLineInput(editFee->getGlobalPosition() + COORD{15, 0});
            try {
                consultationFee = std::stof(input);
            } catch (...) { /* Handle conversion error silently */ }
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
        
        // Working Hours (row 6)
        auto editHours = std::make_shared<NodeButton>("editHours", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editHours->setOnClick([this, editHours, update_doctor_label, saveChanges](){
            strncpy(workingHours, (Input::getLineInput(editHours->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Phone (row 7)
        auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 8}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editPhone->setOnClick([this, editPhone, update_doctor_label, saveChanges](){
            strncpy(phone, (Input::getLineInput(editPhone->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        // Email (row 8)
        auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editEmail->setOnClick([this, editEmail, update_doctor_label, saveChanges](){
            strncpy(email, (Input::getLineInput(editEmail->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_doctor_label();
            saveChanges->setLocalPosition(COORD{43, 10});
        });
    
        auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
        // goToAppointments->setOnClick([this](){
        //     SceneManager::getInstance().changeScene(checkAppointmentsOf(nullptr, this, 2));
        // });
    
        root->addChild(square);
        root->addChild(label);
            label->addChild(editName);
            label->addChild(editLastname);
            label->addChild(editNationalId);
            label->addChild(editSpecialty);
            label->addChild(editExp);
            label->addChild(editFee);
            label->addChild(editHours);
            label->addChild(editPhone);
            label->addChild(editEmail);
            label->addChild(goToAppointments);
        root->addChild(saveChanges);
        root->addChild(deleteDoctor);
        root->addChild(confirmDelete);
    
        return root;
    }
    
    

};


#endif