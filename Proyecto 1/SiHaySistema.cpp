#include <functional>
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <vector>
#include <memory>
#include <string>

// --- Input Class and Console Helpers ---
namespace Input{
    HANDLE hStdin = INVALID_HANDLE_VALUE;
    HANDLE hStdout = INVALID_HANDLE_VALUE;
    
    POINT MousePos  = {0, 0};
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
                        // Input::MousePos = mouseRec.dwMousePosition;
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

POINT addCoords(POINT A, POINT B){
    return {(A.x + B.x), (A.y + B.y)};
}
POINT subtractCoords(POINT A, POINT B){
    return {(A.x - B.x), (A.y - B.y)};
}

// class ConsoleRenderer {
// private:
//     POINT consoleBufferSize;
//     std::vector<CHAR_INFO> screenBuffer; // El búfer de caracteres y atributos
//
// public:
//     ConsoleRenderer(POINT bufferSize) : consoleBufferSize(bufferSize) {
//         screenBuffer.resize(bufferSize.x * bufferSize.y);
//         clearBuffer(); // Inicializar con espacios en blanco
//     }
//     // Limpia el búfer con espacios y atributos por defecto
//     void clearBuffer() {
//         CHAR_INFO emptyCharInfo;
//         emptyCharInfo.Char.UnicodeChar = L' '; // Usar L' ' para wchar_t
//         emptyCharInfo.Attributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Blanco por defecto
//
//         for (size_t i = 0; i < screenBuffer.size(); ++i) {
//             screenBuffer[i] = emptyCharInfo;
//         }
//     }
//
//     void putChar(POINT pos, WCHAR character, WORD attributes) {
//         // Asegurarse de que la posición esté dentro de los límites del búfer
//         if (pos.x >= 0 && pos.x < consoleBufferSize.x &&
//             pos.y >= 0 && pos.y < consoleBufferSize.y) {
//            
//             // Calcular el índice lineal en el vector
//             int index = pos.y * consoleBufferSize.x + pos.x;
//             screenBuffer[index].Char.UnicodeChar = character;
//             screenBuffer[index].Attributes = attributes;
//         }
//     }
//    
//     void putString(POINT pos, const std::string& text, WORD attributes) {
//         for (size_t i = 0; i < text.length(); ++i) {
//             putChar(addCoords(pos, {static_cast<SHORT>(i), 0}), static_cast<WCHAR>(text[i]), attributes);
//         }
//     }
//
//     void present() {
//         SMALL_RECT writeRegion = {0, 0, static_cast<SHORT>(consoleBufferSize.x - 1), static_cast<SHORT>(consoleBufferSize.y - 1)};
//
//         if (!WriteConsoleOutput(
//             Input::hStdout,     // Handle al búfer de pantalla
//             screenBuffer.data(),// Puntero al primer elemento del array CHAR_INFO
//             consoleBufferSize,  // Tamaño del búfer de CHAR_INFO (ancho x alto)
//             {0,0},              // Coordenada de la celda superior izquierda en el búfer
//             &writeRegion        // Región del destino en el búfer de pantalla
//         )) {
//             std::cerr << "Error writing to console output: " << GetLastError() << std::endl;
//         }
//     }
// };

class Node : public std::enable_shared_from_this<Node>{
protected:
    std::vector<std::shared_ptr<Node>> Childs;
    std::weak_ptr<Node> Parent;
    std::string name;
    std::function<void()> process;
    // std::function<void()> at_draw;

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
            Childs.push_back(childNode);
            childNode->setParent(shared_from_this());
        } 
        else {
            std::cerr << "Warning: Attempted to add a null shared_ptr child node." << std::endl;
        }
    }

    size_t getChildCount() const {
        return Childs.size();
    }

    std::shared_ptr<Node> getChild(size_t index) {
        if (index < Childs.size()) {
            return Childs[index];
        } else {
            throw std::out_of_range("Node::getChild(index): Index out of bounds.");
        }
    }

    std::shared_ptr<const Node> getChild(size_t index) const {
        if (index < Childs.size()) {
            return Childs[index];
        } else {
            throw std::out_of_range("Node::getChild(index): Index out of bounds.");
        }
    }

    std::vector<std::shared_ptr<Node>>::iterator begin() { return Childs.begin();}
    std::vector<std::shared_ptr<Node>>::iterator end() { return Childs.end();}

    std::vector<std::shared_ptr<Node>>::const_iterator begin() const { return Childs.cbegin();} // Use cbegin for const access
    std::vector<std::shared_ptr<Node>>::const_iterator end() const { return Childs.cend();}     // Use cend for const access

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
        for (const auto& child : Childs) {
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
    for (const auto& child : Childs) {
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

        for (const auto& child : Childs) {
            child->printHierarchy(indentLevel + 1);
        }
    }
};

