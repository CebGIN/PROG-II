#ifndef PATIENT_HPP
#define PATIENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../../CGINLibs/UIPrefabs.hpp"
#include <cstdint>
#include <vector>


namespace ptn{
    const std::string folder = "Hospital/Patients";
    
    class Patient {
        uint64_t id;
        char firstName[50] = "Default";
        char lastName[50] = "Default";
        char nationalId[20] = "V00.000.000";
        uint8_t age = 0;
        char gender = 'M';
        char bloodType[5] = "Df+-";
        char phone[15] = "+5800005555555";
        char address[100] = "Default";
        char email[50] = "default@dominio";
        char allergies[500] = "Default";
        char notes[500] = "Default";
        
        bool isActive;

        std::weak_ptr<Node2D> card;

        char historyRoute[20];
        char AppoinmentsRoute[20];

        public :
        Patient() = default;
        Patient(uint64_t ID): id(ID) {}

        bool saveAtIn(uint64_t idx, cfm::IndexedFile<Patient> &list){
            Patient copy = *this;
            copy.eraseCard();
            list.modifyAtIdx(idx, copy);
            // try{
            // }catch(...){
            //     return false;
            // }
            return true;
        }
        void erasefrom(cfm::IndexedFile<Patient> &list) {list.eraseAtIdx(id);}
        int getID() const {return id;}

        std::string getFirstName() const {return std::string(firstName);}
        void setFirstName(std::string value) {strncpy(firstName, value.c_str(), 49);}

        std::string getLastName() const {return std::string(lastName);}
        void setLastName(std::string value) {strncpy(lastName, value.c_str(), 49);}

        std::string getNationalID() const {return std::string(nationalId);}
        void setNationalID(std::string value) {strncpy(nationalId, value.c_str(), 19);}

        int getAge() const {return static_cast<int>(age);}
        bool setAge(uint8_t value) {age = std::min(value, uint8_t(150)); return value <= uint8_t(150);}
        
        char getGender() const {return gender;}
        void setGender(char n) {((n == 'f' || n == 'F' || n == 'm' || n == 'M') ? gender = n : gender = '?');}

        std::string getBloodType() const {return std::string(bloodType);}
        void setBloodType(std::string value) {strncpy(bloodType, value.c_str(), 4);}

        std::string getPhone() const {return std::string(phone);}
        void setPhone(std::string value) {strncpy(phone, value.c_str(), 14);}

        std::string getAddress() const {return std::string(address);}
        void setAddress(std::string value) {strncpy(address, value.c_str(), 99);}

        std::string getEmail() const {return std::string(email);}
        void setEmail(std::string value) {strncpy(email, value.c_str(), 49);}

        std::string getAllergies() const {return std::string(allergies);}
        void setAllergies(std::string value) {strncpy(allergies, value.c_str(), 499);}

        std::string getNotes() const {return std::string(notes);}
        void setNotes(std::string value) {strncpy(notes, value.c_str(), 99);}

        std::weak_ptr<Node2D> getCard() {return card;}
        void eraseCard(){card.reset();}

        std::shared_ptr<Node2D> createCard(cfm::IndexedFile<ptn::Patient> &list){
            auto root = std::make_shared<Node2D>("pivot", COORD{0, 0});
            
            auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
                "Paciente N:"   + std::to_string(id + 1),
                "Nombre: "      + getFirstName(),
                "Apellido: "    + getLastName(),
                "Cedula: "      + getNationalID(),
                "Edad: "        + std::to_string(getAge()),
                "Genero: "      + std::string(1, getGender()),
                "Tipo Sangre: " + getBloodType(),
                "Telefono: "    + getPhone(),
                "Direccion: "   + getAddress(),
                "Email: "       + getEmail(),
                "Alergias: "    + getAllergies().substr(0, 40) + "...",
                "Notas: "       + getNotes().substr(0, 40) + "..."
            });
        
