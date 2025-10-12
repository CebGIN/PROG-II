#include <iostream>
#include <windows.h>
#include <tchar.h> // Necesario para _T

// 1. Declaración del Procedimiento de Ventana (WndProc)
// Esta es la función que procesa todos los mensajes de la ventana.
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CUT :
            std::cout<<"Hola";
            break;
        case WM_CLOSE: // Se cierra la ventana (p.ej., el usuario hizo clic en la 'X')
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY: // La ventana ha sido destruida
            PostQuitMessage(0); // Envía un mensaje WM_QUIT a la cola de mensajes
            break;
        default:
            // Para todos los demás mensajes que no manejamos,
            // los pasamos a la función de procedimiento de ventana predeterminada.
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 2. Función de Entrada de la Aplicación (WinMain)
// Este es el equivalente a main() para las aplicaciones GUI de Windows.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const TCHAR CLASS_NAME[] = _T("MiClaseVentana");

    // ==========================================================
    // PARTE A: REGISTRAR LA CLASE DE VENTANA
    // ==========================================================
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);                // Tamaño de la estructura
    wc.lpfnWndProc = WndProc;                      // Puntero a la función de manejo de mensajes
    wc.hInstance = hInstance;                      // Identificador de la instancia de la aplicación
    wc.lpszClassName = CLASS_NAME;                 // Nombre de la clase (usado al crear la ventana)
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // Cursor por defecto (flecha)
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Color de fondo (blanco/gris claro)

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, _T("El registro de la clase falló."), _T("Error"), MB_ICONERROR);
        return 0;
    }

    // ==========================================================
    // PARTE B: CREAR LA VENTANA (Instancia)
    // ==========================================================
    
    HWND hwnd = CreateWindowEx(
        0,                                 // Estilos de ventana extendidos
        CLASS_NAME,                        // Nombre de la Clase (el que registramos)
        _T("HolaWindows"),      // Título de la ventana
        WS_OVERLAPPEDWINDOW,               // Estilo de ventana (borde, barra de título, minimizar/maximizar)
        CW_USEDEFAULT, CW_USEDEFAULT,      // Posición inicial (X, Y)
        500, 350,                          // Tamaño (Ancho, Alto)
        NULL,                              // Ventana Padre (NULL para la principal)
        NULL,                              // Menú (NULL, no hay menú)
        hInstance,                         // Identificador de la instancia de la aplicación
        NULL                               // Datos de creación adicionales
    );

    if (hwnd == NULL) {
        MessageBox(NULL, _T("La creación de la ventana falló."), _T("Error"), MB_ICONERROR);
        return 0;
    }

    // Mostrar y actualizar la ventana
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // ==========================================================
    // PARTE C: EL BUCLE DE MENSAJES (Message Loop)
    // ==========================================================
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg); // Traduce mensajes de teclado a caracteres
        DispatchMessage(&msg);  // Envía el mensaje a la función WndProc
    }
    
    return (int)msg.wParam;
}