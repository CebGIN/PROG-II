#include "LegacyEngine.hpp"
#include <cmath>
std::shared_ptr<Node> createMainMenu(SceneManager &manager){
    (void) manager;

    //Declarar Nodos
    std::shared_ptr<Node> root = std::make_shared<Node>("Root");

    std::shared_ptr<Node2D> headersContainer = std::make_shared<Node2D>("headersContainer", COORD{1, 1});

    std::shared_ptr<Node2D> buttonsContainer = std::make_shared<Node2D>("buttonContainer", COORD{5, 6});


    std::shared_ptr<NodeUI> texto = std::make_shared<NodeUI>("label", COORD{0, 0},std::vector<std::string>{"TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO"});
    std::shared_ptr<NodeUI> texto2 = std::make_shared<NodeUI>("label2", COORD{0, 30},std::vector<std::string>{"TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO","TEXTOTEXTOTEXTOTEXTOTEXTOTEXTO"});
    
    std::shared_ptr<NodeButton> pacientsButton = std::make_shared<NodeButton>("pacientsButton", COORD{0, 10}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Boton A  |",
        "'-----------'"});
    std::shared_ptr<NodeButton> doctorsButton = std::make_shared<NodeButton>("doctorsButton", COORD{20, 15}, Color::CYAN, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Boton B  |",
        "'-----------'"});
    std::shared_ptr<NodeButton> appointmentsButton = std::make_shared<NodeButton>("appointmentsButton", COORD{40, 20}, Color::RED, Color::BLACK, std::vector<std::string>{
        ".-----------.",
        "|  Boton C  |",
        "'-----------'"});
        
    std::shared_ptr<NodeButton> dragableButton = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 0}, Color::RED, Color::BLACK, std::vector<std::string>{
        "||",
        "||",
        "||"});
    
    bool *movable = new bool(false);

    dragableButton->setOnClick([movable](){
            *movable = !*movable;
        });
    dragableButton->setProcessFunction([dragableButton, movable, buttonsContainer](double){
        static SHORT limit = 20;
        if (Input::MousePos.X == dragableButton->getGlobalPosition().X && Input::LClick)
        dragableButton->setGlobalPosition({dragableButton->getGlobalPosition().X, std::min(Input::MousePos.Y, limit)});
        float progress = (dragableButton->getGlobalPosition().Y / float(limit));
        buttonsContainer->setGlobalPosition({buttonsContainer->getGlobalPosition().X, SHORT(-progress *60)});
    });
    
    dragableButton->setAtExitFunction([movable](){delete movable;});
    root->addChild(dragableButton);

    //Armar el arbol
    root->addChild(headersContainer);
    root->addChild(buttonsContainer);
        buttonsContainer->addChild(texto);
        buttonsContainer->addChild(pacientsButton);
        buttonsContainer->addChild(doctorsButton);
        buttonsContainer->addChild(appointmentsButton);
        buttonsContainer->addChild(texto2);
    return root;
}
// std::shared_ptr<Node2D> createPatientMenu(){

// }

int main(){

    SceneManager manager = SceneManager::getInstance();
    auto root = createMainMenu(manager);
    manager.changeScene(root);
    manager.startRunning();
}
