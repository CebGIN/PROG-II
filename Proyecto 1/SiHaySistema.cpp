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
    T get_iteration(){
        return nextIteration->value;
    }
    void continue_iteration(){
        nextIteration = nextIteration->next;
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

struct Hospital {
    char name[100];
    char address[150];
    char phone[15];
    
    LinkedList<Patient*> patients;
    LinkedList<Doctor*> doctors;
    LinkedList<Appointment*> appointments;
};

std::shared_ptr<Node2D> createPatientCard(COORD position, Patient *patient){
    Patient *localPatient = new Patient(*patient);

    auto root = std::make_shared<Node2D>("pivot", position);
    auto label = std::make_shared<NodeUI>("label", COORD{2, 1}, std::vector<std::string>{
        "Paciente N:" + std::to_string(localPatient->id),
        "Nombre: "    + std::string(localPatient->firstName),
        "  __________________________________________________.     ",
        "Apellido: "  + std::string(localPatient->lastName),
        "  __________________________________________________.     "});

    std::shared_ptr<NodeButton> saveChanges = std::make_shared<NodeButton>("saveChanges", COORD{42, 100}, Color::BLUE, Color::BLACK, std::vector<std::string>{
        ".---------.",
        "| Guardar |",
        "'---------'"});
    saveChanges->setOnClick([localPatient, patient, saveChanges](){
        strcpy(patient->firstName, localPatient->firstName);
        strcpy(patient->lastName, localPatient->lastName);
        saveChanges->setLocalPosition(COORD{42, 100});
    });

    auto square = std::make_shared<NodeSQ>("Cuadro", COORD{0, 0}, COORD{62, 10}, Color::WHITE, Color::BLACK);
    auto editName = std::make_shared<NodeButton>("editName", COORD{0, 2}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[Edit]"});
        editName->setOnClick([localPatient, editName, label, saveChanges](){
            strncpy(localPatient->firstName, (Input::getLineInput(editName->getGlobalPosition() + COORD{6, 0})).c_str(), 50);
            label->set_text(std::vector<std::string>{
                "Paciente N:" + std::to_string(localPatient->id),
                "Nombre: "    + std::string(localPatient->firstName),
                "       __________________________________________________.",
                "Apellido: "  + std::string(localPatient->lastName),
                "  __________________________________________________.     "});
            saveChanges->setLocalPosition(COORD{42, 6});
        });
    auto editLastname = std::make_shared<NodeButton>("editLastname", COORD{0, 4}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[]"});
    // auto editName = std::make_shared<NodeButton>("editName", COORD{-1, 0}, Color::BLUE, Color::BLACK, std::vector<std::string>{"[]"});



    root->addChild(square);
    root->addChild(label);
        label->addChild(editName);
        label->addChild(editLastname);
    root->addChild(saveChanges);

    return root;
}

std::shared_ptr<Node> createPatientMenu(SceneManager &manager, std::shared_ptr<Node> mainMenu, LinkedList<Patient*> &patients){
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");

    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 10});

    LinkedList<std::shared_ptr<Node2D>> *cards = new LinkedList<std::shared_ptr<Node2D>>();

    patients.reset_iteration();
    for (int i = 0; i < patients.get_size(); i++){
        cards->push_back(createPatientCard({SHORT(i*65), 0}, patients.get_iteration() ) ) ;
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
        Patient* newPatient = new Patient();
        newPatient->id = patients.get_size();
        patients.push_back(newPatient);
        cards->push_back( createPatientCard( { SHORT( cards->get_size() * 65 ), 0 } , newPatient ) );
        cardCont->addChild(cards->getEnd());
    });

    // root->setProcessFunction([cards](double){
        
    // });

    root->setAtExitFunction([cards](){
        delete cards;
    });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{0, 5}, COORD{100, 1}, Color::BRIGHT_YELLOW, Color::BLACK);
    
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

std::shared_ptr<Node> createMainMenu(SceneManager &manager, Hospital &hospital){
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
    
    
    // std::shared_ptr<NodeButton> dragableButton = std::make_shared<NodeButton>("appointmentsButton", COORD{30, 10}, Color::RED, Color::BLACK, std::vector<std::string>{
    //     ".----------.",
    //     "|   Drag   |",
    //     "'----------'"});

    //Armar el arbol
    root->addChild(headersContainer);
        headersContainer->addChild(hospitalName);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(pacientsButton);
        buttonsContainer->addChild(doctorsButton);
        buttonsContainer->addChild(appointmentsButton);

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
