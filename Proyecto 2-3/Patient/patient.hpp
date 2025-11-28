#ifndef PATIENT_HPP
#define PATIENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include <cstdint>

class Patient{
    static uint64_t nextId;
    uint64_t id;
    char firstName[50];
    char lastName[50];
    char nationalId[20];
    int age;
    char gender;
    char bloodType[5];
    char phone[15];
    char address[100];
    char email[50];
    char allergies[500];
    char notes[500];
    
    bool isActive;

    std::weak_ptr<Node2D> card;

    // char historyRoute[20];
    // char AppoinmentsRoute[20];

    public :

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

    std::shared_ptr<Node2D> createCard(){
        auto root = std::make_shared<Node2D>("pivot", COORD{0, 0});
    
        auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
            "Paciente N:" + std::to_string(id),
            "Nombre: "    + std::string(firstName),
            "Apellido: "  + std::string(lastName),
            "Cedula: "    + std::string(nationalId),
            "Edad: "      + std::to_string(age),
            "Genero: "    + std::string(1, gender),
            "Tipo Sangre: "+ std::string(bloodType),
            "Telefono: "  + std::string(phone),
            "Direccion: " + std::string(address),
            "Email: "     + std::string(email),
            "Alergias: "  + std::string(allergies).substr(0, 40) + "...",
            "Notas: "     + std::string(notes).substr(0, 40) + "..."
        });
    
        /*
            Implementar el guardado para que en vez de copiar de un paciente local a uno que permanece, se guarde al archivo
        */
        std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
            "[ Guardar ]"});
        saveChanges->setOnClick([this, saveChanges](){

            saveChanges->setLocalPosition(COORD{300, 300});
        });
    
        /*
            Eliminar pendiente
        */

        // std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([patientPTR, patientLEPTR, root](){
            // --- 1. Cleanup Appointments (CRITICAL) ---
            // patientPTR->scheduledAppointments.rese();
            // for (int i = 0; i < patientPTR->scheduledAppointments.get_size(); ++i) {
            //     Appointment* appt_to_delete = patientPTR->scheduledAppointments.get_iteration();  
            //     // Remove from Doctor's scheduled list 
            //     if (appt_to_delete->doctorPTR) {
            //         appt_to_delete->doctorPTR->scheduledAppointments.remove_by_value(appt_to_delete);
            //     }
            //   
            //     // Remove from the Hospital's global list
            //     // hospital.appointments.remove_by_value(appt_to_delete);
            //
            //     // Deleting the appointment entity itself
            //     delete appt_to_delete; 
            //     patientPTR->scheduledAppointments.continue_iteration();
            // }
            //
            // The Patient's scheduledAppointments list destructor will now safely call clear() 
            // without deleting the pointers again (since we deleted them above).
        // });
        
        std::shared_ptr<NodeButton> deletePatient = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_YELLOW, std::vector<std::string>{" X "});
        // deletePatient->setOnClick([confirmDelete](){
        //     confirmDelete->setGlobalPosition(COORD{25, 8});
        // });
    
        std::shared_ptr<NodeSQ> square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 14}, Color::BRIGHT_YELLOW, Color::BRIGHT_YELLOW);
    
        std::function<void()> update_patient_label = [label, this](){
            std::vector<std::string> labelText = label->getText();
            labelText[1] = "Nombre: "      + std::string(firstName);
            labelText[2] = "Apellido: "    + std::string(lastName);
            labelText[3] = "Cedula: "    + std::string(nationalId);
            labelText[4] = "Edad: "        + std::to_string(age);
            labelText[5] = "Genero: "      + std::string(1, gender);
            labelText[6] = "Tipo Sangre: " + std::string(bloodType);
            labelText[7] = "Telefono: "    + std::string(phone);
            labelText[8] = "Direccion: "   + std::string(address);
            labelText[9] = "Email: "       + std::string(email);
            labelText[10] = "Alergias: "    + std::string(allergies).substr(0, 40) + "...";
            labelText[11] = "Notas: "      + std::string(notes).substr(0, 40) + "...";
            label->set_text(labelText);
        };
    
        // First Name (row 1)
        auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editName->setOnClick([this, editName, update_patient_label, saveChanges](){
            strncpy(firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
        
        // Last Name (row 2)
        auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editLastname->setOnClick([this, editLastname, update_patient_label, saveChanges](){
            strncpy(lastName, (Input::getLineInput(editLastname->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
        // National ID (row 2)
        auto editNationalID = std::make_shared<NodeButton>("editNationalID", COORD{-6, 3}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editNationalID->setOnClick([this, editNationalID, update_patient_label, saveChanges](){
            strncpy(nationalId, (Input::getLineInput(editNationalID->getGlobalPosition() + COORD{15, 0})).c_str(), 20);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Age (row 3)
        auto editAge = std::make_shared<NodeButton>("editAge", COORD{-6, 4}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editAge->setOnClick([this, editAge, update_patient_label, saveChanges](){
            std::string input = Input::getLineInput(editAge->getGlobalPosition() + COORD{15, 0});
            try {
                age = std::stoi(input);
            } catch (...) { /* Handle conversion error silently */ }
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Gender (row 4)
        auto editGender = std::make_shared<NodeButton>("editGender", COORD{-6, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editGender->setOnClick([this, editGender, update_patient_label, saveChanges](){
            std::string input = Input::getLineInput(editGender->getGlobalPosition() + COORD{15, 0});
            if (!input.empty()) gender = input[0];
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Blood Type (row 5)
        auto editBlood = std::make_shared<NodeButton>("editBlood", COORD{-6, 6}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editBlood->setOnClick([this, editBlood, update_patient_label, saveChanges](){
            strncpy(bloodType, (Input::getLineInput(editBlood->getGlobalPosition() + COORD{15, 0})).c_str(), 5);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
        
        // Phone (row 6)
        auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 7}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editPhone->setOnClick([this, editPhone, update_patient_label, saveChanges](){
            strncpy(phone, (Input::getLineInput(editPhone->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Address (row 7)
        auto editAddress = std::make_shared<NodeButton>("editAddress", COORD{-6, 8}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editAddress->setOnClick([this, editAddress, update_patient_label, saveChanges](){
            strncpy(address, (Input::getLineInput(editAddress->getGlobalPosition() + COORD{15, 0})).c_str(), 100);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Email (row 8)
        auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editEmail->setOnClick([this, editEmail, update_patient_label, saveChanges](){
            strncpy(email, (Input::getLineInput(editEmail->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        // Allergies (row 9)
        auto editAllergies = std::make_shared<NodeButton>("editAllergies", COORD{-6, 10}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editAllergies->setOnClick([this, editAllergies, update_patient_label, saveChanges](){
            strncpy(allergies, (Input::getLineInput(editAllergies->getGlobalPosition() + COORD{15, 0})).c_str(), 500);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
        
        // Notes (row 10)
        auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 11}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editNotes->setOnClick([this, editNotes, update_patient_label, saveChanges](){
            strncpy(notes, (Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0})).c_str(), 500);
            update_patient_label();
            saveChanges->setLocalPosition(COORD{42, 12});
        });
    
        auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
        goToAppointments->setOnClick([this](){
            // SceneManager::getInstance().changeScene();
        });
    
        auto goToRecords = std::make_shared<NodeButton>("checkRecords", COORD{5, 12}, Color::CYAN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Ver Expediente ]"});
        goToRecords->setOnClick([this, root](){
            // The current patient menu will be the back scene
            // SceneManager::getInstance().changeScene(createMedicalRecordViewer(this)); 
        });
    
        root->addChild(square);
        root->addChild(label);
            label->addChild(editName);
            label->addChild(editLastname);
            label->addChild(editNationalID);
            label->addChild(editAge);
            label->addChild(editGender);
            label->addChild(editBlood);
            label->addChild(editPhone);
            label->addChild(editAddress);
            label->addChild(editEmail);
            label->addChild(editAllergies);
            label->addChild(editNotes);
            label->addChild(goToAppointments);
            label->addChild(goToRecords);
        root->addChild(saveChanges);
        root->addChild(deletePatient);
        // root->addChild(confirmDelete);
    
        card = root;
        return root;
    }

};
#endif