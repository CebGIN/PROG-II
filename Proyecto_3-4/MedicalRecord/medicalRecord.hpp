// MedicalRecord.hpp
#ifndef RECORD_HPP
#define RECORD_HPP

#include <cstdint>
#include <string>

namespace app{

    class MedicalRecord {
    private:
        int Id;
        char date[11];
        char time[6];
        char diagnosis[200];
        char treatment[200];
        char medications[150];
        uint64_t doctorId;
        float cost;
        

    public:

        /// @brief Constructor por defecto del registro médico.
        MedicalRecord() = default;
        
        /// @brief Constructor que inicializa el registro con un ID y el ID del doctor.
        /// @param id ID único del registro.
        /// @param doctor ID del doctor que generó el registro.
        MedicalRecord(uint64_t id, uint64_t doctor);

        // --- Getters y Setters ---

        /// @brief Obtiene el ID del registro médico.
        /// @return El ID del registro.
        int getId() const;

        /// @brief Obtiene la fecha del registro (formato "XX/XX/XXXX").
        /// @return Una cadena (std::string) con la fecha.
        std::string getDate() const;
        /// @brief Establece la fecha del registro.
        /// @param value La nueva cadena de fecha.
        void setDate(const std::string& value);

        /// @brief Obtiene la hora del registro (formato "XX:XX").
        /// @return Una cadena (std::string) con la hora.
        std::string getTime() const;
        /// @brief Establece la hora del registro.
        /// @param value La nueva cadena de hora.
        void setTime(const std::string& value);

        /// @brief Obtiene el diagnóstico registrado.
        /// @return Una cadena (std::string) con el diagnóstico.
        std::string getDiagnosis() const;
        /// @brief Establece el diagnóstico.
        /// @param value La nueva cadena de diagnóstico.
        void setDiagnosis(const std::string& value);

        /// @brief Obtiene el tratamiento registrado.
        /// @return Una cadena (std::string) con el tratamiento.
        std::string getTreatment() const;
        /// @brief Establece el tratamiento.
        /// @param value La nueva cadena de tratamiento.
        void setTreatment(const std::string& value);

        /// @brief Obtiene las medicaciones prescritas.
        /// @return Una cadena (std::string) con las medicaciones.
        std::string getMedications() const;
        /// @brief Establece las medicaciones.
        /// @param value La nueva cadena de medicaciones.
        void setMedications(const std::string& value);

        /// @brief Obtiene el ID del doctor que generó el registro.
        /// @return El ID del doctor.
        int getDoctorId() const;

        /// @brief Obtiene el costo asociado al registro o la consulta.
        /// @return El costo como flotante (float).
        float getCost() const;
        /// @brief Establece el costo, asegurando que sea al menos 0.0f.
        /// @param value El nuevo costo.
        void setCost(float value);
    };
}

#endif // RECORD_HPP