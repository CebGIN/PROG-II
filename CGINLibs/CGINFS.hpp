#ifndef CGINFS_HPP
#define CGINFS_HPP

#include "CGINFU.hpp"
#include <fstream>
#include <string>

namespace cfm {
    template <typename T> class FileStack {
    private:
        const uint64_t HEADERSIZE = sizeof(uint64_t);
        std::string fileName;
        std::fstream file;
        uint64_t size = 0;

    public:
        FileStack(const std::string& nameOfFile)
            : fileName(nameOfFile) 
        {
            file.open(fileName, std::ios::in | std::ios::out | std::ios::binary); 

            if (!file.is_open()) {
                std::ofstream temp_creator(fileName, std::ios::binary);//Crea el archivo, peligro de truncado
                temp_creator.write(reinterpret_cast<char*>(&size), sizeof(uint64_t));
                if(!temp_creator) throw std::runtime_error("Error: Creando header de la pila: " + fileName);
                temp_creator.close();
        
                file.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
                if (!file.is_open()) {
                     throw std::runtime_error("Error critico: Permisos o I/O fallaron al abrir/crear la pila: " + fileName);
                }
            }

            
            file.clear();
            file.seekg(0, std::ios::end);
            uint64_t fileSize = static_cast<uint64_t>(file.tellg());
            //Si el tamaño del archivo es igual al del header, entonces el contador es 0, sin importar si el archivo existia o no.
            //Si es mayor entonces ya tiene entradas y hay que leer el contador.
            //Si es menor el archivo fue creado incorrectamente.
            if (fileSize > HEADERSIZE) {
                file.seekg(0, std::ios::beg);
                file.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
                if (!file) throw std::runtime_error("Error leyendo header de archivo de pila: " + fileName);
            } else if (fileSize < HEADERSIZE) throw std::runtime_error("Error: tamaño inesperado en archivo de pila: " + fileName);
            
        }
        
        ~FileStack(){
            if (file.is_open()) {
            file.close();
            }
        }

        uint64_t getSize() const {return size;}

        bool isStackEmpty(){return size == 0;}

        void push(const T &value){
            file.seekp(0, std::ios::end);
            file.write(reinterpret_cast<const char*>(&value), sizeof(T));
            if (!file) throw std::runtime_error("Error empujando en el archivo de pila: " + fileName);
            size++;

            file.seekp(0, std::ios::beg);
            file.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));

            if (!file) throw std::runtime_error("Error actualiando el contador en el archivo de pila: " + fileName);
        }

        T pop(){
            // 1. Validar que la pila no esté vacía
            if (isStackEmpty()) {
                throw std::runtime_error("Error: Intento de obtener dato de la pila vacia: " + fileName);
            }

            T popped_data;
            
            // Mover el puntero de lectura ('g' de get)
            file.seekg(HEADERSIZE + (size - 1) * sizeof(T), std::ios::beg);
            file.read(reinterpret_cast<char*>(&popped_data), sizeof(T));
            
            if (!file) {
                throw std::runtime_error("Error al leer el ultimo dato del archivo.");
            }

            // --- 3. Truncar Archivo y Actualizar Contador ---
            // 3.1. Decrementar el contador en memoria.
            size--;
            // 3.2. Calcular el nuevo tamaño del archivo:
            // Nuevo tamaño = (1 * SIZE_OF_U64) + (Nuevo Contador) * SIZE_OF_U64
            std::streamoff new_file_size = HEADERSIZE + size * sizeof(T);
            // 3.3. Cerrar el fstream para que el archivo no esté bloqueado antes de truncar
            file.close(); 
            // 3.4. Truncar el archivo a su nuevo tamaño
            // Esto elimina físicamente el último elemento del archivo.
            cfm::truncateFile(fileName, new_file_size);
            
            // --- 4. Sobrescribir el Contador---
            file.open(fileName, std::ios::binary | std::ios::out | std::ios::in);
            if (!file.is_open()) {
                throw std::runtime_error("Error al reabrir el archivo para actualizar el contador.");
            }
            file.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
            return popped_data;
        }
    };
}
#endif