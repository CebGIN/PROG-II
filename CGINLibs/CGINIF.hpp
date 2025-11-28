#ifndef CGINIF_hpp
#define CGINIF_hpp

#include "CGINFU.hpp"
#include <iostream>
#include <fstream>
#include <string>

namespace cfm {

    class Data{
        std::string fileName = "Folder/index.idx";
        

        public:

        Data() = default;
        Data(std::string Name): fileName(Name + "/data.txt"){
            std::cout<< "Data creada en " + fileName << std::endl;
            std::fstream archivo(fileName, std::ios::in | std::ios::out);
        }

        uint64_t getEndOfFile(){
            std::fstream File(fileName, std::ios::in | std::ios::out | std::ios::binary | std::ios::ate);
            uint64_t endOfFile = File.tellp();
            return endOfFile;
        }

        void add(std::string datos){
            std::ofstream data(fileName, std::ios::app);
            data << datos << std::endl;
        }
    };

    class Index{
        std::string indexFileName = "Folder/index.idx";
        std::string freeSpacesFileName = "Folder/spaces.idx";
        uint64_t nextID = 1;
        uint64_t amountOfFreeSpaces = 0;
        uint64_t amountOfEntries = 0;

        public:
        Index() = default;
        Index(std::string Name): indexFileName(Name + "/index.idx"), freeSpacesFileName(Name + "/spaces.idx") {
            std::cout<< "Index creado en " + indexFileName << std::endl;
            
            std::ifstream checkIndexFile(indexFileName, std::ios::binary);
            if (checkIndexFile.is_open()) {
                checkIndexFile.read(reinterpret_cast<char*>(&nextID), sizeof(uint64_t));
                checkIndexFile.read(reinterpret_cast<char*>(&amountOfEntries), sizeof(uint64_t));
                checkIndexFile.close();
            } else {
                std::ofstream outfile(indexFileName, std::ios::out | std::ios::binary);

                if (outfile.is_open()) {
                    outfile.write(reinterpret_cast<const char*>(&nextID), sizeof(uint64_t));
                    outfile.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
                    if (outfile.good()) {
                        // std::cout << " Dato binario escrito con éxito al inicio del nuevo archivo." << std::endl;
                    } else {
                        std::cerr << " Error durante la escritura binaria." << std::endl;
                    }
                    outfile.close();
                } else {
                    std::cerr << " Error al crear el archivo (Verifique la ruta/permisos)." << std::endl;
                }
            }

            std::ifstream checkSpacesFile(freeSpacesFileName, std::ios::binary);
            if (checkSpacesFile.is_open()) {
                checkSpacesFile.read(reinterpret_cast<char*>(&amountOfFreeSpaces), sizeof(uint64_t));
                checkSpacesFile.close();   
            } else {
                std::ofstream outfile(freeSpacesFileName, std::ios::out | std::ios::binary);
                if (outfile.is_open()) {
                    outfile.write(reinterpret_cast<const char*>(&amountOfFreeSpaces), sizeof(uint64_t));
                    if (outfile.good()) {
                        // std::cout << " Dato binario escrito con éxito al inicio del nuevo archivo." << std::endl;
                    } else {
                        std::cerr << " Error durante la escritura binaria." << std::endl;
                    }
                    outfile.close();
                } else {
                    std::cerr << " Error al crear el archivo (Verifique la ruta/permisos)." << std::endl;
                }
            }

        }

