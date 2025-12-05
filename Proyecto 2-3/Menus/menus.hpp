#ifndef MENUS_HPP
#define MENUS_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Hospital/hospital.hpp"
#include "../MedicalRecord/medicalRecord.hpp"

std::shared_ptr<cfm::Index>  getSubAppoinmentIndex(std::string folder, std::string subfolder, uint64_t id){
    cfm::Folder Fold(folder + "/" + subfolder + "/" + std::to_string(id));
    std::shared_ptr<cfm::Index> index = std::make_shared<cfm::Index>(folder + "/" + subfolder + "/" + std::to_string(id));
    return index;
}


namespace ptn{
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital);

    // std::shared_ptr<cfm::IndexedFile<app::Appointment>> getAppoinmentList(const Patient &patient){
    //     std::shared_ptr<cfm::IndexedFile<app::Appointment>>
    // }
}
namespace doc{
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital);

    std::shared_ptr<cfm::Index>  getAppoinmentList(const Doctor &doctor){
        cfm::Folder folder(std::to_string(doctor.getID()));
        std::shared_ptr<cfm::Index> index = std::make_shared<cfm::Index>(std::to_string(doctor.getID()));
        return index;
    }
}
namespace app{
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital, int contextFlag, uint64_t contextIDX);
}

namespace hos{
    std::shared_ptr<Node> createMenu(Hospital &hospital){
        std::shared_ptr<Node> root = std::make_shared<Node>("Root");

        std::shared_ptr<Node2D> headersContainer = std::make_shared<Node2D>("headersContainer", COORD{1, 1});
    
        std::shared_ptr<NodePCT> hospitalName = std::make_shared<NodePCT>("name", COORD{0,0}, Color::BRIGHT_WHITE, Color::BLACK, std::vector<std::string>{
            hospital.name,
            hospital.address,
            hospital.phone
        });
    
        std::shared_ptr<Node2D> buttonsContainer = std::make_shared<Node2D>("buttonContainer", COORD{5, 6});
        std::shared_ptr<NodeButton> pacientsButton = std::make_shared<NodeButton>("pacientsButton", COORD{0, 0}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
            ".-----------.",
            "| Pacientes |",
            "'-----------'"});
        pacientsButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
        });
        std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 0}, Color::CYAN, Color::BLACK, std::vector<std::string>{
            ".-----------.",
            "|  Doctors  |",
            "'-----------'"});
        doctorsButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(doc::createMenu(hospital));
        });
        std::shared_ptr<NodeButton> appoinmentButton = std::make_shared<NodeButton>("doctorsButton", COORD{40, 0}, Color::GREEN, Color::BLACK, std::vector<std::string>{
            ".-----------.",
            "|   Citas   |",
            "'-----------'"});
            appoinmentButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(app::createMenu(hospital, 0, 0));
        });

        root->addChild(headersContainer);
            headersContainer->addChild(hospitalName);
        root->addChild(buttonsContainer);
            buttonsContainer->addChild(pacientsButton);
            buttonsContainer->addChild(doctorsButton);
            buttonsContainer->addChild(appoinmentButton);
        return root;
    }
}

namespace ptn{
    std::unique_ptr<cfm::IndexedFile<app::MedicalRecord>>  getMedicalRecord(std::string folder, uint64_t ptnID){
        cfm::Folder Fold(folder + "/Patients/" + std::to_string(ptnID));
        std::unique_ptr<cfm::IndexedFile<app::MedicalRecord>> record = std::make_unique<cfm::IndexedFile<app::MedicalRecord>>(folder + "/Patients/" + std::to_string(ptnID));
        return record;
    }

