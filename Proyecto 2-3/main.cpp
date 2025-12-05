#include "../CGINLibs/LegacyEngine.hpp"
#include "../CGINLibs/CGINDE.hpp"
#include "../CGINLibs/CGINIF.hpp"
#include "Hospital/hospital.hpp"
#include "Appoinment/appoinment.hpp"
#include "Patient/patient.hpp"
#include "Doctor/doctor.hpp"
#include <cassert>
#include <cstring>
#include <vector>

struct Doctor;
struct Patient;




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
std::shared_ptr<Node2D> createPatientCard(COORD position, Patient *patientPTR, cde::ListElement<Patient*> *patientLEPTR);
std::shared_ptr<Node> createSinglePatientCardView(Patient *patientPTR);

std::shared_ptr<Node> createDoctorMenu();
std::shared_ptr<Node2D> createDoctorCard(COORD position, Doctor *doctorPTR, cde::ListElement<Doctor*> *doctorLEPTR);
std::shared_ptr<Node> createSingleDoctorCardView(Doctor *doctorPTR);

std::shared_ptr<Node> checkAppointmentsOf(Patient*actualPatientPTR, Doctor*actualDoctorPTR, int contextFlag);
std:: shared_ptr<Node2D> createAppoinmentCard(COORD position, Appointment *appoinmentPTR, int contextFlag);

std::shared_ptr<Node> createMedicalRecordMenu(Appointment *apptPtr, int contextFlag);
std::shared_ptr<Node> createMedicalRecordViewer(Patient *patientPTR);

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
    std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Doctors  |",
        "'-----------'"});
    doctorsButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createDoctorMenu());
    });

    //Armar el arbol
    root->addChild(headersContainer);
        headersContainer->addChild(hospitalName);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(pacientsButton);
        buttonsContainer->addChild(doctorsButton);

    return root;
}

