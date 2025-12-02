#include "../CGINLibs/LegacyEngine.hpp"
#include "../CGINLibs/CGINDE.hpp"
#include "../CGINLibs/CGINIF.hpp"
#include "../CGINLibs/CGINFU.hpp"
// #include "Appoinment/appoinment.hpp"
#include "Patient/patient.hpp"
// #include "Doctor/doctor.hpp"
#include <cstring>
#include <vector>

struct Doctor;
struct Patient;

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



hos::Hospital hospital;

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

std::shared_ptr<Node> createMainMenu();
std::shared_ptr<Node> createPatientMenu();
std::shared_ptr<Node> createSinglePatientCardView(ptn::Patient *patientPTR);

std::shared_ptr<Node> createMainMenu(){
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
    pacientsButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createPatientMenu());
    });
    // std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
    //     ".-----------.",
    //     "|  Doctors  |",
    //     "'-----------'"});
    // doctorsButton->setOnClick([](){
    //     SceneManager::getInstance().changeScene(createDoctorMenu());
    // });

    //Armar el arbol
    root->addChild(headersContainer);
        headersContainer->addChild(hospitalName);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(pacientsButton);
        // buttonsContainer->addChild(doctorsButton);

    return root;
}

std::shared_ptr<Node> createPatientMenu(){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");
    std::shared_ptr<NodeSQ> border = std::make_shared<NodeSQ>("PatientsBorder", COORD{0, 0}, COORD{100, 40}, Color::YELLOW, Color::YELLOW);
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});
    
    std::shared_ptr<std::vector<std::shared_ptr<Node2D>>> cards = std::make_shared<std::vector<std::shared_ptr<Node2D>>>();
    
    std::shared_ptr<std::vector<std::unique_ptr<ptn::Patient>>> loadedPatients = std::make_shared<std::vector<std::unique_ptr<ptn::Patient>>>();
    loadedPatients->reserve(hospital.patients.getListSize() * 1.5);
    
    for (uint64_t i = 0; i < hospital.patients.getTotalIDXs(); i++){
        auto patient = hospital.patients.getAtIDX(i);
        if (!patient) continue;
        loadedPatients->push_back(std::move(patient));
    }

    for (const auto &patient : *loadedPatients){
        cards->push_back(patient->createCard());
        patient->getCard().lock()->setLocalPosition(COORD{(SHORT)(55 * cards->size()), 0});
    }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});

    mainMenuButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createMainMenu());
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{21, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    
    createPatient->setOnClick([loadedPatients, cardCont, cards](){
        loadedPatients->emplace_back(std::make_unique<ptn::Patient>(hospital.patients.getTotalIDXs() + 1));
        hospital.patients.add(*(loadedPatients->back()));
        cards->push_back(loadedPatients->back()->createCard());
        cards->back()->setLocalPosition(COORD{(SHORT)(55 * cards->size()), 0});
        cardCont->addChild(cards->back());
    });

    // ptn::Patient **foundPatientPTR = new ptn::Patient*(nullptr);

    // root->setAtExitFunction([cards, foundPatientPTR](){
    //     delete cards;
    //     delete foundPatientPTR;
    // });

    // std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::BLACK, std::vector<std::string>{"[ Ver Tarjeta ]"});
    // viewFoundCard->setOnClick([root, foundPatientPTR](){
    //     if (*foundPatientPTR) {
    //         // Pass the found Patient pointer and the current scene (root) as the back scene
    //         SceneManager::getInstance().changeScene(createSinglePatientCardView(*foundPatientPTR));
    //     }
    // });

    // std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 1}, std::vector<std::string>{"Busqueda:"});
    // // Search input buttons (using the same Input::getLineInput pattern)
    // auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 2}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por ID"});
    // auto searchByNatID = std::make_shared<NodeButton>("SearchNatID", COORD{32, 3}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Cedula"});
    // auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 4}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Nombre"});

    // // --- Search Logic ---

    // // Helper function to perform the search and update UI/state
    // auto performPatientSearch = [loadedPatients, searchStatus, foundPatientPTR, viewFoundCard, root](const std::string& query, const std::string& type) {
    //     *foundPatientPTR = nullptr; // Reset previous result
        
    //     for (const std::unique_ptr<ptn::Patient>& patient : *loadedPatients){
    //         bool match = false;
            
    //         if (type == "ID" && std::to_string(patient->getID()) == query) {
    //             match = true;
    //         } else if (type == "NAT_ID" && std::string(patient->getNationalID()) == query) {
    //             match = true;
    //         } else if (type == "FNAME" && std::string(patient->getFirstName()).find(query) != std::string::npos) {
    //             match = true;
    //         }

    //         if (match) {
    //             *foundPatientPTR = patient.get();
    //             searchStatus->set_text({"Busqueda: EXITO", "Encontrado: " + std::string(patient->getFirstName()) + " " + std::string(patient->getLastName())});
    //             viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
    //             return;
    //         }
    //     }
    //     // If loop finishes without finding anything
    //     searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
    //     viewFoundCard->setLocalPosition(COORD{75, 100}); // Hide the view button
    // };

    // // Button Handlers
    // searchByID->setOnClick([searchByID, performPatientSearch](){
    //     std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{20, 0});
    //     performPatientSearch(input, "ID");
    // });

    // searchByNatID->setOnClick([searchByNatID, performPatientSearch](){
    //     std::string input = Input::getLineInput(searchByNatID->getGlobalPosition() + COORD{20, 0});
    //     performPatientSearch(input, "NAT_ID");
    // });

    // searchByFName->setOnClick([searchByFName, performPatientSearch](){
    //     std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{20, 0});
    //     performPatientSearch(input, "FNAME");
    // });

    std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{1, 5}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        "|/////|",});
    std::shared_ptr<NodeSQ> sliderline = std::make_shared<NodeSQ>("Line", COORD{1, 5}, COORD{98, 1}, Color::BRIGHT_YELLOW, Color::BLACK);
    
    slider->setProcessFunction([slider, cardCont, cards](double){
        SHORT limit = 98;
        int amount = cards->size() - 1;
        static bool moving;

        if (Input::MousePos.Y == slider->getGlobalPosition().Y && Input::LClickJustPressed) moving = true;
        if (!Input::LClick) moving = false;

        if (moving)
            slider->setGlobalPosition({std::max(std::min(Input::MousePos.X, SHORT(limit - 6)), (SHORT)1), slider->getGlobalPosition().Y});

        float progress = ((slider->getGlobalPosition().X -1) / float(limit));
        cardCont->setGlobalPosition({SHORT(1-progress * (amount * 65 + 40)), cardCont->getGlobalPosition().Y});
    });

    
    // root->addChild(searchStatus);
    // root->addChild(searchByID);
    // root->addChild(searchByNatID);
    // root->addChild(searchByFName);
    // root->addChild(viewFoundCard);
    root->addChild(createPatient);
    root->addChild(cardCont);
    for (auto card : *cards) { cardCont->addChild(card);}
    root->addChild(sliderline);
    root->addChild(slider);
    root->addChild(border);
    root->addChild(mainMenuButton);
    return root;
}

std::shared_ptr<Node> createSinglePatientCardView(ptn::Patient *patientPTR){
    std::shared_ptr<Node> root = std::make_shared<Node>("SinglePatientView");

    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
    backButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createPatientMenu());
    });

    // We need the ListElement pointers only for deletion/modification within a list, 
    // which won't happen here, so we pass nullptrs for the ListElement arguments.
    std::shared_ptr<Node2D> patientCard = patientPTR->createCard();
    
    root->addChild(backButton);
    // root->addChild(patientCard);
    
    return root;
}
int main(){
    strcpy(hospital.name, "HOLAMUNDO");
    strcpy(hospital.address, "ALGUNAPARTE");



    SceneManager::getInstance().changeScene(createMainMenu());
    SceneManager::getInstance().startRunning();
}