    std::shared_ptr<Node> createMedicalRecordViewer(hos::Hospital &hospital, const Patient &patient){
        std::shared_ptr<Node> root = std::make_shared<Node>("RootRecordsViewer");
        
        std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
            "--- EXPEDIENTE MEDICO DE: " + std::string(patient.getFirstName()) + " " + std::string(patient.getLastName()) + " ---"
        });
        
        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{0, 3}, Color::RED, Color::BLACK, std::vector<std::string>{"[ VOLVER ]"});
        backButton->setOnClick([&hospital](){
            SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
        });
        
        std::shared_ptr<Node2D> recordCont = std::make_shared<Node2D>("RecordCont", COORD{1, 5});
        
        // Display all records
        auto list = getMedicalRecord(hospital.folderName, patient.getID());
        for (unsigned int i = 0; i < list->getTotalIDXs(); i++){
            auto record = list->getAtIDX(i);
            if (!record) continue;
            auto recordCard = std::make_shared<NodeUI>("RecCard" + std::to_string(i), COORD{0, SHORT(i * 7)}, std::vector<std::string>{
                "ID: " + std::to_string(record->getId()) + " | Doctor ID: " + std::to_string(record->getDoctorId()),
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

    std::shared_ptr<Node> createMenu(hos::Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");

        std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::RED, Color::BLACK, std::vector<std::string>{
                ".----------------.",
                "| Menu principal |",
                "'----------------'"});
        mainMenuButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::createMenu(hospital));});

        std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});
        std::shared_ptr<std::vector<std::shared_ptr<ptn::Patient>>> loadedlist = std::make_shared<std::vector<std::shared_ptr<ptn::Patient>>>();
        loadedlist->reserve(hospital.patients.getListSize() * 1.5);
        
        for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++){
            std::shared_ptr<ptn::Patient> patient = hospital.patients.getAtIDX(i);
            if (!patient) continue;

            cardCont->addChild(patient->createCard(hospital.patients));
            std::static_pointer_cast<Node2D>(cardCont->getChild(loadedlist->size()))->setLocalPosition(COORD{static_cast<SHORT>(loadedlist->size() * 66), 0});
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 1, i));
            });
            patient->getCard().lock()->getChild(1)->addChild(goToAppointments);
            auto goToRecords = std::make_shared<NodeButton>("checkRecords", COORD{5, 12}, Color::CYAN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[ Ver Expediente ]"});
            goToRecords->setOnClick([&hospital, patient](){
                // The current patient menu will be the back scene
                SceneManager::getInstance().changeScene(createMedicalRecordViewer(hospital, *patient)); 
            });
            patient->getCard().lock()->getChild(1)->addChild(goToRecords);
            loadedlist->push_back(patient);
        }
        //extender loadedList a toda la vida de la escena
        root->setAtExitFunction([loadedlist](){loadedlist->clear();});

        std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{21, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
            ".-------.",
            "| Nuevo |",
            "'-------'"});
        
        createPatient->setOnClick([&hospital, loadedlist, cardCont](){
            loadedlist->emplace_back(std::make_unique<ptn::Patient>(hospital.patients.getTotalIDXs()));
            hospital.patients.add(*(loadedlist->back()));
            auto card = loadedlist->back()->createCard(hospital.patients);

            card->setLocalPosition(COORD{(SHORT)(66 * cardCont->getChildCount()), 0});

            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 12}, Color::BRIGHT_GREEN, Color::BRIGHT_YELLOW, std::vector<std::string>{"[Ver citas]"});
            uint64_t i = loadedlist->size() - 1;
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 1, i));
            });
            card->getChild(1)->addChild(goToAppointments);
            cardCont->addChild(card);
        });

        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cardCont->getChildCount());
        cardCont->setProcessFunction([cardCont, dynamicOffset](double){ *dynamicOffset = (cardCont->getChildCount() -1)* 66;});

        auto slider = pui::hSlider(cardCont, dynamicOffset);
        slider->setGlobalPosition(COORD{0, 5});

        root->addChild(mainMenuButton);
        root->addChild(createPatient);
        root->addChild(cardCont);
        root->addChild(slider);
        return root;
    }
}