class Node2D : public Node{
protected:
    POINT position;
    public:
    
    Node2D(const std::string& nodeName = "Unnamed Node", POINT nodePosition = {0, 0}) : Node(nodeName), position(nodePosition){}


    POINT getLocalPosition() const {
        return position;
    }
    void setLocalPosition(POINT new_position){
        position = new_position;
        // std::cout << "Node2D '" << name << "' local position set to (" << position.X << ", " << position.Y << ")." << std::endl;
    }

    std::string getType() const override {
        return "Node2D";
    }

    POINT getGlobalPosition() const {
        POINT globalPos = position;

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
    
    void setGlobalPosition(POINT newGlobalPosition) {
        POINT currentGlobalPosition = getGlobalPosition(); // G
        POINT currentLocalPosition = getLocalPosition();   // L

        POINT newLocalPosition = addCoords(newGlobalPosition, currentLocalPosition); // Gn + L
        newLocalPosition = subtractCoords(newLocalPosition, currentGlobalPosition);  // (Gn + L) - G

        setLocalPosition(newLocalPosition);
        // std::cout << "Node2D '" << name << "' global position set to ("
        //           << newGlobalPosition.X << ", " << newGlobalPosition.Y << ")"
        //           << " (New Local: " << newLocalPosition.X << ", " << newLocalPosition.Y << ")." << std::endl;
    }
};

class NodeUI : public Node2D{
protected:
    POINT size;
    std::vector<std::string> text;
    
public: 

    NodeUI(const std::string& nodeName, POINT nodePosition, std::vector<std::string> nodeText) :
        Node2D(nodeName, nodePosition), text(nodeText)
    {
        if (!text.empty()) {
            size.x = static_cast<LONG>(text[0].size());
            size.y = static_cast<LONG>(text.size());
        } else {
            size.x = 0;
            size.y = 0;
            // std::cerr << "Warning: NodeUI '" << nodeName << "' created with empty text." << std::endl;
        }
    }

    void set_text(std::vector<std::string> NewText){
        text = NewText;
        if (!text.empty()) {
            size.x = static_cast<LONG>(text[0].size());
            size.y = static_cast<LONG>(text.size());
        } else {
            size.x = 0;
            size.y = 0;
        }
    }

    std::string getType() const override {
        return "NodeUI";
    }

    bool is_inside(const POINT point){
        POINT globalPos = getGlobalPosition(); 
        return (point.x >= globalPos.x && point.x < globalPos.x + size.x &&
                point.y >= globalPos.y && point.y < globalPos.y + size.y);
    }

    void draw(ConsoleRenderer& renderer) override {
        POINT globalPos = getGlobalPosition();
        WORD defaultAttributes = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // White

        for (LONG y_offset = 0; y_offset < static_cast<LONG>(text.size()); ++y_offset) {
            POINT currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
            std::string line = text[y_offset];
            renderer.putString(currentLineGlobalPos, line, defaultAttributes);
        }
        Node::draw(renderer);
    }
    
    POINT getSize() const { return size; }
};

class NodePCT : public NodeUI{
protected:
    WORD text_attributes;
    WORD background_attributes;

public: 
    NodePCT(const std::string& nodeName, POINT nodePosition, 
            const std::string& textColorName, const std::string& backgroundColorName, 
            std::vector<std::string> nodeText) : NodeUI(nodeName, nodePosition, nodeText), 
            text_attributes(Color::getColorAttribute(textColorName)), background_attributes(Color::getBackgroundColorAttribute(backgroundColorName)) {}

    std::string getType() const override {
        return "NodePCT";
    }

