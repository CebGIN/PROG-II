#ifndef MENUS_HPP
#define MENUS_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Hospital/hospital.hpp"
// namespace ptn{

// }

namespace ptn{
    std::shared_ptr<Node> createPatientMenu(hos::Hospital &hospital);
}

namespace hos{
    std::shared_ptr<Node> CreateHospitalMenu(Hospital &hospital){
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
            SceneManager::getInstance().changeScene(ptn::createPatientMenu(hospital));
        });
        root->addChild(headersContainer);
            headersContainer->addChild(hospitalName);
        root->addChild(buttonsContainer);
            buttonsContainer->addChild(pacientsButton);
        return root;
    }
}

namespace ptn{
    std::shared_ptr<Node> createPatientMenu(hos::Hospital &hospital) {
        std::shared_ptr<Node> root = std::make_shared<Node>("RootPatients");

        std::shared_ptr<NodeButton> mainMenuButton = std::make_shared<NodeButton>("mainMenuButton", COORD{1, 1}, Color::RED, Color::BLACK, std::vector<std::string>{
                ".----------------.",
                "| Menu principal |",
                "'----------------'"});
        mainMenuButton->setOnClick([&hospital](){SceneManager::getInstance().changeScene(hos::CreateHospitalMenu(hospital));});

        std::shared_ptr<Node2D> cardCont = std::make_shared<Node2D>("cardCont", COORD{0, 6});
        std::shared_ptr<std::vector<std::unique_ptr<ptn::Patient>>> loadedlist = std::make_shared<std::vector<std::unique_ptr<ptn::Patient>>>();
        loadedlist->reserve(hospital.patients.getListSize() * 1.5);
        
        for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++){
            auto patient = hospital.patients.getAtIDX(i);
            if (!patient) continue;

            cardCont->addChild(patient->createCard(hospital.patients));
            std::static_pointer_cast<Node2D>(cardCont->getChild(loadedlist->size()))->setLocalPosition(COORD{static_cast<SHORT>(loadedlist->size() * 66), 0});
            loadedlist->push_back(std::move(patient));
        }
        //extender loadedList a toda la vida de la escena
        root->setAtExitFunction([loadedlist](){loadedlist->clear();});
        // loadedlist->at(0)->setFirstName("LaBona");
        // loadedlist->at(0)->saveAtIn(0, hospital.patients);

        std::shared_ptr<NodeButton> createPatient = std::make_shared<NodeButton>("createPatient", COORD{21, 1}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
            ".-------.",
            "| Nuevo |",
            "'-------'"});
        
        createPatient->setOnClick([&hospital, loadedlist, cardCont](){
            loadedlist->emplace_back(std::make_unique<ptn::Patient>(hospital.patients.getTotalIDXs()));
            hospital.patients.add(*(loadedlist->back()));
            auto card = loadedlist->back()->createCard(hospital.patients);
            card->setLocalPosition(COORD{(SHORT)(66 * cardCont->getChildCount()), 0});
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


#endif