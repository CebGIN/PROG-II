#include "../CGINLibs/CGINDE.hpp"
#include "../CGINLibs/CGINIF.hpp"
#include "../CGINLibs/CGINFU.hpp"

#include "Menus/menus.hpp"
#include "Hospital/hospital.hpp"
#include "Patient/patient.hpp"
#include <cstring>
#include <vector>

void anadirPacientes(auto &list){
for (int i = 0; i < 3; i++){
    ptn::Patient p(list.getTotalIDXs());
    std::string text;
    std::cout<<"Introducir Nombre\n";
    std::cin >> text;
    p.setFirstName(text);
    std::cout<<"Introducir Apellido\n";
    std::cin >> text;
    p.setLastName(text);
    std::cout<<"Introducir cedula\n";
    std::cin >> text;
    p.setNationalID(text);
    list.add(p);
}}

int main(){
    hos::Hospital hospital("Prueba");

    // anadirPacientes(hospital.patients);

    std::shared_ptr<std::vector<std::unique_ptr<ptn::Patient>>> loadedlist = std::make_shared<std::vector<std::unique_ptr<ptn::Patient>>>();
    loadedlist->reserve(hospital.patients.getListSize() * 1.5);
    
    for (unsigned int i = 0; i < hospital.patients.getTotalIDXs(); i++){
        auto patient = hospital.patients.getAtIDX(i);
        if (!patient) continue;
        loadedlist->push_back(std::move(patient));
    }
    // loadedlist->at(0)->setAge(5);

    for (const auto &p : *loadedlist){
        std::cout<< "Leido: " << p->getID() << ", " << p->getFirstName() << ", " << p->getLastName() << ", " << p->getAge()<< std::endl;
    }

    // loadedlist->at(1)->setFirstName("Josefo");
    // loadedlist->at(1)->saveAtIn(1, hospital.patients);

    // loadedlist->at(2)->setFirstName("Luigi");
    // loadedlist->at(2)->saveAtIn(2, hospital.patients);
    // loadedlist->at(1)->erasefrom(hospital.patients);

    std::cout<< hospital.patients.getListSize() << ", " << hospital.patients.getTotalIDXs() << std::endl;

    loadedlist->clear();
    system("pause");
    // SceneManager::getInstance().changeScene(loadedlist->at(0)->createCard(hospital.patients));
    SceneManager::getInstance().changeScene(hos::CreateHospitalMenu(hospital));
    SceneManager::getInstance().startRunning();
}