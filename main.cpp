#include <iostream>
#include <string>

using namespace std;

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
    if (cin.peek() == '\n') cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void leerLinea(const string &prompt, string &out) {
    cout << prompt;
    limpiarBufferEntrada();
    getline(cin, out);
}

// Gestion del almacen 2D
LoteProduccion** crearAlmacen(int F, int C) {
    LoteProduccion **almacen = new LoteProduccion*[F];
    for (int i = 0; i < F; ++i) {
        almacen[i] = new LoteProduccion*[C];
        for (int j = 0; j < C; ++j) {
            almacen[i][j] = nullptr;
        }
    }
    return almacen;
}

void liberarAlmacen(LoteProduccion **&almacen, int F) {
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
    cout << "\n--- Menu Principal ---\n";
    cout << "1) Inicializar Almacen\n";
    cout << "2) Colocar Lote\n";
    cout << "3) Control de Calidad (Push)\n";
    cout << "4) Deshacer (Pop)\n";
    cout << "5) Reporte por Fila\n";
    cout << "7) Salir\n";
    cout << "Opcion: ";
}

void reportePorFila(LoteProduccion **almacen, int F, int C, int fila) {
    cout << "--- Reporte de Fila " << fila << " ---\n";
    for (int j = 0; j < C; ++j) {
        cout << "(" << fila << ", " << j << "): ";
        if (almacen[fila][j] == nullptr) {
            cout << "Vacio\n";
        } else {
            cout << "ID: " << almacen[fila][j]->idLote
                 << ", Nombre: " << almacen[fila][j]->nombreComponente << "\n";
        }
    }
}

// Programa principal
int main() {
    cout << "--- AlphaTech: Control de Lotes Dinamicos ---\n";

    LoteProduccion **almacen = nullptr;
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
        int opcion; cin >> opcion;

        switch (opcion) {
            case 1: {
                if (almacen) {
                    liberarAlmacen(almacen, F);
                    F = C = 0;
                }
                cout << "Fila inicial: "; cin >> F;
                cout << "Columna inicial: "; cin >> C;
                if (F <= 0 || C <= 0) {
                    cout << "Dimensiones invalidas.\n";
                    F = C = 0;
                    break;
                }
                almacen = crearAlmacen(F, C);
                inicializado = true;
                cout << "\nInicializacion exitosa. Almacen " << F << "x" << C
                     << " creado. Capacidad Maestro: " << capacidadMaestro << ".\n";
                break;
            }
            case 2: {
                if (!inicializado) { cout << "Debe inicializar el almacen primero.\n"; break; }
                int f, c;
                cout << "Posicion (F C): "; cin >> f >> c;
                if (f < 0 || f >= F || c < 0 || c >= C) {
                    cout << "Posicion fuera de rango.\n";
                    break;
                }
                if (almacen[f][c] != nullptr) {
                    cout << "La celda ya esta ocupada.\n";
                    break;
                }

                int idxLibre = buscarIndiceLibre(ocupado, capacidadMaestro);
                if (idxLibre == -1) {
                    cout << "Maestro de lotes lleno.\n";
                    break;
                }

                int id; float peso; int cantidad; string nombre;
                cout << "ID: "; cin >> id;
                leerLinea("Nombre: ", nombre);
                cout << "Peso Unitario: "; cin >> peso;
                cout << "Cantidad Total: "; cin >> cantidad;

                maestroLotes[idxLibre].idLote = id;
                strncpy(maestroLotes[idxLibre].nombreComponente, nombre.c_str(), 
                        sizeof(maestroLotes[idxLibre].nombreComponente) - 1);
                maestroLotes[idxLibre].nombreComponente[sizeof(maestroLotes[idxLibre].nombreComponente) - 1] = '\0';
                maestroLotes[idxLibre].pesoUnitario = peso;
                maestroLotes[idxLibre].cantidadTotal = cantidad;
                ocupado[idxLibre] = 1;

                almacen[f][c] = &maestroLotes[idxLibre];

                cout << "Lote " << id << " (" << maestroLotes[idxLibre].nombreComponente
                     << ") registrado y colocado en (" << f << ", " << c << ").\n";
                break;
            }
            case 3: {
                if (!inicializado) { cout << "Debe inicializar el almacen primero.\n"; break; }
                int id; int res;
                cout << "ID de Lote a inspeccionar: "; cin >> id;
                int idx = buscarIndicePorID(maestroLotes, ocupado, capacidadMaestro, id);
                if (idx == -1) {
                    cout << "El ID no existe en el maestro.\n";
                    break;
                }
                cout << "Resultado (1=Aprob, 0=Rech): "; cin >> res;
                res = (res != 0) ? 1 : 0;

                pushInspeccion(pila, id, res);
                cout << "Inspeccion Lote " << id << " (" << maestroLotes[idx].nombreComponente
                     << "): Resultado " << (res == 1 ? "Aprobado (1)" : "Rechazado (0)") << ".\n";
                cout << "Evento PUSH a Pila: Lote " << id << " | Resultado " << res << ".\n";
                break;
            }
            case 4: {
                int id, res;
                if (popInspeccion(pila, id, res)) {
                    cout << "POP de Pila: Evento Lote " << id << " | Resultado "
                         << (res == 1 ? "Aprobado (1)" : "Rechazado (0)") << ".\n";
                    cout << "Historial de inspeccion revertido.\n";
                } else {
                    cout << "La pila esta vacia.\n";
                }
                break;
            }
            case 5: {
                if (!inicializado) { cout << "Debe inicializar el almacen primero.\n"; break; }
                int fila; cout << "Fila a reportar: "; cin >> fila;
                if (fila < 0 || fila >= F) { cout << "Fila fuera de rango.\n"; break; }
                reportePorFila(almacen, F, C, fila);
                break;
            }
            case 7: {
                salir = true;
                break;
            }
            default:
                cout << "Opcion invalida.\n";
        }
    }

    // Liberacion de memoria
    cout << "Liberando memoria de estructuras LoteProduccion...\n";
    cout << "Liberando memoria de Matriz 2D...\n";
    liberarAlmacen(almacen, F);
    cout << "Liberando memoria de Pila y Vectores Paralelos...\n";
    liberarPila(pila);
    liberarMaestro(maestroLotes, ocupado);
    cout << "¡Memoria libre! Sistema terminado.\n";
    
    return 0;
}
