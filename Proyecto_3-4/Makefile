# --- Variables ---
# Nombre del ejecutable final
TARGET = HospitalApp.exe

# El compilador que estamos usando
CC = g++

# Archivos fuente (C++ files)
SRCS = main.cpp Menus/menus.cpp Patient/patient.cpp Doctor/doctor.cpp Appointment/appointment.cpp MedicalRecord/medicalRecord.cpp

# Archivos objeto (se generan a partir de los SRCS)
OBJS = $(SRCS:.cpp=.o)

# Flags del compilador (ej. advertencias, estándar C++, optimizaciones)
CFLAGS = -Wall -Wextra -std=c++17 -O2

# Directorios de inclusión (donde buscar los .h)
INCLUDES = -I./Include

# Flags para el linker (bibliotecas)
LDFLAGS = 

# --- Reglas de Compilación ---

# Regla principal: Construir el TARGET
all: $(TARGET)

$(TARGET): $(OBJS)
    @echo "Linking..."
    $(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
    @echo "Build successful: $(TARGET)"

# Regla para compilar archivos individuales (.cpp a .o)
%.o: %.cpp
    @echo "Compiling $<..."
    $(CC) -c $< -o $@ $(CFLAGS) $(INCLUDES)

# Regla para limpiar los archivos generados
clean:
    @echo "Cleaning up..."
    del /f $(OBJS) $(TARGET) # Usar 'del' si estás en Windows/MinGW
    @echo "Cleanup complete."