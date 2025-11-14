#include <cassert>
#include <cstring>
#include "LegacyEngine.hpp"

template<typename T> class LinkedList;

template<typename T>
class ListElement{
    friend  class LinkedList<T>;
    private:
    ListElement<T>* prev = nullptr;
    T value;
    ListElement<T>* next = nullptr;
    
    ListElement(T tvalue, ListElement<T>* previous = nullptr, ListElement<T>* next_ptr = nullptr)
    : prev(previous), value(tvalue), next(next_ptr) {}
};
template<typename T>
class LinkedList{
    int size = 0;
    ListElement<T>* start;
    ListElement<T>* end;

    ListElement<T>* nextIteration;

    private:
    ListElement<T>* getListElement(int idx) const { 
        if (idx < 0 || idx >= size) {
            assert(false && "Out of range index");
        }
    
        ListElement<T>* current = start;
        for (int i = 0; i < idx; ++i){
            current = current->next;
        }
        return current;
    }

    public:
    LinkedList() : size(0), start(nullptr), end(nullptr) {}

    ~LinkedList() {
        clear();
    }

    int get_size() const {
        return size;
    }
    
    T get(int idx) const { 
        if (idx < 0 || idx >= size) {
            assert(false && "Out of range index");
        }
    
        ListElement<T>* current = start;
        for (int i = 0; i < idx; ++i){
            current = current->next;
        }
        return current->value;
    }

    T getStart() const { 
        return start->value;
    }

    T getEnd() const { 
        return end->value;
    }

    ListElement<T>* getEndLE(){
        return end;
    }

    void remove_LE(ListElement<T>* elementPTR){
        if (elementPTR == start) {
            start = elementPTR->next;
        }
        if (elementPTR == end) {
            end = elementPTR->prev;
        }

        ListElement<T>* behind = elementPTR->prev;
        ListElement<T>* next = elementPTR->next;
        if(behind != nullptr) behind->next = next;
        if(next != nullptr) next->prev = behind;
        delete elementPTR;
        --size;//Precaucion, asume sin comprobar que el ListElement forma parte de esta lista.
    }

    void remove_at(int idx){
        if (idx < 0 || idx >= size) assert(false && "Out of range index");

        if (size == 1) {
            delete start;
            start = nullptr;
            end = nullptr;
            --size;
            return;
        }

        if (idx == 0) {
            ListElement<T>* next_node = start->next;
            delete start;
            start = next_node;
            start->prev = nullptr;
            
            --size;
            return;
        }

        if (idx == (size-1)){
            ListElement<T>* behind = end->prev;
            delete end;
            end = behind;
            end->next = nullptr;
            --size;
            return;
        }

        ListElement<T>* element = getListElement(idx);
        ListElement<T>* behind = element->prev;
        ListElement<T>* next = element->next;
        behind->next = next;
        next->prev = behind;
        
        delete element;
        --size;
    }

    void remove_by_value(const T& value){
        ListElement<T>* current = start;
        while (current != nullptr) {
            if (current->value == value) {
                remove_LE(current);
                return; // Found and removed, exit function
            }
            current = current->next;
        }
        // If execution reaches here, the element was not found (not an error if checking multiple lists)
    }


    void push_back(const T& value) {
        ListElement<T>* newElement = new ListElement<T>(value, end);

        if (start == nullptr) {
            start = newElement;
            end = newElement;
        } 
        else {
            end->next = newElement; 
            end = newElement; 
        }
        size++;
    }

    void reset_iteration(){
        nextIteration = start;
    }
    
    //Usar dentro de la condicion de un while
    // bool iterateByLE(ListElement<T>* &element){
        
    //     return (nextIteration != end)
    // }

    T get_iteration(){
        return nextIteration->value;
    }
    
    ListElement<T>* get_iteration_LE(){
        return nextIteration;
    }
    
    void continue_iteration(){
        nextIteration = nextIteration->next;
    }

    void clear() {
        ListElement<T>* current = start;
        ListElement<T>* next_node = nullptr;
    
        while (current != nullptr) {
            next_node = current->next;
            delete current;
            current = next_node;
        }
        
        start = nullptr;
        end = nullptr;
        nextIteration = nullptr;
        size = 0;
    }
};
struct Doctor;
struct Patient;
struct Appointment {
    int id;
    // int patientId;
    // int doctorId;
    Doctor *doctorPTR;
    Patient *patientPTR;
    char date[11];
    char time[6];
    char reason[150];
    char status[20];
    char notes[200];
    bool wasAttended;
};

struct Doctor {
    static int nextId;
    int id;
    char firstName[50];
    char lastName[50];
    char nationalId[20];
    char specialty[50];
    int yearsExperience;
    float consultationFee;
    char workingHours[50];
    char phone[15];
    char email[50];
    
    LinkedList<Appointment*> scheduledAppointments;
    
    bool isAvailable;
};
int Doctor::nextId = 1;
struct MedicalRecord {
    int recordId;
    char date[11];
    char time[6];
    char diagnosis[200];
    char treatment[200];
    char medications[150];
    int doctorId;
    float cost;
};

struct Patient {
    static int nextId;
    int id;
    char firstName[50];
    char lastName[50];
    char nationalId[20];
    int age;
    char gender;
    char bloodType[5];
    char phone[15];
    char address[100];
    char email[50];
    
    LinkedList<MedicalRecord*> history;
    LinkedList<Appointment*> scheduledAppointments;
    
    char allergies[500];
    char notes[500];
    
    bool isActive;
};
int Patient::nextId = 1;

struct Hospital {
    char name[100];
    char address[150];
    char phone[15];
    
    LinkedList<Patient*> patients;
    LinkedList<Doctor*> doctors;
    LinkedList<Appointment*> appointments;
};

Hospital hospital;

std::shared_ptr<NodeSQ> confirmDialog(std::function<void()> onConfirm){
    std::shared_ptr<NodeSQ> square = std::make_shared<NodeSQ>("Cuadro", COORD{300, 300}, COORD{27, 6}, Color::RED, Color::RED);
    std::shared_ptr<NodeButton> label = std::make_shared<NodeButton>("label", COORD{1, 1}, Color::BLACK, Color::RED, std::vector<std::string>{"         Seguro?         "});
    
    bool *move = new bool(false);
    COORD *offset = new COORD();
    label->setOnClick([move, offset, square](){
        *move = true;
        *offset = (square->getGlobalPosition() - Input::MousePos);
    });
    label->setProcessFunction([square, move, offset](double){
        if(*move){
            if (!Input::LClick) *move = false;
            square->setGlobalPosition(Input::MousePos + *offset);
        }
    });
    label->setAtExitFunction([move, offset](){delete move; delete offset;});


    std::shared_ptr<NodeButton> cancel = std::make_shared<NodeButton>("cancel", COORD{1, 2}, Color::BLACK, Color::RED, std::vector<std::string>{
        ".----------.",
        "| Cancelar |",
        "'----------'"});
    cancel->setOnClick([square](){square->setLocalPosition(COORD{300, 300});});
    std::shared_ptr<NodeButton> accept = std::make_shared<NodeButton>("accept", COORD{13, 2}, Color::BLACK, Color::RED, std::vector<std::string>{
        ".-----------.",
        "| Continuar |",
        "'-----------'"});

    accept->setOnClick([onConfirm, square](){square->setLocalPosition({300, 300}); onConfirm();});

    square->addChild(label);
    square->addChild(cancel);
    square->addChild(accept);

    return square;
}

