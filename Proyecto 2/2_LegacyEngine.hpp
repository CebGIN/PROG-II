#include <functional>
#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

class Node;
class Node2D;
class NodeUI;
class NodePCT;
class NodeButton;
class SceneManager;

enum class NodeType{
    NODE,
    NODE2D,
    NODEUI,
    NODEPCT,
    NODEBUTTON,
};

inline COORD const operator+(COORD a, COORD b){return {static_cast<SHORT>(a.X + b.X), static_cast<SHORT>(a.Y + b.Y)};}
inline COORD const operator-(COORD a, COORD b){return {static_cast<SHORT>(a.X - b.X), static_cast<SHORT>(a.Y - b.Y)};}
inline bool operator==(COORD a, COORD b){return {(a.X == b.X)&&(a.Y == b.Y)};}
inline bool operator!=(COORD a, COORD b){return {!(a == b)};}

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

    template<typename T>
    T getTypedInput(COORD position = {0, 0}){
        T input;
        SetConsoleCursorPosition(Input::hStdout, position);
        
        CONSOLE_CURSOR_INFO cci;
        cci.dwSize = 1;
        cci.bVisible = TRUE;

        SetConsoleCursorInfo(Input::hStdout, &cci);
        std::cin >> input;
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        cci.bVisible = FALSE;
        SetConsoleCursorInfo(Input::hStdout, &cci);


        return input;
    }

    std::string getLineInput(COORD position = {0, 0}){
        std::string input;
        SetConsoleCursorPosition(Input::hStdout, position);
        
        CONSOLE_CURSOR_INFO cci;
        cci.dwSize = 1;
        cci.bVisible = TRUE;

        SetConsoleCursorInfo(Input::hStdout, &cci);
        std::getline(std::cin, input);

        cci.bVisible = FALSE;
        SetConsoleCursorInfo(Input::hStdout, &cci);

        return input;
    }
};

namespace Color {
    const std::string RED = "RED";
    const std::string GREEN = "GREEN";
    const std::string BLUE = "BLUE";
    const std::string YELLOW = "YELLOW";
    const std::string CYAN = "CYAN";
    const std::string MAGENTA = "MAGENTA";
    const std::string WHITE = "WHITE";
    const std::string BLACK = "BLACK";
    const std::string BRIGHT_RED = "BRIGHT_RED";
    const std::string BRIGHT_GREEN = "BRIGHT_GREEN";
    const std::string BRIGHT_BLUE = "BRIGHT_BLUE";
    const std::string BRIGHT_YELLOW = "BRIGHT_YELLOW";
    const std::string BRIGHT_CYAN = "BRIGHT_CYAN";
    const std::string BRIGHT_MAGENTA = "BRIGHT_MAGENTA";
    const std::string BRIGHT_WHITE = "BRIGHT_WHITE";

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
            putChar((pos + COORD{static_cast<SHORT>(i), 0}), static_cast<WCHAR>(text[i]), attributes);
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
    friend class SceneManager;
protected:
    std::vector<std::shared_ptr<Node>> Children;
    std::weak_ptr<Node> Parent;
    std::string name;
    std::function<void(double)> process;
    std::function<void()> atEnterTree;
    std::function<void()> atExitTree;

    mutable bool its_in_the_tree = false;

    virtual void enterTree() {
        // 1. Ejecutar la lógica de entrada de este nodo
        its_in_the_tree = true;
        atEnterTree();
        // 2. Propagar la llamada a   a todos los hijos
        for (const auto& child : Children) {
            child->enterTree(); 
        }
    }

