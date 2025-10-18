#include <functional>
#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

// --- Input Class and Console Helpers ---
namespace Input{
    HANDLE hStdin = INVALID_HANDLE_VALUE;
    HANDLE hStdout = INVALID_HANDLE_VALUE;
    
    COORD MousePos  = {0, 0};
    bool LClick = false;
    bool RClick = false;
    bool LClickPrev = false;
    bool RClickPrev = false;

    bool LClickJustPressed = false;
    bool RClickJustPressed = false;

    const int InteractKey = VK_RETURN;

    DWORD prevMode = 0;
    void iniciateInput(){
        Input::hStdin = GetStdHandle(STD_INPUT_HANDLE);
        Input::hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (Input::hStdin == INVALID_HANDLE_VALUE || Input::hStdout == INVALID_HANDLE_VALUE) return;
    
        if (!GetConsoleMode(Input::hStdin, &prevMode)) return;
        DWORD newMode = prevMode;
        newMode &= ~ENABLE_QUICK_EDIT_MODE;
        newMode |= ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT;
        if (!SetConsoleMode(Input::hStdin, newMode)) return;
    
        CONSOLE_CURSOR_INFO cci;
        cci.dwSize = 1;
        cci.bVisible = FALSE;
        SetConsoleCursorInfo(Input::hStdout, &cci);
    }

    void refresh_input(){
        Input::LClickPrev = Input::LClick;
        Input::RClickPrev = Input::RClick;
    
        Input::LClick = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        Input::RClick = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    
        Input::LClickJustPressed = (Input::LClick && !Input::LClickPrev);
        Input::RClickJustPressed = (Input::RClick && !Input::RClickPrev);
    
        const int EVENT_COUNT = 128;
        INPUT_RECORD inputBuffer[EVENT_COUNT];
        DWORD eventsRead = 0;
        DWORD eventsAvailable = 0;
    
        GetNumberOfConsoleInputEvents(Input::hStdin, &eventsAvailable);
        if (eventsAvailable > 0) { //Only read if there are events
            if (!ReadConsoleInput(Input::hStdin, inputBuffer, EVENT_COUNT, &eventsRead)) return;
    
            for (DWORD i = 0; i < eventsRead; ++i) {
                INPUT_RECORD &rec = inputBuffer[i];
                if (rec.EventType == MOUSE_EVENT) {
                    MOUSE_EVENT_RECORD &mouseRec = rec.Event.MouseEvent;
                    if (mouseRec.dwEventFlags == MOUSE_MOVED) {
                        Input::MousePos = mouseRec.dwMousePosition;
                    }
                }
            }
            if (eventsAvailable > eventsRead) {
                FlushConsoleInputBuffer(Input::hStdin);
            }
        }
    }

};

namespace Color {
    const std::string COLOR_RED = "RED";
    const std::string COLOR_GREEN = "GREEN";
    const std::string COLOR_BLUE = "BLUE";
    const std::string COLOR_YELLOW = "YELLOW";
    const std::string COLOR_CYAN = "CYAN";
    const std::string COLOR_MAGENTA = "MAGENTA";
    const std::string COLOR_WHITE = "WHITE";
    const std::string COLOR_BLACK = "BLACK";
    const std::string COLOR_BRIGHT_RED = "BRIGHT_RED";
    const std::string COLOR_BRIGHT_GREEN = "BRIGHT_GREEN";
    const std::string COLOR_BRIGHT_BLUE = "BRIGHT_BLUE";
    const std::string COLOR_BRIGHT_YELLOW = "BRIGHT_YELLOW";
    const std::string COLOR_BRIGHT_CYAN = "BRIGHT_CYAN";
    const std::string COLOR_BRIGHT_MAGENTA = "BRIGHT_MAGENTA";
    const std::string COLOR_BRIGHT_WHITE = "BRIGHT_WHITE";

