// Doctor.hpp
#ifndef DOCTOR_HPP
#define DOCTOR_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include <cstdint>
#include <string>

namespace doc{
    class Doctor {
    private:
        uint64_t id;
        char firstName[50] = "Default";
        char lastName[50] = "Default";
        char nationalId[20] = "V00.000.000";
        char specialty[50] = "Default";
        uint8_t yearsExperience = 0;
        float consultationFee = 1.0f;
        char workingHours[50];
        char phone[15] = "+5800005555555";
        char email[50] = "default@dominio";
        
        std::weak_ptr<Node2D> card;

    public:
        
        /// @brief Constructor por defecto de Doctor. Inicializa los campos con valores por defecto.
        Doctor() = default;
        
        /// @brief Constructor que inicializa el objeto Doctor con un ID específico.
        /// @param ID Identificador único del doctor.
        Doctor(uint64_t ID);

        // --- Métodos de Archivo y Persistencia ---

        /// @brief Guarda los datos actuales del doctor en un índice específico dentro del archivo indexado.
        /// @param idx Índice donde se guardará el registro.
        /// @param list Referencia al archivo indexado (IndexedFile) donde se almacenan los doctores.
        /// @return true si la operación de guardado fue exitosa.
        bool saveAtIn(uint64_t idx, cfm::IndexedFile<Doctor> &list);
        
        /// @brief Elimina el registro del doctor del archivo indexado usando su ID.
        /// @param list Referencia al archivo indexado (IndexedFile) de doctores.
        void erasefrom(cfm::IndexedFile<Doctor> &list);

        // --- Getters y Setters ---

        /// @brief Obtiene el ID único del doctor.
        /// @return El ID del doctor.
        int getID() const;

        /// @brief Obtiene el nombre del doctor.
        /// @return Una cadena (std::string) con el nombre.
        std::string getFirstName() const;
        /// @brief Establece el nombre del doctor.
        /// @param value La nueva cadena de nombre.
        void setFirstName(const std::string& value);

        /// @brief Obtiene el apellido del doctor.
        /// @return Una cadena (std::string) con el apellido.
        std::string getLastName() const;
        /// @brief Establece el apellido del doctor.
        /// @param value La nueva cadena de apellido.
        void setLastName(const std::string& value);

        /// @brief Obtiene el número de identificación nacional del doctor.
        /// @return Una cadena (std::string) con la identificación nacional.
        std::string getNationalID() const;
        /// @brief Establece el número de identificación nacional del doctor.
        /// @param value La nueva cadena de identificación nacional.
        void setNationalID(const std::string& value);

        /// @brief Obtiene la especialidad del doctor.
        /// @return Una cadena (std::string) con la especialidad.
        std::string getSpecialty() const;
        /// @brief Establece la especialidad del doctor.
        /// @param value La nueva cadena de especialidad.
        void setSpecialty(const std::string& value);

        /// @brief Obtiene los años de experiencia del doctor.
        /// @return Los años de experiencia como entero.
        int getYearsExperience() const;
        /// @brief Establece los años de experiencia, limitándolo a 150.
        /// @param value El nuevo valor de años de experiencia (uint8_t).
        /// @return true si el valor proporcionado es menor o igual a 150.
        bool setYearsExperience(uint8_t value);

        /// @brief Obtiene la tarifa de consulta del doctor.
        /// @return La tarifa como flotante (float).
        float getConsultationFee() const;
        /// @brief Establece la tarifa de consulta, asegurando que sea al menos 1.0.
        /// @param value La nueva tarifa de consulta.
        /// @return true si el valor proporcionado es mayor o igual a 1.0f.
        bool setConsultationFee(float value);

        /// @brief Obtiene el horario de trabajo del doctor.
        /// @return Una cadena (std::string) con el horario.
        std::string getWorkingHours() const;
        /// @brief Establece el horario de trabajo del doctor.
        /// @param value La nueva cadena de horario.
        void setWorkingHours(const std::string& value);

        /// @brief Obtiene el número de teléfono del doctor.
        /// @return Una cadena (std::string) con el teléfono.
        std::string getPhone() const;
        /// @brief Establece el número de teléfono del doctor.
        /// @param value La nueva cadena de teléfono.
        void setPhone(const std::string& value);

        /// @brief Obtiene el correo electrónico del doctor.
        /// @return Una cadena (std::string) con el email.
        std::string getEmail() const;
        /// @brief Establece el correo electrónico del doctor.
        /// @param value La nueva cadena de correo electrónico.
        void setEmail(const std::string& value);
        
        // --- Métodos de Interfaz de Usuario (UI) ---

        /// @brief Obtiene una referencia débil (weak_ptr) al nodo UI que representa la tarjeta del doctor.
        /// @return std::weak_ptr<Node2D> al nodo de la tarjeta.
        std::weak_ptr<Node2D> getCard();
        
        /// @brief Borra la referencia a la tarjeta UI (libera el weak_ptr).
        void eraseCard();

        /// @brief Crea y configura el árbol de nodos de la interfaz de usuario para la tarjeta del doctor.
        /// @param list Referencia al archivo indexado (IndexedFile) de doctores para guardar cambios.
        /// @return std::shared_ptr<Node2D> al nodo raíz de la tarjeta UI.
        std::shared_ptr<Node2D> createCard(cfm::IndexedFile<Doctor> &list);
    };

}
#endif // DOCTOR_HPP