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


std::shared_ptr<Node> createMainMenu(SceneManager manager, Hospital &hospital){
    (void) manager;
    //Declarar Nodos
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");

    std::shared_ptr<Node2D> headersContainer = std::make_shared<Node2D>("headersContainer", COORD{1, 1});

    std::shared_ptr<NodePCT> hospitalName = std::make_shared<NodePCT>("name", COORD{0,0}, Color::BRIGHT_WHITE, Color::BLACK, std::vector<std::string>{
        hospital.name,
        hospital.address
    });

    std::shared_ptr<Node2D> buttonsContainer = std::make_shared<Node2D>("buttonContainer", COORD{5, 10});
    
    std::shared_ptr<NodeButton> pacientsButton = std::make_shared<NodeButton>("pacientsButton", COORD{0, 0}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "| Pacientes |",
        "'-----------'"});
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

int main(){
    Hospital hospital;
    strcpy(hospital.name, "HOLAMUNDO");
    strcpy(hospital.address, "ALGUNAPARTE");

    SceneManager manager = SceneManager::getInstance();
    auto root = createMainMenu(manager, hospital);
    manager.changeScene(root);
    manager.startRunning();
}