    WORD getColorAttribute(const std::string& colorName) {
        if (colorName == "RED") return FOREGROUND_RED;
        if (colorName == "GREEN") return FOREGROUND_GREEN;
        if (colorName == "BLUE") return FOREGROUND_BLUE;
        if (colorName == "YELLOW") return FOREGROUND_RED | FOREGROUND_GREEN;
        if (colorName == "CYAN") return FOREGROUND_BLUE | FOREGROUND_GREEN;
        if (colorName == "MAGENTA") return FOREGROUND_RED | FOREGROUND_BLUE;
        if (colorName == "WHITE") return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        if (colorName == "BLACK") return 0; 
    
        if (colorName == "BRIGHT_RED") return FOREGROUND_RED | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_GREEN") return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_BLUE") return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_YELLOW") return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_CYAN") return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_MAGENTA") return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        if (colorName == "BRIGHT_WHITE") return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    
        return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; 
    }
    
    WORD getBackgroundColorAttribute(const std::string& colorName) {
        return getColorAttribute(colorName) << 4;
    }
    
};

COORD addCoords(COORD A, COORD B){
    return {static_cast<SHORT>(A.X + B.X), static_cast<SHORT>(A.Y + B.Y)};
}
COORD subtractCoords(COORD A, COORD B){
    return {static_cast<SHORT>(A.X - B.X), static_cast<SHORT>(A.Y - B.Y)};
}

class ConsoleRenderer {
private:
    COORD consoleBufferSize;
    std::vector<CHAR_INFO> screenBuffer; // El búfer de caracteres y atributos

public:
    ConsoleRenderer(COORD bufferSize) : consoleBufferSize(bufferSize) {
        screenBuffer.resize(bufferSize.X * bufferSize.Y);
        clearBuffer(); // Inicializar con espacios en blanco
    }
    // Limpia el búfer con espacios y atributos por defecto
    void clearBuffer() {
        CHAR_INFO emptyCharInfo;
        emptyCharInfo.Char.UnicodeChar = L' '; // Usar L' ' para wchar_t
        emptyCharInfo.Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Blanco por defecto

        for (size_t i = 0; i < screenBuffer.size(); ++i) {
            screenBuffer[i] = emptyCharInfo;
        }
    }

    void putChar(COORD pos, WCHAR character, WORD attributes) {
        // Asegurarse de que la posición esté dentro de los límites del búfer
        if (pos.X >= 0 && pos.X < consoleBufferSize.X &&
            pos.Y >= 0 && pos.Y < consoleBufferSize.Y) {
            
            // Calcular el índice lineal en el vector
            int index = pos.Y * consoleBufferSize.X + pos.X;
            screenBuffer[index].Char.UnicodeChar = character;
            screenBuffer[index].Attributes = attributes;
        }
    }
    
    void putString(COORD pos, const std::string& text, WORD attributes) {
        for (size_t i = 0; i < text.length(); ++i) {
            putChar(addCoords(pos, {static_cast<SHORT>(i), 0}), static_cast<WCHAR>(text[i]), attributes);
        }
    }

    void present() {
        SMALL_RECT writeRegion = {0, 0, static_cast<SHORT>(consoleBufferSize.X - 1), static_cast<SHORT>(consoleBufferSize.Y - 1)};

        if (!WriteConsoleOutput(
            Input::hStdout,     // Handle al búfer de pantalla
            screenBuffer.data(),// Puntero al primer elemento del array CHAR_INFO
            consoleBufferSize,  // Tamaño del búfer de CHAR_INFO (ancho x alto)
            {0,0},              // Coordenada de la celda superior izquierda en el búfer
            &writeRegion        // Región del destino en el búfer de pantalla
        )) {
            std::cerr << "Error writing to console output: " << GetLastError() << std::endl;
        }
    }
};

class Node : public std::enable_shared_from_this<Node>{
protected:
    std::vector<std::shared_ptr<Node>> Children;
    std::weak_ptr<Node> Parent;
    std::string name;
    std::function<void()> process;

public:
    Node(const std::string& nodeName = "Unnamed Node") : name(nodeName) {
        // std::cout << "Node '" << name << "' created." << std::endl;
        process = [](){ /* Do nothing by default */ };
    }
    
    virtual ~Node() {
        // std::cout << "Node '" << name << "' destroyed." << std::endl;
    }

    void setParent(std::shared_ptr<Node> parentNode) {
        Parent = parentNode;
    }

    std::shared_ptr<Node> getParent() const {
        return Parent.lock();
    }

    bool isRoot() const {
        return Parent.expired() || !Parent.lock();
    }

