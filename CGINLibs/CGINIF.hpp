#ifndef CGINIF_HPP
#define CGINIF_HPP

#include "CGINFU.hpp"
#include "CGINFS.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>

namespace cfm {

    class Data{
        std::string fileName = "Folder/data.dat";
        unsigned int sizeOfDataInUse = 4;

        std::fstream dataFile;

        public:

        Data(std::string Name, unsigned int Size) : 
            fileName(Name + "/data.bin"), 
            sizeOfDataInUse(Size) 
        {
            // 2. Abrir el stream
            // ios::in | ios::out permite lectura/escritura.
            // ios::binary es esencial.
            // ios::ate (at end) coloca el puntero al final, pero es opcional aquí.
            dataFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary); 

            if (!dataFile.is_open()) {
                // Forzar creación
                std::ofstream temp_creator(fileName, std::ios::binary);
                temp_creator.close();
        
                dataFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
                
                if (!dataFile.is_open()) {
                    throw std::runtime_error("Error critico: Permisos o I/O fallaron al abrir/crear el archivo de datos: " + fileName);
                }
            }
            
            // 3. (Opcional) Limpiar flags si se reabrió, aunque el modo in|out es seguro
            dataFile.clear(); 
        }

        //Cerrar el stream
        ~Data() {
            if (dataFile.is_open()) {
                dataFile.close();
            }
        }

        uint64_t getEndOfFile(){
            dataFile.seekp(0, std::ios::end);
            uint64_t endOfFile = static_cast<uint64_t>(dataFile.tellp());
            return endOfFile;
        }

        /**
         * @brief Añade o modifica datos en el archivo en una posición específica.
         *
         * @param datos Puntero a los datos a escribir.
         * @param pos Posición (offset) de inicio de la escritura en el archivo.
         */
        void addModify(const char* datos, std::streampos pos) {
            // 1. Posicionar el puntero de escritura
            dataFile.seekp(pos);

            if (dataFile.fail()) {
                throw std::runtime_error("Error al posicionar el puntero (seekp). Posición inválida o error interno del stream.");
            }

            // 3. Escribir los datos
            dataFile.write(datos, sizeOfDataInUse);

            if (dataFile.fail()) {
                throw std::runtime_error("Error de escritura de datos.");
            }
        }

