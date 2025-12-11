#include <iostream>
#include <vector>
#include <string>
// --- NUEVAS LIBRERÍAS NECESARIAS ---
#include <thread> // Para crear hilos (concurrencia)
#include <mutex>  // Para usar el MUTEX (el cerrojo)

using namespace std;

// Estructura para representar un proceso
struct Proceso {
    string nombre;
    int memoria;
    int velocidadCPU;
    int solicitudMemoria;
    int solicitudCPU;
};

// --- VARIABLES GLOBALES (Recursos Compartidos) ---
// Las movemos fuera del main para que los hilos puedan acceder a ellas.
int numProcesos = 3;
vector<Proceso> procesos(numProcesos);
int memoriaDisponible = 500;
int velocidadCPU = 4;

// --- DECLARACIÓN DEL MUTEX (El "Cerrojo") ---
mutex mtxRecursos;

// --- FUNCIÓN QUE EJECUTARÁN LOS HILOS ---
// Esta función representa a un proceso intentando entrar al "baño" (usar recursos).
void intentarObtenerRecursos(int i) {
    // 1. BLOQUEAR (Echar llave)
    // Antes de tocar nada importante, el hilo pide el cerrojo.
    // Si ya está ocupado, espera aquí.
    mtxRecursos.lock();

    cout << "\n[Hilo " << procesos[i].nombre << "] Entrando a la zona critica (Tengo la llave)..." << endl;

    // --- INICIO DE LA SECCIÓN CRÍTICA (Zona de peligro) ---
    // Justo aquí es donde podrían ocurrir errores si no tuviéramos el mutex,
    // porque varios hilos intentarían leer y restar a la vez.
    if (procesos[i].solicitudMemoria <= memoriaDisponible &&
        procesos[i].solicitudCPU <= velocidadCPU) {

        // Asignar los recursos
        memoriaDisponible -= procesos[i].solicitudMemoria;
        velocidadCPU -= procesos[i].solicitudCPU;

        cout << "   -> " << procesos[i].nombre << " OBTIENE "
             << procesos[i].solicitudMemoria << "MB y " << procesos[i].solicitudCPU << "GHz." << endl;
    } else {
        // Rechazar
        cout << "   -> " << procesos[i].nombre << " NO puede obtener recursos." << endl;
    }
    // Mostramos cómo quedan los recursos para verificar que el cálculo fue correcto.
    cout << "      (Quedan: Memoria=" << memoriaDisponible << ", CPU=" << velocidadCPU << ")" << endl;
    // --- FIN DE LA SECCIÓN CRÍTICA ---


    cout << "[Hilo " << procesos[i].nombre << "] Saliendo y devolviendo la llave.\n" << endl;

    // 2. DESBLOQUEAR (Abrir el cerrojo)
    // Es OBLIGATORIO soltar la llave para que otros hilos puedan entrar.
    mtxRecursos.unlock();
}


int main() {
    // Inicializar los datos de los procesos
    procesos[0] = {"Proceso A", 100, 2, 50, 1};
    procesos[1] = {"Proceso B", 150, 3, 80, 2};
    procesos[2] = {"Proceso C", 200, 1, 100, 3};

    cout << "--- Inicio de la Simulacion Concurrente (con MUTEX) ---" << endl;
    cout << "Recursos iniciales: Memoria=" << memoriaDisponible << ", CPU=" << velocidadCPU << endl << endl;

    // --- CREACIÓN DE LOS HILOS ---
    // En lugar de un bucle 'for', lanzamos 3 hilos a la vez.
    // Cada hilo ejecutará la función 'intentarObtenerRecursos'.
    thread t1(intentarObtenerRecursos, 0); // Hilo para el Proceso A
    thread t2(intentarObtenerRecursos, 1); // Hilo para el Proceso B
    thread t3(intentarObtenerRecursos, 2); // Hilo para el Proceso C

    // --- ESPERAR A LOS HILOS ---
    // 'join()' obliga al programa principal (main) a esperar a que
    // los hilos terminen su trabajo antes de finalizar.
    t1.join();
    t2.join();
    t3.join();

    cout << "--- Fin de la Simulacion ---" << endl;

    // Pausa para Dev-C++
    system("pause");

    return 0;
}
