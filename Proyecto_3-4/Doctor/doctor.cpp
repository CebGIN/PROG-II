// Doctor.cpp
#include "Doctor.hpp"

#include "../../CGINLibs/UIPrefabs.hpp"
#include <cstring>
#include <string>

using namespace doc;

// Implementación del constructor que toma el ID
Doctor::Doctor(uint64_t ID): id(ID) {}

// --- Métodos de Archivo y Persistencia ---

// Implementación de saveAtIn
bool Doctor::saveAtIn(uint64_t idx, cfm::IndexedFile<Doctor> &list){
    Doctor copy = *this;
    copy.eraseCard(); // Asegura que la referencia UI no se guarde
    list.modifyAtIdx(idx, copy);
    return true;
}

// Implementación de erasefrom
void Doctor::erasefrom(cfm::IndexedFile<Doctor> &list) {
    list.eraseAtIdx(id);
}

// --- Getters y Setters ---

// Triviales
int Doctor::getID() const { return static_cast<int>(id); }

// Getters de buffer
std::string Doctor::getFirstName() const { return std::string(firstName); }
std::string Doctor::getLastName() const { return std::string(lastName); }
std::string Doctor::getNationalID() const { return std::string(nationalId); }
std::string Doctor::getSpecialty() const { return std::string(specialty); }
std::string Doctor::getWorkingHours() const { return std::string(workingHours); }
std::string Doctor::getPhone() const { return std::string(phone); }
std::string Doctor::getEmail() const { return std::string(email); }

// Setters de buffer
void Doctor::setFirstName(const std::string& value) { 
    std::strncpy(firstName, value.c_str(), sizeof(firstName)-1); 
    firstName[sizeof(firstName)-1] = '\0'; 
}
void Doctor::setLastName(const std::string& value) { 
    std::strncpy(lastName, value.c_str(), sizeof(lastName)-1); 
    lastName[sizeof(lastName)-1] = '\0'; 
}
void Doctor::setNationalID(const std::string& value) { 
    std::strncpy(nationalId, value.c_str(), sizeof(nationalId)-1); 
    nationalId[sizeof(nationalId)-1] = '\0'; 
}
void Doctor::setSpecialty(const std::string& value) { 
    std::strncpy(specialty, value.c_str(), sizeof(specialty)-1); 
    specialty[sizeof(specialty)-1] = '\0'; 
}
void Doctor::setWorkingHours(const std::string& value) { 
    std::strncpy(workingHours, value.c_str(), sizeof(workingHours)-1); 
    workingHours[sizeof(workingHours)-1] = '\0'; 
}
void Doctor::setPhone(const std::string& value) { 
    std::strncpy(phone, value.c_str(), sizeof(phone)-1); 
    phone[sizeof(phone)-1] = '\0'; 
}
void Doctor::setEmail(const std::string& value) { 
    std::strncpy(email, value.c_str(), sizeof(email)-1); 
    email[sizeof(email)-1] = '\0'; 
}

// Getters y Setters especiales
int Doctor::getYearsExperience() const { return static_cast<int>(yearsExperience); }
bool Doctor::setYearsExperience(uint8_t value) { 
    yearsExperience = std::min(value, uint8_t(150)); 
    return value <= uint8_t(150); 
}

float Doctor::getConsultationFee() const { return consultationFee; }
bool Doctor::setConsultationFee(float value) { 
    consultationFee = std::max(value, 1.0f); 
    return value >= 1.0f; 
}

// --- Métodos de Interfaz de Usuario (UI) ---

std::weak_ptr<Node2D> Doctor::getCard() { return card; }
void Doctor::eraseCard() { card.reset(); }

