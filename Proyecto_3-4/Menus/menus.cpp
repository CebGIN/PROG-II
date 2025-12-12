// Menus.cpp
#include "menus.hpp"
#include <iostream>
#include <string>

// --- Funciones Utilitarias (Globales o fuera de namespace) ---

std::shared_ptr<cfm::Index> getSubAppoinmentIndex(std::string folder, std::string subfolder, uint64_t id) {
    // Nota: El original usaba Fold(folder +...) y luego Index(folder +...).
    // Simplificamos asumiendo que el constructor de Index maneja la creación de la carpeta.
    // Usar el ID como nombre de subcarpeta requiere que el Index lo maneje.
    // Adaptamos para usar la convención del código original.
    cfm::Folder Fold(folder + "/" + subfolder + "/" + std::to_string(id));
    std::shared_ptr<cfm::Index> index = std::make_shared<cfm::Index>(folder + "/" + subfolder + "/" + std::to_string(id));
    return index;
}

// ====================================================================
// --- Implementación del Namespace HOS (Hospital) ---
// ====================================================================

namespace hos {
    std::shared_ptr<Node> createFilesVisor(Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootFilesVisor");
        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::BLACK, Color::RED, std::vector<std::string>{
            "         ",
            " Volver  ",
            "         "});
        backButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::createMenu(hospital));});

        // Contenedor para la información de archivos
        std::shared_ptr<Node2D> fileInfoCont = std::make_shared<Node2D>("FileInfoCont", COORD{1, 5});

        // Patients File Info
        std::shared_ptr<NodePCT> Patients = std::make_shared<NodePCT>("Patients", COORD{0, 0}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{
            "--- Archivos de los pacientes ---",
            "Ruta: " + hospital.folderName + "/Patients",
            "Bytes del archivo de data (data.bin): " + std::to_string(hospital.patients.getDataFileSize()) + "b",
            "Bytes del archivo de indice (index.idx): " + std::to_string(hospital.patients.getIndexFileSize())+ "b",
            "Bytes del archivo de pila de espacios (spaces.bin): " + std::to_string(hospital.patients.getSpacesFileSize())+ "b",
        });
        
        // Doctors File Info
        std::shared_ptr<NodePCT> Doctors = std::make_shared<NodePCT>("Doctors", COORD{0, 7}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{
            "--- Archivos de los doctores ---",
            "Ruta: " + hospital.folderName + "/Doctors",
            "Bytes del archivo de data (data.bin): " + std::to_string(hospital.doctors.getDataFileSize()) + "b",
            "Bytes del archivo de indice (index.idx): " + std::to_string(hospital.doctors.getIndexFileSize())+ "b",
            "Bytes del archivo de pila de espacios (spaces.bin): " + std::to_string(hospital.doctors.getSpacesFileSize()) + "b",
        });
        
        // Appointments File Info
        std::shared_ptr<NodePCT> Appoiments = std::make_shared<NodePCT>("Appoiments", COORD{0, 14}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{
            "--- Archivos de las citas ---",
            "Ruta: " + hospital.folderName + "/Appoinments",
            "Bytes del archivo de data (data.bin): " + std::to_string(hospital.appoinments.getDataFileSize()) + "b",
            "Bytes del archivo de indice (index.idx): " + std::to_string(hospital.appoinments.getIndexFileSize())+ "b",
            "Bytes del archivo de pila de espacios (spaces.bin): " + std::to_string(hospital.appoinments.getSpacesFileSize()) + "b",
        });

        fileInfoCont->addChild(Patients);
        fileInfoCont->addChild(Doctors);
        fileInfoCont->addChild(Appoiments);

        root->addChild(backButton);
        root->addChild(fileInfoCont);
        return root;
    }

    std::shared_ptr<Node> createMenu(Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootHospitalMenu");
        std::shared_ptr<Node2D> headersContainer = std::make_shared<Node2D>("headersContainer", COORD{1, 1});
        std::shared_ptr<NodePCT> hospitalName = std::make_shared<NodePCT>("name", COORD{0,0}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{
            hospital.getName(),
            hospital.getAddress(),
            hospital.getPhone()
        }); 
        
        std::shared_ptr<Node2D> buttonsContainer = std::make_shared<Node2D>("buttonContainer", COORD{20, 12});
        
        // Button Patients
        std::shared_ptr<NodeButton> pacientsButton = std::make_shared<NodeButton>("pacientsButton", COORD{0, 0}, Color::BLACK, Color::YELLOW, std::vector<std::string>{
            "                                                                ",
            "             Ir al menu de gestion de pacientes                 ",
            "                                                                "});
        pacientsButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
        });
        
        // Button Doctors
        std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{0, 4}, Color::BLACK, Color::CYAN, std::vector<std::string>{
            "                                                                ",
            "             Ir al menu de gestion de doctores                  ",
            "                                                                "});
        doctorsButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(doc::createMenu(hospital));
        });
        
        // Button Appointments
        std::shared_ptr<NodeButton> appoinmentButton = std::make_shared<NodeButton>("doctorsButton", COORD{0, 8}, Color::BLACK, Color::GREEN, std::vector<std::string>{
            "                                                                ",
            "             Ir al menu de gestion de citas                     ",
            "                                                                "});
        appoinmentButton->setOnClick([&hospital](){
            // contextFlag = 0 (Global view), contextIDX = 0 (Ignorado en vista global)
            SceneManager::getInstance().changeScene(app::createMenu(hospital, 0, 0));
        });
        
        // Button File Visor
        std::shared_ptr<NodeButton> manageFiles = std::make_shared<NodeButton>("manageFiles", COORD{0, 39}, Color::BLACK, Color::WHITE, std::vector<std::string>{
            "             Revision de los archivos                           "});
        manageFiles->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(createFilesVisor(hospital));
        });

        // Construcción del árbol
        root->addChild(headersContainer);
            headersContainer->addChild(hospitalName);
        root->addChild(buttonsContainer);
            buttonsContainer->addChild(pacientsButton);
            buttonsContainer->addChild(doctorsButton);
            buttonsContainer->addChild(appoinmentButton);
        root->addChild(manageFiles);
        return root;
    }
}