    void addChild(std::shared_ptr<Node> childNode) {
        if (childNode) {
            Children.push_back(childNode);
            childNode->setParent(shared_from_this());
        } 
        // else {
        //     std::cerr << "Warning: Attempted to add a null shared_ptr child node." << std::endl;
        // }
    }

    size_t getChildCount() const {
        return Children.size();
    }

    std::shared_ptr<Node> getChild(size_t index) {
        if (index < Children.size()) {
            return Children[index];
        } else {
            throw std::out_of_range("Node::getChild(index): Index out of bounds.");
        }
    }

    std::shared_ptr<const Node> getChild(size_t index) const {
        if (index < Children.size()) {
            return Children[index];
        } else {
            throw std::out_of_range("Node::getChild(index): Index out of bounds.");
        }
    }

    std::vector<std::shared_ptr<Node>>::iterator begin() { return Children.begin();}
    std::vector<std::shared_ptr<Node>>::iterator end() { return Children.end();}

    std::vector<std::shared_ptr<Node>>::const_iterator begin() const { return Children.cbegin();} // Use cbegin for const access
    std::vector<std::shared_ptr<Node>>::const_iterator end() const { return Children.cend();}     // Use cend for const access

    const std::string& getName() const {
        return name;
    }

    virtual std::string getType() const {
        return "Node";
    }

    virtual void update() {
        // 1. Ejecutar la lógica de proceso de este nodo
        process(); 

        // 2. Propagar la llamada a update a todos los hijos
        for (const auto& child : Children) {
            child->update(); 
        }
    }

    void setProcessFunction(std::function<void()> func) {
        if (func) {
            process = func;
        } else {
            process = [](){}; 
            // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
        }
    }

    virtual void draw(ConsoleRenderer& renderer) {
    for (const auto& child : Children) {
        child->draw(renderer); // Propagar la llamada a draw a los hijos
    }
}

    void printHierarchy(int indentLevel = 0) const {//debug
        std::cout << std::string(indentLevel * 4, ' ');
        std::cout << "- " << name << " (" << getType() << ")"; // Print node type
        if (isRoot()) {
            std::cout << " (Root)";
        }
        std::cout << std::endl;

        for (const auto& child : Children) {
            child->printHierarchy(indentLevel + 1);
        }
    }
};

class Node2D : public Node{
protected:
    COORD position;
    public:
    
    Node2D(const std::string& nodeName = "Unnamed Node", COORD nodePosition = {0, 0}) : Node(nodeName), position(nodePosition){}


    COORD getLocalPosition() const {
        return position;
    }
    void setLocalPosition(COORD new_position){
        position = new_position;
        std::cout << "Node2D '" << name << "' local position set to (" << position.X << ", " << position.Y << ")." << std::endl;
    }

    std::string getType() const override {
        return "Node2D";
    }

    COORD getGlobalPosition() const {
        COORD globalPos = position;

        std::shared_ptr<Node> currentParent = getParent(); 
        while (currentParent) {
            std::shared_ptr<Node2D> parent2D = std::dynamic_pointer_cast<Node2D>(currentParent);
            if (parent2D) {
                globalPos = addCoords(parent2D->position, globalPos);
            }
            currentParent = currentParent->getParent();
        }
        return globalPos;
    }
    
    void setGlobalPosition(COORD newGlobalPosition) {
        COORD currentGlobalPosition = getGlobalPosition(); // G
        COORD currentLocalPosition = getLocalPosition();   // L

        COORD newLocalPosition = addCoords(newGlobalPosition, currentLocalPosition); // Gn + L
        newLocalPosition = subtractCoords(newLocalPosition, currentGlobalPosition);  // (Gn + L) - G

        setLocalPosition(newLocalPosition);
        std::cout << "Node2D '" << name << "' global position set to ("
                  << newGlobalPosition.X << ", " << newGlobalPosition.Y << ")"
                  << " (New Local: " << newLocalPosition.X << ", " << newLocalPosition.Y << ")." << std::endl;
    }
};

class NodeUI : public Node2D{
protected:
    COORD size;
    std::vector<std::string> text;
    
public: 

