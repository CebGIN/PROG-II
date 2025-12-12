// menus.hpp
#ifndef MENUS_HPP
#define MENUS_HPP

#include "../../CGINLibs/LegacyEngine.hpp"
#include "../../CGINLibs/UIPrefabs.hpp"
#include "../../CGINLibs/CGINIF.hpp"
#include "../Hospital/hospital.hpp"
#include "../MedicalRecord/medicalRecord.hpp"

// ====================================================================
// --- Funciones Utilitarias (Globales o fuera de namespace) ---
// ====================================================================

/// @brief Crea un índice CFM para las sub-citas o sub-registros asociados a un ID específico.
/// @param folder Ruta base (ej. Hospital/Appoinments).
/// @param subfolder Carpeta del recurso (ej. Patients).
/// @param id ID del paciente/doctor.
/// @return Puntero compartido al objeto Index creado.
std::shared_ptr<cfm::Index> getSubAppoinmentIndex(std::string folder, std::string subfolder, uint64_t id);

// ====================================================================
// --- FORWARD DECLARATIONS de Menús (Necesarias para llamadas cruzadas) ---
// ====================================================================

namespace ptn {
    /**
     * @brief Crea el nodo raíz para el menú de Pacientes (Patients).
     * * Este menú puede cargar las tarjetas de visualizacion y editado de los pacientes y crear nuevos pacientes.
     * * @param hospital Referencia al objeto principal del Hospital (datos, colecciones, etc.).
     * @return std::shared_ptr<Node> El nodo raíz de la escena del Menú de Pacientes.
     */
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital);
    // Vista de tarjeta de paciente individual
    std::shared_ptr<Node> createSinglePatientCardView(hos::Hospital &hospital, uint64_t i);
    // Obtiene el IndexedFile de registros médicos del paciente
    std::unique_ptr<cfm::IndexedFile<app::MedicalRecord>> getMedicalRecord(std::string folder, uint64_t ptnID);
    // Visor de todos los registros médicos de un paciente
    std::shared_ptr<Node> createMedicalRecordViewer(hos::Hospital &hospital, const Patient &patient);
}

namespace doc {
    /**
     * @brief Crea el nodo raíz para el menú de Doctores (Doctors).
     * * Este menú puede cargar las tarjetas de visualizacion y editado de los doctores y crear nuevos doctores.
     * * @param hospital Referencia al objeto principal del Hospital (datos, colecciones, etc.).
     * @return std::shared_ptr<Node> El nodo raíz de la escena del Menú de Doctores.
     */
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital);
    // Vista de tarjeta de doctor individual
    std::shared_ptr<Node> createSingleDoctorCardView(hos::Hospital &hospital, uint64_t i);
}

namespace app {
    /**
     * @brief Crea el nodo raíz para el menú de Citas (Appointments).
     * * Este menú puede cargar citas a nivel global, o específicas de un paciente o doctor.
     * Permite crear nuevas citas y archivar las existentes (creando un registro médico).
     * * @param hospital Referencia al objeto principal del Hospital (datos, colecciones, etc.).
     * @param contextFlag 0: Lista global, 1: Específico de Paciente, 2: Específico de Doctor.
     * @param contextIDX ID del Paciente o Doctor si contextFlag es 1 o 2, respectivamente.
     * @return std::shared_ptr<Node> El nodo raíz de la escena del Menú de Citas.
     */
    std::shared_ptr<Node> createMenu(hos::Hospital &hospital, int contextFlag, uint64_t contextIDX);
    // Menú para crear un registro médico a partir de una cita y eliminar la cita
    std::shared_ptr<Node> createMedicalRecordMenu(hos::Hospital &hospital, uint64_t appID, int contextFlag);
}

namespace hos {
    // Menú principal del hospital
    std::shared_ptr<Node> createMenu(Hospital &hospital);
    // Visor de tamaños de archivos de persistencia
    std::shared_ptr<Node> createFilesVisor(Hospital &hospital);
}

#endif // MENUS_HPP