// ====================================================================
// --- Implementación del Namespace PTN (Patient) ---
// ====================================================================

namespace ptn {
    // Vista de tarjeta de paciente individual
    std::shared_ptr<Node> createSinglePatientCardView(hos::Hospital &hospital, uint64_t i) {
        std::shared_ptr<Node> root = std::make_shared<Node>("SinglePatientView");
        auto patient = hospital.patients.getAtIDX(i);
        
        if (!patient) return root; // Retornar vacío si no se encuentra el paciente

        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BRIGHT_WHITE, std::vector<std::string>{"[ VOLVER ]"});
        backButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
        });
        
        // Usamos el método de la clase Patient para crear su tarjeta UI
        std::shared_ptr<Node2D> patientCard = patient->createCard(hospital.patients);
        patientCard->setLocalPosition(COORD{3, 3});
        
        // Nota: La tarjeta del paciente aquí no tiene botones de citas/registros.

        root->addChild(backButton);
        root->addChild(patientCard);
        return root;
    }

    // Obtiene el IndexedFile de registros médicos del paciente
    std::unique_ptr<cfm::IndexedFile<app::MedicalRecord>> getMedicalRecord(std::string folder, uint64_t ptnID) {
        // La carpeta de registros médicos es: folder/Patients/ptnID
        cfm::Folder Fold(folder + "/Patients/" + std::to_string(ptnID));
        std::unique_ptr<cfm::IndexedFile<app::MedicalRecord>> record = std::make_unique<cfm::IndexedFile<app::MedicalRecord>>(folder + "/Patients/" + std::to_string(ptnID));
        return record;
    }

    // Visor de todos los registros médicos de un paciente
    std::shared_ptr<Node> createMedicalRecordViewer(hos::Hospital &hospital, const Patient &patient) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootRecordsViewer");
        std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
            "--- EXPEDIENTE MEDICO DE: " + std::string(patient.getFirstName()) + " " + std::string(patient.getLastName()) + " ---"
        });
        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 3}, Color::RED, Color::WHITE, std::vector<std::string>{"[ VOLVER ]"});
        backButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
        });
        
        std::shared_ptr<Node2D> recordCont = std::make_shared<Node2D>("RecordCont", COORD{1, 5});
        
        // Cargar los registros médicos del paciente
        auto list = getMedicalRecord(hospital.folderName, patient.getID());
        
        for (unsigned int i = 0; i < list->getTotalIDXs(); i++){
            auto record = list->getAtIDX(i);
            if (!record) continue; // Saltar registros borrados/inválidos
            
            // Crear la tarjeta de visualización para cada registro
            auto recordCard = std::make_shared<NodeUI>("RecCard" + std::to_string(i), COORD{0, static_cast<SHORT>(i * 7)}, std::vector<std::string>{
                "ID: " + std::to_string(record->getId()) + " | Doctor ID: " + std::to_string(record->getDoctorId()),
                "Fecha/Hora: " + record->getDate() + " " + record->getTime() + " | Costo: " + std::to_string(record->getCost()),
                "------------------------------------------------",
                "Diagnosis: " + record->getDiagnosis().substr(0, 50) + "...",
                "Treatment: " + record->getTreatment().substr(0, 50) + "...",
                "Medications: " + record->getMedications().substr(0, 50) + "...",
                "------------------------------------------------"
            });
            recordCont->addChild(recordCard);
        }
        
        root->addChild(header);
        root->addChild(backButton);
        root->addChild(recordCont);
        return root;
    }

    // Menú principal de pacientes (lista de pacientes)
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");

        std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::BLACK, Color::RED, std::vector<std::string>{
                "                   ",
                "  Menu principal   ",
                "                   "});
        mainMenuButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::createMenu(hospital));});

        std::shared_ptr<Node2D> cards = std::make_shared<Node2D>("cardCont", COORD{0, 7});
        // La lista cargada debe ser shared_ptr para que viva con la escena y evitar que los pacientes se destruyan al salir del for.
        std::shared_ptr<std::vector<std::shared_ptr<ptn::Patient>>> loadedlist = std::make_shared<std::vector<std::shared_ptr<ptn::Patient>>>();
        loadedlist->reserve(hospital.patients.getListSize() * 2);
        
        for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++){
            std::shared_ptr<ptn::Patient> patient = hospital.patients.getAtIDX(i);
            if (!patient) continue;

            auto card = patient->createCard(hospital.patients);
            cards->addChild(card);

            card->setLocalPosition(COORD{static_cast<SHORT>(loadedlist->size() * 66), 0});
            
            // Botón para ver citas
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                // contextFlag = 1 (Contexto de Paciente), contextIDX = ID del paciente (i)
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 1, i));
            });
            // El card->getChild(1) es la etiqueta (label) del paciente dentro de la tarjeta
            card->getChild(1)->addChild(goToAppointments);

            // Botón para ver registros médicos
            auto goToRecords = std::make_shared<NodeButton>("checkRecords", COORD{5, 12}, Color::CYAN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Ver Expediente ]"});
            goToRecords->setOnClick([&hospital, patient](){
                // Pasar una copia del patient (por valor) para que viva en el lambda, aunque si el patient está en loadedlist ya está seguro.
                SceneManager::getInstance().changeScene(createMedicalRecordViewer(hospital, *patient)); 
            });

            card->getChild(1)->addChild(goToRecords);
            loadedlist->push_back(patient);
        }

        // Botón para crear un nuevo paciente
        std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{21, 1}, Color::BLACK, Color::YELLOW, std::vector<std::string>{
                "          ",
                "  Nuevo   ",
                "          "});
            
        createPatient->setOnClick([&hospital, loadedlist, cards](){
            uint64_t newID = hospital.patients.getTotalIDXs();
            loadedlist->emplace_back(std::make_unique<ptn::Patient>(newID));
            hospital.patients.add(*(loadedlist->back()));
            
            auto card = loadedlist->back()->createCard(hospital.patients);
            card->setLocalPosition(COORD{static_cast<SHORT>(66 * (cards->getChildCount())), 0}); // Usar getChildCount() antes de añadir
            
            uint64_t i = newID; // El ID es el índice
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 1, i));
            });
            
            auto goToRecords = std::make_shared<NodeButton>("checkRecords", COORD{5, 12}, Color::CYAN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Ver Expediente ]"});
            goToRecords->setOnClick([&hospital, patient=loadedlist->back()](){
                SceneManager::getInstance().changeScene(createMedicalRecordViewer(hospital, *patient)); 
            });

            card->getChild(1)->addChild(goToAppointments);
            card->getChild(1)->addChild(goToRecords);
            cards->addChild(card);
        });

        // Lógica de búsqueda (omito la implementación de búsqueda compleja para centrarme en la estructura)
        // ... (Tu código de búsqueda de pacientes va aquí)

        // Elementos de la búsqueda
        std::shared_ptr<uint64_t> foundPatientIDX = std::make_shared<uint64_t>(UINT64_MAX);
        std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::WHITE, std::vector<std::string>{"[ Ver Tarjeta ]"});
        // ... (Definiciones de searchStatus, searchByID, searchByNatID, searchByFName y performPatientSearch)

        // Se re-introduce la lógica de búsqueda para mantener la funcionalidad
        std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 1}, std::vector<std::string>{"Busqueda:"});
        auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 2}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por ID"});
        auto searchByNatID = std::make_shared<NodeButton>("SearchNatID", COORD{32, 3}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por Cedula"});
        auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 4}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por Nombre"});

        auto performPatientSearch = [loadedlist, searchStatus, foundPatientIDX, viewFoundCard](const std::string& query, const std::string& type) {
            *foundPatientIDX = UINT64_MAX; 
            for (auto patient : *loadedlist){
                // ... (Lógica de coincidencia) ...
                bool match = false;
                if (type == "ID" && std::to_string(patient->getID()) == query) { match = true; } // Corrección: paciente->getID() + 1 en original era extraño
                else if (type == "NAT_ID" && std::string(patient->getNationalID()) == query) { match = true; } 
                else if (type == "FNAME" && std::string(patient->getFirstName()).find(query) != std::string::npos) { match = true; }
                
                if (match) {
                    *foundPatientIDX = patient->getID();
                    searchStatus->set_text({"Busqueda: Encontrado: " + std::string(patient->getFirstName()) + ", " + std::string(patient->getLastName())});
                    viewFoundCard->setLocalPosition(COORD{80, 1}); // Show the view button
                    return;
                }
            }
            searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
            viewFoundCard->setLocalPosition(COORD{75, 100}); 
        };

        // Handlers de botones de búsqueda
        searchByID->setOnClick([searchByID, performPatientSearch](){
            std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{14, 0});
            performPatientSearch(input, "ID");
        });
        searchByNatID->setOnClick([searchByNatID, performPatientSearch](){
            std::string input = Input::getLineInput(searchByNatID->getGlobalPosition() + COORD{18, 0});
            performPatientSearch(input, "NAT_ID");
        });
        searchByFName->setOnClick([searchByFName, performPatientSearch](){
            std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{18, 0});
            performPatientSearch(input, "FNAME");
        });
        viewFoundCard->setOnClick([&hospital, foundPatientIDX](){
            if (*foundPatientIDX != UINT64_MAX) {
                SceneManager::getInstance().changeScene(createSinglePatientCardView(hospital, *foundPatientIDX));
            }
        });
        
        // Slider
        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cards->getChildCount());
        cards->setProcessFunction([cards, dynamicOffset](double){ *dynamicOffset = (cards->getChildCount() -1)* 66;});
        auto slider = pui::hSlider(cards, dynamicOffset, 100, Color::YELLOW, Color::YELLOW, Color::BRIGHT_YELLOW, Color::BRIGHT_YELLOW);
        slider->setGlobalPosition(COORD{0, 5});

        // Asegurar que loadedList se limpie al salir
        root->setAtExitFunction([loadedlist](){loadedlist->clear();});

        root->addChild(searchStatus);
        root->addChild(searchByID);
        root->addChild(searchByNatID);
        root->addChild(searchByFName);
        root->addChild(viewFoundCard);
        root->addChild(mainMenuButton);
        root->addChild(createPatient);
        root->addChild(cards);
        root->addChild(slider);
        return root;
    }
}

