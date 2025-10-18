#include <functional>
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
        virtual void enterTree() {
            // 1. Ejecutar la lógica de entrada de este nodo
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
            std::shared_ptr<Node2D> parent2D = getNode2DParent();
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