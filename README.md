[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/8vy3mN7u)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-2972f46106e565e64193e422d61a12cf1da4916b45550586e14ef0a7c637dd04.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=20955899)
# Actividad 2b - Vectores y Matrices

## Sistema de Gestión de Lotes de Producción y Control de Calidad

La fábrica de componentes electrónicos "AlphaTech" requiere un sistema para rastrear la ubicación de sus lotes de producción dentro de un almacén bidimensional (Filas y Columnas) y monitorear los eventos de control de calidad de cada lote. El sistema debe permitir el manejo dinámico del tamaño del almacén y registrar el historial de inspecciones mediante una pila.

El sistema debe utilizar la siguiente combinación de estructuras dinámicas:

  1. Matriz Dinámica 2D (`struct LoteProduccion**`): Simula el almacén, donde cada celda almacena un puntero a una estructura LoteProduccion. Las dimensiones (Filas x Columnas) deben ser dinámicas.
  2. Vectores Paralelos Dinámicos (Maestro de Lotes): Almacenan todos los detalles de los lotes activos en el sistema.
  3. Pila Dinámica: Registra el historial de las últimas 10 inspecciones de calidad realizadas (LIFO), implementada usando los índices de un vector dinámico.

  > **typedef struct**
  >
  > Se define la estructura LoteProduccion para agrupar los datos complejos de cada lote:
  >
  > ```C++
  > typedef struct {
  >  int idLote;
  >  char nombreComponente[50];
  >  float pesoUnitario;
  >  int cantidadTotal;
  > } LoteProduccion;
  > ```

---

### Definición y Detalles del Proceso a Desarrollar

Para solucionar el problema, el programa debe seguir un flujo lógico y gestionar la memoria en cada paso.

#### Estructuras Dinámicas Requeridas

  1. **Almacén (Matriz 2D)**: Se declara como `LoteProduccion **almacen`;. Se asigna dinámicamente usando dos niveles de `new[]` para crear las filas y luego los punteros a LoteProduccion en cada columna.
  2. Maestro de Lotes (Vectores Paralelos): Se declaran punteros para almacenar los datos de la estructura de forma paralela. Por ejemplo:
     * `LoteProduccion *maestroLotes`; (Almacena las estructuras completas).
     * `int *indicesDisponibles;` (Vector auxiliar para saber qué índice del maestro está ocupado/libre).
  3. **Pila de Inspecciones (Pila)**: Se implementa con punteros para almacenar el ID del lote y el resultado de la inspección.
     * `int *pilaIDLote;`
     * `int *pilaResultado;` (1 = Aprobado, 0 = Rechazado).
     * Se debe mantener un índice superior (top) para gestionar el LIFO.

#### Proceso de Solución Detallado

  1. **Inicialización**: Solicitar al usuario las dimensiones iniciales (F, C). Asignar memoria para la matriz 2D y para el tamaño inicial de los vectores paralelos y la Pila (ej., capacidad inicial de 100 lotes).
  2. **Gestión de Memoria y Expansión**: Implementar la lógica para duplicar el tamaño de los Vectores Paralelos cuando la capacidad se agote, utilizando `new[]` para el nuevo bloque, copiando datos y `delete[]` para el anterior.
  3. Colocación de Lote:
     * El usuario ingresa Fila y Columna.
     * Si la celda `almacen[F][C]` es `NULL`, se pide la información del nuevo lote.
     * Se utiliza una función que busca el primer índice disponible en el maestroLotes.
     * Se asigna memoria para una nueva estructura `LoteProduccion` y se copian los datos a la posición libre del maestroLotes.
     * El puntero `almacen[F][C]` se hace apuntar a esa posición del `maestroLotes`.
  4. **Control de Calidad (Push a Pila)**: El usuario ingresa un ID de lote existente. Se realiza una inspección simulada (ingresa resultado 1 o 0). Este evento se registra en la Pila (Push). Si la Pila está llena (máximo 10), se debe liberar el elemento más antiguo antes de insertar el nuevo (simulando un buffer rotatorio).
  5. **Deshacer (Pop de Pila)**: Al ejecutar el Deshacer, se realiza un Pop de la Pila. Se muestra el evento de inspección que se revierte, pero no se modifica el estado del lote (solo se deshace el registro del evento en el historial).
  6. **Terminación**: Liberar la memoria de la Matriz 2D (liberar columnas y luego filas), liberar los Vectores Paralelos, y liberar la Pila antes de finalizar.

