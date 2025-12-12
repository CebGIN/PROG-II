// Appointment.cpp
#include "appointment.hpp"

#include "../../CGINLibs/UIPrefabs.hpp"
#include <cstring>
#include <string>

using namespace app;

// Implementación del constructor con IDs
Appointment::Appointment(uint64_t ID, uint64_t p, uint64_t d): id(ID), patientID(p), doctorID(d) {}

// --- Getters y Setters de Datos ---

// Getters de ID
int Appointment::getID() const { return static_cast<int>(id); }
int Appointment::getPatientID() const { return static_cast<int>(patientID); }
int Appointment::getdoctorID() const { return static_cast<int>(doctorID); }

// Getters de buffer
std::string Appointment::getDate() const { return std::string(date); }
std::string Appointment::getTime() const { return std::string(time); }
std::string Appointment::getReason() const { return std::string(reason); }
std::string Appointment::getStatus() const { return std::string(status); }
std::string Appointment::getNotes() const { return std::string(notes); }

// Setters de buffer
void Appointment::setDate(const std::string& d) {
    std::strncpy(date, d.c_str(), sizeof(date) - 1); 
    date[sizeof(date) - 1] = '\0';
}
void Appointment::setTime(const std::string& t) {
    std::strncpy(time, t.c_str(), sizeof(time) - 1); 
    time[sizeof(time) - 1] = '\0';
}
void Appointment::setReason(const std::string& r) {
    std::strncpy(reason, r.c_str(), sizeof(reason) - 1); 
    reason[sizeof(reason) - 1] = '\0';
}
void Appointment::setStatus(const std::string& s) {
    std::strncpy(status, s.c_str(), sizeof(status) - 1); 
    status[sizeof(status) - 1] = '\0';
}
void Appointment::setNotes(const std::string& n) {
    std::strncpy(notes, n.c_str(), sizeof(notes) - 1); 
    notes[sizeof(notes) - 1] = '\0';
}

// --- Métodos de UI y Persistencia ---

std::weak_ptr<Node2D> Appointment::getCard() { return card; }
void Appointment::eraseCard() { card.reset(); }

// Implementación de saveAtIn
bool Appointment::saveAtIn(uint64_t idx, cfm::IndexedFile<Appointment> &list){
    Appointment copy = *this;
    copy.eraseCard();
    list.modifyAtIdx(idx, copy);
    return true;
}

// Implementación de erasefrom
void Appointment::erasefrom(cfm::IndexedFile<app::Appointment> &list) { 
    list.eraseAtIdx(id);
}

// Implementación de createCard (Toda la lógica de la UI)
std::shared_ptr<Node2D> Appointment::createCard(cfm::IndexedFile<Appointment> &list, cfm::IndexedFile<ptn::Patient> &patients, cfm::IndexedFile<doc::Doctor> &doctors, int contextFlag){
    auto root = std::make_shared<Node2D>("pivot");
    
    // 1. Obtener objetos de referencia
    // Usamos getAtIDX para obtener el paciente y el doctor por ID, esto puede devolver nullptr si no existen.
    std::shared_ptr<ptn::Patient> patient = patients.getAtIDX(patientID);
    std::shared_ptr<doc::Doctor> doctor = doctors.getAtIDX(doctorID);

    // Si paciente o doctor no existen, no podemos crear la tarjeta
    if(!patient || !doctor) return nullptr;
    
    // 2. Crear Label (PCT - Printable Character Text)
    auto label = std::make_shared<NodePCT>("label", COORD{7, 0}, Color::BLACK, Color::BRIGHT_GREEN, std::vector<std::string>{
        "Cita N:" + std::to_string(getID()),
        "Paciente: " + (patient ? std::string(patient->getFirstName()) + ", " + std::string(patient->getLastName()) : "N/A"),
        "Doctor: " + (doctor ? std::string(doctor->getLastName()) : "N/A"),
        "Fecha: "  + getDate(),
        "Hora: "   + getTime(),
        "Costo: "  + std::to_string(doctor ? doctor->getConsultationFee() : 0.0f), // Display Doctor's fee
        "Motivo: " + getReason().substr(0, 30) + "...",
        "Notas: "  + getNotes().substr(0, 30) + "..."
    });
    
    // 3. Función Lambda de actualización de la etiqueta
    auto update_appt_label = [this, label](){
        std::vector<std::string> labelText = label->getText();
        labelText[3] = "Fecha: "   + std::string(date);
        labelText[4] = "Hora: "    + std::string(time);
        // El costo no se edita aquí, ya que depende del Doctor
        labelText[6] = "Motivo: " + std::string(reason).substr(0, 30) + "...";
        labelText[7] = "Notas: "  + std::string(notes).substr(0, 30) + "...";
        label->set_text(labelText);
    };

    // 4. Botones de Edición (con lógica de guardar y actualizar)
    
    // Date (row 3)
    auto editDate = std::make_shared<NodeButton>("editDate", COORD{-6, 3}, Color::BLUE, Color::WHITE, std::vector<std::string>{"[Edit]"});
    editDate->setOnClick([this, &list, editDate, update_appt_label](){
        setDate(Input::getLineInput(editDate->getGlobalPosition() + COORD{15, 0}));
        update_appt_label();
        saveAtIn(id, list); // Guardar inmediatamente después de editar
    });
    
    // Time (row 4)
    auto editTime = std::make_shared<NodeButton>("editTime", COORD{-6, 4}, Color::BLUE, Color::WHITE, std::vector<std::string>{"[Edit]"});
    editTime->setOnClick([this, &list, editTime, update_appt_label](){
        setTime(Input::getLineInput(editTime->getGlobalPosition() + COORD{15, 0}));
        update_appt_label();
        saveAtIn(id, list);
    });
    
    // Reason (row 6)
    auto editReason = std::make_shared<NodeButton>("editReason", COORD{-6, 6}, Color::BLUE, Color::WHITE, std::vector<std::string>{"[Edit]"});
    editReason->setOnClick([this, &list, editReason, update_appt_label](){
        setReason(Input::getLineInput(editReason->getGlobalPosition() + COORD{15, 0}));
        update_appt_label();
        saveAtIn(id, list);
    });
    
    // Notes (row 7)
    auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 7}, Color::BLUE, Color::WHITE, std::vector<std::string>{"[Edit]"});
    editNotes->setOnClick([this, &list, editNotes, update_appt_label](){
        setNotes(Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0}));
        update_appt_label();
        saveAtIn(id, list);
    });
    
    // --- Card Structure ---
    auto square = std::make_shared<NodeBox>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::BRIGHT_GREEN);

    // 5. Construcción del árbol de nodos
    root->addChild(square);
    root->addChild(label);
        label->addChild(editDate);
        label->addChild(editTime);
        label->addChild(editReason);
        label->addChild(editNotes);

    card = root;
    return root;
}