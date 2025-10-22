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
inline bool const operator==(POINT a, POINT b){return {(a.x == b.x)&&(a.y == b.y)};}
inline bool const operator!=(POINT a, POINT b){return {(a.x != b.x)&&(a.y != b.y)};}

class Node : public std::enable_shared_from_this<Node>{
    protected:
        std::vector<std::shared_ptr<Node>> Children;
        std::weak_ptr<Node> Parent;
        std::string name;
        std::function<void()> process;
        std::function<void()> atEnterTree;
        std::function<void()> atExitTree;

        mutable bool its_in_the_tree = false;
    
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
            
            std::shared_ptr<const Node> currentParent = getParent();
            
            while (currentParent) {
                std::shared_ptr<T> parentOfType = std::dynamic_pointer_cast<T>(
                    std::const_pointer_cast<Node>(currentParent)
                );
                
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

        virtual void enterTree() {
            // 1. Ejecutar la lógica de entrada de este nodo
            atEnterTree();
            its_in_the_tree = true;
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
            its_in_the_tree = false;
            // 2. Ejecutar la lógica de salida de este nodo
            atExitTree();
        }

        void setProcessFunction(std::function<void()> func) {
            if (func) {
                process = func;
            } else {
                process = [](){}; 
                // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
            }
        }

        virtual void invalidateCacheRecursively() const{
            for (const auto& child : Children) {
                child->invalidateCacheRecursively();  
            }
        }
    };
    
class Node2D : public Node{
    protected:
        POINT position;
        mutable POINT cached_global_position;
        mutable bool is_cached_position_valid = false;

    public:
        Node2D(const std::string& nodeName = "Unnamed Node", POINT nodePosition = {0, 0}) : Node(nodeName), position(nodePosition){}

        std::shared_ptr<Node2D> getNode2DParent() const {
            std::shared_ptr<const Node> currentParent = getParent();
            while (currentParent){
                std::shared_ptr<Node2D> parent2D = std::dynamic_pointer_cast<Node2D>(currentParent);
                if (parent2D) {
                    return parent2D;
                }
                currentParent = currentParent->getParent();
            }
            return nullptr;
        }
        
        void invalidateCacheRecursively() const override {
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
protected:
    POINT size = {1, 1};
    HWND hControl = NULL;

    mutable bool win32_needs_sync = false; 

    virtual HWND createControl(HWND hParent) = 0;

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

    void invalidateCacheRecursively() const override {
        this->win32_needs_sync = true;
        Node2D::invalidateCacheRecursively();
    }

    void enterTree() override {
        if (hControl != NULL) {
            ShowWindow(hControl, SW_SHOW);
            this->win32_needs_sync = true;
        }
        if (hControl == NULL) {
            auto win32Parent = getParentOfType<NodeWin32>();
            HWND hParent = NULL;
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
     
    void exitTree() override {
        Node2D::exitTree(); 
        if (this->hControl != NULL) {
            ShowWindow(this->hControl, SW_HIDE);
            this->win32_needs_sync = false;
            // Desvincular el puntero C++
            SetWindowLongPtr(this->hControl, GWLP_USERDATA, 0); 
        }
    }

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
    
        // WS_CHILD espera coordenadas relativas a la ventana padre.
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