            std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
                "[ Guardar ]"});
            saveChanges->setOnClick([this, &list, saveChanges](){
                this->saveAtIn(id, list);
                saveChanges->setLocalPosition(COORD{300, 300});
            });

            std::shared_ptr<NodeSQ> confirmDelete = pui::confirmDialog([this, &list](){
                this->erasefrom(list);
                card.lock()->setGlobalPosition(COORD{300, 300});
                //Falta logica para eliminar los appoinments
            });
            
            std::shared_ptr<NodeButton> deletePatient = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_YELLOW, std::vector<std::string>{" X "});
            deletePatient->setOnClick([confirmDelete](){
                confirmDelete->setGlobalPosition(COORD{25, 8});
            });
        
            std::shared_ptr<NodeSQ> square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{64, 14}, Color::BRIGHT_YELLOW, Color::BRIGHT_YELLOW);
        
            std::function<void()> update_patient_label = [this, label](){
                std::vector<std::string> labelText = label->getText();
                labelText[1] = "Nombre: "      + getFirstName();
                labelText[2] = "Apellido: "    + getLastName();
                labelText[3] = "Cedula: "      + getNationalID();
                labelText[4] = "Edad: "        + std::to_string(getAge());
                labelText[5] = "Genero: "      + std::string(1, getGender());
                labelText[6] = "Tipo Sangre: " + getBloodType();
                labelText[7] = "Telefono: "    + getPhone();
                labelText[8] = "Direccion: "   + getAddress();
                labelText[9] = "Email: "       + getEmail();
                labelText[10] = "Alergias: "   + getAllergies().substr(0, 40) + "...";
                labelText[11] = "Notas: "      + getNotes().substr(0, 40) + "...";
                label->set_text(labelText);
            };
        
            // First Name (row 1)
            auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editName->setOnClick([this, editName, update_patient_label, saveChanges](){
                setFirstName(Input::getLineInput(editName->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
            
            // Last Name (row 2)
            auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editLastname->setOnClick([this, editLastname, update_patient_label, saveChanges](){
                setLastName(Input::getLineInput(editLastname->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
            // National ID (row 2)
            auto editNationalID = std::make_shared<NodeButton>("editNationalID", COORD{-6, 3}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editNationalID->setOnClick([this, editNationalID, update_patient_label, saveChanges](){
                setNationalID(Input::getLineInput(editNationalID->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Age (row 3)
            auto editAge = std::make_shared<NodeButton>("editAge", COORD{-6, 4}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editAge->setOnClick([this, editAge, update_patient_label, saveChanges](){
                std::string input = Input::getLineInput(editAge->getGlobalPosition() + COORD{15, 0});
                try {
                    setAge(static_cast<uint8_t>(std::stoi(input)));
                } catch (...) { /* Handle conversion error silently */ }
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Gender (row 4)
            auto editGender = std::make_shared<NodeButton>("editGender", COORD{-6, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editGender->setOnClick([this, editGender, update_patient_label, saveChanges](){
                std::string input = Input::getLineInput(editGender->getGlobalPosition() + COORD{15, 0});
                if (!input.empty()) setGender(input[0]);
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Blood Type (row 5)
            auto editBlood = std::make_shared<NodeButton>("editBlood", COORD{-6, 6}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editBlood->setOnClick([this, editBlood, update_patient_label, saveChanges](){
                setBloodType(Input::getLineInput(editBlood->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
            
            // Phone (row 6)
            auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 7}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editPhone->setOnClick([this, editPhone, update_patient_label, saveChanges](){
                setPhone(Input::getLineInput(editPhone->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Address (row 7)
            auto editAddress = std::make_shared<NodeButton>("editAddress", COORD{-6, 8}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editAddress->setOnClick([this, editAddress, update_patient_label, saveChanges](){
                setAddress(Input::getLineInput(editAddress->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Email (row 8)
            auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editEmail->setOnClick([this, editEmail, update_patient_label, saveChanges](){
                setEmail(Input::getLineInput(editEmail->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
        
            // Allergies (row 9)
            auto editAllergies = std::make_shared<NodeButton>("editAllergies", COORD{-6, 10}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editAllergies->setOnClick([this, editAllergies, update_patient_label, saveChanges](){
                setAllergies(Input::getLineInput(editAllergies->getGlobalPosition() + COORD{15, 0}));
                update_patient_label();
                saveChanges->setLocalPosition(COORD{42, 12});
            });
            
            // Notes (row 10)
            auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 11}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
            editNotes->setOnClick([this, editNotes, update_patient_label, saveChanges](){
                setNotes(Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0}));
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
            root->addChild(confirmDelete);
        
            card = root;
            return root;
        }

    };
    

}
#endif