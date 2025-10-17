#include <functional>
#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <memory>
#include <vector>
#include <string>

inline POINT const operator+(POINT a, POINT b){return {a.x + b.x, a.y + b.y};}
inline POINT const operator-(POINT a, POINT b){return {a.x - b.x, a.y - b.y};}

class Node : public std::enable_shared_from_this<Node>{
    protected:
        std::vector<std::shared_ptr<Node>> Childs;
        std::weak_ptr<Node> Parent;
        std::string name;
        std::function<void()> process;
        std::function<void()> at_enter_tree;
        std::function<void()> at_exit_tree;
    
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
        virtual void enter_tree() {
            // 1. Ejecutar la lógica de entrada de este nodo
            at_enter_tree(); 
            
            // 2. Propagar la llamada a   a todos los hijos
            for (const auto& child : Childs) {
                child->enter_tree(); 
            }
        }
        virtual void exit_tree() {
            // 1. Propagar la llamada a salir a todos los hijos
            for (const auto& child : Childs) {
                child->exit_tree(); 
            }
            // 2. Ejecutar la lógica de salida de este nodo
            at_exit_tree(); 
        }
        void setProcessFunction(std::function<void()> func) {
            if (func) {
                process = func;
            } else {
                process = [](){}; 
                // std::cerr << "Warning: Attempted to set a null process function for Node '" << name << "'." << std::endl;
            }
        }
    
        // virtual void draw(ConsoleRenderer& renderer) {
        // for (const auto& child : Childs) {
        //     child->draw(renderer); // Propagar la llamada a draw a los hijos
        // }
        // }
    
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
                    globalPos = (parent2D->position) + globalPos;
                }
                currentParent = currentParent->getParent();
            }
            return globalPos;
        }
        
        void setGlobalPosition(POINT newGlobalPosition) {
            POINT currentGlobalPosition = getGlobalPosition(); // G
            POINT currentLocalPosition = getLocalPosition();   // L
    
            // POINT newLocalPosition = (newGlobalPosition + currentLocalPosition); // Gn + L
            POINT newLocalPosition = ((newGlobalPosition + currentLocalPosition) - currentGlobalPosition);  // (Gn + L) - G
    
            setLocalPosition(newLocalPosition);
            // std::cout << "Node2D '" << name << "' global position set to ("
            //           << newGlobalPosition.X << ", " << newGlobalPosition.Y << ")"
            //           << " (New Local: " << newLocalPosition.X << ", " << newLocalPosition.Y << ")." << std::endl;
        }
    };