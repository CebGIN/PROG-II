#ifndef UIPREFABS_HPP
#define UIPREFABS_HPP

#include "LegacyEngine.hpp"

namespace pui {
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

    std::shared_ptr<NodeSQ> hSlider(std::shared_ptr<Node2D> container, std::shared_ptr<int> offset, int size = 100){
        std::shared_ptr<NodeSQ> line = std::make_shared<NodeSQ>("Line", COORD{0, 0}, COORD{SHORT(size), 1}, Color::BRIGHT_YELLOW, Color::BLACK);
        std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 0}, Color::YELLOW, Color::BLACK, std::vector<std::string>{
            "|/////|",});
        
        slider->setProcessFunction([line, slider, container, offset, size](double){
            SHORT limit = size - 7;
            static bool moving;
            if (Input::MousePos.Y == line->getGlobalPosition().Y &&
                Input::MousePos.X >= line->getGlobalPosition().X &&
                Input::MousePos.X < (line->getGlobalPosition().X + size) &&
                Input::LClickJustPressed) moving = true;

            if (!Input::LClick) moving = false;
    
            if (moving)
                slider->setGlobalPosition({std::max(std::min(Input::MousePos.X, SHORT(line->getGlobalPosition().X + limit)), line->getGlobalPosition().X), line->getGlobalPosition().Y});
    
            float progress = ((slider->getGlobalPosition().X - line->getLocalPosition().X) / float(limit));
            container->setGlobalPosition({SHORT(-progress * *offset), container->getGlobalPosition().Y});
        });
        line->addChild(slider);
        return line;
    }
}
#endif