namespace doc{
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital){
        std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");

        std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::RED, Color::BLACK, std::vector<std::string>{
                ".----------------.",
                "| Menu principal |",
                "'----------------'"});
        mainMenuButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::createMenu(hospital));});

        std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});
        std::shared_ptr<std::vector<std::unique_ptr<doc::Doctor>>> loadedlist = std::make_shared<std::vector<std::unique_ptr<Doctor>>>();
        loadedlist->reserve(hospital.doctors.getListSize() * 1.5);
        
        for (unsigned int i = 0; i < hospital.doctors.getTotalIDXs(); i++){
            auto doctor = hospital.doctors.getAtIDX(i);
            if (!doctor) continue;

            cardCont->addChild(doctor->createCard(hospital.doctors));
            std::static_pointer_cast<Node2D>(cardCont->getChild(loadedlist->size()))->setLocalPosition(COORD{static_cast<SHORT>(loadedlist->size() * 66), 0});
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 2, i));
            });
            doctor->getCard().lock()->getChild(1)->addChild(goToAppointments);
            loadedlist->push_back(std::move(doctor));
        }
        //extender loadedList a toda la vida de la escena
        root->setAtExitFunction([loadedlist](){loadedlist->clear();});

        std::shared_ptr<NodeButton> createDoctor = std::make_shared<NodeButton>("createDoctor", COORD{21, 1}, Color::CYAN, Color::BLACK, std::vector<std::string>{
            ".-------.",
            "| Nuevo |",
            "'-------'"});
        
        createDoctor->setOnClick([&hospital, loadedlist, cardCont](){
            loadedlist->emplace_back(std::make_unique<doc::Doctor>(hospital.doctors.getTotalIDXs()));
            hospital.doctors.add(*(loadedlist->back()));
            auto card = loadedlist->back()->createCard(hospital.doctors);
            card->setLocalPosition(COORD{(SHORT)(66 * cardCont->getChildCount()), 0});
            uint64_t i = loadedlist->size() - 1;
            auto goToAppointments = std::make_shared<NodeButton>("checkAppointments", COORD{-6, 10}, Color::BRIGHT_GREEN, Color::BRIGHT_CYAN, std::vector<std::string>{"[Ver citas]"});
            goToAppointments->setOnClick([&hospital, i](){
                SceneManager::getInstance().changeScene(app::createMenu(hospital, 2, i));
            });
            card->getChild(1)->addChild(goToAppointments);
            cardCont->addChild(card);
        });
    

        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cardCont->getChildCount());
        cardCont->setProcessFunction([cardCont, dynamicOffset](double){ *dynamicOffset = (cardCont->getChildCount() -1)* 66;});

        auto slider = pui::hSlider(cardCont, dynamicOffset);
        slider->setGlobalPosition(COORD{0, 5});

        root->addChild(mainMenuButton);
        root->addChild(createDoctor);
        root->addChild(cardCont);
        root->addChild(slider);
        return root;
    }
}