std::shared_ptr<Node> createMainMenu(SceneManager &manager);
std::shared_ptr<Node> createPatientMenu(SceneManager &manager);
std::shared_ptr<Node2D> createPatientCard(SceneManager &manager, COORD position, Patient *patientPTR, ListElement<Patient*> *patientLEPTR);
std::shared_ptr<Node> createSinglePatientCardView(SceneManager &manager, Patient *patientPTR);

std::shared_ptr<Node> createDoctorMenu(SceneManager &manager);
std::shared_ptr<Node2D> createDoctorCard(SceneManager &manager, COORD position, Doctor *doctorPTR, ListElement<Doctor*> *doctorLEPTR);
std::shared_ptr<Node> createSingleDoctorCardView(SceneManager &manager, Doctor *doctorPTR);

std::shared_ptr<Node> checkAppointmentsOf(SceneManager &manager, Patient*actualPatientPTR, Doctor*actualDoctorPTR, int contextFlag);
std:: shared_ptr<Node2D> createAppoinmentCard(SceneManager &manager, COORD position, Appointment *appoinmentPTR, int contextFlag);

std::shared_ptr<Node> createMedicalRecordMenu(SceneManager &manager, Appointment *apptPtr, int contextFlag);
std::shared_ptr<Node> createMedicalRecordViewer(SceneManager &manager, Patient *patientPTR);

std::shared_ptr<Node> createMainMenu(SceneManager &manager){
    //Declarar Nodos
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");

    std::shared_ptr<Node2D> headersContainer = std::make_shared<Node2D>("headersContainer", COORD{1, 1});

    std::shared_ptr<NodePCT> hospitalName = std::make_shared<NodePCT>("name", COORD{0,0}, Color::BRIGHT_WHITE, Color::BLACK, std::vector<std::string>{
        hospital.name,
        hospital.address
    });

    std::shared_ptr<Node2D> buttonsContainer = std::make_shared<Node2D>("buttonContainer", COORD{5, 6});
    
    std::shared_ptr<NodeButton> pacientsButton = std::make_shared<NodeButton>("pacientsButton", COORD{0, 0}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "| Pacientes |",
        "'-----------'"});
    pacientsButton->setOnClick([&manager](){
        manager.changeScene(createPatientMenu(manager));
    });
    std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Doctors  |",
        "'-----------'"});
    doctorsButton->setOnClick([&manager](){
        manager.changeScene(createDoctorMenu(manager));
    });

    //Armar el arbol
    root->addChild(headersContainer);
        headersContainer->addChild(hospitalName);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(pacientsButton);
        buttonsContainer->addChild(doctorsButton);

    return root;
}

std::shared_ptr<Node> createPatientMenu(SceneManager &manager){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");
    std::shared_ptr<NodeSQ> border = std::make_shared<NodeSQ>("PatientsBorder", COORD{0, 0}, COORD{100, 40}, Color::YELLOW, Color::YELLOW);
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});

    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();

    hospital.patients.reset_iteration();
    for (int i = 0; i < hospital.patients.get_size(); i++){
        cards->push_back(createPatientCard(manager, {SHORT(i*65), 0}, hospital.patients.get_iteration(), hospital.patients.get_iteration_LE()) ) ;
        hospital.patients.continue_iteration();
    }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});
    mainMenuButton->setOnClick([&manager](){
        manager.changeScene(createMainMenu(manager));
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{21, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    
    createPatient->setOnClick([cardCont, cards, &manager](){
        Patient* newPatientPTR = new Patient();
        newPatientPTR->id = Patient::nextId++;
        hospital.patients.push_back(newPatientPTR);
        cards->push_back( createPatientCard(manager, { SHORT( cards->get_size() * 65 ), 0 } , newPatientPTR, hospital.patients.getEndLE()) );
        cardCont->addChild(cards->getEnd());
    });

    Patient **foundPatientPTR = new Patient*(nullptr);

    root->setAtExitFunction([cards, foundPatientPTR](){
        delete cards;
        delete foundPatientPTR;
    });

    std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::BLACK, std::vector<std::string>{"[ Ver Tarjeta ]"});
    viewFoundCard->setOnClick([&manager, root, foundPatientPTR](){
        if (*foundPatientPTR) {
            // Pass the found Patient pointer and the current scene (root) as the back scene
            manager.changeScene(createSinglePatientCardView(manager, *foundPatientPTR));
        }
    });

    std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 1}, std::vector<std::string>{"Busqueda:"});
    // Search input buttons (using the same Input::getLineInput pattern)
    auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 2}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por ID"});
    auto searchByNatID = std::make_shared<NodeButton>("SearchNatID", COORD{32, 3}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Cedula"});
    auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 4}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Nombre"});

    // --- Search Logic ---

    // Helper function to perform the search and update UI/state
    auto performPatientSearch = [ searchStatus, foundPatientPTR, viewFoundCard, &manager, root](const std::string& query, const std::string& type) {
        *foundPatientPTR = nullptr; // Reset previous result
        hospital.patients.reset_iteration();
        
        for (int i = 0; i < hospital.patients.get_size(); ++i) {
            Patient* currentPatient = hospital.patients.get_iteration();
            bool match = false;
            
            if (type == "ID" && std::to_string(currentPatient->id) == query) {
                match = true;
            } else if (type == "NAT_ID" && std::string(currentPatient->nationalId) == query) {
                match = true;
            } else if (type == "FNAME" && std::string(currentPatient->firstName).find(query) != std::string::npos) {
                match = true;
            }

            if (match) {
                *foundPatientPTR = currentPatient;
                searchStatus->set_text({"Busqueda: EXITO", "Encontrado: " + std::string(currentPatient->firstName) + " " + std::string(currentPatient->lastName)});
                viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
                return; 
            }
            hospital.patients.continue_iteration();
        }
        
        // If loop finishes without finding anything
        searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
        viewFoundCard->setLocalPosition(COORD{75, 100}); // Hide the view button
    };

    // Button Handlers
    searchByID->setOnClick([searchByID, performPatientSearch](){
        std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{20, 0});
        performPatientSearch(input, "ID");
    });

    searchByNatID->setOnClick([searchByNatID, performPatientSearch](){
        std::string input = Input::getLineInput(searchByNatID->getGlobalPosition() + COORD{20, 0});
        performPatientSearch(input, "NAT_ID");
    });

    searchByFName->setOnClick([searchByFName, performPatientSearch](){
        std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{20, 0});
        performPatientSearch(input, "FNAME");
    });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{1, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{1, 5}, COORD{98, 1}, Color::BRIGHT_YELLOW, Color::BLACK);
    
    slider->setProcessFunction([slider, cardCont, cards](double){
        SHORT limit = 98;
        int amount = cards->get_size() - 1;
        static bool moving;

        if (Input::MousePos.Y == slider->getGlobalPosition().Y && Input::LClickJustPressed) moving = true;
        if (!Input::LClick) moving = false;

        if (moving)
            slider->setGlobalPosition({std::max(std::min(Input::MousePos.X, SHORT(limit - 6)), (SHORT)1), slider->getGlobalPosition().Y});

        float progress = ((slider->getGlobalPosition().X -1) / float(limit));
        cardCont->setGlobalPosition({SHORT(1-progress * (amount * 65 + 40)), cardCont->getGlobalPosition().Y});
    });

    
    root->addChild(searchStatus);
    root->addChild(searchByID);
    root->addChild(searchByNatID);
    root->addChild(searchByFName);
    root->addChild(viewFoundCard);
    root->addChild(createPatient);
    root->addChild(cardCont);
    cards->reset_iteration();
    for (int i = 0; i < hospital.patients.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);
    root->addChild(border);
    root->addChild(mainMenuButton);
    return root;
}