---

### Requerimientos Funcionales y No Funcionales

#### Requisitos Funcionales

1. **Inicialización y Redimensión de la Matriz 2D**: Implementar la lógica para asignar, copiar y liberar la matriz 2D.
2. **Registro y Ubicación de Lote**: Asignar un lote (estructura) a una celda vacía de la matriz 2D y registrarlo en el Maestro de Lotes (vectores paralelos).
3. **Control de Calidad (Pila Push)**: Registrar el ID y el resultado de la inspección en la Pila. La Pila debe mantener un tamaño máximo de 10 elementos (gestión de buffer rotatorio manual).
4. **Historial (Pila Pop)**: Implementar la función Deshacer que realiza un Pop de la Pila y muestra el evento de inspección revertido.
5. **Reporte por Fila**: Dado un número de Fila, el sistema debe listar todos los lotes (ID y Nombre) que están físicamente en esa fila de la matriz 2D.
6. **Búsqueda por Componente**: Dado el nombre de un componente, el sistema debe encontrar y mostrar la ubicación (F, C) y la cantidad de ese lote.

#### Requisitos No Funcionales

1. **Asignación Manual Obligatoria**: Toda la memoria (struct, Matriz 2D, Vectores Paralelos, Pila) debe gestionarse con punteros y las funciones new / delete.
2. **Tipos Definidos por Usuario**: El almacenamiento de los datos complejos de los lotes debe utilizar la estructura `LoteProduccion`.
3. **Gestión de Memoria**: El programa debe ser capaz de liberar toda la memoria dinámicamente asignada, incluyendo los punteros dentro de las celdas de la matriz.

---

### Ejemplo de Entradas y Salidas del Problema

#### Entradas (Simulación de Interacción)

```bash
// Inicialización
Fila inicial: 3
Columna inicial: 3

// Colocar Lote 1
ID: 1001, Nombre: Resistor A, Peso: 0.01, Cantidad: 5000
Posición (F C): 0 0

// Colocar Lote 2
ID: 1002, Nombre: Capacitor B, Peso: 0.05, Cantidad: 2000
Posición (F C): 0 1

// Control de Calidad Lote 1 (Push a Pila)
ID de Lote a inspeccionar: 1001, Resultado (1=Aprob, 0=Rech): 1

// Reporte Fila 0
Fila a reportar: 0

// Deshacer (Pop de Pila)
```

### Salida Esperada del Sistema

```bash
--- AlphaTech: Control de Lotes Dinámico ---

Inicialización exitosa. Almacén 3x3 creado. Capacidad Maestro: 100.

--- Menú Principal ---
...

Opción seleccionada: 2 (Colocar Lote)
Lote 1001 (Resistor A) registrado y colocado en (0, 0).

Opción seleccionada: 2 (Colocar Lote)
Lote 1002 (Capacitor B) registrado y colocado en (0, 1).

Opción seleccionada: 3 (Control de Calidad)
Inspección Lote 1001 (Resistor A): Resultado Aprobado (1).
Evento PUSH a Pila: Lote 1001 | Resultado 1.

Opción seleccionada: 5 (Reporte por Fila)
--- Reporte de Fila 0 ---
(0, 0): ID: 1001, Nombre: Resistor A
(0, 1): ID: 1002, Nombre: Capacitor B
(0, 2): Vacío

Opción seleccionada: 4 (Deshacer)
POP de Pila: Evento Lote 1001 | Resultado Aprobado (1).
Historial de inspección revertido.

Opción seleccionada: 7 (Salir)
Liberando memoria de estructuras LoteProduccion...
Liberando memoria de Matriz 2D...
Liberando memoria de Pila y Vectores Paralelos...
¡Memoria libre! Sistema terminado.
```