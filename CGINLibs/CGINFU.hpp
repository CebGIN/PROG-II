#ifndef CGINFU_hpp
#define CGINFU_hpp

#include <windows.h>
#include <fcntl.h>
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
            throw std::runtime_error("Error al truncar el archivo (Windows).");
        }
        #endif 

        _close(fd);
    }

}
#endif