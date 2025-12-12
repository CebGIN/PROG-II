c#!/bin/bash

# Comandos de compilación: de .cpp a .o
g++ -c Appointment/appointment.cpp -o Appointment/appointment.o
g++ -c Doctor/doctor.cpp -o Doctor/doctor.o
g++ -c Patient/patient.cpp -o Patient/patient.o
g++ -c Hospital/hospital.cpp -o Hospital/hospital.o
g++ -c MedicalRecord/medicalRecord.cpp -o MedicalRecord/medicalRecord.o
g++ -c Menus/menus.cpp -o Menus/menus.o
g++ -c main.cpp -o main.o

# Comando de enlazado: de .o a ejecutable (llamado mi_programa)
g++ Appointment/appointment.o Doctor/doctor.o Patient/patient.o Hospital/hospital.o MedicalRecord/medicalRecord.o Menus/menus.o main.o -o mi_programa

# Verifica si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. El ejecutable se llama 'mi_programa'."
    # Opcional: Ejecutar el programa después de compilar
    # ./mi_programa
else
    echo "¡Error de compilación!"
fi
