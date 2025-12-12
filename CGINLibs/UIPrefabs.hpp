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

    std::shared_ptr<NodeSQ> hSlider(std::shared_ptr<Node2D> container, std::shared_ptr<int> offset, int size, const std::string& colorA, const std::string& colorB, const std::string& colorC, const std::string& colorD){
        std::shared_ptr<NodeSQ> line = std::make_shared<NodeSQ>("Line", COORD{0, 0}, COORD{SHORT(size), 1}, colorC, colorD);
        std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("appointmentsButton", COORD{0, 0}, colorA, colorB, std::vector<std::string>{
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
    std::shared_ptr<NodeSQ> vSlider(std::shared_ptr<Node2D> container, std::shared_ptr<int> offset, int size, const std::string& colorA, const std::string& colorB, const std::string& colorC, const std::string& colorD) {
        std::shared_ptr<NodeSQ> line = std::make_shared<NodeSQ>("VLine", COORD{0, 0}, COORD{1, SHORT(size)}, colorC, colorD);

        std::shared_ptr<NodeButton> slider = std::make_shared<NodeButton>("vSliderButton", COORD{0, 0}, colorA, colorB, std::vector<std::string>{
            "-","|","|","-"
        });
    
        slider->setProcessFunction([line, slider, container, offset, size](double) {
            // La altura del slider es de 1 (una línea de texto), y el límite es: tamaño de la línea - altura del slider.
            SHORT limit = size - 4;
            static bool moving;
    
            // 3. Detección de clic: Comprobar la coordenada Y dentro del rango 'size' de la línea y la coordenada X en la posición de la línea.
            if (Input::MousePos.X == line->getGlobalPosition().X &&
                Input::MousePos.Y >= line->getGlobalPosition().Y &&
                Input::MousePos.Y < (line->getGlobalPosition().Y + size) &&
                Input::LClickJustPressed) moving = true;
    
            if (!Input::LClick) moving = false;
    
            if (moving)
                slider->setGlobalPosition({line->getGlobalPosition().X, std::max(std::min(Input::MousePos.Y, SHORT(line->getGlobalPosition().Y + limit)), line->getGlobalPosition().Y)});
    
            float progress = ((slider->getGlobalPosition().Y - line->getLocalPosition().Y) / float(limit));
            container->setGlobalPosition({container->getGlobalPosition().X, SHORT(-progress * *offset)});
        });
    
        line->addChild(slider);
        return line;
    }
}
#endif