namespace app{
    std::shared_ptr<Node> createMedicalRecordMenu(hos::Hospital &hospital, uint64_t appID, int contextFlag){
        std::shared_ptr<Node> root = std::make_shared<Node>("RootMedicalRecord");
        
        std::shared_ptr<Appointment> apptPtr = hospital.appoinments.getAtIDX(appID);
        uint64_t patID = apptPtr->getPatientID();
        uint64_t docID = apptPtr->getdoctorID();

        std::shared_ptr<cfm::IndexedFile<app::MedicalRecord>> records = ptn::getMedicalRecord(hospital.folderName, patID);
        
        std::unique_ptr<ptn::Patient> patientPTR = hospital.patients.getAtIDX(patID);
        std::unique_ptr<doc::Doctor> doctorPTR = hospital.doctors.getAtIDX(docID);
        
        // Create a new MedicalRecord struct to hold the data being entered
        std::shared_ptr<MedicalRecord> newRecord = std::make_shared<MedicalRecord>(records->getTotalIDXs(), docID);
        
        // Capture necessary metadata from the Appointment and Doctor
        newRecord->setDate(apptPtr->getDate());
        newRecord->setTime(apptPtr->getTime());
        
        // Set the cost from the Doctor's consultationFee
        newRecord->setCost(doctorPTR->getConsultationFee());
    
        std::shared_ptr<NodeUI> header = std::make_shared<NodeUI>("Header", COORD{1, 1}, std::vector<std::string>{
            "--- CREAR EXPEDIENTE MEDICO ---",
            "Paciente: " + std::string(patientPTR->getFirstName()) + " " + std::string(patientPTR->getLastName()),
            "Doctor: " + (doctorPTR ? std::string(doctorPTR->getLastName()) : "N/A"),
            "Fecha: " + std::string(newRecord->getDate()) + " | Hora: " + std::string(newRecord->getTime()) + " | Costo: " + std::to_string(newRecord->getCost())
        });
    
        // Diagnosis Input
        auto diagButton = std::make_shared<NodeButton>("DiagBtn", COORD{1, 5}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Diagnosis: (Click to Enter) ]"});
        diagButton->setOnClick([diagButton, newRecord](){
            std::string input = Input::getLineInput(diagButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setDiagnosis(input);
            diagButton->set_text({"[ Diagnosis: " + input + " ]"});
        });
        
        // Treatment Input
        auto treatButton = std::make_shared<NodeButton>("TreatBtn", COORD{1, 7}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Treatment: (Click to Enter) ]"});
        treatButton->setOnClick([treatButton, newRecord](){
            std::string input = Input::getLineInput(treatButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setTreatment(input);
            treatButton->set_text({"[ Treatment: " + input + " ]"});
        });
        
        // Medications Input
        auto medButton = std::make_shared<NodeButton>("MedBtn", COORD{1, 9}, Color::WHITE, Color::BLACK, std::vector<std::string>{"[ Medications: (Click to Enter) ]"});
        medButton->setOnClick([medButton, newRecord](){
            std::string input = Input::getLineInput(medButton->getGlobalPosition() + COORD{32, 0});
            newRecord->setMedications(input);
            medButton->set_text({"[ Medications: " + input + " ]"});
        });
    
        // --- Save and Finalize Button ---
        std::shared_ptr<NodeButton> saveAndFinalize = std::make_shared<NodeButton>("SaveBtn", COORD{1, 15}, Color::BRIGHT_GREEN, Color::BLACK, std::vector<std::string>{"[ GUARDAR Y FINALIZAR CITA ]"});
        saveAndFinalize->setOnClick([&hospital, newRecord, records, appID, patID, docID, contextFlag](){
            // 1. Archive the new MedicalRecord (add to Patient's history list)
            records->add(*newRecord);
            
            // 2. Perform Appointment Cleanup
            hospital.appoinments.eraseAtIdx(appID);
    
            // --- 3. Return to the correct scene using the flag ---
            
            uint64_t contextIDX;
            if (contextFlag == 1) { // Patient View
                contextIDX = patID;
            } else if (contextFlag == 2) { // Doctor View
                contextIDX = docID;
            }
    
            // Safety Fix: Recreate the appointments scene, ensuring fresh state
            SceneManager::getInstance().changeScene(createMenu(hospital, contextFlag, contextIDX));
        });
        // --- Cancel Button ---
    std::shared_ptr<NodeButton> cancel = std::make_shared<NodeButton>("CancelBtn", COORD{35, 15}, Color::RED, Color::BLACK, std::vector<std::string>{"[ CANCELAR ]"});
    cancel->setOnClick([&hospital, root, patID, docID, contextFlag](){
        uint64_t contextIDX;
        if (contextFlag == 1) { // Patient View
            contextIDX = patID;
        } else if (contextFlag == 2) { // Doctor View
            contextIDX = docID;
        }
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

    std::shared_ptr<Node> createMenu(hos::Hospital &hospital, int contextFlag, uint64_t contextIDX){
        std::shared_ptr<Node> root = std::make_shared<Node>("RootAppoinments");
        std::shared_ptr<uint64_t> contextIDX_p = std::make_shared<uint64_t>(UINT64_MAX); 
        // 0: Global Appointments List (If both are NULL)
        // 1: Patient-Specific Appointments List
        // 2: Doctor-Specific Appointments  List
        // Determine the return scene based on context
        std::shared_ptr<NodeButton> backButton = std::make_shared<NodeButton>("backButton", COORD{40, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
            ".--------.",
            "| Volver |",
            "'--------'"});
            
        std::shared_ptr<std::vector<std::unique_ptr<app::Appointment>>> loadedList = std::make_shared<std::vector<std::unique_ptr<app::Appointment>>>();
        std::shared_ptr<Node2D> cards = std::make_shared<Node2D>("Card", COORD{1, 6});
        loadedList->reserve(hospital.appoinments.getListSize() * 1.5);
        // Determine the return scene creation based on context
        if (contextFlag == 1) {
            // Return to Patient Menu
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(ptn::createMenu(hospital));
            });
            auto idxs = getSubAppoinmentIndex(hospital.folderName, "Patients", contextIDX);
            //Load appoinments in the subset of appoiments from the patient
            for (unsigned int i = 0; i < idxs->getNextIDX(); i++){
                auto idx = idxs->getPosOfIDX(i);
                auto appoinment = hospital.appoinments.getAtIDX(idx);
                if (!appoinment) continue;

                auto card = appoinment->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                cards->addChild(card);
                std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{
                    "[ Archivar ]"});
                finalizeAppointment->setOnClick([&hospital, idx, contextFlag](){
                    SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, idx, contextFlag));
                });
                card->getChild(1)->addChild(finalizeAppointment);
                std::static_pointer_cast<Node2D>(cards->getChild(loadedList->size()))->setLocalPosition( COORD{0, static_cast<SHORT>(loadedList->size() * 12)});
                loadedList->push_back(std::move(appoinment));
            }
        } else if (contextFlag == 2) {
            // Return to Doctor Menu
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(doc::createMenu(hospital));
            });
            auto idxs = getSubAppoinmentIndex(hospital.folderName, "Doctors", contextIDX);
            //Load appoinments in the subset of appoiments from the doctor
            for (unsigned int i = 0; i < idxs->getNextIDX(); i++){
                auto idx = idxs->getPosOfIDX(i);
                auto appoinment = hospital.appoinments.getAtIDX(idx);
                if (!appoinment) continue;

                auto card = appoinment->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                cards->addChild(card);
                std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{
                    "[ Archivar ]"});
                finalizeAppointment->setOnClick([&hospital, idx, contextFlag](){
                    SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, idx, contextFlag));
                });
                card->getChild(1)->addChild(finalizeAppointment);
                std::static_pointer_cast<Node2D>(cards->getChild(loadedList->size()))->setLocalPosition( COORD{0, static_cast<SHORT>(loadedList->size() * 12)});
                loadedList->push_back(std::move(appoinment));
            }
        } else {
            backButton->setOnClick([&hospital](){
                SceneManager::getInstance().changeScene(hos::createMenu(hospital));
            });
            //Load all appoiments
            for (unsigned int i = 0; i < hospital.appoinments.getTotalIDXs(); i++){
                auto appoinment = hospital.appoinments.getAtIDX(i);
                if (!appoinment) continue;

                auto card = appoinment->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                cards->addChild(card);
                std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{
                    "[ Archivar ]"});
                    finalizeAppointment->setOnClick([&hospital, i, contextFlag](){
                    SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, i, contextFlag));
                });
                card->getChild(1)->addChild(finalizeAppointment);
                std::static_pointer_cast<Node2D>(cards->getChild(loadedList->size()))->setLocalPosition( COORD{0, static_cast<SHORT>(loadedList->size() * 12)});
                loadedList->push_back(std::move(appoinment));
            }
        }

        // --- DOCTOR SELECTOR DIALOG SETUP ---
        std::shared_ptr<NodeSQ> doctorSelector = std::make_shared<NodeSQ>("selectDoctor", COORD{300, 300}, COORD{26, 40}, Color::CYAN, Color::CYAN);
        doctorSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_CYAN, Color::CYAN, std::vector<std::string>{"Doctores"}));
        
        for (unsigned int i = 0; i < hospital.doctors.getTotalIDXs(); i++){
            auto doctor = hospital.doctors.getAtIDX(i);
            if (!doctor) continue;
            uint64_t currID = doctor->getID();

            auto doctortN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(i + 1)}, Color::BRIGHT_CYAN, Color::CYAN, std::vector<std::string>{doctor->getFirstName()});
            doctortN->setOnClick([cards, doctorSelector, &hospital, i, contextFlag, contextIDX_p, loadedList, currID](){
                // 1. Create the new Appointment with the selected Doctor and the context Patient
                loadedList->emplace_back(std::make_unique<Appointment>(hospital.appoinments.getTotalIDXs(), *contextIDX_p, i));
                
                // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
                uint64_t newAppID = hospital.appoinments.add(*loadedList->back());
                //Add the index of the new Appoiments to the ids file of the Patient
                auto pApps = getSubAppoinmentIndex(hospital.folderName, "Patients", *contextIDX_p);
                pApps->getSpaces().push(loadedList->back()->getID());
                pApps->addToIndex();
                //Add the index of the new Appoiments to the ids file of the Doctor
                auto dApps = getSubAppoinmentIndex(hospital.folderName,"Doctors", currID);
                dApps->getSpaces().push(loadedList->back()->getID());
                dApps->addToIndex();

                // // 3. Create and add the UI card
                auto card  = loadedList->back()->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                cards->addChild(card);
                card->setLocalPosition(COORD{ 0, static_cast<SHORT>((loadedList->size()-1) * 12)});
                std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{
                    "[ Archivar ]"});
                finalizeAppointment->setOnClick([&hospital, newAppID, contextFlag](){
                    SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, newAppID, contextFlag));
                });
                card->getChild(1)->addChild(finalizeAppointment);
                // 4. Hide the selector
                doctorSelector->setLocalPosition({300, 300}); 
            });
            doctorSelector->addChild( doctortN );
        }
        
        // --- PATIENT SELECTOR DIALOG SETUP ---
        std::shared_ptr<NodeSQ> patientSelector = std::make_shared<NodeSQ>("selectPatient", COORD{300, 300}, COORD{26, 40}, Color::YELLOW, Color::YELLOW);
        patientSelector->addChild(std::make_shared<NodePCT>("Boni", COORD{8,0}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{"Pacientes"}));
        
        for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++){
            auto patient = hospital.patients.getAtIDX(i);
            if (!patient) continue;
            uint64_t currID = patient->getID();
            
            auto patientN = std::make_shared<NodeButton>("px", COORD{(SHORT)1, (SHORT)(i + 1)}, Color::BRIGHT_YELLOW, Color::YELLOW, std::vector<std::string>{patient->getFirstName()});
            if (contextFlag == 0){
                patientN->setOnClick([patientSelector, contextIDX_p, currID, doctorSelector](){
                    *contextIDX_p = currID;
                    patientSelector->setLocalPosition({300, 300}); // Hide the selector
                    doctorSelector->setGlobalPosition(COORD{65, 0}); // Show Doctor Selector
                });
            } else{
                patientN->setOnClick([cards, patientSelector, &hospital, i, contextIDX, contextFlag, loadedList, currID](){
                    // 1. Create the new Appointment with the selected Patient and the context Doctor
                    loadedList->emplace_back(std::make_unique<Appointment>(hospital.appoinments.getTotalIDXs(), contextIDX, i));
                    
                    // 2. Add to all necessary lists (Hospital main list, Doctor's list, Patient's list)
                    uint64_t newAppID = hospital.appoinments.add(*loadedList->back());
                    //Add the index of the new Appoiments to the ids file of the Patient
                    auto pApps = getSubAppoinmentIndex(hospital.folderName, "Patients", currID);
                    pApps->getSpaces().push(loadedList->back()->getID());
                    pApps->addToIndex();
                    //Add the index of the new Appoiments to the ids file of the Doctor
                    auto dApps = getSubAppoinmentIndex(hospital.folderName,"Doctors", contextIDX);
                    dApps->getSpaces().push(loadedList->back()->getID());
                    dApps->addToIndex();
                    // 3. Create and add the UI card
                    auto card  = loadedList->back()->createCard(hospital.appoinments, hospital.patients, hospital.doctors, contextFlag);
                    cards->addChild(card);
                    card->setLocalPosition(COORD{ 0, static_cast<SHORT>((loadedList->size()-1) * 12)});
                    std::shared_ptr<NodeButton> finalizeAppointment = std::make_shared<NodeButton>("finalizeAppointment", COORD{43,8}, Color::BRIGHT_WHITE, Color::BRIGHT_GREEN, std::vector<std::string>{
                        "[ Archivar ]"});
                    finalizeAppointment->setOnClick([&hospital, newAppID, contextFlag](){
                        SceneManager::getInstance().changeScene(createMedicalRecordMenu(hospital, newAppID, contextFlag));});
                    card->getChild(1)->addChild(finalizeAppointment);
                    // 4. Hide the selector
                    patientSelector->setLocalPosition({300, 300}); 
                });
            }
            patientSelector->addChild( patientN );   
        }
        
        // --- CREATE APPOINTMENT BUTTON (Nuevo) ---
        std::shared_ptr<NodeButton> createAppoinment = std::make_shared<NodeButton>("createAppoinment", COORD{54, 0}, Color::GREEN, Color::BLACK, std::vector<std::string>{
            ".-------.",
            "| Nuevo |",
            "'-------'"}); 
        createAppoinment->setOnClick([patientSelector, doctorSelector, cards, contextFlag](){
            // If we are in the Patient Menu context, we need to select a Doctor
            if (contextFlag == 1) {
                doctorSelector->setGlobalPosition(COORD{65, 0}); // Show Doctor Selector
            }
            // If we are in the Doctor Menu context, we need to select a Patient
            else if (contextFlag == 2 || contextFlag == 0) {
                patientSelector->setGlobalPosition(COORD{65, 0}); // Show Patient Selector
            } 
        });
        
        std::shared_ptr<int> dynamicOffset = std::make_shared<int>(cards->getChildCount());
        cards->setProcessFunction([cards, dynamicOffset](double){ *dynamicOffset = (cards->getChildCount() -1)* 12;});
        auto slider = pui::vSlider(cards, dynamicOffset, 40);
            
        root->setAtExitFunction([loadedList, contextIDX_p](){loadedList->clear(); *contextIDX_p = 0;});
        
        root->addChild(cards);
        root->addChild(slider);
        root->addChild(doctorSelector);
        root->addChild(patientSelector);
        root->addChild(createAppoinment);   
        root->addChild(backButton);
        return root;
        
    }
}
#endif