// ====================================================================
// --- Implementación del Namespace DOC (Doctor) ---
// ====================================================================

namespace doc {
    // Vista de tarjeta de doctor individual
    std::shared_ptr<Node> createSingleDoctorCardView(hos::Hospital &hospital, uint64_t i) {
        std::shared_ptr<Node> root = std::make_shared<Node>("SingleDoctorView");
        auto doctor = hospital.doctors.getAtIDX(i);

        if (!doctor) return root;

        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 0}, Color::RED, Color::BRIGHT_WHITE, std::vector<std::string>{"[ VOLVER ]"});
        backButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(doc::createMenu(hospital));
        });
        
        std::shared_ptr<Node2D> doctorCard = doctor->createCard(hospital.doctors);
        doctorCard->setLocalPosition(COORD{3, 3});
        
        root->addChild(backButton);
        root->addChild(doctorCard);
        
        return root;
    }

    // Menú principal de doctores (lista de doctores)
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootDoctors");

        std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::BLACK, Color::RED, std::vector<std::string>{
                "                   ",
                "  Menu principal   ",
                "                   "});
        mainMenuButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::createMenu(hospital));});

        std::shared_ptr<Node2D> cards = std::make_shared<Node2D>("cardCont", COORD{0, 7});
        std::shared_ptr<std::vector<std::shared_ptr<doc::Doctor>>> loadedlist = std::make_shared<std::vector<std::shared_ptr<Doctor>>>();
        loadedlist->reserve(hospital.doctors.getListSize() * 2);
        
        for (unsigned int i = 0; i < hospital.doctors.getTotalIDXs(); i++){
            std::shared_ptr<doc::Doctor> doctor = hospital.doctors.getAtIDX(i);
            if (!doctor) continue;

            auto card = doctor->createCard(hospital.doctors);
            cards->addChild(card);

            card->setLocalPosition(COORD{static_cast<SHORT>(loadedlist->size() * 66), 0});

            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                // contextFlag = 2 (Contexto de Doctor), contextIDX = ID del doctor (i)
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 2, i));
            });

            card->getChild(1)->addChild(goToAppointments);
            loadedlist->push_back(doctor);
        }

        std::shared_ptr<NodeButton> createDoctor = std::make_shared<NodeButton>("createDoctor", COORD{21, 1}, Color::BLACK, Color::CYAN, std::vector<std::string>{
                "          ",
                "  Nuevo   ",
                "          "});
        
        createDoctor->setOnClick([&hospital, loadedlist, cards](){
            uint64_t newID = hospital.doctors.getTotalIDXs();
            loadedlist->emplace_back(std::make_unique<doc::Doctor>(newID));
            hospital.doctors.add(*(loadedlist->back()));
            
            auto card = loadedlist->back()->createCard(hospital.doctors);
            card->setLocalPosition(COORD{static_cast<SHORT>(66 * (cards->getChildCount())), 0});

            uint64_t i = newID;
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 2, i));
            });
            card->getChild(1)->addChild(goToAppointments);
            cards->addChild(card);
        });

        // Lógica de búsqueda (similar a Patient)
        std::shared_ptr<uint64_t> foundDoctorIDX = std::make_shared<uint64_t>(UINT64_MAX);
        std::shared_ptr<NodeButton> viewFoundCard = std::make_shared<NodeButton>("ViewFoundCard", COORD{75, 100}, Color::MAGENTA, Color::WHITE, std::vector<std::string>{"[ Ver Tarjeta ]"});
        viewFoundCard->setOnClick([&hospital, foundDoctorIDX](){
            if (*foundDoctorIDX != UINT64_MAX) {
                SceneManager::getInstance().changeScene(createSingleDoctorCardView(hospital, *foundDoctorIDX));
            }
        });

        std::shared_ptr<NodeUI> searchStatus = std::make_shared<NodeUI>("SearchStatus", COORD{32, 1}, std::vector<std::string>{"Busqueda:"});
        auto searchByID = std::make_shared<NodeButton>("SearchID", COORD{32, 2}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por ID"});
        auto searchByFName = std::make_shared<NodeButton>("SearchFName", COORD{32, 3}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por Nombre"});
        auto searchBySpecialty = std::make_shared<NodeButton>("SearchSpecialty", COORD{32, 4}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"Buscar por Especialidad"});
        
        auto performDoctorSearch = [loadedlist, searchStatus, foundDoctorIDX, viewFoundCard](const std::string& query, const std::string& type) {
            *foundDoctorIDX = UINT64_MAX; 
            for (auto doctor : *loadedlist){
                bool match = false;
                if (type == "ID" && std::to_string(doctor->getID()) == query) { match = true; } 
                else if (type == "FNAME" && std::string(doctor->getFirstName()).find(query) != std::string::npos) { match = true; } 
                else if (type == "SPECIALTY" && std::string(doctor->getSpecialty()).find(query) != std::string::npos) { match = true; }
        
                if (match) {
                    *foundDoctorIDX = doctor->getID();
                    searchStatus->set_text({"Busqueda: Encontrado: " + std::string(doctor->getFirstName()) + ", " + std::string(doctor->getLastName())});
                    viewFoundCard->setLocalPosition(COORD{80, 1}); 
                    return;
                }
            }
            searchStatus->set_text({"Busqueda: FALLIDA No se encontro coincidencia."});
            viewFoundCard->setLocalPosition(COORD{75, 100}); 
        };

        // Handlers
        searchByID->setOnClick([searchByID, performDoctorSearch](){
            std::string input = Input::getLineInput(searchByID->getGlobalPosition() + COORD{14, 0});
            performDoctorSearch(input, "ID");
        });
        searchByFName->setOnClick([searchByFName, performDoctorSearch](){
            std::string input = Input::getLineInput(searchByFName->getGlobalPosition() + COORD{18, 0});
            performDoctorSearch(input, "FNAME");
        });
        searchBySpecialty->setOnClick([searchBySpecialty, performDoctorSearch](){
            std::string input = Input::getLineInput(searchBySpecialty->getGlobalPosition() + COORD{24, 0});
            performDoctorSearch(input, "SPECIALTY");
        });

        // Slider
        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cards->getChildCount());
        cards->setProcessFunction([cards, dynamicOffset](double){ *dynamicOffset = (cards->getChildCount() -1)* 66;});
        auto slider = pui::hSlider(cards, dynamicOffset, 100, Color::CYAN, Color::CYAN, Color::BRIGHT_CYAN, Color::BRIGHT_CYAN);
        slider->setGlobalPosition(COORD{0, 5});

        root->setAtExitFunction([loadedlist](){loadedlist->clear();});

        root->addChild(searchStatus);
        root->addChild(searchByID);
        root->addChild(searchByFName);
        root->addChild(searchBySpecialty);
        root->addChild(viewFoundCard);
        root->addChild(mainMenuButton);
        root->addChild(createDoctor);
        root->addChild(cards);
        root->addChild(slider);
        return root;
    }
}

