#include "../CGINLibs/CGINDE.hpp"
#include "../CGINLibs/CGINIF.hpp"
#include "../CGINLibs/CGINFU.hpp"

#include "Menus/menus.hpp"
#include "Hospital/hospital.hpp"
#include "Patient/patient.hpp"
#include <cstring>
#include <vector>

int main(){  
    hos::Hospital hospital("Hospital");
    hospital.setName("EL HOSPITAL ADIOS MUNDO");
    hospital.setAddres("Av Code con calle embarcadero");
    hospital.setPhone("04121122333");

    SceneManager::getInstance().changeScene(hos::createMenu(hospital));
    SceneManager::getInstance().startRunning();
}