std::shared_ptr<Node2D> createPatientCard(SceneManager &manager, COORD position, Patient *patientPTR, ListElement<Patient*> *patientLEPTR){
    Patient *localPatientPTR = new Patient(*patientPTR);

    auto root = std::make_shared<Node2D>("pivot", position);

    root->setAtExitFunction([localPatientPTR](){delete localPatientPTR;});

    auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
        "Paciente N:" + std::to_string(localPatientPTR->id),
        "Nombre: "    + std::string(localPatientPTR->firstName),
        "Apellido: "  + std::string(localPatientPTR->lastName),
        "Cedula: "    + std::string(localPatientPTR->nationalId),
        "Edad: "      + std::to_string(localPatientPTR->age),
        "Genero: "    + std::string(1, localPatientPTR->gender),
        "Tipo Sangre: "+ std::string(localPatientPTR->bloodType),
        "Telefono: "  + std::string(localPatientPTR->phone),
        "Direccion: " + std::string(localPatientPTR->address),
        "Email: "     + std::string(localPatientPTR->email),
        "Alergias: "  + std::string(localPatientPTR->allergies).substr(0, 40) + "...",
        "Notas: "     + std::string(localPatientPTR->notes).substr(0, 40) + "..."
    });

    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        "[ Guardar ]",});
    saveChanges->setOnClick([localPatientPTR, patientPTR, saveChanges](){
        // Copy all fields from the temporary copy back to the persistent structure
        strcpy(patientPTR->firstName, localPatientPTR->firstName); 
        strcpy(patientPTR->lastName, localPatientPTR->lastName);
        strcpy(patientPTR->nationalId, localPatientPTR->nationalId);
        patientPTR->age = localPatientPTR->age;
        patientPTR->gender = localPatientPTR->gender;
        strcpy(patientPTR->bloodType, localPatientPTR->bloodType);
        strcpy(patientPTR->phone, localPatientPTR->phone);
        strcpy(patientPTR->address, localPatientPTR->address);
        strcpy(patientPTR->email, localPatientPTR->email);
        strcpy(patientPTR->allergies, localPatientPTR->allergies);
        strcpy(patientPTR->notes, localPatientPTR->notes);
        
        saveChanges->setLocalPosition(COORD{42, 100});
    });

    std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([patientPTR, patientLEPTR, root](){
        // --- 1. Cleanup Appointments (CRITICAL) ---
        patientPTR->scheduledAppointments.reset_iteration();
        for (int i = 0; i < patientPTR->scheduledAppointments.get_size(); ++i) {
            Appointment* appt_to_delete = patientPTR->scheduledAppointments.get_iteration();
            
            // Remove from Doctor's scheduled list 
            if (appt_to_delete->doctorPTR) {
                appt_to_delete->doctorPTR->scheduledAppointments.remove_by_value(appt_to_delete);
            }
            
            // Remove from the Hospital's global list
            // hospital.appointments.remove_by_value(appt_to_delete);
            
            // Deleting the appointment entity itself
            delete appt_to_delete; 
            patientPTR->scheduledAppointments.continue_iteration();
        }
        
        // The Patient's scheduledAppointments list destructor will now safely call clear() 
        // without deleting the pointers again (since we deleted them above).
        
        // --- 2. Cleanup Patient Entity ---
        // Note: localPatient is deleted in exitTree always
        delete patientPTR;
        hospital.patients.remove_LE(patientLEPTR);
        root->setLocalPosition({0, 100});
    });
    
    std::shared_ptr<NodeButton> deletePatient = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_YELLOW, std::vector<std::string>{" X "});
    deletePatient->setOnClick([confirmDelete](){
        confirmDelete->setGlobalPosition(COORD{25, 8});
    });

    auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 14}, Color::BRIGHT_YELLOW, Color::BRIGHT_YELLOW);

    auto update_patient_label = [label, localPatientPTR](){
        std::vector<std::string> labelText = label->getText();
        labelText[1] = "Nombre: "      + std::string(localPatientPTR->firstName);
        labelText[2] = "Apellido: "    + std::string(localPatientPTR->lastName);
        labelText[3] = "Cedula: "    + std::string(localPatientPTR->nationalId);
        labelText[4] = "Edad: "        + std::to_string(localPatientPTR->age);
        labelText[5] = "Genero: "      + std::string(1, localPatientPTR->gender);
        labelText[6] = "Tipo Sangre: " + std::string(localPatientPTR->bloodType);
        labelText[7] = "Telefono: "    + std::string(localPatientPTR->phone);
        labelText[8] = "Direccion: "   + std::string(localPatientPTR->address);
        labelText[9] = "Email: "       + std::string(localPatientPTR->email);
        labelText[10] = "Alergias: "    + std::string(localPatientPTR->allergies).substr(0, 40) + "...";
        labelText[11] = "Notas: "      + std::string(localPatientPTR->notes).substr(0, 40) + "...";
        label->set_text(labelText);
    };

    // First Name (row 1)
    auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editName->setOnClick([localPatientPTR, editName, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });
    
    // Last Name (row 2)
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editLastname->setOnClick([localPatientPTR, editLastname, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->lastName, (Input::getLineInput(editLastname->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });
    // Last Name (row 2)
    auto editNationalID = std::make_shared<NodeButton>("editNationalID", COORD{-6, 3}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editNationalID->setOnClick([localPatientPTR, editNationalID, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->nationalId, (Input::getLineInput(editNationalID->getGlobalPosition() + COORD{15, 0})).c_str(), 20);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Age (row 3)
    auto editAge = std::make_shared<NodeButton>("editAge", COORD{-6, 4}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editAge->setOnClick([localPatientPTR, editAge, update_patient_label, saveChanges](){
        std::string input = Input::getLineInput(editAge->getGlobalPosition() + COORD{15, 0});
        try {
            localPatientPTR->age = std::stoi(input);
        } catch (...) { /* Handle conversion error silently */ }
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Gender (row 4)
    auto editGender = std::make_shared<NodeButton>("editGender", COORD{-6, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editGender->setOnClick([localPatientPTR, editGender, update_patient_label, saveChanges](){
        std::string input = Input::getLineInput(editGender->getGlobalPosition() + COORD{15, 0});
        if (!input.empty()) localPatientPTR->gender = input[0];
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Blood Type (row 5)
    auto editBlood = std::make_shared<NodeButton>("editBlood", COORD{-6, 6}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editBlood->setOnClick([localPatientPTR, editBlood, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->bloodType, (Input::getLineInput(editBlood->getGlobalPosition() + COORD{15, 0})).c_str(), 5);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });
    
    // Phone (row 6)
    auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 7}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editPhone->setOnClick([localPatientPTR, editPhone, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->phone, (Input::getLineInput(editPhone->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Address (row 7)
    auto editAddress = std::make_shared<NodeButton>("editAddress", COORD{-6, 8}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editAddress->setOnClick([localPatientPTR, editAddress, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->address, (Input::getLineInput(editAddress->getGlobalPosition() + COORD{15, 0})).c_str(), 100);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Email (row 8)
    auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editEmail->setOnClick([localPatientPTR, editEmail, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->email, (Input::getLineInput(editEmail->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    // Allergies (row 9)
    auto editAllergies = std::make_shared<NodeButton>("editAllergies", COORD{-6, 10}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editAllergies->setOnClick([localPatientPTR, editAllergies, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->allergies, (Input::getLineInput(editAllergies->getGlobalPosition() + COORD{15, 0})).c_str(), 500);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });
    
    // Notes (row 10)
    auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 11}, Color::YELLOW, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editNotes->setOnClick([localPatientPTR, editNotes, update_patient_label, saveChanges](){
        strncpy(localPatientPTR->notes, (Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0})).c_str(), 500);
        update_patient_label();
        saveChanges->setLocalPosition(COORD{42, 12});
    });

    auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
    goToAppointments->setOnClick([patientPTR, &manager](){
        manager.changeScene(checkAppointmentsOf(manager, patientPTR, nullptr, 1));
    });

    auto goToRecords = std::make_shared<NodeButton>("checkRecords", COORD{5, 12}, Color::CYAN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Ver Expediente ]"});
    goToRecords->setOnClick([patientPTR, &manager, root](){
        // The current patient menu will be the back scene
        manager.changeScene(createMedicalRecordViewer(manager, patientPTR)); 
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

    return root;
}

std::shared_ptr<Node> createSinglePatientCardView(SceneManager &manager, Patient *patientPTR){
    std::shared_ptr<Node> root = std::make_shared<Node>("SinglePatientView");

    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
    backButton->setOnClick([&manager](){
        manager.changeScene(createPatientMenu(manager));
    });

    // We need the ListElement pointers only for deletion/modification within a list, 
    // which won't happen here, so we pass nullptrs for the ListElement arguments.
    std::shared_ptr<Node2D> patientCard = createPatientCard(
        manager,
        COORD{2, 3}, 
        patientPTR,
        nullptr // No ListElement pointer needed for viewing
    );
    
    root->addChild(backButton);
    root->addChild(patientCard);
    
    return root;
}

std::shared_ptr<Node> createDoctorMenu(SceneManager &manager){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootDoctors");

    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});

    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();

    hospital.doctors.reset_iteration();
    for (int i = 0; i < hospital.doctors.get_size(); i++){
        cards->push_back(createDoctorCard(manager,{SHORT(i*65), 0}, hospital.doctors.get_iteration(), hospital.doctors.get_iteration_LE()) ) ;
        hospital.doctors.continue_iteration();
    }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});
    mainMenuButton->setOnClick([&manager](){
        manager.changeScene(createMainMenu(manager));
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createDoctoor", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    createPatient->setOnClick([cardCont, cards, &manager](){
        Doctor* newDoctorPTR = new Doctor();
        newDoctorPTR->id = Doctor::nextId++;
        hospital.doctors.push_back(newDoctorPTR);
        cards->push_back( createDoctorCard(manager, { SHORT( cards->get_size() * 65 ), 0 }, newDoctorPTR, hospital.doctors.getEndLE()) );
        cardCont->addChild(cards->getEnd());
    });

    Doctor **foundDoctorPTR = new Doctor*(nullptr); 

    root->setAtExitFunction([cards, foundDoctorPTR](){
        delete cards;
        delete foundDoctorPTR;
    });

    
    // Button to view the found doctor's card in a separate menu
    std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::BLACK, std::vector<std::string>{"[ Ver Tarjeta ]"});
    
    // Placeholder to display the search result status/name
    std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 0}, std::vector<std::string>{"Busqueda:"});
    
    // Search input buttons
    auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 1}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por ID"});
    auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 2}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Nombre"});
    auto searchBySpecialty = std::make_shared<NodeButton>("SearchSpecialty", COORD{32, 3}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Especialidad"});
    
    // --- Search Logic ---
    
    // Helper function to perform the search and update UI/state
    auto performDoctorSearch = [searchStatus, foundDoctorPTR, viewFoundCard, &manager](const std::string& query, const std::string& type) {
        *foundDoctorPTR = nullptr; // Reset previous result
        hospital.doctors.reset_iteration();
        
        for (int i = 0; i < hospital.doctors.get_size(); ++i) {
            Doctor* currentDoctor = hospital.doctors.get_iteration();
            bool match = false;
            
            if (type == "ID" && std::to_string(currentDoctor->id) == query) {
                match = true;
            } else if (type == "FNAME" && std::string(currentDoctor->firstName).find(query) != std::string::npos) {
                match = true;
            } else if (type == "SPECIALTY" && std::string(currentDoctor->specialty).find(query) != std::string::npos) {
                match = true;
            }
    
            if (match) {
                *foundDoctorPTR = currentDoctor;
                searchStatus->set_text({"Busqueda: EXITO", "Encontrado: " + std::string(currentDoctor->firstName) + " " + std::string(currentDoctor->lastName)});
                viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
                return; 
            }
            hospital.doctors.continue_iteration();
        }
        
        searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
        viewFoundCard->setLocalPosition(COORD{75, 100}); // Hide the view button
    };
    
    // Button Handlers
    searchByID->setOnClick([searchByID, performDoctorSearch](){
        std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{20, 0});
        performDoctorSearch(input, "ID");
    });
    
    searchByFName->setOnClick([searchByFName, performDoctorSearch](){
        std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{20, 0});
        performDoctorSearch(input, "FNAME");
    });
    
    searchBySpecialty->setOnClick([searchBySpecialty, performDoctorSearch](){
        std::string input = Input::getLineInput(searchBySpecialty->getGlobalPosition() + COORD{20, 0});
        performDoctorSearch(input, "SPECIALTY");
    });
    
    // --- View Found Card Logic ---
    
    viewFoundCard->setOnClick([&manager, root, foundDoctorPTR](){
        if (*foundDoctorPTR) {
            // Pass the found Doctor pointer
            manager.changeScene(createSingleDoctorCardView(manager, *foundDoctorPTR));
        }
    });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 4}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{0, 4}, COORD{100, 1}, Color::BRIGHT_CYAN, Color::BLACK);
    
    slider->setProcessFunction([slider, cardCont, cards](double){
        SHORT limit = 100 - 7;
        int amount = cards->get_size() - 1;
        static bool moving;

        if (Input::MousePos.Y == slider->getGlobalPosition().Y && Input::LClickJustPressed) moving = true;
        if (!Input::LClick) moving = false;

        if (moving)
            slider->setGlobalPosition({std::min(Input::MousePos.X, SHORT(limit)), slider->getGlobalPosition().Y});

        float progress = (slider->getGlobalPosition().X / float(limit));
        cardCont->setGlobalPosition({SHORT(-progress * (amount * 65 + 10)), cardCont->getGlobalPosition().Y});
    });

    root->addChild(searchStatus);
    root->addChild(searchByID);
    root->addChild(searchByFName);
    root->addChild(searchBySpecialty);
    root->addChild(viewFoundCard);
    root->addChild(createPatient);
    root->addChild(cardCont);
    cards->reset_iteration();
    for (int i = 0; i < hospital.doctors.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);
    root->addChild(mainMenuButton);

    return root;
}

std::shared_ptr<Node2D> createDoctorCard(SceneManager &manager, COORD position, Doctor *doctorPTR, ListElement<Doctor*> *doctorLEPTR){
    Doctor *localDoctorPTR = new Doctor(*doctorPTR);

    auto root = std::make_shared<Node2D>("pivot", position);

    root->setAtExitFunction([localDoctorPTR](){delete localDoctorPTR;});

    auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
        "Doctor N:"         + std::to_string(localDoctorPTR->id),
        "Nombre: "          + std::string(localDoctorPTR->firstName),
        "Apellido: "        + std::string(localDoctorPTR->lastName),
        "Cedula: "          + std::string(localDoctorPTR->nationalId),
        "Especialidad: "    + std::string(localDoctorPTR->specialty),
        "Experiencia(A): "  + std::to_string(localDoctorPTR->yearsExperience),
        "Tarifa: "          + std::to_string(localDoctorPTR->consultationFee),
        "Horario: "         + std::string(localDoctorPTR->workingHours),
        "Telefono: "        + std::string(localDoctorPTR->phone),
        "Email: "           + std::string(localDoctorPTR->email)
    });

    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::BLUE, Color::BRIGHT_CYAN, std::vector<std::string>{
        "[Guardar]",});
    saveChanges->setOnClick([localDoctorPTR, doctorPTR, saveChanges](){
        // Copy all fields from the temporary copy back to the persistent structure
        strcpy(doctorPTR->firstName, localDoctorPTR->firstName);
        strcpy(doctorPTR->lastName, localDoctorPTR->lastName);
        strcpy(doctorPTR->nationalId, localDoctorPTR->nationalId);
        strcpy(doctorPTR->specialty, localDoctorPTR->specialty);
        doctorPTR->yearsExperience = localDoctorPTR->yearsExperience;
        doctorPTR->consultationFee = localDoctorPTR->consultationFee;
        strcpy(doctorPTR->workingHours, localDoctorPTR->workingHours);
        strcpy(doctorPTR->phone, localDoctorPTR->phone);
        strcpy(doctorPTR->email, localDoctorPTR->email);

        saveChanges->setLocalPosition(COORD{42, 100}); // Hide save button
    });

    auto update_doctor_label = [label, localDoctorPTR](){
        std::vector<std::string> labelText = label->getText();
        labelText[1] = "Nombre: "       + std::string(localDoctorPTR->firstName);
        labelText[2] = "Apellido: "     + std::string(localDoctorPTR->lastName);
        labelText[3] = "Cedula: "       + std::string(localDoctorPTR->nationalId);
        labelText[4] = "Especialidad: " + std::string(localDoctorPTR->specialty);
        labelText[5] = "Experiencia(A): "  + std::to_string(localDoctorPTR->yearsExperience);
        labelText[6] = "Tarifa: "       + std::to_string(localDoctorPTR->consultationFee);
        labelText[7] = "Horario: "      + std::string(localDoctorPTR->workingHours);
        labelText[8] = "Telefono: "     + std::string(localDoctorPTR->phone);
        labelText[9] = "Email: "        + std::string(localDoctorPTR->email);
        label->set_text(labelText);
    };

    std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([doctorPTR, doctorLEPTR, root](){
        // --- 1. Cleanup Appointments (CRITICAL) ---
        doctorPTR->scheduledAppointments.reset_iteration();
        for (int i = 0; i < doctorPTR->scheduledAppointments.get_size(); ++i) {
            Appointment* appt_to_delete = doctorPTR->scheduledAppointments.get_iteration();
            
            // Remove from Patient's scheduled list using the new method
            if (appt_to_delete->patientPTR) {
                appt_to_delete->patientPTR->scheduledAppointments.remove_by_value(appt_to_delete);
            }
            
            // Remove from the Hospital's global list using the new method
            hospital.appointments.remove_by_value(appt_to_delete);
            
            // Deleting the appointment entity itself
            delete appt_to_delete; 
            doctorPTR->scheduledAppointments.continue_iteration();
        }
        
        // --- 2. Cleanup Doctor Entity ---
        delete doctorPTR;
        hospital.doctors.remove_LE(doctorLEPTR);
        root->setLocalPosition({0, 100});
    });
    
    std::shared_ptr<NodeButton> deleteDoctor = std::make_shared<NodeButton>("deletePatient", COORD{58, 1}, Color::RED, Color::BRIGHT_CYAN, std::vector<std::string>{" X "});
    deleteDoctor->setOnClick([confirmDelete](){
        confirmDelete->setGlobalPosition(COORD{25, 8});
    });

    auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 12}, Color::BRIGHT_CYAN, Color::BRIGHT_CYAN);
    
    // First Name (row 1)
    auto editName = std::make_shared<NodeButton>("editName", COORD{-6, 1}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editName->setOnClick([localDoctorPTR, editName, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });
    
    // Last Name (row 2)
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{-6, 2}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editLastname->setOnClick([localDoctorPTR, editLastname, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->lastName, (Input::getLineInput(editLastname->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Last Name (row 2)
    auto editNationalId = std::make_shared<NodeButton>("editNationalId", COORD{-6, 3}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editNationalId->setOnClick([localDoctorPTR, editNationalId, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->nationalId, (Input::getLineInput(editNationalId->getGlobalPosition() + COORD{15, 0})).c_str(), 20);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Specialty (row 3)
    auto editSpecialty = std::make_shared<NodeButton>("editSpecialty", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editSpecialty->setOnClick([localDoctorPTR, editSpecialty, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->specialty, (Input::getLineInput(editSpecialty->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Years Experience (row 4)
    auto editExp = std::make_shared<NodeButton>("editExp", COORD{-6, 5}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editExp->setOnClick([localDoctorPTR, editExp, update_doctor_label, saveChanges](){
        std::string input = Input::getLineInput(editExp->getGlobalPosition() + COORD{15, 0});
        try {
            localDoctorPTR->yearsExperience = std::stoi(input);
        } catch (...) { /* Handle conversion error silently */ }
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Consultation Fee (row 5)
    auto editFee = std::make_shared<NodeButton>("editFee", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editFee->setOnClick([localDoctorPTR, editFee, update_doctor_label, saveChanges](){
        std::string input = Input::getLineInput(editFee->getGlobalPosition() + COORD{15, 0});
        try {
            localDoctorPTR->consultationFee = std::stof(input);
        } catch (...) { /* Handle conversion error silently */ }
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });
    
    // Working Hours (row 6)
    auto editHours = std::make_shared<NodeButton>("editHours", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editHours->setOnClick([localDoctorPTR, editHours, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->workingHours, (Input::getLineInput(editHours->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Phone (row 7)
    auto editPhone = std::make_shared<NodeButton>("editPhone", COORD{-6, 8}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editPhone->setOnClick([localDoctorPTR, editPhone, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->phone, (Input::getLineInput(editPhone->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    // Email (row 8)
    auto editEmail = std::make_shared<NodeButton>("editEmail", COORD{-6, 9}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editEmail->setOnClick([localDoctorPTR, editEmail, update_doctor_label, saveChanges](){
        strncpy(localDoctorPTR->email, (Input::getLineInput(editEmail->getGlobalPosition() + COORD{15, 0})).c_str(), 50);
        update_doctor_label();
        saveChanges->setLocalPosition(COORD{43, 10});
    });

    auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
    goToAppointments->setOnClick([doctorPTR, &manager](){
        manager.changeScene(checkAppointmentsOf(manager, nullptr, doctorPTR, 2));
    });

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

std::shared_ptr<Node> createSingleDoctorCardView(SceneManager &manager, Doctor *doctorPTR){
    std::shared_ptr<Node> root = std::make_shared<Node>("SingleDoctorView");

    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
    backButton->setOnClick([&manager](){
        manager.changeScene(createDoctorMenu(manager));
    });

    // Pass nullptrs for the ListElement arguments since we are only viewing
    std::shared_ptr<Node2D> doctorCard = createDoctorCard(
        manager,
        {2, 3}, 
        doctorPTR, // Pass the main list for save/delete context
        nullptr // No ListElement pointer needed for viewing
    );
    
    root->addChild(backButton);
    root->addChild(doctorCard);
    
    return root;
}

std::shared_ptr<Node> checkAppointmentsOf(SceneManager &manager, Patient*actualPatientPTR, Doctor*actualDoctorPTR, int contextFlag){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootAppoinments");
    
    // 0: Global Appointments List (If both are NULL)
    // 1: Patient-Specific Appointments List
    // 2: Doctor-Specific Appointments List

    // Determine the return scene based on context
    // This allows navigating back to the Patient Menu or Doctor Menu, not just Main Menu
    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".--------.",
        "| Volver |",
        "'--------'"});
    // Determine the return scene creation based on context
    if (contextFlag == 1) {
        // Return to Patient Menu (MUST be recreated)
        backButton->setOnClick([&manager](){
            manager.changeScene(createPatientMenu(manager));
        });
    } else if (contextFlag == 2) {
        // Return to Doctor Menu (MUST be recreated)
        backButton->setOnClick([&manager](){
            manager.changeScene(createDoctorMenu(manager));
        });
    } else {
        backButton->setOnClick([&manager](){
            manager.changeScene(createMainMenu(manager));
        });
    }
        
    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{1, 6});
    
    root->setAtExitFunction([cards](){
        delete cards;
    });
    
    
    // --- PATIENT SELECTOR DIALOG SETUP ---
    std::shared_ptr<NodeSQ> patientSelector = std::make_shared<NodeSQ>("selectPatient", COORD{300, 300}, COORD{26, 40}, Color::YELLOW, Color::YELLOW);
    patientSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{"Pacientes"}));
    
    hospital.patients.reset_iteration();
    for(SHORT i = 0; i < hospital.patients.get_size(); ++i){
        Patient* patientPTR = hospital.patients.get_iteration();
        // Capture patientSelector, cardCont, cards by value/reference for the lambda
        auto patientN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(i + 1)}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{patientPTR->firstName});
        patientN->setOnClick([&manager, patientPTR, actualDoctorPTR, patientSelector, cardCont, cards, contextFlag](){
            // 1. Create the new Appointment with the selected Patient and the context Doctor
            Appointment* newAppoinmentPTR = new Appointment();
            newAppoinmentPTR->id = Patient::nextId++;
            newAppoinmentPTR->patientPTR = patientPTR;
            newAppoinmentPTR->doctorPTR = actualDoctorPTR;

            // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
            if (patientPTR) patientPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            if (actualDoctorPTR) actualDoctorPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            
            // 3. Create and add the UI card
            cards->push_back( createAppoinmentCard(manager,
                COORD{0, SHORT( cards->get_size() * 14 + 4 )}, newAppoinmentPTR, contextFlag ) );
            cardCont->addChild(cards->getEnd());
            
            // 4. Hide the selector
            patientSelector->setLocalPosition({300, 300}); 
        });
        patientSelector->addChild( patientN );
        hospital.patients.continue_iteration();
    }

    // --- DOCTOR SELECTOR DIALOG SETUP ---
    std::shared_ptr<NodeSQ> doctorSelector = std::make_shared<NodeSQ>("selectDoctor", COORD{300, 300}, COORD{26, 40}, Color::CYAN, Color::CYAN);
    doctorSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_CYAN, Color::CYAN, std::vector<std::string>{"Doctores"}));
    
    hospital.doctors.reset_iteration();
    for(SHORT i = 0; i < hospital.doctors.get_size(); ++i){
        Doctor* doctorPTR = hospital.doctors.get_iteration();
        // Capture doctorSelector, cardCont, cards by value/reference for the lambda
        auto doctorN = std::make_shared<NodeButton>("dx", COORD{(SHORT)1, (SHORT)(i + 1)}, Color::BRIGHT_CYAN, Color::CYAN, std::vector<std::string>{doctorPTR->firstName});
        doctorN->setOnClick([&manager, doctorPTR, actualPatientPTR, doctorSelector, cardCont, cards, contextFlag](){
            // 1. Create the new Appointment with the selected Doctor and the context Patient
            Appointment* newAppoinmentPTR = new Appointment();
            newAppoinmentPTR->id = Patient::nextId++;
            newAppoinmentPTR->patientPTR = actualPatientPTR;
            newAppoinmentPTR->doctorPTR = doctorPTR;

            // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
            if (actualPatientPTR) actualPatientPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            if (doctorPTR) doctorPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            
            // 3. Create and add the UI card
            cards->push_back( createAppoinmentCard(manager,
                COORD{0, SHORT( cards->get_size() * 20 + 4 )}, newAppoinmentPTR, contextFlag ) );
            cardCont->addChild(cards->getEnd());

            // 4. Hide the selector
            doctorSelector->setLocalPosition({300, 300});
        });
        doctorSelector->addChild(doctorN );
        hospital.doctors.continue_iteration();
    }

    // --- CREATE APPOINTMENT BUTTON (Nuevo) ---
    std::shared_ptr<NodeButton> createAppoinment = std::make_shared<NodeButton>("createAppoinment", COORD{20, 0}, Color::GREEN, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
        
    createAppoinment->setOnClick([patientSelector, doctorSelector, cardCont, cards, contextFlag](){
        // If we are in the Patient Menu context, we need to select a Doctor
        if (contextFlag == 1) {
            doctorSelector->setGlobalPosition(COORD{65, 0}); // Show Doctor Selector
        } 
        // If we are in the Doctor Menu context, we need to select a Patient
        else if (contextFlag == 2) {
            patientSelector->setGlobalPosition(COORD{65, 0}); // Show Patient Selector
        } 
    });

    LinkedList<Appointment*>* appointments = (contextFlag == 1 ? &(actualPatientPTR->scheduledAppointments) : &(actualDoctorPTR->scheduledAppointments));
    appointments->reset_iteration();
    for (int i = 0; i < appointments->get_size(); i++){
        Appointment* appPtr = appointments->get_iteration();
        cards->push_back(createAppoinmentCard(manager, COORD{0, SHORT(i* 14 + 4)}, appPtr, contextFlag) ) ;
        appointments->continue_iteration();
    }

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{99, 0}, Color::GREEN, Color::GREEN, std::vector<std::string>{
        " ",
        " ",
        " ",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{99, 0}, COORD{1, 40}, Color::BRIGHT_GREEN, Color::BLACK);

    slider->setProcessFunction([slider, cardCont, cards](double){
        SHORT limit = 40;
        int amount = cards->get_size() - 1;
        static bool moving;

        if (Input::MousePos.X == slider->getGlobalPosition().X && Input::LClickJustPressed) moving = true;
        if (!Input::LClick) moving = false;

        if (moving) slider->setGlobalPosition({slider->getGlobalPosition().X, std::min(Input::MousePos.Y, SHORT(limit - 3))});
        float progress = ( slider->getGlobalPosition().Y / float(limit) );
        cardCont->setGlobalPosition({cardCont->getGlobalPosition().X, SHORT( -progress * (amount * 65) )});
    });
   

    root->addChild(createAppoinment);
    root->addChild(cardCont);
    cards->reset_iteration();
    for (int i = 0; i < appointments->get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);
    root->addChild(patientSelector);
    root->addChild(doctorSelector);
    root->addChild(backButton);

    return root;
}

std:: shared_ptr<Node2D> createAppoinmentCard(SceneManager &manager, COORD position, Appointment *appoinmentPTR, int contextFlag){
    // Use the persistent appoinmentPTR directly as data is immutable for this feature
    // actualPatientPTR and actualDoctorPTR here are the assigned entity Pointers

    auto root = std::make_shared<Node2D>("pivot", position);

    // No local copy, no setAtExitFunction needed for the data pointer here

    auto label = std::make_shared<NodeUI>("label", COORD{7, 1}, std::vector<std::string>{
        "Cita N:" + std::to_string(appoinmentPTR->id),
        "Paciente: " + (appoinmentPTR->patientPTR ? std::string(appoinmentPTR->patientPTR->firstName) + " " + std::string(appoinmentPTR->patientPTR->lastName) : "N/A"),
        "Doctor: " + (appoinmentPTR->doctorPTR ? std::string(appoinmentPTR->doctorPTR->lastName) : "N/A"),
        "Fecha: "  + std::string(appoinmentPTR->date),
        "Hora: "   + std::string(appoinmentPTR->time),
        "Costo: "  + std::to_string(appoinmentPTR->doctorPTR ? appoinmentPTR->doctorPTR->consultationFee : 0.0f), // Display Doctor's fee
        "Motivo: " + std::string(appoinmentPTR->reason).substr(0, 30) + "...",
        "Notas: "  + std::string(appoinmentPTR->notes).substr(0, 30) + "..."
    });
    
    auto update_appt_label = [label, appoinmentPTR](){
        std::vector<std::string> labelText = label->getText();
        labelText[3] = "Fecha: "   + std::string(appoinmentPTR->date);
        labelText[4] = "Hora: "   + std::string(appoinmentPTR->time);
        labelText[6] = "Motivo: " + std::string(appoinmentPTR->reason).substr(0, 30) + "...";
        labelText[7] = "Notas: "  + std::string(appoinmentPTR->notes).substr(0, 30) + "...";
        label->set_text(labelText);
    };

    // Date (row 3)
    auto editDate = std::make_shared<NodeButton>("editDate", COORD{-6, 3}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editDate->setOnClick([appoinmentPTR, editDate, update_appt_label](){
        strncpy(appoinmentPTR->date, (Input::getLineInput(editDate->getGlobalPosition() + COORD{15, 0})).c_str(), 15);
        update_appt_label();
    });

    // Time (row 4)
    auto editTime = std::make_shared<NodeButton>("editTime", COORD{-6, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editTime->setOnClick([appoinmentPTR, editTime, update_appt_label](){
        strncpy(appoinmentPTR->time, (Input::getLineInput(editTime->getGlobalPosition() + COORD{15, 0})).c_str(), 6);
        update_appt_label();
    });

    // Reason (row 6)
    auto editReason = std::make_shared<NodeButton>("editReason", COORD{-6, 6}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editReason->setOnClick([appoinmentPTR, editReason, update_appt_label](){
        strncpy(appoinmentPTR->reason, (Input::getLineInput(editReason->getGlobalPosition() + COORD{15, 0})).c_str(), 100);
        update_appt_label();
    });

    // Notes (row 7)
    auto editNotes = std::make_shared<NodeButton>("editNotes", COORD{-6, 7}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editNotes->setOnClick([appoinmentPTR, editNotes, update_appt_label](){
        strncpy(appoinmentPTR->notes, (Input::getLineInput(editNotes->getGlobalPosition() + COORD{15, 0})).c_str(), 200);
        update_appt_label();
    });

    // --- Delete Confirmation Dialog (REPURPOSED as ARCHIVE) ---
    std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([appoinmentPTR, root, &manager, contextFlag](){
        // Pass the appointment data and the ListElement pointer (for cleanup after saving the record)
        // The current scene (RootAppoinments) will act as the backScene
        manager.changeScene(createMedicalRecordMenu(manager, appoinmentPTR, contextFlag));
        
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

std::shared_ptr<Node> createMedicalRecordMenu(SceneManager &manager, Appointment *apptPtr, int contextFlag){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootMedicalRecord");
    
    // Create a new MedicalRecord struct to hold the data being entered
    MedicalRecord *newRecordPTR = new MedicalRecord();
    newRecordPTR->recordId = apptPtr->patientPTR->history.get_size() + 1; // Simple ID assignment
    newRecordPTR->doctorId = apptPtr->doctorPTR ? apptPtr->doctorPTR->id : 0;
    
    // Capture necessary metadata from the Appointment and Doctor
    strncpy(newRecordPTR->date, apptPtr->date, 15);
    strncpy(newRecordPTR->time, apptPtr->time, 10);
    
    // Set the cost from the Doctor's consultationFee
    newRecordPTR->cost = apptPtr->doctorPTR ? apptPtr->doctorPTR->consultationFee : 0.0f;

    std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
        "--- CREAR EXPEDIENTE MEDICO ---",
        "Paciente: " + std::string(apptPtr->patientPTR->firstName) + " " + std::string(apptPtr->patientPTR->lastName),
        "Doctor: " + (apptPtr->doctorPTR ? std::string(apptPtr->doctorPTR->lastName) : "N/A"),
        "Fecha: " + std::string(newRecordPTR->date) + " | Hora: " + std::string(newRecordPTR->time) + " | Costo: " + std::to_string(newRecordPTR->cost)
    });

    // Diagnosis Input
    auto diagButton = std::make_shared<NodeButton>("DiagBtn", COORD{1, 5}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Diagnosis: (Click to Enter) ]"});
    diagButton->setOnClick([diagButton, newRecordPTR](){
        std::string input = Input::getLineInput(diagButton->getGlobalPosition() + COORD{32, 0});
        strncpy(newRecordPTR->diagnosis, input.c_str(), 200);
        diagButton->set_text({"[ Diagnosis: " + input + " ]"});
    });
    
    // Treatment Input
    auto treatButton = std::make_shared<NodeButton>("TreatBtn", COORD{1, 7}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Treatment: (Click to Enter) ]"});
    treatButton->setOnClick([treatButton, newRecordPTR](){
        std::string input = Input::getLineInput(treatButton->getGlobalPosition() + COORD{32, 0});
        strncpy(newRecordPTR->treatment, input.c_str(), 200);
        treatButton->set_text({"[ Treatment: " + input + " ]"});
    });
    
    // Medications Input
    auto medButton = std::make_shared<NodeButton>("MedBtn", COORD{1, 9}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Medications: (Click to Enter) ]"});
    medButton->setOnClick([medButton, newRecordPTR](){
        std::string input = Input::getLineInput(medButton->getGlobalPosition() + COORD{32, 0});
        strncpy(newRecordPTR->medications, input.c_str(), 150);
        medButton->set_text({"[ Medications: " + input + " ]"});
    });

    // --- Save and Finalize Button ---
    std::shared_ptr<NodeButton> saveAndFinalize = std::make_shared<NodeButton>("SaveBtn", COORD{1, 15}, Color::BRIGHT_GREEN, Color::BLACK, std::vector<std::string>{"[ GUARDAR Y FINALIZAR CITA ]"});
    saveAndFinalize->setOnClick([&manager, newRecordPTR, apptPtr, contextFlag](){
        // 1. Archive the new MedicalRecord (add to Patient's history list)
        apptPtr->patientPTR->history.push_back(newRecordPTR);
        
        // 2. Perform Appointment Cleanup (DELETE from all lists)
        // A. Delink from Doctor's list
        if (apptPtr->doctorPTR) {
            apptPtr->doctorPTR->scheduledAppointments.remove_by_value(apptPtr);
        }
        // B. Delink from Patient's list
        if (apptPtr->patientPTR) {
            apptPtr->patientPTR->scheduledAppointments.remove_by_value(apptPtr);
        }

        // --- 3. Return to the correct scene using the flag ---
        Patient* returnPatient = nullptr;
        Doctor* returnDoctor = nullptr;

        if (contextFlag == 1) { // Patient View
            returnPatient = apptPtr->patientPTR;
        } else if (contextFlag == 2) { // Doctor View
            returnDoctor = apptPtr->doctorPTR;
        }

        // Safety Fix: Recreate the appointments scene, ensuring fresh state
        manager.changeScene(checkAppointmentsOf(manager,
            returnPatient, 
            returnDoctor,
            contextFlag  // Pass the correct list reference
        ));

        delete apptPtr;
    });

    // --- Cancel Button ---
    std::shared_ptr<NodeButton> cancel = std::make_shared<NodeButton>("CancelBtn", COORD{35, 15}, Color::RED, Color::BLACK, std::vector<std::string>{"[ CANCELAR ]"});
    cancel->setOnClick([&manager, apptPtr, root, newRecordPTR, contextFlag](){
        delete newRecordPTR;
        Patient* returnPatient = nullptr;
        Doctor* returnDoctor = nullptr;
        if (contextFlag == 1) { // Patient View
            returnPatient = apptPtr->patientPTR;
        } else if (contextFlag == 2) { // Doctor View
            returnDoctor = apptPtr->doctorPTR;
        }
        manager.changeScene(checkAppointmentsOf(manager,
            returnPatient, 
            returnDoctor, 
            contextFlag
        ));
    });
    
    root->addChild(header);
    root->addChild(diagButton);
    root->addChild(treatButton);
    root->addChild(medButton);
    root->addChild(saveAndFinalize);
    root->addChild(cancel);

    return root;
}

std::shared_ptr<Node> createMedicalRecordViewer(SceneManager &manager, Patient *patientPTR){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootRecordsViewer");
    
    std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
        "--- EXPEDIENTE MEDICO DE: " + std::string(patientPTR->firstName) + " " + std::string(patientPTR->lastName) + " ---"
    });
    
    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 3}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
    backButton->setOnClick([&manager](){
        manager.changeScene(createPatientMenu(manager));
    });
    
    std::shared_ptr<Node2D> recordCont = std::make_shared<Node2D>("RecordCont", COORD{1, 5});
    
    // Display all records
    patientPTR->history.reset_iteration();
    for (int i = 0; i < patientPTR->history.get_size(); i++){
        MedicalRecord *record = patientPTR->history.get_iteration();
        
        auto recordCard = std::make_shared<NodeUI>("RecCard" + std::to_string(i), COORD{0, SHORT(i * 7)}, std::vector<std::string>{
            "ID: " + std::to_string(record->recordId) + " | Doctor ID: " + std::to_string(record->doctorId),
            "------------------------------------------------",
            "Diagnosis: " + std::string(record->diagnosis).substr(0, 50) + "...",
            "Treatment: " + std::string(record->treatment).substr(0, 50) + "...",
            "Medications: " + std::string(record->medications).substr(0, 50) + "...",
            "------------------------------------------------"
        });
        recordCont->addChild(recordCard);
        
        patientPTR->history.continue_iteration();
    }
    
    root->addChild(header);
    root->addChild(backButton);
    root->addChild(recordCont);
    
    return root;
}

int main(){
    
    strcpy(hospital.name, "HOLAMUNDO");
    strcpy(hospital.address, "ALGUNAPARTE");

    SceneManager manager = SceneManager::getInstance();
    manager.changeScene(createMainMenu(manager));
    manager.startRunning();
}