// Implementación de createCard (Toda la lógica de la UI)
std::shared_ptr<Node2D> Doctor::createCard(cfm::IndexedFile<Doctor> &list){
    auto root = std::make_shared<Node2D>("pivot", COORD{0, 0});
    auto square = std::make_shared<NodeBox>("Cuadro", COORD{0, 0}, COORD{62, 11}, Color::BRIGHT_CYAN);
    
    auto label = std::make_shared<NodePCT>("label", COORD{7, 0}, Color::BLACK, Color::BRIGHT_CYAN, std::vector<std::string>{
        "Doctor N:"         + std::to_string(id + 1),
        "Nombre: "          + getFirstName(),
        "Apellido: "        + getLastName(),
        "Cedula: "          + getNationalID(),
        "Especialidad: "    + getSpecialty(),
        "Experiencia(A): "  + std::to_string(getYearsExperience()),
        "Tarifa: "          + std::to_string(getConsultationFee()),
        "Horario: "         + getWorkingHours(),
        "Telefono: "        + getPhone(),
        "Email: "           + getEmail()
    });
    
    // Botón de Guardar
    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[ Guardar ]"});
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
    std::shared_ptr<NodeButton> deleteDoctor = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_CYAN, std::vector<std::string>{" X "});
    deleteDoctor->setOnClick([confirmDelete](){
        confirmDelete->setGlobalPosition(COORD{25, 8});
    });

    // Función de actualización de la etiqueta (Label)
    std::function<void()> update_doctor_label = [this, label](){
        std::vector<std::string> labelText = label->getText();
        labelText[1] = "Nombre: "      + getFirstName();
        labelText[2] = "Apellido: "    + getLastName();
        labelText[3] = "Cedula: "      + getNationalID();
        labelText[4] = "Especialidad: " + getSpecialty();
        labelText[5] = "Experiencia(A): "  + std::to_string(getYearsExperience());
        labelText[6] = "Tarifa: "      + std::to_string(getConsultationFee());
        labelText[7] = "Horario: "     + getWorkingHours();
        labelText[8] = "Telefono: "    + getPhone();
        labelText[9] = "Email: "       + getEmail();
        label->set_text(labelText);
    };
    
    // Creación de botones de edición (con sus lambdas de lógica)
    
    // First Name (row 1)
    auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editName->setOnClick([this, editName, update_doctor_label, saveChanges](){
        setFirstName(Input::getLineInput(editName->getGlobalPosition() + COORD{14, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Last Name (row 2)
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editLastname->setOnClick([this, editLastname, update_doctor_label, saveChanges](){
        setLastName(Input::getLineInput(editLastname->getGlobalPosition() + COORD{16, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // National ID (row 3)
    auto editNationalId = std::make_shared<NodeButton>("editNationalId", COORD{-6, 3}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editNationalId->setOnClick([this, editNationalId, update_doctor_label, saveChanges](){
        setNationalID(Input::getLineInput(editNationalId->getGlobalPosition() + COORD{14, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Specialty (row 4)
    auto editSpecialty = std::make_shared<NodeButton>("editSpecialty", COORD{-6, 4}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editSpecialty->setOnClick([this, editSpecialty, update_doctor_label, saveChanges](){
        setSpecialty(Input::getLineInput(editSpecialty->getGlobalPosition() + COORD{20, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Years Experience (row 5)
    auto editExp = std::make_shared<NodeButton>("editExp", COORD{-6, 5}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editExp->setOnClick([this, editExp, update_doctor_label, saveChanges](){
        std::string input = Input::getLineInput(editExp->getGlobalPosition() + COORD{22, 0});
        try {
            setYearsExperience(static_cast<uint8_t>(std::stoi(input)));
        } catch (...) { /* Handle conversion error silently */ }
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Consultation Fee (row 6)
    auto editFee = std::make_shared<NodeButton>("editFee", COORD{-6, 6}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editFee->setOnClick([this, editFee, update_doctor_label, saveChanges](){
        std::string input = Input::getLineInput(editFee->getGlobalPosition() + COORD{14, 0});
        try {
            setConsultationFee(std::stof(input));
        } catch (...) { /* Handle conversion error silently */ }
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Working Hours (row 7)
    auto editHours = std::make_shared<NodeButton>("editHours", COORD{-6, 7}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editHours->setOnClick([this, editHours, update_doctor_label, saveChanges](){
        setWorkingHours(Input::getLineInput(editHours->getGlobalPosition() + COORD{15, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    // Phone (row 8)
    auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 8}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editPhone->setOnClick([this, editPhone, update_doctor_label, saveChanges](){
        setPhone(Input::getLineInput(editPhone->getGlobalPosition() + COORD{16, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    
    // Email (row 9)
    auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{"[Edit]"});
    editEmail->setOnClick([this, editEmail, update_doctor_label, saveChanges](){
        setEmail(Input::getLineInput(editEmail->getGlobalPosition() + COORD{13, 0}));
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{50, 10});
    });
    
    // Construcción del árbol de nodos
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
    root->addChild(saveChanges);
    root->addChild(deleteDoctor);
    root->addChild(confirmDelete);
    
    card = root;
    return root;
}