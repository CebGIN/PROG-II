#include "../CGINLibs/CGINDE.hpp"
#include "../CGINLibs/CGINIF.hpp"
#include "../CGINLibs/CGINFU.hpp"

#include "Menus/menus.hpp"
#include "Hospital/hospital.hpp"
#include "Doctor/doctor.hpp"
#include <cstring>
#include <vector>

void anadirDoctores(auto &list){
for (int i = 0; i < 3; i++){
    doc::Doctor d(list.getTotalIDXs());
    std::string text;
    std::cout<<"Introducir Nombre\n";
    std::cin >> text;
    d.setFirstName(text);
    std::cout<<"Introducir Apellido\n";
    std::cin >> text;
    d.setLastName(text);
    // std::cout<<"Introducir cedula\n";
    // std::cin >> text;
    // d.setNationalID(text);
    list.add(d);
}}

int main(){
    hos::Hospital hospital("Prueba");

    // anadirDoctores(hospital.doctors);

    std::shared_ptr<std::vector<std::unique_ptr<doc::Doctor>>> loadedlist = std::make_shared<std::vector<std::unique_ptr<doc::Doctor>>>();
    loadedlist->reserve(hospital.doctors.getListSize() * 1.5);
    
    for (unsigned int i = 0; i < hospital.doctors.getTotalIDXs(); i++){
        auto doctor = hospital.doctors.getAtIDX(i);
        if (!doctor) continue;
        loadedlist->push_back(std::move(doctor));
    };

    for (const auto &p : *loadedlist){
        std::cout<< "Leido: " << p->getID() << ", " << p->getFirstName() << ", " << p->getLastName() << ", " << p->getYearsExperience()<< std::endl;
    }

    std::cout<< hospital.doctors.getListSize() << ", " << hospital.doctors.getTotalIDXs() << std::endl;

    loadedlist->clear();
    system("pause");
    // SceneManager::getInstance().changeScene(loadedlist->at(0)->createCard(hospital.doctors));
    // SceneManager::getInstance().changeScene(hos::CreateHospitalMenu(hospital));
    SceneManager::getInstance().changeScene(doc::createMenu(hospital));
    SceneManager::getInstance().startRunning();
}