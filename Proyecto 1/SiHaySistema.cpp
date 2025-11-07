#include<cassert>
#include<cstring>
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

struct Appointment {
    int id;
    int patientId;
    int doctorId;
    char date[11];
    char time[6];
    char reason[150];
    char status[20];
    char notes[200];
    bool wasAttended;
};

struct Doctor {
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

std::shared_ptr<NodeSQ> confirmDialog(std::function<void()> onConfirm){
    std::shared_ptr<NodeSQ> square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 100}, COORD{27, 6}, Color::RED, Color::RED);
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
    cancel->setOnClick([square](){square->setLocalPosition(COORD{0, 100});});
    std::shared_ptr<NodeButton> accept = std::make_shared<NodeButton>("accept", COORD{13, 2}, Color::BLACK, Color::RED, std::vector<std::string>{
        ".-----------.",
        "| Continuar |",
        "'-----------'"});

    accept->setOnClick([onConfirm, square](){square->setLocalPosition({0, 100}); onConfirm();});

    square->addChild(label);
    square->addChild(cancel);
    square->addChild(accept);

    return square;
}

std::shared_ptr<Node2D> createPatientCard(COORD position, Patient *patientPTR, LinkedList<Patient*> &patients, ListElement<Patient*> *patientLEPTR){
    Patient *localPatientPTR = new Patient(*patientPTR);

    auto root = std::make_shared<Node2D>("pivot", position);

    root->setAtExitFunction([localPatientPTR](){delete localPatientPTR;});

    auto label = std::make_shared<NodeUI>("label", COORD{2, 1}, std::vector<std::string>{
        "Paciente N:" + std::to_string(localPatientPTR->id),
        "Nombre: "    + std::string(localPatientPTR->firstName),
        "  __________________________________________________.     ",
        "Apellido: "  + std::string(localPatientPTR->lastName),
        "  __________________________________________________.     "});

    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{50, 100}, Color::BLUE, Color::BLACK, std::vector<std::string>{
        ".---------.",
        "| Guardar |",
        "'---------'"});
    saveChanges->setOnClick([localPatientPTR, patientPTR, saveChanges](){
        strcpy(patientPTR->firstName, localPatientPTR->firstName);       
        strcpy(patientPTR->lastName, localPatientPTR->lastName);         
        saveChanges->setLocalPosition(COORD{42, 100});             
    });
    
    std::shared_ptr<NodeSQ> confirmDelete = confirmDialog([patientPTR, &patients, patientLEPTR, root](){
        //Nota: localPatient sera elminado en exitTree siempre
        delete patientPTR;
        patients.remove_LE(patientLEPTR);
        root->setLocalPosition({0, 100});
    });
    
    std::shared_ptr<NodeButton> deletePatient = std::make_shared<NodeButton>("deletePatient", COORD{59, 1}, Color::RED, Color::BLACK, std::vector<std::string>{"X"});
    deletePatient->setOnClick([confirmDelete](){
        confirmDelete->setGlobalPosition(COORD{25, 8});
    });

    auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::WHITE, Color::BLACK);
    auto editName = std::make_shared<NodeButton>("editName", COORD{0, 2}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
    editName->setOnClick([localPatientPTR, editName, label, saveChanges](){
        strncpy(localPatientPTR->firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{6, 0})).c_str(), 50);
        label->set_text(std::vector<std::string>{
            "Paciente N:" + std::to_string(localPatientPTR->id),
            "Nombre: "    + std::string(localPatientPTR->firstName),
            "       __________________________________________________.",
            "Apellido: "  + std::string(localPatientPTR->lastName),
            "  __________________________________________________.     "});
        saveChanges->setLocalPosition(COORD{42, 6});
    });
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{0, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});


    root->addChild(square);
    root->addChild(label);
        label->addChild(editName);
        label->addChild(editLastname);
    root->addChild(saveChanges);
    root->addChild(deletePatient);
    root->addChild(confirmDelete);

    return root;
}