        char* getDataAtPos(uint64_t pos){
            char* dataBlob = new char[sizeOfDataInUse];
            dataFile.seekg(pos);
            dataFile.read(dataBlob, sizeOfDataInUse);
            if (dataFile.fail() && !dataFile.eof()) {
                delete[] dataBlob;
                throw std::runtime_error("Error de lectura de datos (I/O error).");
            }
            return dataBlob;
        }
    };

    class Index{
        const uint64_t HEADERSIZE =  2 * sizeof(uint64_t);
        uint64_t nextIDX = 0;
        uint64_t amountOfEntries = 0;
        
        
        std::string fileName = "Folder/index.idx";
        cfm::FileStack<uint64_t> freeSpaces;
        
        std::fstream indexFile;

        public:
        cfm::FileStack<uint64_t>& getSpaces(){return freeSpaces;}

        Index(std::string Name) : fileName(Name + "/index.idx"), freeSpaces(Name + "/spaces.idx"){
            // --- ARCHIVO DE ÍNDICE (index.idx) ---
            indexFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);

            if (!indexFile.is_open()) {
                // El archivo NO PUDO abrirse. Intentamos forzar su creación
                std::ofstream temp_creator(fileName, std::ios::binary);
                temp_creator.write(reinterpret_cast<const char*>(&nextIDX), sizeof(uint64_t));
                temp_creator.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
                if(!temp_creator) throw std::runtime_error("Error: Creando header del indice: " + fileName);
                temp_creator.close(); // Fuerza la creación y cierra el descriptor.
        
                // Ahora reintentamos la apertura para lectura/escritura.
                indexFile.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
                
                if (!indexFile.is_open()) {
                     // Si falla por segunda vez, es probablemente un problema de permisos CRÍTICO.
                     throw std::runtime_error("Error critico: Permisos o I/O fallaron al abrir/crear el índice: " + fileName);
                }
            }

            // Mover el puntero al final para verificar el tamaño
            indexFile.seekg(0, std::ios::end);
            uint64_t indexSize = static_cast<uint64_t>(indexFile.tellg());
            if (indexSize > HEADERSIZE) {
                // El archivo existe, leer el header
                indexFile.seekg(0, std::ios::beg);
                indexFile.read(reinterpret_cast<char*>(&nextIDX), sizeof(uint64_t));
                indexFile.read(reinterpret_cast<char*>(&amountOfEntries), sizeof(uint64_t));
            } else if (indexSize < HEADERSIZE) throw std::runtime_error("Error: header invalido en el indice: " + fileName);
        }

        ~Index() {
            if (indexFile.is_open()) {
                indexFile.close();
            }
        }

        uint64_t getAmountOfEntries(){return amountOfEntries;}
        
        void increaseAmountOfEntries(){
            amountOfEntries++;
            nextIDX++;
            indexFile.seekp(0, std::ios::beg);
            indexFile.write(reinterpret_cast<const char*>(&nextIDX), sizeof(uint64_t));
            indexFile.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
        }

        void decreaseAmountOfEntries(){
            amountOfEntries--;
            indexFile.seekp(sizeof(uint64_t), std::ios::beg);
            indexFile.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
        }

        void eraseFromIdx(uint64_t idx){
            // 1. Verificación de Rango (usando nextID, asumiendo que es el límite superior no inclusivo)
            // El límite se basa en cuántos IDs se han generado, no en cuántos existen actualmente.
            // **NOTA:** Esta verificación está comentada en tu código original, pero se incluye para completitud.
            if (idx >= nextIDX){
                throw std::out_of_range("Error: Intento de eliminar un indice fuera del rango generado.");
            }
            
            // El offset es: (Tamaño del Header: 2x uint64_t) + (Posición del Índice)
            std::streamoff offset = idx * sizeof(uint64_t) + HEADERSIZE;

            
            // --- 2. Preparar y Leer la posición actual del índice ---
            indexFile.clear(); // Limpiar el estado del stream antes de la lectura/seek
            indexFile.seekg(offset, std::ios::beg);
            
            uint64_t actualPosInside;
            // Si la lectura intenta ir más allá del final actual del archivo, read/failbit fallará.
            indexFile.read(reinterpret_cast<char*>(&actualPosInside), sizeof(uint64_t));
            
            if (indexFile.fail() && !indexFile.eof()) {
                indexFile.clear();
                throw std::runtime_error("Error I/O: Fallo al leer la entrada del índice.");
            }

            uint64_t nullPos = UINT64_MAX;

            // // --- 3. Verificación de Eliminado Previo ---
            // if (actualPosInside == nullPos){
            //     // El registro ya está marcado como eliminado, o nunca fue escrito.
            //     throw std::runtime_error("Error: Intento de eliminar índice que ya está marcado como libre/eliminado.");
            // }
            
            // --- 4. Sobrescribir y Marcar como Eliminado (UINT64_MAX) ---
            indexFile.clear(); // Limpiar el estado antes de la escritura
            indexFile.seekp(offset, std::ios::beg);
            
            indexFile.write(reinterpret_cast<char*>(&nullPos), sizeof(uint64_t));
            
            if (indexFile.fail()) {
                indexFile.clear();
                throw std::runtime_error("Error I/O: Fallo al escribir el marcador de eliminado en el índice.");
            }
            
            // Asegurar que la escritura se complete antes de continuar
            indexFile.flush();

            // --- 5. Lógica de Mantenimiento de Contadores ---
            // (ActualPosInside es la posición real en el archivo de datos)
            freeSpaces.push(actualPosInside);
            // Llama a decreaseAmountOfEntries, que también debe manejar sus propios errores.
            decreaseAmountOfEntries(); 
        };

        std::streampos addToIndex(){
            indexFile.seekp(0, std::ios::end);
            uint64_t whereToSave = freeSpaces.pop();
            indexFile.write(reinterpret_cast<char*>(&whereToSave), sizeof(uint64_t));
            increaseAmountOfEntries();
            return whereToSave;
        }
    
        uint64_t getPosOfIDX(uint64_t idx){
            if (idx >= nextIDX){
                return UINT64_MAX;
            }
            indexFile.seekg(idx * sizeof(uint64_t) + HEADERSIZE, std::ios::beg);
            uint64_t pos = 0;
            indexFile.read(reinterpret_cast<char*>(&pos), sizeof(uint64_t));
            return pos;
        }
    
    };

    template <typename T> class IndexedFile {
        std::string folderName = "Folder";
        cfm::Folder folder;
        Index index;
        Data data;

        public:
        IndexedFile(std::string Name) : 
            folderName(Name),
            folder(Name),
            index(Name),
            data(Name, sizeof(T)) {}

        void add(const T &object){
            cfm::FileStack<uint64_t>& stackREF = index.getSpaces();
            if (stackREF.isStackEmpty()) stackREF.push(data.getEndOfFile());
            std::streampos pos = index.addToIndex();
            data.addModify(reinterpret_cast<const char*>(&object), pos);
        }

        void eraseAtIdx(uint64_t idx){
            index.eraseFromIdx(idx);
        }

        int64_t getListSize(){
            return index.getAmountOfEntries();
        }

        std::unique_ptr<T> getAtIDX(uint64_t idx){
            // 1. Obtener la posición del dato en el archivo de datos principal
            uint64_t pos = index.getPosOfIDX(idx);
            
            if (pos == UINT64_MAX) return nullptr;
            
            // 2. Leer el bloque de bytes
            char* dataBlob = data.getDataAtPos(pos); 
        
            if (dataBlob == nullptr) {
                return nullptr;
            }
            // --- RECONSTRUCCIÓN DEL OBJETO T ---
            
            // 3. Crear el espacio de memoria para el nuevo objeto T
            // Usamos new T() para asegurar que la memoria se inicialice (opcional, pero seguro)
            std::unique_ptr<T> object_ptr = std::make_unique<T>();
        
            // 4. Copiar los bytes directamente del buffer al nuevo objeto
            // memcpy es la herramienta estándar para la copia eficiente de bloques de memoria.
            std::memcpy(object_ptr.get(), dataBlob, sizeof(T)); 
            
            // 5. Liberar la memoria temporal del dataBlob
            delete[] dataBlob;
            return object_ptr;
        }
    };

}

#endif