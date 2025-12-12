// Appointment.hpp
#ifndef APPOINTMENT_HPP
#define APPOINTMENT_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Patient/patient.hpp" // Dependencia: Clase Patient
#include "../Doctor/doctor.hpp"   // Dependencia: Clase Doctor
#include <cstdint>
#include <string>

namespace app{
    class Appointment {
    private:
        uint64_t id;
        uint64_t patientID;
        uint64_t doctorID;

        char date[11] = "XX/XX/XXXX";
        char time[6] = "XX:XX";
        char reason[150] = "Default";
        char status[20] = "Default";
        char notes[200] = "Default";

        std::weak_ptr<Node2D> card;

    public :
        
        /// @brief Constructor por defecto de Appointment.
        Appointment() = default;
        
        /// @brief Constructor que inicializa la cita con IDs específicos de cita, paciente y doctor.
        /// @param ID Identificador único de la cita.
        /// @param p ID del paciente asociado.
        /// @param d ID del doctor asociado.
        Appointment(uint64_t ID, uint64_t p, uint64_t d);
        
        // --- Getters de ID ---

        /// @brief Obtiene el ID único de la cita.
        /// @return El ID de la cita.
        int getID() const;
        /// @brief Obtiene el ID del paciente asociado a la cita.
        /// @return El ID del paciente.
        int getPatientID() const;
        /// @brief Obtiene el ID del doctor asociado a la cita.
        /// @return El ID del doctor.
        int getdoctorID() const;
        
        // --- Getters y Setters de Datos ---

        /// @brief Obtiene la fecha de la cita (formato "XX/XX/XXXX").
        /// @return Una cadena (std::string) con la fecha.
        std::string getDate() const;
        /// @brief Establece la fecha de la cita.
        /// @param d La nueva cadena de fecha.
        void setDate(const std::string& d);
        
        /// @brief Obtiene la hora de la cita (formato "XX:XX").
        /// @return Una cadena (std::string) con la hora.
        std::string getTime() const;
        /// @brief Establece la hora de la cita.
        /// @param t La nueva cadena de hora.
        void setTime(const std::string& t);
        
        /// @brief Obtiene el motivo de la consulta.
        /// @return Una cadena (std::string) con el motivo.
        std::string getReason() const;
        /// @brief Establece el motivo de la consulta.
        /// @param r La nueva cadena de motivo.
        void setReason(const std::string& r);
        
        /// @brief Obtiene el estado actual de la cita (ej. Pendiente, Confirmada, Cancelada).
        /// @return Una cadena (std::string) con el estado.
        std::string getStatus() const;
        /// @brief Establece el estado de la cita.
        /// @param s La nueva cadena de estado.
        void setStatus(const std::string& s);
        
        /// @brief Obtiene notas adicionales sobre la cita.
        /// @return Una cadena (std::string) con las notas.
        std::string getNotes() const;
        /// @brief Establece notas adicionales sobre la cita.
        /// @param n La nueva cadena de notas.
        void setNotes(const std::string& n);
        
        // --- Métodos de UI y Persistencia ---

        /// @brief Obtiene una referencia débil (weak_ptr) al nodo UI que representa la tarjeta de la cita.
        /// @return std::weak_ptr<Node2D> al nodo de la tarjeta.
        std::weak_ptr<Node2D> getCard();
        /// @brief Borra la referencia a la tarjeta UI.
        void eraseCard();
        
        /// @brief Guarda los datos actuales de la cita en un índice específico dentro del archivo indexado.
        /// @param idx Índice donde se guardará el registro.
        /// @param list Referencia al archivo indexado (IndexedFile) donde se almacenan las citas.
        /// @return true si la operación de guardado fue exitosa.
        bool saveAtIn(uint64_t idx, cfm::IndexedFile<Appointment> &list);
        
        /// @brief Elimina el registro de la cita del archivo indexado usando su ID.
        /// @param list Referencia al archivo indexado (IndexedFile) de citas.
        void erasefrom(cfm::IndexedFile<app::Appointment> &list);
        
        /// @brief Crea y configura el árbol de nodos de la interfaz de usuario para la tarjeta de la cita.
        /// @param list Referencia al archivo de citas (para guardar cambios).
        /// @param patients Referencia al archivo de pacientes (para obtener datos del paciente).
        /// @param doctors Referencia al archivo de doctores (para obtener datos del doctor y la tarifa).
        /// @param contextFlag Flag para indicar el contexto de la tarjeta (si es relevante para la UI).
        /// @return std::shared_ptr<Node2D> al nodo raíz de la tarjeta UI, o nullptr si faltan Patient/Doctor.
        std::shared_ptr<Node2D> createCard(cfm::IndexedFile<Appointment> &list, cfm::IndexedFile<ptn::Patient> &patients, cfm::IndexedFile<doc::Doctor> &doctors, int contextFlag);
    };
}
#endif // APPOINTMENT_HPP