        bool pushFreeSpaces(uint64_t space){
            std::fstream spacesFile(freeSpacesFileName, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
            
            spacesFile.write(reinterpret_cast<const char*>(&space), sizeof(uint64_t));
            if (!spacesFile) {
                std::cerr << " Error al sobrescribir el contador." << std::endl;
                return false;}
                
            amountOfFreeSpaces++;

            spacesFile.seekp(0, std::ios::beg);

            spacesFile.write(reinterpret_cast<const char*>(&amountOfFreeSpaces), sizeof(uint64_t));

            if (!spacesFile) {
                std::cerr << " Error al sobrescribir el contador." << std::endl;
                return false;
            }
            spacesFile.close();
            
            return true;
        }

        uint64_t popFreeSpaces(){
            // 1. Validar que la pila no esté vacía
            if (amountOfFreeSpaces == 0) {
                // std::cout << "La pila de espacios libres está vacía." << std::endl;
                
                return 1;
            }
            // Abrir el archivo en modo lectura/escritura binario
            std::fstream spaces_file(freeSpacesFileName, std::ios::binary | std::ios::in | std::ios::out);

            if (!spaces_file.is_open()) {
                throw std::runtime_error("Error: No se pudo abrir el archivo de espacios libres.");
            }
            
            // --- 2. Calcular la posición y leer el último dato ---
            
            // Posición del primer byte del último dato:
            // Posición = (Tamaño total del encabezado) + (Índice del último elemento) * (Tamaño de cada elemento)
            // Posición = (1 * SIZE_OF_U64) + (amountOfFreeSpaces - 1) * SIZE_OF_U64
            
            // El tamaño del archivo actual (la posición después del último dato):
            std::streamoff current_file_size = (1 + amountOfFreeSpaces) * sizeof(uint64_t);

            // Posición de inicio de la lectura del último elemento:
            std::streamoff read_position = current_file_size - sizeof(uint64_t); 
            
            uint64_t popped_space = 0;
            
            // Mover el puntero de lectura ('g' de get)
            spaces_file.seekg(read_position, std::ios::beg);
            spaces_file.read(reinterpret_cast<char*>(&popped_space), sizeof(uint64_t));
            
            if (!spaces_file) {
                spaces_file.close();
                throw std::runtime_error("Error al leer el último dato del archivo.");
            }

            // --- 3. Truncar Archivo y Actualizar Contador ---
            
            // 3.1. Decrementar el contador en memoria.
            amountOfFreeSpaces--;
            
            // 3.2. Calcular el nuevo tamaño del archivo:
            // Nuevo tamaño = (1 * SIZE_OF_U64) + (Nuevo Contador) * SIZE_OF_U64
            std::streamoff new_file_size = (1 + amountOfFreeSpaces) * sizeof(uint64_t);
            
            // 3.3. Cerrar el fstream para que el archivo no esté bloqueado antes de truncar
            spaces_file.close(); 
            
            // 3.4. Truncar el archivo a su nuevo tamaño
            // Esto elimina físicamente el último elemento del archivo.
            cfm::truncateFile(freeSpacesFileName, new_file_size);
            
            // --- 4. Sobrescribir el Contador (requiere reabrir o usar el mismo método de truncado) ---
            
            // Reabrimos el archivo en modo escritura para sobrescribir el contador al inicio
            std::fstream update_counter(freeSpacesFileName, std::ios::binary | std::ios::out | std::ios::in);
            
            if (!update_counter.is_open()) {
                throw std::runtime_error("Error al reabrir el archivo para actualizar el contador.");
            }
            
            // El puntero de escritura ya está en 0 por defecto con este modo de apertura
            update_counter.write(reinterpret_cast<const char*>(&amountOfFreeSpaces), sizeof(uint64_t));
            update_counter.close();
            
            // std::cout << "Pop exitoso. Dato retornado: " << popped_space 
            //         << ", Nuevo contador: " << amountOfFreeSpaces << ", Nuevo tamaño: " 
            //         << new_file_size << " bytes." << std::endl;
                    
            return popped_space;
        }

        bool increaseAmountOfEntries(){
            amountOfEntries++;
            std::fstream index(indexFileName, std::ios::out | std::ios::binary);
            index.seekp(sizeof(uint64_t), std::ios::beg);
            index.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
            index.close();
            return true;
        }

        bool reduceAmountOfEntries(){
            amountOfEntries--;
            std::ofstream index(indexFileName, std::ios::out | std::ios::binary);
            index.seekp(sizeof(uint64_t), std::ios::beg);
            index.write(reinterpret_cast<const char*>(&amountOfEntries), sizeof(uint64_t));
            index.close();
            return true;
        }
        
        bool eraseFromIdx(uint64_t idx){
            if(idx >= nextID){
                //fuera de rango
                return false;
            }
            std::fstream index(indexFileName, std::ios::binary | std::ios::in | std::ios::out);
            std::streamoff offset = (1 + idx) * sizeof(uint64_t);

            uint64_t actualPosInside;
            index.seekg(offset, std::ios::beg);
            index.read(reinterpret_cast<char*>(&actualPosInside), sizeof(uint64_t) );

            if (actualPosInside == 0){
                // 0 significa que ya fue eliminada esa posicion.
                return false;
            }
            uint64_t zero = 0;
            index.seekp(offset, std::ios::beg);
            index.write(reinterpret_cast<char*>(&zero), sizeof(uint64_t));
            if (!index) return false; //error de escritura

            pushFreeSpaces(idx);

            index.close();
            return true;
        };

        std::streampos add(){
            uint64_t whereToSave = popFreeSpaces();
            std::fstream index(indexFileName, std::ios::app | std::ios::binary);
            index.write(reinterpret_cast<char*>(&whereToSave), sizeof(uint64_t));

        }
    };

    

    template <typename T>
    class IndexedFile {
        std::string folderName = "Folder";
        Index index;
        Data data;

        public:
        IndexedFile(std::string Name) : folderName(Name) {
            cfm::createFolder(Name);
            index = Index(Name);
            data = Data(Name);
        }

        void add(const T &object){
            std::streampos = index.add();
            data.add(reinterpret_cast<char*>object, sizeof(T));
        }
    };
}

#endif