std::shared_ptr<Node> createPatientMenu(){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");
    std::shared_ptr<NodeSQ> border = std::make_shared<NodeSQ>("PatientsBorder", COORD{0, 0}, COORD{100, 40}, Color::YELLOW, Color::YELLOW);
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});
    
    cde::LinkedList<std::shared_ptr<Node2D>> *cards = new cde::LinkedList<std::shared_ptr<Node2D>>();
    
    std::shared_ptr<std::vector<std::unique_ptr<ptn::Patient>>> loadedPatients;
    loadedPatients->reserve(hospital.patients.getListSize() * 1.5);
    
    for (int i = 0; i < hospital.patients.getTotalIDXs(); i++){
        auto patient = hospital.patients.getAtIDX(i);
        if (!patient) continue;
        cards->push_back(patient->createCard()); 
        loadedPatients->push_back(std::move(patient));
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
        loadedPatients->emplace_back(std::make_unique<ptn::Patient>(hospital.patients.getTotalIDXs() + 2));
        hospital.patients.add(*(loadedPatients->back()));
        cards->push_back(loadedPatients->back()->createCard());
        cardCont->addChild(cards->get_end()->get());
    });

    ptn::Patient **foundPatientPTR = new ptn::Patient*(nullptr);

    root->setAtExitFunction([cards, foundPatientPTR](){
        delete cards;
        delete foundPatientPTR;
    });

    std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::BLACK, std::vector<std::string>{"[ Ver Tarjeta ]"});
    viewFoundCard->setOnClick([root, foundPatientPTR](){
        if (*foundPatientPTR) {
            // Pass the found Patient pointer and the current scene (root) as the back scene
            SceneManager::getInstance().changeScene(createSinglePatientCardView(*foundPatientPTR));
        }
    });

    std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 1}, std::vector<std::string>{"Busqueda:"});
    // Search input buttons (using the same Input::getLineInput pattern)
    auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 2}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por ID"});
    auto searchByNatID = std::make_shared<NodeButton>("SearchNatID", COORD{32, 3}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Cedula"});
    auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 4}, Color::WHITE, Color::BLACK, std::vector<std::string>{"Buscar por Nombre"});

    // --- Search Logic ---

    // Helper function to perform the search and update UI/state
    auto performPatientSearch = [loadedPatients, searchStatus, foundPatientPTR, viewFoundCard, root](const std::string& query, const std::string& type) {
        *foundPatientPTR = nullptr; // Reset previous result
        
        for (const std::unique_ptr<ptn::Patient>& patient : *loadedPatients){
            bool match = false;
            
            if (type == "ID" && std::to_string(patient->getID()) == query) {
                match = true;
            } else if (type == "NAT_ID" && std::string(patient->getNationalID()) == query) {
                match = true;
            } else if (type == "FNAME" && std::string(patient->getFirstName()).find(query) != std::string::npos) {
                match = true;
            }

            if (match) {
                *foundPatientPTR = patient.get();
                searchStatus->set_text({"Busqueda: EXITO", "Encontrado: " + std::string(patient->getFirstName()) + " " + std::string(patient->getLastName())});
                viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
                return;
            }
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
    // cards->reset_iteration();
    // for (int i = 0; i < hospital.patients.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
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
    // std::shared_ptr<Node2D> patientCard = createPatientCard(
    //     COORD{2, 3}, 
    //     patientPTR,
    //     nullptr // No ListElement pointer needed for viewing
    // );
    
    root->addChild(backButton);
    // root->addChild(patientCard);
    
    return root;
}

std::shared_ptr<Node> createDoctorMenu(){
    std::shared_ptr<Node> root = std::make_shared<Node>("RootDoctors");

    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});

    cde::LinkedList<std::shared_ptr<Node2D>> *cards = new cde::LinkedList<std::shared_ptr<Node2D>>();

    // hospital.doctors.reset_iteration();
    // for (int i = 0; i < hospital.doctors.get_size(); i++){
    //     cards->push_back(createDoctorCard({SHORT(i*65), 0}, hospital.doctors.get_iteration(), hospital.doctors.get_iteration_LE()) ) ;
    //     hospital.doctors.continue_iteration();
    // }

    std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".----------------.",
        "| Menu principal |",
        "'----------------'"});
    mainMenuButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createMainMenu());
    });

    std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createDoctoor", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-------.",
        "| Nuevo |",
        "'-------'"});
    // createPatient->setOnClick([cardCont, cards](){
    //     Doctor* newDoctorPTR = new Doctor();
    //     newDoctorPTR->id = Doctor::nextId++;
    //     hospital.doctors.push_back(newDoctorPTR);
    //     cards->push_back( createDoctorCard({ SHORT( cards->get_size() * 65 ), 0 }, newDoctorPTR, hospital.doctors.get_end()) );
    //     cardCont->addChild(cards->get_end()->get());
    // });

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
    
    // // Helper function to perform the search and update UI/state
    // auto performDoctorSearch = [searchStatus, foundDoctorPTR, viewFoundCard](const std::string& query, const std::string& type) {
    //     *foundDoctorPTR = nullptr; // Reset previous result

    //     cde::ListElement<Doctor*>* currentDoctor = nullptr;
    //     while(hospital.doctors.go_through(currentDoctor)){
    //         bool match = false;
            
    //         if (type == "ID" && std::to_string(currentDoctor->get()->getID()) == query) {
    //             match = true;
    //         } else if (type == "FNAME" && std::string(currentDoctor->get()->getFirstName()).find(query) != std::string::npos) {
    //             match = true;
    //         } else if (type == "SPECIALTY" && std::string(currentDoctor->get()->getSpecialty()).find(query) != std::string::npos) {
    //             match = true;
    //         }
    
    //         if (match) {
    //             *foundDoctorPTR = currentDoctor->get();
    //             searchStatus->set_text({"Busqueda: EXITO", "Encontrado: " + std::string(currentDoctor->get()->getFirstName()) + " " + std::string(currentDoctor->get()->getLastName())});
    //             viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
    //             return;
    //         }
    //     }
        
    //     searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
    //     viewFoundCard->setLocalPosition(COORD{75, 100}); // Hide the view button
    // };
    
    // // Button Handlers
    // searchByID->setOnClick([searchByID, performDoctorSearch](){
    //     std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{20, 0});
    //     performDoctorSearch(input, "ID");
    // });
    
    // searchByFName->setOnClick([searchByFName, performDoctorSearch](){
    //     std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{20, 0});
    //     performDoctorSearch(input, "FNAME");
    // });
    
    // searchBySpecialty->setOnClick([searchBySpecialty, performDoctorSearch](){
    //     std::string input = Input::getLineInput(searchBySpecialty->getGlobalPosition() + COORD{20, 0});
    //     performDoctorSearch(input, "SPECIALTY");
    // });
    
    
    viewFoundCard->setOnClick([root, foundDoctorPTR](){
        if (*foundDoctorPTR) {
            // Pass the found Doctor pointer
            SceneManager::getInstance().changeScene(createSingleDoctorCardView(*foundDoctorPTR));
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
    // cards->reset_iteration();
    // for (int i = 0; i < hospital.doctors.get_size(); i++) { cardCont->addChild(cards->get_iteration()); cards->continue_iteration();}
    root->addChild(sliderline);
    root->addChild(slider);
    root->addChild(mainMenuButton);

    return root;
}


std::shared_ptr<Node> createSingleDoctorCardView(Doctor *doctorPTR){
    std::shared_ptr<Node> root = std::make_shared<Node>("SingleDoctorView");

    std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
    backButton->setOnClick([](){
        SceneManager::getInstance().changeScene(createDoctorMenu());
    });

    // Pass nullptrs for the ListElement arguments since we are only viewing
    // std::shared_ptr<Node2D> doctorCard = createDoctorCard(
    //     {2, 3}, 
    //     doctorPTR, // Pass the main list for save/delete context
    //     nullptr // No ListElement pointer needed for viewing
    // );
    
    root->addChild(backButton);
    // root->addChild(doctorCard);
    
    return root;
}

// std::shared_ptr<Node> checkAppointmentsOf(Patient*actualPatientPTR, Doctor*actualDoctorPTR, int contextFlag){
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
        backButton->setOnClick([](){
            SceneManager::getInstance().changeScene(createPatientMenu());
        });
    } else if (contextFlag == 2) {
        // Return to Doctor Menu (MUST be recreated)
        backButton->setOnClick([](){
            SceneManager::getInstance().changeScene(createDoctorMenu());
        });
    } else {
        backButton->setOnClick([](){
            SceneManager::getInstance().changeScene(createMainMenu());
        });
    }
       
    cde::LinkedList<std::shared_ptr<Node2D>> *cards = new cde::LinkedList<std::shared_ptr<Node2D>>();
    std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{1, 6});
   
    root->setAtExitFunction([cards](){
        delete cards;
    });
   
 
    // --- PATIENT SELECTOR DIALOG SETUP ---
    std::shared_ptr<NodeSQ> patientSelector = std::make_shared<NodeSQ>("selectPatient", COORD{300, 300}, COORD{26, 40}, Color::YELLOW, Color::YELLOW);
    patientSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{"Pacientes"}));
 
    Patient* patientPTR;
    cde::ListElement<Patient*>* current = nullptr;
    int i = 0;
    while(hospital.patients.go_through(current)){

    // hospital.patients.reset_iteration();
    // for(SHORT i = 0; i < hospital.patients.get_size(); ++i){
        // Patient* patientPTR = hospital.patients.get_iteration();
        // Capture patientSelector, cardCont, cards by value/reference for the lambda
        auto patientN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(i + 1)}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{patientPTR->firstName});
        patientN->setOnClick([patientPTR, actualDoctorPTR, patientSelector, cardCont, cards, contextFlag](){
            // 1. Create the new Appointment with the selected Patient and the context Doctor
            Appointment* newAppoinmentPTR = new Appointment();
            newAppoinmentPTR->id = Patient::nextId++;
            newAppoinmentPTR->patientPTR = patientPTR;
            newAppoinmentPTR->doctorPTR = actualDoctorPTR;
            // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
            if (patientPTR) patientPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            if (actualDoctorPTR) actualDoctorPTR->scheduledAppointments.push_back(newAppoinmentPTR);
         
            // 3. Create and add the UI card
            cards->push_back( createAppoinmentCard(COORD{0, SHORT( cards->get_size() * 14 + 4 )}, newAppoinmentPTR, contextFlag ) );
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
        doctorN->setOnClick([&doctorPTR, actualPatientPTR, doctorSelector, cardCont, cards, contextFlag](){
            // 1. Create the new Appointment with the selected Doctor and the context Patient
            Appointment* newAppoinmentPTR = new Appointment();
            newAppoinmentPTR->id = Patient::nextId++;
            newAppoinmentPTR->patientPTR = actualPatientPTR;
            newAppoinmentPTR->doctorPTR = doctorPTR;
            // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
            if (actualPatientPTR) actualPatientPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            if (doctorPTR) doctorPTR->scheduledAppointments.push_back(newAppoinmentPTR);
            // 3. Create and add the UI card
            cards->push_back( createAppoinmentCard(
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
        cards->push_back(createAppoinmentCard(COORD{0, SHORT(i* 14 + 4)}, appPtr, contextFlag) ) ;
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

std::shared_ptr<Node> createMedicalRecordMenu(Appointment *apptPtr, int contextFlag){
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
    saveAndFinalize->setOnClick([&newRecordPTR, apptPtr, contextFlag](){
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
        SceneManager::getInstance().changeScene(checkAppointmentsOf(
            returnPatient, 
            returnDoctor,
            contextFlag  // Pass the correct list reference
        ));

        delete apptPtr;
    });

    // --- Cancel Button ---
    std::shared_ptr<NodeButton> cancel = std::make_shared<NodeButton>("CancelBtn", COORD{35, 15}, Color::RED, Color::BLACK, std::vector<std::string>{"[ CANCELAR ]"});
    cancel->setOnClick([&apptPtr, root, newRecordPTR, contextFlag](){
        delete newRecordPTR;
        Patient* returnPatient = nullptr;
        Doctor* returnDoctor = nullptr;
        if (contextFlag == 1) { // Patient View
            returnPatient = apptPtr->patientPTR;
        } else if (contextFlag == 2) { // Doctor View
            returnDoctor = apptPtr->doctorPTR;
        }
        SceneManager::getInstance().changeScene(checkAppointmentsOf(
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

// std::shared_ptr<Node> createMedicalRecordViewer(Patient *patientPTR){
//     std::shared_ptr<Node> root = std::make_shared<Node>("RootRecordsViewer");
    
//     std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
//         "--- EXPEDIENTE MEDICO DE: " + std::string(patientPTR->firstName) + " " + std::string(patientPTR->lastName) + " ---"
//     });
    
//     std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 3}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
//     backButton->setOnClick([](){
//         SceneManager::getInstance().changeScene(createPatientMenu());
//     });
    
//     std::shared_ptr<Node2D> recordCont = std::make_shared<Node2D>("RecordCont", COORD{1, 5});
    
//     // Display all records
//     patientPTR->history.reset_iteration();
//     for (int i = 0; i < patientPTR->history.get_size(); i++){
//         MedicalRecord *record = patientPTR->history.get_iteration();
        
//         auto recordCard = std::make_shared<NodeUI>("RecCard" + std::to_string(i), COORD{0, SHORT(i * 7)}, std::vector<std::string>{
//             "ID: " + std::to_string(record->recordId) + " | Doctor ID: " + std::to_string(record->doctorId),
//             "------------------------------------------------",
//             "Diagnosis: " + std::string(record->diagnosis).substr(0, 50) + "...",
//             "Treatment: " + std::string(record->treatment).substr(0, 50) + "...",
//             "Medications: " + std::string(record->medications).substr(0, 50) + "...",
//             "------------------------------------------------"
//         });
//         recordCont->addChild(recordCard);
        
//         patientPTR->history.continue_iteration();
//     }
    
//     root->addChild(header);
//     root->addChild(backButton);
//     root->addChild(recordCont);
    
//     return root;
// }

int main(){
    
    strcpy(hospital.name, "HOLAMUNDO");
    strcpy(hospital.address, "ALGUNAPARTE");

    SceneManager::getInstance().changeScene(createMainMenu());
    SceneManager::getInstance().startRunning();
}