// ====================================================================
// --- Implementación del Namespace APP (Appointment) ---
// ====================================================================

namespace app {
    std::shared_ptr<Node> createMedicalRecordMenu(hos::Hospital &hospital, uint64_t appID, int contextFlag) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootMedicalRecord");
        
        std::shared_ptr<Appointment> apptPtr = hospital.appoinments.getAtIDX(appID);
        if (!apptPtr) { /* Manejar error */ return root; }
        
        uint64_t patID = apptPtr->getPatientID();
        uint64_t docID = apptPtr->getdoctorID();

        // Obtener el registro del paciente (para añadir el nuevo registro)
        std::shared_ptr<cfm::IndexedFile<app::MedicalRecord>> records = ptn::getMedicalRecord(hospital.folderName, patID);
        
        // Obtener la data del paciente y doctor para la UI
        std::shared_ptr<ptn::Patient> patientPTR = hospital.patients.getAtIDX(patID);
        std::shared_ptr<doc::Doctor> doctorPTR = hospital.doctors.getAtIDX(docID);
        if (!patientPTR || !doctorPTR) { /* Manejar error */ return root; }

        // Crear una nueva instancia de MedicalRecord (DTO)
        std::shared_ptr<MedicalRecord> newRecord = std::make_shared<MedicalRecord>(records->getTotalIDXs(), docID);
        
