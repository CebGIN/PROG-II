// Patient.hpp
#ifndef PATIENT_HPP
#define PATIENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include <cstdint>
#include <string> // Necesario para std::string

namespace ptn{
    const std::string folder = "Hospital/Patients";
    
    class Patient {
    private:
        uint64_t id;
        char firstName[50] = "Default";
        char lastName[50] = "Default";
        char nationalId[20] = "V00.000.000";
        uint8_t age = 0;
        char gender = 'M';
        char bloodType[5] = "Df+-";
        char phone[15] = "+5800005555555";
        char address[100] = "Default";
        char email[50] = "default@dominio";
        char allergies[500] = "Default";
        char notes[500] = "Default";

        std::weak_ptr<Node2D> card;

    public :
        /// @brief Constructor por defecto de Patient. Inicializa los campos con valores por defecto.
        Patient() = default;
        
        /// @brief Constructor que inicializa el objeto Patient con un ID específico.
        /// @param ID Identificador único del paciente.
        Patient(uint64_t ID);

        // --- Métodos de Archivo y Persistencia ---

        /// @brief Guarda los datos actuales del paciente en un índice específico dentro del archivo indexado.
        /// @param idx Índice donde se guardará el registro.
        /// @param list Referencia al archivo indexado (IndexedFile) donde se almacenan los pacientes.
        /// @return true si la operación de guardado fue exitosa.
        bool saveAtIn(uint64_t idx, cfm::IndexedFile<Patient> &list);
        
        /// @brief Elimina el registro del paciente del archivo indexado usando su ID.
        /// @param list Referencia al archivo indexado (IndexedFile) de pacientes.
        void erasefrom(cfm::IndexedFile<Patient> &list);

        // --- Getters y Setters ---
        // Los métodos de una sola línea (getters triviales y setters de buffer simple)
        // se pueden dejar como 'inline' en la cabecera por razones de rendimiento
        // y conveniencia, pero los sacaré al CPP para mantener la consistencia.

        /// @brief Obtiene el ID único del paciente.
        /// @return El ID del paciente.
        int getID() const;

        /// @brief Obtiene el nombre del paciente.
        /// @return Una cadena (std::string) con el nombre.
        std::string getFirstName() const;
        /// @brief Establece el nombre del paciente.
        /// @param value La nueva cadena de nombre.
        void setFirstName(const std::string& value);

        /// @brief Obtiene el apellido del paciente.
        /// @return Una cadena (std::string) con el apellido.
        std::string getLastName() const;
        /// @brief Establece el apellido del paciente.
        /// @param value La nueva cadena de apellido.
        void setLastName(const std::string& value);

        /// @brief Obtiene el número de identificación nacional del paciente.
        /// @return Una cadena (std::string) con la identificación nacional.
        std::string getNationalID() const;
        /// @brief Establece el número de identificación nacional del paciente.
        /// @param value La nueva cadena de identificación nacional.
        void setNationalID(const std::string& value);

        /// @brief Obtiene la edad del paciente.
        /// @return La edad como entero.
        int getAge() const;
        /// @brief Establece la edad del paciente, limitándola a 150.
        /// @param value El nuevo valor de edad (uint8_t).
        /// @return true si el valor proporcionado es menor o igual a 150.
        bool setAge(uint8_t value);

        /// @brief Obtiene el género del paciente.
        /// @return El carácter del género ('M', 'F', u '?').
        char getGender() const;
        /// @brief Establece el género del paciente. Solo acepta 'm', 'M', 'f', 'F'.
        /// @param n El carácter del nuevo género.
        void setGender(char n);

        /// @brief Obtiene el tipo de sangre del paciente.
        /// @return Una cadena (std::string) con el tipo de sangre.
        std::string getBloodType() const;
        /// @brief Establece el tipo de sangre del paciente.
        /// @param value La nueva cadena de tipo de sangre.
        void setBloodType(const std::string& value);

        /// @brief Obtiene el número de teléfono del paciente.
        /// @return Una cadena (std::string) con el teléfono.
        std::string getPhone() const;
        /// @brief Establece el número de teléfono del paciente.
        /// @param value La nueva cadena de teléfono.
        void setPhone(const std::string& value);

        /// @brief Obtiene la dirección del paciente.
        /// @return Una cadena (std::string) con la dirección.
        std::string getAddress() const;
        /// @brief Establece la dirección del paciente.
        /// @param value La nueva cadena de dirección.
        void setAddress(const std::string& value);

        /// @brief Obtiene el correo electrónico del paciente.
        /// @return Una cadena (std::string) con el email.
        std::string getEmail() const;
        /// @brief Establece el correo electrónico del paciente.
        /// @param value La nueva cadena de correo electrónico.
        void setEmail(const std::string& value);

        /// @brief Obtiene la lista de alergias del paciente.
        /// @return Una cadena (std::string) con las alergias.
        std::string getAllergies() const;
        /// @brief Establece la lista de alergias del paciente.
        /// @param value La nueva cadena de alergias.
        void setAllergies(const std::string& value);

        /// @brief Obtiene las notas médicas del paciente.
        /// @return Una cadena (std::string) con las notas.
        std::string getNotes() const;
        /// @brief Establece las notas médicas del paciente.
        /// @param value La nueva cadena de notas.
        void setNotes(const std::string& value);
        
        // --- Métodos de Interfaz de Usuario (UI) ---

        /// @brief Obtiene una referencia débil (weak_ptr) al nodo UI que representa la tarjeta del paciente.
        /// @return std::weak_ptr<Node2D> al nodo de la tarjeta.
        std::weak_ptr<Node2D> getCard();
        
        /// @brief Borra la referencia a la tarjeta UI (libera el weak_ptr).
        void eraseCard();

        /// @brief Crea y configura el árbol de nodos de la interfaz de usuario para la tarjeta del paciente, incluyendo botones de edición y eliminación.
        /// @param list Referencia al archivo indexado (IndexedFile) de pacientes para manejar las acciones de guardado y eliminación.
        /// @return std::shared_ptr<Node2D> al nodo raíz de la tarjeta UI.
        std::shared_ptr<Node2D> createCard(cfm::IndexedFile<Patient> &list);
    };
}
#endif // PATIENT_HPP