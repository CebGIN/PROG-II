// Patient.cpp
#include "Patient.hpp"
#include "../../CGINLibs/UIPrefabs.hpp"
#include <cstring>

using namespace ptn;

// Implementación del constructor que toma el ID
Patient::Patient(uint64_t ID): id(ID) {}

// --- Métodos de Archivo y Persistencia ---

// Implementación de saveAtIn
bool Patient::saveAtIn(uint64_t idx, cfm::IndexedFile<Patient> &list) {
    Patient copy = *this;
    copy.eraseCard(); // Asegura que la referencia UI no se guarde en el archivo
    list.modifyAtIdx(idx, copy);
    return true;
}

// Implementación de erasefrom
void Patient::erasefrom(cfm::IndexedFile<Patient> &list) {
    list.eraseAtIdx(id);
}

// --- Getters y Setters ---

// Triviales (se pueden dejar como inline en .hpp, pero por consistencia los movemos aquí)
int Patient::getID() const { return static_cast<int>(id); }

// Getters de buffer
std::string Patient::getFirstName() const { return std::string(firstName); }
std::string Patient::getLastName() const { return std::string(lastName); }
std::string Patient::getNationalID() const { return std::string(nationalId); }
std::string Patient::getBloodType() const { return std::string(bloodType); }
std::string Patient::getPhone() const { return std::string(phone); }
std::string Patient::getAddress() const { return std::string(address); }
std::string Patient::getEmail() const { return std::string(email); }
std::string Patient::getAllergies() const { return std::string(allergies); }
std::string Patient::getNotes() const { return std::string(notes); }

// Setters de buffer (usan strncpy)
void Patient::setFirstName(const std::string& value) {
    std::strncpy(firstName, value.c_str(), sizeof(firstName) - 1); 
    firstName[sizeof(firstName) - 1] = '\0';
}
void Patient::setLastName(const std::string& value) {
    std::strncpy(lastName, value.c_str(), sizeof(lastName) - 1); 
    lastName[sizeof(lastName) - 1] = '\0';
}
void Patient::setNationalID(const std::string& value) {
    std::strncpy(nationalId, value.c_str(), sizeof(nationalId) - 1); 
    nationalId[sizeof(nationalId) - 1] = '\0';
}
void Patient::setBloodType(const std::string& value) {
    std::strncpy(bloodType, value.c_str(), sizeof(bloodType) - 1);
    bloodType[sizeof(bloodType) - 1] = '\0';
}
void Patient::setPhone(const std::string& value) {
    std::strncpy(phone, value.c_str(), sizeof(phone) - 1);
    phone[sizeof(phone) - 1] = '\0';
}
void Patient::setAddress(const std::string& value) {
    std::strncpy(address, value.c_str(), sizeof(address) - 1); 
    address[sizeof(address) - 1] = '\0';
}
void Patient::setEmail(const std::string& value) {
    std::strncpy(email, value.c_str(), sizeof(email) - 1); 
    email[sizeof(email) - 1] = '\0';
}
void Patient::setAllergies(const std::string& value) {
    std::strncpy(allergies, value.c_str(), sizeof(allergies) - 1); 
    allergies[sizeof(allergies) - 1] = '\0';
}
void Patient::setNotes(const std::string& value) {
    std::strncpy(notes, value.c_str(), sizeof(notes) - 1); 
    notes[sizeof(notes) - 1] = '\0';
}

// Getters y Setters especiales
int Patient::getAge() const { return static_cast<int>(age); }
bool Patient::setAge(uint8_t value) {
    age = std::min(value, uint8_t(150));
    return value <= uint8_t(150);
}

char Patient::getGender() const { return gender; }
void Patient::setGender(char n) {
    gender = (n == 'f' || n == 'F' || n == 'm' || n == 'M') ? n : '?';
}

// --- Métodos de Interfaz de Usuario (UI) ---

std::weak_ptr<Node2D> Patient::getCard() { return card; }
void Patient::eraseCard() { card.reset(); }