        // Sincronizar data con la cita
        newRecord->setDate(apptPtr->getDate());
        newRecord->setTime(apptPtr->getTime());
        newRecord->setCost(doctorPTR->getConsultationFee());
    
        // Header UI
        std::shared_ptr<NodePCT> header = std::make_shared<NodePCT>("Header", COORD{1, 1}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{
            "--- CREAR EXPEDIENTE MEDICO (ID Cita: " + std::to_string(appID) + ") ---",
            "Paciente: " + std::string(patientPTR->getFirstName()) + " " + std::string(patientPTR->getLastName()),
            "Doctor: " + std::string(doctorPTR->getLastName()),
            "Fecha: " + std::string(newRecord->getDate()) + " | Hora: " + std::string(newRecord->getTime()) + " | Costo: " + std::to_string(newRecord->getCost())
        });
    
        // Diagnosis Input
        auto diagButton = std::make_shared<NodeButton>("DiagBtn", COORD{1, 5}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"[ Diagnosis: (Click to Enter) ]"});
        diagButton->setOnClick([diagButton, newRecord](){
            std::string input = Input::getLineInput(diagButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setDiagnosis(input);
            diagButton->set_text({"[ Diagnosis: " + newRecord->getDiagnosis() + " ]"});
        });
        
        // Treatment Input
        auto treatButton = std::make_shared<NodeButton>("TreatBtn", COORD{1, 7}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"[ Treatment: (Click to Enter) ]"});
        treatButton->setOnClick([treatButton, newRecord](){
            std::string input = Input::getLineInput(treatButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setTreatment(input);
            treatButton->set_text({"[ Treatment: " + newRecord->getTreatment() + " ]"});
        });
        
        // Medications Input
        auto medButton = std::make_shared<NodeButton>("MedBtn", COORD{1, 9}, Color::BLACK, Color::BRIGHT_WHITE, std::vector<std::string>{"[ Medications: (Click to Enter) ]"});
        medButton->setOnClick([medButton, newRecord](){
            std::string input = Input::getLineInput(medButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setMedications(input);
            medButton->set_text({"[ Medications: " + newRecord->getMedications() + " ]"});
        });
    
        // --- Save and Finalize Button ---
        std::shared_ptr<NodeButton> saveAndFinalize = std::make_shared<NodeButton>("SaveBtn", COORD{1, 15}, Color::BRIGHT_GREEN, Color::BRIGHT_WHITE, std::vector<std::string>{"[ GUARDAR Y FINALIZAR CITA ]"});
        saveAndFinalize->setOnClick([&hospital, newRecord, records, appID, patID, docID, contextFlag](){
            // 1. Archivar el MedicalRecord (add to Patient's history list)
            records->add(*newRecord);
            
            // 2. Eliminar Cita
            hospital.appoinments.eraseAtIdx(appID);
    
            // 3. Volver al menú de citas con el contexto correcto
            uint64_t contextIDX = (contextFlag == 1) ? patID : docID;
            SceneManager::getInstance().changeScene(createMenu(hospital, contextFlag, contextIDX));
        });
        
        // --- Cancel Button ---
        std::shared_ptr<NodeButton> cancel = std::make_shared<NodeButton>("CancelBtn", COORD{35, 15}, Color::RED, Color::BRIGHT_WHITE, std::vector<std::string>{"[ CANCELAR ]"});
        cancel->setOnClick([&hospital, patID, docID, contextFlag](){
            // Volver sin guardar ni borrar la cita
            uint64_t contextIDX = (contextFlag == 1) ? patID : docID;
            SceneManager::getInstance().changeScene(createMenu(hospital, contextFlag, contextIDX));
        });
        
        root->addChild(header);
        root->addChild(diagButton);
        root->addChild(treatButton);
        root->addChild(medButton);
        root->addChild(saveAndFinalize);
        root->addChild(cancel);

        return root;
    }

    
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital, int contextFlag, uint64_t contextIDX) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootAppoinments");
        
        // Almacena el ID del Paciente seleccionado al crear una cita en el contexto global (contextFlag == 0)
        // Opcionalmente, se podría usar para almacenar el ID del Doctor en otros contextos.
        std::shared_ptr<uint64_t> contextIDX_p = std::make_shared<uint64_t>(UINT64_MAX); 
        
        // --- Botón de Volver (Back Button) ---
        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{91, 0}, Color::BLACK, Color::RED, std::vector<std::string>{
            "       ",
            " Volver",
            "       "});
            
        // Lógica para determinar a qué menú volver.
        if (contextFlag == 1) { // Vuelve al Menú de Pacientes
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
            });
        } else if (contextFlag == 2) { // Vuelve al Menú de Doctores
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(doc::createMenu(hospital));
            });
        } else { // Vuelve al Menú Principal del Hospital (contextFlag == 0)
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(hos::createMenu(hospital));
            });
        }
    
        // --- Contenedores de Citas y Datos ---
        std::shared_ptr<std::vector<std::unique_ptr<app::Appointment>>> loadedList = 
            std::make_shared<std::vector<std::unique_ptr<app::Appointment>>>();
        std::shared_ptr<Node2D> cards = std::make_shared<Node2D>("Cards", COORD{2, 6});
        
        // Evita la reasignación constante durante la carga inicial si el tamaño es conocido.
        loadedList->reserve(hospital.appoinments.getListSize() * 2 + 5); 
    
        // --- Lógica de Carga de Citas (Dependiendo del Contexto) ---
        std::vector<uint64_t> indicesA_cargar;
    
        if (contextFlag == 1) { // Citas de un Paciente Específico
            auto idxs = getSubAppoinmentIndex(hospital.folderName, "Patients", contextIDX);
            for (unsigned int i = 0; i < idxs->getNextIDX(); i++) {
                indicesA_cargar.push_back(idxs->getPosOfIDX(i));
            }
        } else if (contextFlag == 2) { // Citas de un Doctor Específico
            auto idxs = getSubAppoinmentIndex(hospital.folderName, "Doctors", contextIDX);
            for (unsigned int i = 0; i < idxs->getNextIDX(); i++) {
                indicesA_cargar.push_back(idxs->getPosOfIDX(i));
            }
        } else { // Citas Globales (Todas)
            for (unsigned int i = 0; i < hospital.appoinments.getTotalIDXs(); i++) {
                indicesA_cargar.push_back(i);
            }
        }
    
        // --- Procesamiento de Carga y Creación de Tarjetas (Cards) ---
        for (uint64_t idx : indicesA_cargar) {
            auto appoinment = hospital.appoinments.getAtIDX(idx);
            
            // Verifica si la cita existe y está cargada.
            if (!appoinment) continue; 
    
            auto card = appoinment->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
            
            // Si la tarjeta no se pudo crear (El paciente o doctor fueron eliminados), borra la cita y continua.
            if (!card) {
                appoinment->erasefrom(hospital.appoinments); 
                continue;
            }
    
            cards->addChild(card);
            
            // Botón para finalizar la cita (Crear Registro Médico)
            std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43, 8}, 
                Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{"[ Archivar ]"});
            
            finalizeAppointment->setOnClick([&hospital, idx, contextFlag]() {
                // Navega a la escena de Creación de Registro Médico.
                SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, idx, contextFlag));
            });
    
            // Asume que el hijo 1 de la tarjeta es el contenedor de botones/acciones
            card->getChild(1)->addChild(finalizeAppointment);
            
            // Posiciona la tarjeta en la lista
            card->setLocalPosition(COORD{0, static_cast<SHORT>(loadedList->size() * 12)});
            
            // Mueve la propiedad única de la cita a la lista de citas cargadas para mantenerla viva
            loadedList->push_back(std::move(appoinment));
        }
    
    
        // --- DOCTOR SELECTOR DIALOG SETUP (Para crear una nueva cita) ---
        std::shared_ptr<NodeBox> doctorSelector = std::make_shared<NodeBox>("selectDoctor", COORD{300, 300}, COORD{26, 40}, Color::CYAN);
        doctorSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_CYAN, Color::CYAN, std::vector<std::string>{"Doctores"}));
        
        int c = 0;
        for (unsigned int i = 0; i < hospital.doctors.getTotalIDXs(); i++) {
            auto doctor = hospital.doctors.getAtIDX(i);
            if (!doctor) continue;
            uint64_t currID = doctor->getID();
    
            auto doctortN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(c + 1)}, Color::BRIGHT_CYAN, Color::CYAN, 
                std::vector<std::string>{doctor->getFirstName()});
            
            // Lógica al seleccionar un Doctor:
            // * Si contextFlag == 1: El paciente está definido (contextIDX), solo se necesita el doctor (currID).
            // * Si contextFlag == 0: El paciente ya fue seleccionado en el selector de pacientes (contextIDX_p), solo se necesita el doctor (currID).
            doctortN->setOnClick([cards, doctorSelector, &hospital, currID, contextFlag, contextIDX_p, contextIDX, loadedList, i]() {
                uint64_t idPaciente = (contextFlag == 0) ? *contextIDX_p : contextIDX;
                uint64_t idDoctor   = (contextFlag == 2) ? contextIDX : currID;
    
                // 1. Crear la nueva Cita
                loadedList->emplace_back(std::make_unique<app::Appointment>(hospital.appoinments.getTotalIDXs(), idPaciente, idDoctor));
                
                // 2. Agregar a todas las listas necesarias
                uint64_t newAppID = hospital.appoinments.add(*loadedList->back());
                
                // Agregar el índice de la nueva cita al archivo del Paciente
                auto pApps = getSubAppoinmentIndex(hospital.folderName, "Patients", idPaciente);
                pApps->getSpaces().push(newAppID);
                pApps->addToIndex();
                
                // Agregar el índice de la nueva cita al archivo del Doctor
                auto dApps = getSubAppoinmentIndex(hospital.folderName, "Doctors", idDoctor);
                dApps->getSpaces().push(newAppID);
                dApps->addToIndex();
    
                // 3. Crear y agregar la tarjeta (Card) de la UI
                auto card = loadedList->back()->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                cards->addChild(card);
                
                // Posiciona la nueva tarjeta al final de la lista
                card->setLocalPosition(COORD{ 0, static_cast<SHORT>((loadedList->size() - 1) * 12)});
                
                // Botón de Archivar para la nueva cita
                std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, 
                    Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{"[ Archivar ]"});
    
                finalizeAppointment->setOnClick([&hospital, newAppID, contextFlag]() {
                    SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, newAppID, contextFlag));
                });
    
                card->getChild(1)->addChild(finalizeAppointment);
                
                // 4. Ocultar el selector de doctor
                doctorSelector->setLocalPosition({300, 300}); 
            });
            
            doctorSelector->addChild(doctortN);
            c++;
        }
    
    
        // --- PATIENT SELECTOR DIALOG SETUP (Para crear una nueva cita) ---
        std::shared_ptr<NodeBox> patientSelector = std::make_shared<NodeBox>("selectPatient", COORD{300, 300}, COORD{26, 40}, Color::YELLOW);
        patientSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{"Pacientes"}));
        c = 0;
        for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++) {
            auto patient = hospital.patients.getAtIDX(i);
            if (!patient) continue;
            uint64_t currID = patient->getID();
            
            auto patientN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(c + 1)}, Color::BRIGHT_YELLOW, Color::YELLOW, 
                std::vector<std::string>{patient->getFirstName()});
            
            if (contextFlag == 0) {
                // Contexto Global: Al seleccionar paciente, guarda su ID y muestra el selector de Doctor.
                patientN->setOnClick([patientSelector, contextIDX_p, currID, doctorSelector]() {
                    *contextIDX_p = currID; // Guarda el ID del paciente seleccionado.
                    patientSelector->setLocalPosition({300, 300}); // Oculta el selector de paciente
                    doctorSelector->setGlobalPosition(COORD{64, 0}); // Muestra el selector de Doctor
                });
            } else { // contextFlag == 2 (Contexto Doctor Específico)
                // Contexto Doctor: Al seleccionar paciente, crea la cita inmediatamente con el doctor (contextIDX) y el paciente (currID).
                patientN->setOnClick([cards, patientSelector, &hospital, contextIDX, contextFlag, loadedList, currID, i]() {
                    uint64_t idPaciente = currID;
                    uint64_t idDoctor   = contextIDX;
    
                    // 1. Crear la nueva Cita
                    loadedList->emplace_back(std::make_unique<app::Appointment>(hospital.appoinments.getTotalIDXs(), idPaciente, idDoctor));
                    
                    // 2. Agregar a todas las listas necesarias
                    uint64_t newAppID = hospital.appoinments.add(*loadedList->back());
                    
                    // Agregar el índice de la nueva cita al archivo del Paciente
                    auto pApps = getSubAppoinmentIndex(hospital.folderName, "Patients", idPaciente);
                    pApps->getSpaces().push(newAppID);
                    pApps->addToIndex();
                    
                    // Agregar el índice de la nueva cita al archivo del Doctor
                    auto dApps = getSubAppoinmentIndex(hospital.folderName, "Doctors", idDoctor);
                    dApps->getSpaces().push(newAppID);
                    dApps->addToIndex();
                    
                    // 3. Crear y agregar la tarjeta (Card) de la UI
                    auto card = loadedList->back()->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                    cards->addChild(card);
                    
                    // Posiciona la nueva tarjeta al final de la lista
                    card->setLocalPosition(COORD{ 0, static_cast<SHORT>((loadedList->size() - 1) * 12)});
                    
                    // Botón de Archivar para la nueva cita
                    std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43, 8}, 
                        Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{"[ Archivar ]"});
                    
                    finalizeAppointment->setOnClick([&hospital, newAppID, contextFlag]() {
                        SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, newAppID, contextFlag));
                    });
                    
                    card->getChild(1)->addChild(finalizeAppointment);
                    
                    // 4. Ocultar el selector de paciente
                    patientSelector->setLocalPosition({300, 300}); 
                });
            }
            
            patientSelector->addChild(patientN);    
            c++;
        }
    
    
        // --- BOTÓN CREAR CITA (Nuevo) ---
        std::shared_ptr<NodeButton> createAppoinment = std::make_shared<NodeButton>("createAppoinment", COORD{91, 3}, Color::BLACK, Color::GREEN, std::vector<std::string>{
            "       ",
            " Nuevo ",
            "       "}); 
        
        createAppoinment->setOnClick([patientSelector, doctorSelector, contextFlag]() {
            // En contexto Paciente (contextFlag == 1): Se debe seleccionar un Doctor.
            if (contextFlag == 1) {
                patientSelector->setGlobalPosition(COORD{300, 300}); // Oculta por si estaba visible
                doctorSelector->setGlobalPosition(COORD{64, 0}); // Muestra el selector de Doctor
            }
            // En contexto Doctor (contextFlag == 2) o Global (contextFlag == 0): Se debe seleccionar un Paciente.
            else if (contextFlag == 2 || contextFlag == 0) {
                doctorSelector->setGlobalPosition(COORD{300, 300}); // Oculta por si estaba visible
                patientSelector->setGlobalPosition(COORD{64, 0}); // Muestra el selector de Paciente
            }
        });
    
    
        // --- Slider para la Lista de Citas (Scroll) ---
        // El offset dinámico es esencial para que el slider funcione con una lista que crece
        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cards->getChildCount());
        cards->setProcessFunction([cards, dynamicOffset](double){ 
            // Actualiza el límite del scroll (offset) al tamaño total de las tarjetas * 12 (altura de cada tarjeta)
            *dynamicOffset = (cards->getChildCount() > 0) ? (cards->getChildCount() - 1) * 12 : 0;
        });
        auto slider = pui::vSlider(cards, dynamicOffset, 40, Color::GREEN, Color::GREEN, Color::BRIGHT_GREEN, Color::BRIGHT_GREEN);
    
    
        // --- Limpieza al Salir (AtExit) ---
        // Función para liberar la memoria de la lista de citas cargadas al salir de la escena
        root->setAtExitFunction([loadedList, contextIDX_p]() {
            loadedList->clear(); 
            *contextIDX_p = 0; // Reiniciar el contexto de paciente/doctor
        });
        
        // --- Jerarquía de Nodos ---
        root->addChild(cards);          // Contenedor de las tarjetas de citas
        root->addChild(slider);         // Barra de desplazamiento
        root->addChild(doctorSelector); // Diálogo selector de doctor
        root->addChild(patientSelector);// Diálogo selector de paciente
        root->addChild(createAppoinment);// Botón "Nuevo"
        root->addChild(backButton);     // Botón "Volver"
        
        return root;
    }
}