    virtual void exitTree() {
        // 1. Propagar la llamada a salir a todos los hijos
        for (const auto& child : Children) {
            child->exitTree(); 
        }
        // 2. Ejecutar la lógica de salida de este nodo
        atExitTree();
        its_in_the_tree = false;
    }

public:
    Node(const std::string& nodeName = "Unnamed Node") : name(nodeName) {
        // std::cout << "Node '" << name << "' created." << std::endl;
        process = [](double){}; atEnterTree = [](){}; atExitTree = [](){};
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

    template<typename T>
    std::shared_ptr<T> getParentOfType() const {
        static_assert(std::is_base_of<Node, T>::value, "T must be a Node type.");

        std::shared_ptr<Node> currentParent = getParent();
        
        while (currentParent) {
            std::shared_ptr<T> parentOfType = std::dynamic_pointer_cast<T>(currentParent);
            
            if (parentOfType) {
                return parentOfType;
            }
            currentParent = currentParent->getParent();
        }
        return nullptr;
    }

    bool isRoot() const {
        return Parent.expired() || !Parent.lock();
    }

    void addChild(std::shared_ptr<Node> childNode) {
        if (childNode) {
            Children.push_back(childNode);
            childNode->setParent(shared_from_this());
            if (its_in_the_tree) childNode->enterTree();
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

    virtual NodeType getType() const {
        return NodeType::NODE;
    }

    virtual void update(double deltaTime) {
        // 1. Ejecutar la lógica de proceso de este nodo
        process(deltaTime); 

        // 2. Propagar la llamada a update a todos los hijos
        for (const auto& child : Children) {
            child->update(deltaTime); 
        }
    }

    void setProcessFunction(std::function<void(double)> func) {
        if (func) {
            process = func;
        } else {
            process = [](double){}; 
            // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
        }
    }
    void setAtEnterFunction(std::function<void()> func) {
        if (func) {
            atEnterTree = func;
        } else {
            atEnterTree = [](){}; 
            // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
        }
    }
    void setAtExitFunction(std::function<void()> func) {
        if (func) {
            atExitTree = func;
        } else {
            atExitTree = [](){}; 
            // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
        }
    }

    virtual void draw(ConsoleRenderer& renderer) {
        for (const auto& child : Children) {
            child->draw(renderer); // Propagar la llamada a draw a los hijos
        }
    }

    virtual void invalidateCacheRecursively(){
        for (const auto& child : Children) {
            child->invalidateCacheRecursively();  
        }
    }
};

class Node2D : public Node{
protected:
    COORD position;
    mutable COORD cached_global_position;
    mutable bool is_cached_position_valid = false;
    
public:
    
    Node2D(const std::string& nodeName = "Unnamed Node", COORD nodePosition = {0, 0}) : Node(nodeName), position(nodePosition){}

    void invalidateCacheRecursively() override {
        if (!this->is_cached_position_valid) return;// Si ya es inválido, no hacemos nada ni propagamos más
        this->is_cached_position_valid = false;     // Marcar el cache de este nodo como inválido.
        Node::invalidateCacheRecursively();         // Propagar la llamada a los hijos (recursivamente).
    }

    COORD getLocalPosition() const {
        return position;
    }
    void setLocalPosition(COORD new_position){
        if(new_position == position) return;
        position = new_position;
        invalidateCacheRecursively();
        // std::cout << "Node2D '" << name << "' local position set to (" << position.X << ", " << position.Y << ")." << std::endl;
    }
    NodeType getType() const override {
        return NodeType::NODE2D;
    }

    COORD getGlobalPosition() const {
        if (is_cached_position_valid) return cached_global_position;
        COORD globalPos = position;

        std::shared_ptr<Node2D> parent2D = getParentOfType<Node2D>();

        if(parent2D) globalPos = globalPos + parent2D->getGlobalPosition();

        cached_global_position = globalPos;
        is_cached_position_valid = true;
        return globalPos;
    }
    
    void setGlobalPosition(COORD newGlobalPosition) {
        //newLocalPosition = ((newGlobalPosition + currentLocalPosition) - currentGlobalPosition); (Gn + L) - G
        setLocalPosition(((newGlobalPosition + getLocalPosition()) - getGlobalPosition()));
        this->cached_global_position = newGlobalPosition;
        this->is_cached_position_valid = true;
        // std::cout << "Node2D '" << name << "' global position set to (" << newGlobalPosition.X << ", " << newGlobalPosition.Y << ")" << " (New Local: " << newLocalPosition.X << ", " << newLocalPosition.Y << ")." << std::endl;
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

    std::vector<std::string> getText(){
        return text;
    }

    NodeType getType() const override {
        return NodeType::NODEUI;
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
            COORD currentLineGlobalPos = (globalPos + COORD{0, y_offset});
            std::string line = text[y_offset];
            renderer.putString(currentLineGlobalPos, line, defaultAttributes);
        }
        Node::draw(renderer);
    }
    
    COORD getSize() const { return size; }
};

class NodeSQ : public Node2D{
protected:
    COORD size;
    WORD text_attributes;
    WORD background_attributes;
public:
    NodeSQ(const std::string& nodeName, COORD nodePosition, COORD nodeSize,
        const std::string& textColorName, const std::string& backgroundColorName) : Node2D(nodeName, nodePosition), 
        size(nodeSize), text_attributes(Color::getColorAttribute(textColorName)), background_attributes(Color::getBackgroundColorAttribute(backgroundColorName)) {}
    
    COORD getSize() const{
        return size;
    }

    void setSize(COORD newSize = {1, 1}) {
        size = newSize;
    }

    void changeTextColor(const std::string& textColorName){
        text_attributes = Color::getColorAttribute(textColorName);
    }

    void changeBackgroundColor(const std::string& backgroundColorName){
        background_attributes = Color::getBackgroundColorAttribute(backgroundColorName);
    }

    void draw(ConsoleRenderer& renderer) override {
        COORD globalPos = getGlobalPosition();
        WORD combinedAttributes = text_attributes | background_attributes;

        for (SHORT j = 1; j < static_cast<SHORT>(size.Y -1); ++j){
            renderer.putChar(globalPos + COORD{0, j}, L'|', combinedAttributes);
        }
        for (SHORT j = 1; j < static_cast<SHORT>(size.Y -1); ++j){
            renderer.putChar(globalPos + COORD{static_cast<SHORT>(size.X - 1), j}, L'|', combinedAttributes);
        }

        for(SHORT i = 0; i < size.X; ++i){
            renderer.putChar(globalPos + COORD{i, 0}, L'-', combinedAttributes);
        }
        for(SHORT i = 0; i < size.X; ++i){
            renderer.putChar(globalPos + COORD{i, static_cast<SHORT>(size.Y - 1)}, L'-', combinedAttributes);
        }

        Node::draw(renderer); // Propagate to children
    }
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

    NodeType getType() const override {
        return NodeType::NODEPCT;
    }

   void draw(ConsoleRenderer& renderer) override {
        COORD globalPos = getGlobalPosition();
        WORD combinedAttributes = text_attributes | background_attributes;

        for (SHORT y_offset = 0; y_offset < static_cast<SHORT>(text.size()); ++y_offset) {
            COORD currentLineGlobalPos = (globalPos + COORD{0, y_offset});
            std::string line = text[y_offset];
            renderer.putString(currentLineGlobalPos, line, combinedAttributes);
        }

        Node::draw(renderer); // Propagate to children
    }

    void changeTextColor(const std::string& textColorName){
        text_attributes = Color::getColorAttribute(textColorName);
    }

    void changeBackgroundColor(const std::string& backgroundColorName){
        background_attributes = Color::getBackgroundColorAttribute(backgroundColorName);
    }
};

// class NodeRCT : public NodeUI{};

class NodeButton : public NodePCT{
protected:
    bool hovered = false;
    std::function<void()> onClick;

public:
    void update(double deltaTime) override {
        updateHover(Input::MousePos);
        handleClick();
        Node::update(deltaTime); 
    }
    NodeButton(const std::string& nodeName, COORD nodePosition, std::string textColor, std::string backgroundColor, std::vector<std::string> nodeText) : 
        NodePCT(nodeName, nodePosition, textColor, backgroundColor, nodeText) {}

    NodeType getType() const override {
        return NodeType::NODEBUTTON;
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
            COORD currentLineGlobalPos = (globalPos + COORD{0, y_offset});
            std::string line = text[y_offset];
            
            renderer.putString(currentLineGlobalPos, line, combinedAttributes);
        }
        Node::draw(renderer);
    }
};

class SceneManager {
    private:
        std::shared_ptr<Node> root_node = nullptr; 
    
        bool is_running = false; 
        SceneManager() = default;
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
    
        LARGE_INTEGER m_qpf;             // Frecuencia del contador (Hz)
        LARGE_INTEGER m_lastTime;        // Tiempo en el frame anterior
        double m_deltaTime = 0.0;        // Último Delta Time (en segundos)

        unsigned long int frameCount;

    public:
        static SceneManager& getInstance(){
            static SceneManager instance;
            return instance;
        }
    
        void processScene() {
            // 1. Cálculo de Delta Time
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            double timeElapsed = (double)(currentTime.QuadPart - m_lastTime.QuadPart) / m_qpf.QuadPart;
            m_deltaTime = timeElapsed;
            m_lastTime = currentTime;


            if (!root_node) return;
            root_node->update(timeElapsed);
        }
    
        std::shared_ptr<Node> getCurrrentScene() {
            return root_node;
        }

        void changeScene(std::shared_ptr<Node> newRoot) {
            if (newRoot == root_node) return;
    
            if (root_node) {
                root_node->exitTree();
            }
    
            root_node = newRoot;
    
            if (root_node) { 
                root_node->enterTree();
            }
        }
    
        void startRunning() {
            this->is_running = true;
            //Inicializar el Contador de Rendimiento
            if (!QueryPerformanceFrequency(&m_qpf)) {
                m_qpf.QuadPart = 0;
            }
            // Obtener el tiempo inicial
            QueryPerformanceCounter(&m_lastTime);

            Input::iniciateInput();

            ConsoleRenderer renderer = ConsoleRenderer({100, 60});
            while (this->is_running) {
        
                if (this->is_running && this->root_node) {

                    renderer.clearBuffer();
                    Input::refresh_input();

                    this->processScene();
                    this->root_node->draw(renderer);
                    renderer.present();
                    increaseFrameCount();

                }
                // Sleep(50);
            }
        }
    
        void stopRunning() {
            this->is_running = false;
        }

        unsigned long int getFrameCount() const{
            return frameCount;
        }

        void increaseFrameCount(){
            frameCount++;
        }
};

//FIN DEL CODIGO DEL MOTOR
/*
    Uso recomendado: 
    - Crear funciones que se encargan de construir las escenas
    - Las funciones de la forma: std::sharet_ptr crearEscena(){... return root;};
    En el main():
    - Obtener la referencia al SceneManager con manager = SceneManager::getInstance();
    - Crear la root como root = crearEscena();
    - Establecer la root como la escena activa con manager.changeScene(root);
    - Iniciar el bucle de ejecución con manager.startRunning();
    Al iniciar el bucle la ejecución del main se mantendra en ese punto, cualquier lógica adicional debe ser manejada por el contenido de la escena.
*/