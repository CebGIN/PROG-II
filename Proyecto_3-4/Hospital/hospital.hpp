// hospital.hpp
#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp"
#include "../Doctor/doctor.hpp"
#include "../Appointment/appointment.hpp"
#include <string>

namespace hos{
    class Hospital {
    private:
        cfm::Folder folder;
        char name[100];
        char address[150];
        char phone[15];

    public :
        // --- Propiedades Públicas de Almacenamiento ---
        
        /// @brief Nombre de la carpeta base del hospital.
        std::string folderName;
        
        /// @brief Archivo indexado para almacenar los objetos Patient.
        cfm::IndexedFile<ptn::Patient> patients;
        
        /// @brief Archivo indexado para almacenar los objetos Doctor.
        cfm::IndexedFile<doc::Doctor> doctors;
        
        /// @brief Archivo indexado para almacenar los objetos Appointment.
        cfm::IndexedFile<app::Appointment> appoinments;

        // --- Getters y Setters de Información ---

        /// @brief Obtiene el nombre del hospital.
        /// @return El nombre como std::string.
        std::string getName();

        /// @brief Establece el nombre del hospital.
        /// @param hname El nuevo nombre.
        void setName(std::string hname);

        /// @brief Obtiene la dirección del hospital.
        /// @return La dirección como std::string.
        std::string getAddress(); // Corregido: getAddres -> getAddress

        /// @brief Establece la dirección del hospital.
        /// @param addr La nueva dirección.
        void setAddress(std::string addr); // Corregido: setAddres -> setAddress

        /// @brief Obtiene el número de teléfono del hospital.
        /// @return El teléfono como std::string.
        std::string getPhone();

        /// @brief Establece el número de teléfono del hospital.
        /// @param phon El nuevo teléfono.
        void setPhone(std::string phon);

        // --- Constructor ---

        /// @brief Inicializa el objeto Hospital, creando o abriendo la carpeta y los archivos indexados.
        /// @param foldrName Nombre de la carpeta de trabajo del hospital.
        /// @param hosName Nombre visible del hospital (Default: "Default").
        /// @param hosAddress Dirección del hospital (Default: "Default").
        /// @param hosPhone Teléfono del hospital (Default: "Default").
        Hospital(std::string foldrName, std::string hosName = "Default", std::string hosAddress = "Default", std::string hosPhone = "Default");
    };
}

#endif // HOSPITAL_HPP