std::shared_ptr<Node>   createPatientMenu(SceneManager &manager, std::shared_ptr<Node> mainMenu, LinkedList<Patient*> &patients){
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});

    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();

    patients.reset_iteration();
    for (int i = 0; i < patients.get_size(); i++){
        cards->push_back(createPatientCard({SHORT(i*65), 0}, patients.get_iteration(), patients, patients.get_iteration_LE()) ) ;
        patients.continue_iteration();
    }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});
    mainMenuButton->setOnClick([mainMenu, &manager](){
        manager.changeScene(mainMenu);
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{20, 0}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    
    createPatient->setOnClick([cardCont, &patients, cards](){
        Patient* newPatientPTR = new Patient();
        newPatientPTR->id = Patient::nextId++;
        patients.push_back(newPatientPTR);
        cards->push_back( createPatientCard( { SHORT( cards->get_size() * 65 ), 0 } , newPatientPTR, patients, patients.getEndLE()) );
        cardCont->addChild(cards->getEnd());
    });

    root->setAtExitFunction([cards](){
        delete cards;
    });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 4}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{0, 4}, COORD{100, 1}, Color::BRIGHT_YELLOW, Color::BLACK);
    
    slider->setProcessFunction([slider, cardCont, cards](double){
        SHORT limit = 100;
        int amount = cards->get_size() - 1;
        static bool moving;

        if (Input::MousePos.Y == slider->getGlobalPosition().Y && Input::LClickJustPressed) moving = true;
        if (!Input::LClick) moving = false;

        if (moving)
            slider->setGlobalPosition({std::min(Input::MousePos.X, SHORT(limit - 7)), slider->getGlobalPosition().Y});

        float progress = (slider->getGlobalPosition().X / float(limit));
        cardCont->setGlobalPosition({SHORT(-progress * (amount * 65 + 40)), cardCont->getGlobalPosition().Y});
    });

    root->addChild(mainMenuButton);
    root->addChild(createPatient);
    root->addChild(cardCont);
        cards->reset_iteration();
        for (int i = 0; i < patients.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);

    return root;
}

std::shared_ptr<Node>   createMainMenu(SceneManager &manager, Hospital &hospital){
    (void) manager;

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
    pacientsButton->setOnClick([&manager, root, &hospital](){
        manager.changeScene(createPatientMenu(manager, root, hospital.patients));
    });
    std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Doctors  |",
        "'-----------'"});
    std::shared_ptr<NodeButton> appointmentsButton = std::make_shared<NodeButton>("appointmentsButton", COORD{40, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|   Citas   |",
        "'-----------'"});

    //Armar el arbol
    root->addChild(headersContainer);
        headersContainer->addChild(hospitalName);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(pacientsButton);
        buttonsContainer->addChild(doctorsButton);
        buttonsContainer->addChild(appointmentsButton);

    return root;
}

std::shared_ptr<Node> createDoctorMenu(SceneManager &manager, std::shared_ptr<Node> mainMenu, LinkedList<Doctor*> &doctors){
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");

    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 10});

    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();

    doctors.reset_iteration();
    for (int i = 0; i < doctors.get_size(); i++){
        //cards->push_back(createDoctorCard({SHORT(i*65), 0}, doctors.get_iteration() ) ) ;
        doctors.continue_iteration();
    }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});
    mainMenuButton->setOnClick([mainMenu, &manager](){
        manager.changeScene(mainMenu);
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createDoctoor", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    createPatient->setOnClick([cardCont, &doctors, cards](){
        Doctor* newDoctorPTR = new Doctor();
        newDoctorPTR->id = doctors.get_size();
        doctors.push_back(newDoctorPTR);
        //cards->push_back( createPatientCard( { SHORT( cards->get_size() * 65 ), 0 } , newPatient ) );
        cardCont->addChild(cards->getEnd());
    });

    // root->setProcessFunction([cards](double){
        
    // });

    root->setAtExitFunction([cards](){
        delete cards;
    });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 5}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{0, 5}, COORD{100, 1}, Color::BRIGHT_CYAN, Color::BLACK);
    
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

    root->addChild(mainMenuButton);
    root->addChild(createPatient);
    root->addChild(cardCont);
        cards->reset_iteration();
        for (int i = 0; i < doctors.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);

    return root;
}

int main(){
    Hospital hospital;
    strcpy(hospital.name, "HOLAMUNDO");
    strcpy(hospital.address, "ALGUNAPARTE");

    SceneManager manager = SceneManager::getInstance();
    auto root = createMainMenu(manager, hospital);
    manager.changeScene(root);
    manager.startRunning();
}