   void draw(ConsoleRenderer& renderer) override {
        POINT globalPos = getGlobalPosition();
        WORD combinedAttributes = text_attributes | background_attributes;

        for (LONG y_offset = 0; y_offset < static_cast<LONG>(text.size()); ++y_offset) {
            POINT currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
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
    NodeButton(const std::string& nodeName, POINT nodePosition, std::string textColor, std::string backgroundColor, std::vector<std::string> nodeText) : 
        NodePCT(nodeName, nodePosition, textColor, backgroundColor, nodeText) {}

    std::string getType() const override {
        return "NodeButton";
    }
    bool is_hovered(){
        return hovered;
    }
    void updateHover(const POINT mouseGlobalPos) {
        hovered = is_inside(mouseGlobalPos);
    }
    void handleClick() { if (Input::LClickJustPressed && hovered && onClick) onClick(); }

    void setOnClick(std::function<void()> callback) {
        onClick = callback;
    }

    void draw(ConsoleRenderer& renderer) override {
        POINT globalPos = getGlobalPosition();
        WORD currentTextAttrs = text_attributes;
        WORD currentBgAttrs = background_attributes;

        if (hovered) {
            currentTextAttrs = background_attributes >> 4;
            currentBgAttrs = text_attributes << 4;
        }

        WORD combinedAttributes = currentTextAttrs | currentBgAttrs;

        for (LONG y_offset = 0; y_offset < (text.size()); ++y_offset) {
            POINT currentLineGlobalPos = addCoords(globalPos, {0, y_offset});
            std::string line = text[y_offset];
            
            renderer.putString(currentLineGlobalPos, line, combinedAttributes);
        }
        Node::draw(renderer);
    }
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_LBUTTONDOWN :
            // std::cout<<"Hola";
            break;

        case WM_CHAR :
            break;

        case WM_PAINT :
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
            break;

        case WM_CLOSE: // Se cierra la ventana (p.ej., el usuario hizo clic en la 'X')
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY: // La ventana ha sido destruida
            PostQuitMessage(0); // Envía un mensaje WM_QUIT a la cola de mensajes
            break;

        default:
            // Para todos los demás mensajes que no manejamos,
            // los pasamos a la función de procedimiento de ventana predeterminada.
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const TCHAR CLASS_NAME[] = _T("MiClaseVentana");

    // ==========================================================
    // PARTE A: REGISTRAR LA CLASE DE VENTANA
    // ==========================================================
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);                // Tamaño de la estructura
    wc.lpfnWndProc = WndProc;                      // Puntero a la función de manejo de mensajes
    wc.hInstance = hInstance;                      // Identificador de la instancia de la aplicación
    wc.lpszClassName = CLASS_NAME;                 // Nombre de la clase (usado al crear la ventana)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Cursor por defecto (flecha)
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Color de fondo (blanco/gris claro)

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("El registro de la clase falló."), _T("Error"), MB_ICONERROR);
        return 0;
    }

    // ==========================================================
    // PARTE B: CREAR LA VENTANA (Instancia)
    // ==========================================================
    
    HWND hwnd = CreateWindowEx(
        0,                                 // Estilos de ventana extendidos
        CLASS_NAME,                        // Nombre de la Clase (el que registramos)
        _T("HolaWindows"),      // Título de la ventana
        WS_OVERLAPPEDWINDOW,               // Estilo de ventana (borde, barra de título, minimizar/maximizar)
        CW_USEDEFAULT, CW_USEDEFAULT,      // Posición inicial (X, Y)
        500, 350,                          // Tamaño (Ancho, Alto)
        NULL,                              // Ventana Padre (NULL para la principal)
        NULL,                              // Menú (NULL, no hay menú)
        hInstance,                         // Identificador de la instancia de la aplicación
        NULL                               // Datos de creación adicionales
    );

    if (hwnd == NULL) {
        MessageBox(NULL, _T("La creación de la ventana falló."), _T("Error"), MB_ICONERROR);
        return 0;
    }

    // Mostrar y actualizar la ventana
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // ==========================================================
    // PARTE C: EL BUCLE DE MENSAJES (Message Loop)
    // ==========================================================
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg); // Traduce mensajes de teclado a caracteres
        DispatchMessage(&msg);  // Envía el mensaje a la función WndProc
    }
    
    return (int)msg.wParam;
}

//FIN DEL CODIGO DEL MOTOR