// Implementación de createCard (Toda la lógica de la UI)
std::shared_ptr<Node2D> Patient::createCard(cfm::IndexedFile<Patient> &list) {
    // Definiciones de la UI... (Se mantiene el cuerpo original aquí)
    auto root = std::make_shared<Node2D>("pivot", COORD{0, 0});
    std::shared_ptr<NodeBox> square = std::make_shared<NodeBox>("Cuadro", COORD{0, 0}, COORD{64, 13}, Color::BRIGHT_YELLOW);
    
    auto label = std::make_shared<NodePCT>("label", COORD{7, 0}, Color::BLACK, Color::BRIGHT_YELLOW, std::vector<std::string>{
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

    // Botón de Guardar
    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{300, 300}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Guardar ]"});
    saveChanges->setOnClick([this, &list, saveChanges](){
        this->saveAtIn(id, list);
        saveChanges->setLocalPosition(COORD{300, 300});
    });

    // Diálogo de Confirmación de Eliminación
    std::shared_ptr<NodeSQ> confirmDelete = pui::confirmDialog([this, &list](){
        this->erasefrom(list);
        card.lock()->setGlobalPosition(COORD{300, 300});
    });
    
    // Botón de Eliminar
    std::shared_ptr<NodeButton> deletePatient = std::make_shared<NodeButton>("deletePatient", COORD{60, 1}, Color::RED, Color::BRIGHT_YELLOW, std::vector<std::string>{" X "});
    deletePatient->setOnClick([confirmDelete](){
        confirmDelete->setGlobalPosition(COORD{25, 7});
    });

    // Función de actualización de la etiqueta (Label)
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

    // Creación de botones de edición (con sus lambdas de lógica)
    
    // First Name (row 1)
    auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editName->setOnClick([this, editName, update_patient_label, saveChanges](){
        setFirstName(Input::getLineInput(editName->getGlobalPosition() + COORD{14, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Last Name (row 2)
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editLastname->setOnClick([this, editLastname, update_patient_label, saveChanges](){
        setLastName(Input::getLineInput(editLastname->getGlobalPosition() + COORD{16, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // National ID (row 3)
    auto editNationalID = std::make_shared<NodeButton>("editNationalID", COORD{-6, 3}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editNationalID->setOnClick([this, editNationalID, update_patient_label, saveChanges](){
        setNationalID(Input::getLineInput(editNationalID->getGlobalPosition() + COORD{14, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Age (row 4)
    auto editAge = std::make_shared<NodeButton>("editAge", COORD{-6, 4}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editAge->setOnClick([this, editAge, update_patient_label, saveChanges](){
        std::string input = Input::getLineInput(editAge->getGlobalPosition() + COORD{12, 0});
        try {
            setAge(static_cast<uint8_t>(std::stoi(input)));
        } catch (...) { /* Manejar error de conversión silenciosamente */ }
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Gender (row 5)
    auto editGender = std::make_shared<NodeButton>("editGender", COORD{-6, 5}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editGender->setOnClick([this, editGender, update_patient_label, saveChanges](){
        std::string input = Input::getLineInput(editGender->getGlobalPosition() + COORD{14, 0});
        if (!input.empty()) setGender(input[0]);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Blood Type (row 6)
    auto editBlood = std::make_shared<NodeButton>("editBlood", COORD{-6, 6}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editBlood->setOnClick([this, editBlood, update_patient_label, saveChanges](){
        setBloodType(Input::getLineInput(editBlood->getGlobalPosition() + COORD{19, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Phone (row 7)
    auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 7}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editPhone->setOnClick([this, editPhone, update_patient_label, saveChanges](){
        setPhone(Input::getLineInput(editPhone->getGlobalPosition() + COORD{16, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Address (row 8)
    auto editAddress = std::make_shared<NodeButton>("editAddress", COORD{-6, 8}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editAddress->setOnClick([this, editAddress, update_patient_label, saveChanges](){
        setAddress(Input::getLineInput(editAddress->getGlobalPosition() + COORD{17, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Email (row 9)
    auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editEmail->setOnClick([this, editEmail, update_patient_label, saveChanges](){
        setEmail(Input::getLineInput(editEmail->getGlobalPosition() + COORD{13, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Allergies (row 10)
    auto editAllergies = std::make_shared<NodeButton>("editAllergies", COORD{-6, 10}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editAllergies->setOnClick([this, editAllergies, update_patient_label, saveChanges](){
        setAllergies(Input::getLineInput(editAllergies->getGlobalPosition() + COORD{16, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });
    
    // Notes (row 11)
    auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 11}, Color::YELLOW, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Edit]"});
    editNotes->setOnClick([this, editNotes, update_patient_label, saveChanges](){
        setNotes(Input::getLineInput(editNotes->getGlobalPosition() + COORD{13, 0}));
        update_patient_label();
        saveChanges->setLocalPosition(COORD{53, 12});
    });

    // Construcción del árbol de nodos
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
    root->addChild(saveChanges);
    root->addChild(deletePatient);
    root->addChild(confirmDelete);

    card = root;
    return root;
}