#include <iostream>
#include <string>
#include <cstring>
#include <limits>

// Estructura de lote
typedef struct {
    int idLote;
    char nombreComponente[50];
    float pesoUnitario;
    int cantidadTotal;
} LoteProduccion;

// Pila de inspecciones (maximo 10)
struct PilaInspecciones {
    int *pilaIDLote = nullptr;
    int *pilaResultado = nullptr;
    int capacidad = 10;
    int tam = 0;
};

// Utilidades de entrada
void limpiarBufferEntrada() {
    if (std::cin.peek() == '\n') std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void leerLinea(const std::string &prompt, std::string &out) {
    std::cout << prompt;
    limpiarBufferEntrada();
    std::getline(std::cin, out);
}

// Gestion del almacen 2D (matriz de punteros a LoteProduccion)
LoteProduccion*** crearAlmacen(int F, int C) {
    LoteProduccion ***almacen = new LoteProduccion**[F];
    for (int i = 0; i < F; ++i) {
        almacen[i] = new LoteProduccion*[C];
        for (int j = 0; j < C; ++j) {
            almacen[i][j] = nullptr;
        }
    }
    return almacen;
}

void liberarAlmacen(LoteProduccion ***&almacen, int F) {
    if (!almacen) return;
    for (int i = 0; i < F; ++i) {
        delete[] almacen[i];
    }
    delete[] almacen;
    almacen = nullptr;
}

// Gestion del maestro de lotes
void inicializarMaestro(LoteProduccion *&maestro, int *&ocupado, int capacidad) {
    maestro = new LoteProduccion[capacidad];
    ocupado = new int[capacidad];
    for (int i = 0; i < capacidad; ++i) {
        ocupado[i] = 0;
    }
}

void liberarMaestro(LoteProduccion *&maestro, int *&ocupado) {
    delete[] maestro; maestro = nullptr;
    delete[] ocupado; ocupado = nullptr;
}

int buscarIndiceLibre(int *ocupado, int capacidad) {
    for (int i = 0; i < capacidad; ++i) {
        if (ocupado[i] == 0) return i;
    }
    return -1;
}

int buscarIndicePorID(LoteProduccion *maestro, int *ocupado, int capacidad, int id) {
    for (int i = 0; i < capacidad; ++i) {
        if (ocupado[i] == 1 && maestro[i].idLote == id) return i;
    }
    return -1;
}

// Expandir maestro (duplicar capacidad)
void expandirMaestro(LoteProduccion *&maestro, int *&ocupado, int &capacidad) {
    int nuevaCap = (capacidad > 0) ? capacidad * 2 : 1;
    LoteProduccion *nuevoMaestro = new LoteProduccion[nuevaCap];
    int *nuevoOcupado = new int[nuevaCap];
    for (int i = 0; i < nuevaCap; ++i) nuevoOcupado[i] = 0;
    for (int i = 0; i < capacidad; ++i) {
        nuevoMaestro[i] = maestro[i];
        nuevoOcupado[i] = ocupado[i];
    }
    delete[] maestro;
    delete[] ocupado;
    maestro = nuevoMaestro;
    ocupado = nuevoOcupado;
    capacidad = nuevaCap;
}

// Obtener índice libre, expandiendo si es necesario
int obtenerIndiceLibre(LoteProduccion *&maestro, int *&ocupado, int &capacidad) {
    int idx = buscarIndiceLibre(ocupado, capacidad);
    if (idx == -1) {
        expandirMaestro(maestro, ocupado, capacidad);
        idx = buscarIndiceLibre(ocupado, capacidad);
    }
    return idx;
}

// Gestion de la pila
void inicializarPila(PilaInspecciones &pila) {
    pila.pilaIDLote = new int[pila.capacidad];
    pila.pilaResultado = new int[pila.capacidad];
    pila.tam = 0;
}

void liberarPila(PilaInspecciones &pila) {
    delete[] pila.pilaIDLote; pila.pilaIDLote = nullptr;
    delete[] pila.pilaResultado; pila.pilaResultado = nullptr;
    pila.tam = 0;
}

void pushInspeccion(PilaInspecciones &pila, int idLote, int resultado) {
    if (pila.tam == pila.capacidad) {
        for (int i = 1; i < pila.capacidad; ++i) {
            pila.pilaIDLote[i - 1] = pila.pilaIDLote[i];
            pila.pilaResultado[i - 1] = pila.pilaResultado[i];
        }
        pila.pilaIDLote[pila.capacidad - 1] = idLote;
        pila.pilaResultado[pila.capacidad - 1] = resultado;
    } else {
        pila.pilaIDLote[pila.tam] = idLote;
        pila.pilaResultado[pila.tam] = resultado;
        pila.tam++;
    }
}

bool popInspeccion(PilaInspecciones &pila, int &idLote, int &resultado) {
    if (pila.tam == 0) return false;
    idLote = pila.pilaIDLote[pila.tam - 1];
    resultado = pila.pilaResultado[pila.tam - 1];
    pila.tam--;
    return true;
}

// Interfaz de usuario
void mostrarMenu() {
    std::cout << "\n--- Menu Principal ---\n";
    std::cout << "1) Inicializar Almacen\n";
    std::cout << "2) Colocar Lote\n";
    std::cout << "3) Control de Calidad (Push)\n";
    std::cout << "4) Deshacer (Pop)\n";
    std::cout << "5) Reporte por Fila\n";
    std::cout << "6) Buscar por Componente\n";
    std::cout << "7) Redimensionar Almacen\n";
    std::cout << "8) Salir\n";
    std::cout << "Opcion: ";
}

void reportePorFila(LoteProduccion ***almacen, int F, int C, int fila) {
    std::cout << "--- Reporte de Fila " << fila << " ---\n";
    for (int j = 0; j < C; ++j) {
        std::cout << "(" << fila << ", " << j << "): ";
        if (almacen[fila][j] == nullptr) {
            std::cout << "Vacio\n";
        } else {
            std::cout << "ID: " << almacen[fila][j]->idLote
                      << ", Nombre: " << almacen[fila][j]->nombreComponente << "\n";
        }
    }
}

// Buscar por componente recorriendo el almacen (muestra todas las coincidencias)
void buscarPorComponente(LoteProduccion ***almacen, int F, int C, const char *nombre) {
    bool encontrado = false;
    for (int i = 0; i < F; ++i) {
        for (int j = 0; j < C; ++j) {
            if (almacen[i][j] && std::strcmp(almacen[i][j]->nombreComponente, nombre) == 0) {
                std::cout << "Encontrado en (" << i << ", " << j << ") | Cantidad: "
                          << almacen[i][j]->cantidadTotal << "\n";
                encontrado = true;
            }
        }
    }
    if (!encontrado) {
        std::cout << "No se encontro el componente.\n";
    }
}

// Redimensionar almacen preservando punteros
bool redimensionarAlmacen(LoteProduccion ***&almacen, int &F, int &C, int nuevoF, int nuevoC) {
    if (!almacen || nuevoF <= 0 || nuevoC <= 0) return false;
    LoteProduccion ***nuevo = new LoteProduccion**[nuevoF];
    for (int i = 0; i < nuevoF; ++i) {
    nuevo[i] = new LoteProduccion*[nuevoC];
        for (int j = 0; j < nuevoC; ++j) nuevo[i][j] = nullptr;
    }
    int minF = (F < nuevoF ? F : nuevoF);
    int minC = (C < nuevoC ? C : nuevoC);
    for (int i = 0; i < minF; ++i) {
        for (int j = 0; j < minC; ++j) {
            nuevo[i][j] = almacen[i][j];
        }
    }
    // liberar antiguo
    for (int i = 0; i < F; ++i) delete[] almacen[i];
    delete[] almacen;
    almacen = nuevo;
    F = nuevoF; C = nuevoC;
    return true;
}

// Programa principal
int main() {
    std::cout << "--- AlphaTech: Control de Lotes Dinamicos ---\n";

    LoteProduccion ***almacen = nullptr;
    int F = 0, C = 0;

    LoteProduccion *maestroLotes = nullptr;
    int *ocupado = nullptr;
    int capacidadMaestro = 100;

    PilaInspecciones pila;
    bool inicializado = false;

    inicializarMaestro(maestroLotes, ocupado, capacidadMaestro);
    inicializarPila(pila);

    bool salir = false;
    while (!salir) {
        mostrarMenu();
        int opcion; std::cin >> opcion;

        switch (opcion) {
            case 1: {
                if (almacen) {
                    liberarAlmacen(almacen, F);
                    F = C = 0;
                }
                std::cout << "Fila inicial: "; std::cin >> F;
                std::cout << "Columna inicial: "; std::cin >> C;
                if (F <= 0 || C <= 0) {
                    std::cout << "Dimensiones invalidas.\n";
                    F = C = 0;
                    break;
                }
                almacen = crearAlmacen(F, C);
                inicializado = true;
                std::cout << "\nInicializacion exitosa. Almacen " << F << "x" << C
                          << " creado. Capacidad Maestro: " << capacidadMaestro << ".\n";
                break;
            }
            case 2: {
                if (!inicializado) { std::cout << "Debe inicializar el almacen primero.\n"; break; }
                int f, c;
                std::cout << "Posicion (F C): "; std::cin >> f >> c;
                if (f < 0 || f >= F || c < 0 || c >= C) {
                    std::cout << "Posicion fuera de rango.\n";
                    break;
                }
                if (almacen[f][c] != nullptr) {
                    std::cout << "La celda ya esta ocupada.\n";
                    break;
                }
                int idxLibre = obtenerIndiceLibre(maestroLotes, ocupado, capacidadMaestro);

                int id; float peso; int cantidad; std::string nombre;
                std::cout << "ID: "; std::cin >> id;
                leerLinea("Nombre: ", nombre);
                std::cout << "Peso Unitario: "; std::cin >> peso;
                std::cout << "Cantidad Total: "; std::cin >> cantidad;

                maestroLotes[idxLibre].idLote = id;
                std::strncpy(maestroLotes[idxLibre].nombreComponente, nombre.c_str(), 
                        sizeof(maestroLotes[idxLibre].nombreComponente) - 1);
                maestroLotes[idxLibre].nombreComponente[sizeof(maestroLotes[idxLibre].nombreComponente) - 1] = '\0';
                maestroLotes[idxLibre].pesoUnitario = peso;
                maestroLotes[idxLibre].cantidadTotal = cantidad;
                ocupado[idxLibre] = 1;

                almacen[f][c] = &maestroLotes[idxLibre];

                std::cout << "Lote " << id << " (" << maestroLotes[idxLibre].nombreComponente
                          << ") registrado y colocado en (" << f << ", " << c << ").\n";
                break;
            }
            case 3: {
                if (!inicializado) { std::cout << "Debe inicializar el almacen primero.\n"; break; }
                int id; int res;
                std::cout << "ID de Lote a inspeccionar: "; std::cin >> id;
                int idx = buscarIndicePorID(maestroLotes, ocupado, capacidadMaestro, id);
                if (idx == -1) {
                    std::cout << "El ID no existe en el maestro.\n";
                    break;
                }
                std::cout << "Resultado (1=Aprob, 0=Rech): "; std::cin >> res;
                res = (res != 0) ? 1 : 0;

                pushInspeccion(pila, id, res);
                std::cout << "Inspeccion Lote " << id << " (" << maestroLotes[idx].nombreComponente
                          << "): Resultado " << (res == 1 ? "Aprobado (1)" : "Rechazado (0)") << ".\n";
                std::cout << "Evento PUSH a Pila: Lote " << id << " | Resultado " << res << ".\n";
                break;
            }
            case 4: {
                int id, res;
                if (popInspeccion(pila, id, res)) {
                    std::cout << "POP de Pila: Evento Lote " << id << " | Resultado "
                              << (res == 1 ? "Aprobado (1)" : "Rechazado (0)") << ".\n";
                    std::cout << "Historial de inspeccion revertido.\n";
                } else {
                    std::cout << "La pila esta vacia.\n";
                }
                break;
            }
            case 5: {
                if (!inicializado) { std::cout << "Debe inicializar el almacen primero.\n"; break; }
                int fila; std::cout << "Fila a reportar: "; std::cin >> fila;
                if (fila < 0 || fila >= F) { std::cout << "Fila fuera de rango.\n"; break; }
                reportePorFila(almacen, F, C, fila);
                break;
            }
            case 6: {
                if (!inicializado) { std::cout << "Debe inicializar el almacen primero.\n"; break; }
                std::string nombre; leerLinea("Nombre de componente a buscar: ", nombre);
                buscarPorComponente(almacen, F, C, nombre.c_str());
                break;
            }
            case 7: {
                if (!inicializado) { std::cout << "Debe inicializar el almacen primero.\n"; break; }
                int nuevoF, nuevoC;
                std::cout << "Nuevo F: "; std::cin >> nuevoF;
                std::cout << "Nuevo C: "; std::cin >> nuevoC;
                if (nuevoF <= 0 || nuevoC <= 0) { std::cout << "Dimensiones invalidas.\n"; break; }
                if (redimensionarAlmacen(almacen, F, C, nuevoF, nuevoC)) {
                    std::cout << "Almacen redimensionado a " << F << "x" << C << ".\n";
                } else {
                    std::cout << "No se pudo redimensionar.\n";
                }
                break;
            }
            case 8: {
                salir = true;
                break;
            }
            default:
                std::cout << "Opcion invalida.\n";
        }
    }

    // Liberacion de memoria
    std::cout << "Liberando memoria de estructuras LoteProduccion...\n";
    std::cout << "Liberando memoria de Matriz 2D...\n";
    liberarAlmacen(almacen, F);
    std::cout << "Liberando memoria de Pila y Vectores Paralelos...\n";
    liberarPila(pila);
    liberarMaestro(maestroLotes, ocupado);
    std::cout << "Memoria libre! Sistema terminado.\n";
    
    return 0;
}
