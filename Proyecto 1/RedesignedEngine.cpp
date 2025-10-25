#include <functional>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <memory>
#include <vector>
#include <string>

inline POINT const operator+(POINT a, POINT b){return {a.x + b.x, a.y + b.y};}
inline POINT const operator-(POINT a, POINT b){return {a.x - b.x, a.y - b.y};}
inline bool operator==(POINT a, POINT b){return {(a.x == b.x)&&(a.y == b.y)};}
inline bool operator!=(POINT a, POINT b){return {!(a == b)};}

LRESULT CALLBACK GlobalWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Node;
class Node2D;
class NodeWin32;
class SceneManager;

class Node : public std::enable_shared_from_this<Node>{
    friend class SceneManager;
    protected:
        std::vector<std::shared_ptr<Node>> Children;
        std::weak_ptr<Node> Parent;
        std::string name;
        std::function<void()> process;
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
            process = [](){};
        }
        
        std::vector<std::shared_ptr<Node>>::iterator begin() { return Children.begin();}
        std::vector<std::shared_ptr<Node>>::iterator end() { return Children.end();}
    
        std::vector<std::shared_ptr<Node>>::const_iterator begin() const { return Children.cbegin();} // Use cbegin for const access
        std::vector<std::shared_ptr<Node>>::const_iterator end() const { return Children.cend();}     // Use cend for const access

        virtual ~Node() {}
        
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
        
        void removeChild(std::shared_ptr<Node> child) {
            if (!child) return;

            auto it = std::find(Children.begin(), Children.end(), child);
            if (it != Children.end()) {
                Children.erase(it);
                child->Parent.reset();
            }
            child->exitTree();
        }

        bool isRoot() const {
            return Parent.expired() || !Parent.lock();
        }
        
        void addChild(std::shared_ptr<Node> childNode) {
            if (childNode) {
                Children.push_back(childNode);
                childNode->setParent(shared_from_this());
                if (its_in_the_tree){childNode->enterTree();}
            } 
            else {
                std::cerr << "Warning: Attempted to add a null shared_ptr child node." << std::endl;
            }
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
            }
        }
        void setAtEnterFunction(std::function<void()> func) {
            if (func) {
                atEnterTree = func;
            } else {
                atEnterTree = [](){}; 
            }
        }
        void setAtExitFunction(std::function<void()> func) {
            if (func) {
                atExitTree = func;
            } else {
                atExitTree = [](){};
            }
        }
        virtual void invalidateCacheRecursively(){
            for (const auto& child : Children) {
                child->invalidateCacheRecursively();  
            }
        }
    };

class Node2D : public Node{
    friend class SceneManager;
    protected:
        POINT position;
        mutable POINT cached_global_position;
        mutable bool is_cached_position_valid = false;

    public:
        Node2D(const std::string& nodeName = "Unnamed Node", POINT nodePosition = {0, 0}) : Node(nodeName), position(nodePosition){}
        
        void invalidateCacheRecursively() override {
            if (!this->is_cached_position_valid) return;// Si ya es inválido, no hacemos nada ni propagamos más
            this->is_cached_position_valid = false;     // Marcar el cache de este nodo como inválido.
            Node::invalidateCacheRecursively();         // Propagar la llamada a los hijos (recursivamente).
        }

        POINT getLocalPosition() const {
            return position;
        }

        void setLocalPosition(POINT new_position){
            if(new_position == position) return;
            position = new_position;
            invalidateCacheRecursively();
            // std::cout << "Node2D '" << name << "' local position set to (" << position.X << ", " << position.Y << ")." << std::endl;
        }
    
        std::string getType() const override {
            return "Node2D";
        }
        
        POINT getGlobalPosition() const {
            if(is_cached_position_valid) return cached_global_position;
            std::shared_ptr<Node2D> parent2D = getParentOfType<Node2D>();
            POINT globalPos = this->position;
            if (parent2D){
                globalPos = globalPos + parent2D->getGlobalPosition();
            }
            cached_global_position = globalPos;
            is_cached_position_valid = true;
            return globalPos;
        }
        
        void setGlobalPosition(POINT newGlobalPosition) {
            //newLocalPosition = ((newGlobalPosition + currentLocalPosition) - currentGlobalPosition); (Gn + L) - G
            setLocalPosition(((newGlobalPosition + getLocalPosition()) - getGlobalPosition()));
            this->cached_global_position = newGlobalPosition;
            this->is_cached_position_valid = true;
            // std::cout << "Node2D '" << name << "' global position set to (" << newGlobalPosition.X << ", " << newGlobalPosition.Y << ")" << " (New Local: " << newLocalPosition.X << ", " << newLocalPosition.Y << ")." << std::endl;
        }
};

class NodeWin32 : public Node2D {
    friend class SceneManager;
protected:
    POINT size = {1, 1};
    HWND hControl = NULL;

    mutable bool win32_needs_sync = false; 

    virtual HWND createControl(HWND hParent) = 0;

    void enterTree() override;
    void exitTree() override {
        Node2D::exitTree(); 
        if (this->hControl != NULL) {
            ShowWindow(this->hControl, SW_HIDE);
            this->win32_needs_sync = false;
            // Desvincular el puntero C++
            SetWindowLongPtr(this->hControl, GWLP_USERDATA, 0); 
        }
    }
public:
    NodeWin32(const std::string& nodeName = "Unnamed Node", POINT nodePosition = {0, 0}, POINT nodeSize = {1, 1}) : Node2D(nodeName, nodePosition),size(nodeSize) {}
    
    ~NodeWin32() override {
        if (this->hControl != NULL) {
            DestroyWindow(this->hControl);
            this->hControl = NULL; 
        }
    }