    NodeUI(const std::string& nodeName, COORD nodePosition, std::vector<std::string> nodeText) :
        Node2D(nodeName, nodePosition), text(nodeText)
    {
        if (!text.empty()) {
            size.X = static_cast<SHORT>(text[0].size());
            size.Y = static_cast<SHORT>(text.size());
        } else {
            size.X = 0;
            size.Y = 0;
            // std::cerr << "Warning: NodeUI '" << nodeName << "' created with empty text." << std::endl;
        }
    }

    void set_text(std::vector<std::string> NewText){
        text = NewText;
        if (!text.empty()) {
            size.X = static_cast<SHORT>(text[0].size());
            size.Y = static_cast<SHORT>(text.size());
        } else {
            size.X = 0;
            size.Y = 0;
        }
    }

    std::string getType() const override {
        return "NodeUI";
    }

    bool is_inside(const COORD point){
        COORD globalPos = getGlobalPosition(); 
        return (point.X >= globalPos.X && point.X < globalPos.X + size.X &&
                point.Y >= globalPos.Y && point.Y < globalPos.Y + size.Y);
    }

    void draw(ConsoleRenderer& renderer) override {
        COORD globalPos = getGlobalPosition();
        WORD defaultAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // White

        for (SHORT y_offset = 0; y_offset < static_cast<SHORT>(text.size()); ++y_offset) {
            COORD currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
            std::string line = text[y_offset];
            renderer.putString(currentLineGlobalPos, line, defaultAttributes);
        }
        Node::draw(renderer);
    }
    
    COORD getSize() const { return size; }
};

class NodePCT : public NodeUI{
protected:
    WORD text_attributes;
    WORD background_attributes;

public: 
    NodePCT(const std::string& nodeName, COORD nodePosition, 
            const std::string& textColorName, const std::string& backgroundColorName, 
            std::vector<std::string> nodeText) : NodeUI(nodeName, nodePosition, nodeText), 
            text_attributes(Color::getColorAttribute(textColorName)), background_attributes(Color::getBackgroundColorAttribute(backgroundColorName)) {}

    std::string getType() const override {
        return "NodePCT";
    }

   void draw(ConsoleRenderer& renderer) override {
        COORD globalPos = getGlobalPosition();
        WORD combinedAttributes = text_attributes | background_attributes;

        for (SHORT y_offset = 0; y_offset < static_cast<SHORT>(text.size()); ++y_offset) {
            COORD currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
            std::string line = text[y_offset];
            renderer.putString(currentLineGlobalPos, line, combinedAttributes);
        }

        Node::draw(renderer); // Propagate to children
    }
};

// class NodeRCT : public NodeUI{};

class NodeButton : public NodePCT{
protected:
    bool hovered = false;
    std::function<void()> onClick;

public:
    void update() override {
        updateHover(Input::MousePos);
        handleClick();
        Node::update(); 
    }
    NodeButton(const std::string& nodeName, COORD nodePosition, std::string textColor, std::string backgroundColor, std::vector<std::string> nodeText) : 
        NodePCT(nodeName, nodePosition, textColor, backgroundColor, nodeText) {}

    std::string getType() const override {
        return "NodeButton";
    }
    bool is_hovered(){
        return hovered;
    }
    void updateHover(const COORD mouseGlobalPos) {
        hovered = is_inside(mouseGlobalPos);
    }
    void handleClick() { if (Input::LClickJustPressed && hovered && onClick) onClick(); }

    void setOnClick(std::function<void()> callback) {
        onClick = callback;
    }

    void draw(ConsoleRenderer& renderer) override {
        COORD globalPos = getGlobalPosition();
        WORD currentTextAttrs = text_attributes;
        WORD currentBgAttrs = background_attributes;

        if (hovered) {
            currentTextAttrs = background_attributes >> 4;
            currentBgAttrs = text_attributes << 4;
        }

        WORD combinedAttributes = currentTextAttrs | currentBgAttrs;

        for (SHORT y_offset = 0; y_offset < static_cast<SHORT>(text.size()); ++y_offset) {
            COORD currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
            std::string line = text[y_offset];
            
            renderer.putString(currentLineGlobalPos, line, combinedAttributes);
        }
        Node::draw(renderer);
    }
};

//FIN DEL CODIGO DEL MOTOR