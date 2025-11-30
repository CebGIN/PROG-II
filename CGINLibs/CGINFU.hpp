#ifndef CGINFU_HPP
#define CGINFU_HPP

#include <windows.h>
#include <stdexcept>
#include <fcntl.h>
#include <string>
#include <io.h>

namespace cfm{
    bool createFolder(const std::string& path) {
        if (CreateDirectoryA(path.c_str(), NULL)) {
            return true;
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            return true;
        }
        return false;
    }
    
    void truncateFile(const std::string& filename, std::streamoff new_size) {
        // Abrir archivo en modo lectura/escritura binario
        int fd = _open(filename.c_str(), _O_RDWR | _O_BINARY);
        if (fd == -1) {
            throw std::runtime_error("Error al abrir el archivo para truncar (Windows).");
        }

        // Usar la versión segura si está disponible (_chsize_s)
        #if defined(_MSC_VER)
            if (_chsize_s(fd, new_size) != 0) {
                _close(fd);
                throw std::runtime_error("Error al truncar el archivo (Windows).");
            }
        #else
        if (_chsize(fd, static_cast<long>(new_size)) == -1) {
            _close(fd);
            throw std::runtime_error("Er ror al truncar el archivo (Windows).");
        }
        #endif 

        _close(fd);
    }

    class Folder {
    private:
        std::string folderPath;

    public:
        Folder(const std::string& path) : folderPath(path) {
            // La creación de la carpeta se fuerza en la inicialización
            createFolder(folderPath);
        }

        // Método clave: Obtener la ruta completa para un archivo dentro de la carpeta
        std::string getFilePath(const std::string& filename) const {
            // Asegura que no haya doble barra si folderPath termina en '/'
            return folderPath + "/" + filename; 
        }
    };

}
#endif