    HWND getControlHandle(){
        return hControl;
    }   

    void invalidateCacheRecursively() override;

    void setSize(POINT new_size) {
        if (new_size.x == size.x && new_size.y == size.y) return;
        
        size = new_size;
        
        // Invalida el flag de sincronización Win32 para forzar un SetWindowPos
        this->win32_needs_sync = true; 

        // Opcional: Invalidar el cache de posición C++
        // invalidateCacheRecursively(); 
    }

    void synchronizeWin32Control() const {
        if (!this->win32_needs_sync || this->hControl == NULL) {
            return;
        }
    
        POINT nodeGlobalPos = getGlobalPosition(); 
        POINT parentGlobalPos = {0, 0};
        
        if (auto parentWin32 = getParentOfType<NodeWin32>()) {
             parentGlobalPos = parentWin32->getGlobalPosition();
        }
        POINT win32RelativePos = nodeGlobalPos - parentGlobalPos; 
        
        SetWindowPos(this->hControl, NULL, 
                     win32RelativePos.x, 
                     win32RelativePos.y, 
                     this->size.x, // Utiliza el miembro 'size' del NodeWin32
                     this->size.y,
                     SWP_NOZORDER | SWP_NOACTIVATE // No cambiar el Z-Order o activar.
                     );
    
        this->win32_needs_sync = false;
    }
};

class SceneManager {
    private:
        std::vector<std::weak_ptr<NodeWin32>> dirty_win32_nodes;
        std::shared_ptr<Node> root_node = nullptr; 
        
        HWND hMainWnd = NULL; 
        bool is_running = false; 
        SceneManager() = default;
        
    public:
        static SceneManager& getInstance(){
            static SceneManager instance;
            return instance;
        }
        // Método para obtener el HWND de la ventana principal (utilizado por NodeWin32)
        HWND getMainHWND() const {
             return hMainWnd; 
        }
    
        void registerDirtyNode(std::shared_ptr<NodeWin32> node) {
            dirty_win32_nodes.push_back(node); 
        }
    
        void processScene() {
            if (!root_node) return;
            root_node->update();
            for (std::weak_ptr<NodeWin32>& weakNode : dirty_win32_nodes) {
                if (std::shared_ptr<NodeWin32> node = weakNode.lock()) {
                    node->synchronizeWin32Control();
                }
            }
            dirty_win32_nodes.clear();
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
    
        void startRunning(HINSTANCE hInstance, int nCmdShow) {
            const TCHAR* className = TEXT("NodeEngineApp");
            
            // Inicialización de la ventana
            if (!initializeMainWindow(hInstance, nCmdShow, className, TEXT("Node Engine Demo"), 800, 600)) {
                std::cerr << "Engine Initialization Failed." << std::endl;
                return;
            }
    
            this->is_running = true;
            
            MSG msg = {};
            
            // Bucle de Mensajes
            while (this->is_running && GetMessage(&msg, NULL, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                
                // Llamada a la Lógica de Escena y Sincronización
                // (Se ejecuta cuando no hay mensajes bloqueantes)
                if (this->root_node) {
                    this->processScene();
                }
            }
        }
    
        void stopRunning() {
            this->is_running = false;
        }
        // Método de inicialización
        bool initializeMainWindow(HINSTANCE hInstance, int nCmdShow, const TCHAR* className, const TCHAR* title, int width, int height) {
        // 1. Registro de la Clase de Ventana
        WNDCLASSEX wcex = {};
            wcex.cbSize        = sizeof(WNDCLASSEX);
            wcex.style         = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc   = GlobalWindowProc; // Usar la función global
            wcex.cbClsExtra    = 0;
            wcex.cbWndExtra    = 0;
            wcex.hInstance     = hInstance;
            wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
            wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName  = NULL;
            wcex.lpszClassName = className;
            wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    
        if (!RegisterClassEx(&wcex)) {
            return false;
        }
    
        // 2. Creación de la Ventana Principal
        this->hMainWnd = CreateWindow(
            className,
            title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height,
            NULL,
            NULL,
            hInstance,
            NULL // No pasamos 'this' al HWND principal, ya que el Singleton es globalmente accesible.
        );
    
        if (!this->hMainWnd) {
            return false;
        }
    
        // 3. Mostrar y Actualizar la Ventana
        ShowWindow(this->hMainWnd, nCmdShow);
        UpdateWindow(this->hMainWnd);
    
        return true;
    }
};

void NodeWin32::invalidateCacheRecursively() {
    this->win32_needs_sync = true;
    std::shared_ptr<NodeWin32> self = std::static_pointer_cast<NodeWin32>(shared_from_this());
    SceneManager::getInstance().registerDirtyNode(self);
    Node2D::invalidateCacheRecursively();
}
void NodeWin32::enterTree() {
    if (hControl != NULL) {
        ShowWindow(hControl, SW_SHOW);
        this->win32_needs_sync = true;
    }
    if (hControl == NULL) {
        auto win32Parent = getParentOfType<NodeWin32>();
        HWND hParent = SceneManager::getInstance().getMainHWND();
        if(win32Parent){
            hParent = win32Parent->getControlHandle();
        }
        this->hControl = createControl(hParent);   
    }
    if (hControl) {
        SetWindowLongPtr(hControl, GWLP_USERDATA, (LONG_PTR)this);
    }
    this->win32_needs_sync = true; 
    Node2D::enterTree(); 
}

LRESULT CALLBACK GlobalWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY:
            SceneManager::getInstance().stopRunning(); 
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            // Lógica para interceptar eventos de botones, etc.
            // Requiere más complejidad para mapear a los nodos.
            return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}