#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h> 
#include "heap.h"
#include "stack.h"
#include "Map.h"

typedef struct{
    char nombre[21];
    bool explorado;
    int prioridad;
    List * listaPrecedentes;
}TareaNodo;

typedef struct{
    char nombre[21];
    bool visitado;
    int prioridad;
}TareaNodoPrecedente;

typedef struct{
    char nombreAccion[51];
    TareaNodo * tarea;
    TareaNodoPrecedente * tareaPrecedente;
    List* listaTemporal;
}TareaNodoPila;

void agregarTarea(Map*, TareaNodo*);
void precedenciaTareas(Map*, Stack*);
void insertarAccionTareaPrecedente(Stack*, char*,TareaNodo* , TareaNodoPrecedente*);
TareaNodo* verifExiste(char, Map );


int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}
int lower_than_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2) < 0) return 1;
    return 0;
}
int is_equal_int(void * key1, void * key2) {
    if(*(int*)key1 == *(int*)key2) return 1;
    return 0;
}
int lower_than_int(void * key1, void * key2) {
    if(*(int*)key1 < *(int*)key2) return 1;
    return 0;
}

int main() {
  char opcion[11];
  bool ejecucion = true;

  TareaNodo *tarea;
  Map * mapaTareas = createMap(is_equal_string);
  Stack*pilaAcciones = stack_create();
  
  while (ejecucion) {
    printf("\n1.-AGREGAR TAREA\n");
    printf("2.-ESTABLECER PRECEDENCIA ENTRE TAREAS\n");
    printf("3.-MOSTRAR TAREAS POR HACER\n");
    printf("4.-MARCAR TAREA COMO COMPLETADA\n");
    printf("5.-DESHACER ULTIMA ACCION\n");
    printf("6.-CARGAR DATOS DE TAREAS DE UN ARCHIVO\n");

    printf("PRESIONE 0 PARA SALIR\n\n");

    scanf("%10s[^\n]", opcion);
    getchar();

    //Se comprueba que la opcion ingresada sea un número
    if (isdigit(opcion[0])) {
      int opcionNumero = atoi(opcion);

      switch (opcionNumero) {
      case 1:
        tarea = (TareaNodo*) malloc(sizeof(TareaNodo));
        agregarTarea(mapaTareas, tarea);
        break;

      case 2:
        precedenciaTareas(mapaTareas, pilaAcciones);
        break;

      case 3:
        break;

      case 4:
        break;

      case 5:
        break;

      case 6:
        break;
        
      case 0:
        printf("QUE TENGA BUEN DÍA, ADIÓS\n");
        ejecucion = false;
        break;

      default:
        printf("\nSELECCIONE UNA OPCIÓN VÁLIDA\n\n");
        break;
      }
    } else {
      if (isalpha(opcion[0]))
        printf("\nSELECCIONE UNA OPCIÓN VÁLIDA\n\n");
    }
  }
    return 0;
}

void agregarTarea(Map * mapaTareas, TareaNodo *tarea){
  int prioridadAux = -1;
  char prioridadAuxString[21];
  printf("INGRESE NOMBRE DE LA TAREA A AGREGAR: ");
  scanf("%21[^\n]s", tarea->nombre);
  getchar();

  TareaNodo* tareaBuscada = searchMap(mapaTareas, tarea->nombre);
  if(tareaBuscada != NULL) {
    printf("\nLa tarea %s ya existe\n\n", tarea->nombre);
    return;
  }

  printf("\nINGRESE LA PRIORIDAD DE LA TAREA: ");

  do {
    scanf("%20[^\n]s", prioridadAuxString);
    getchar();
    if(isdigit(prioridadAuxString[0])) {
      prioridadAux = atoi(prioridadAuxString);
    }

    if (prioridadAux <= 0 || isalpha(prioridadAuxString[0])) {
      printf("\nINGRESE UNA PRIORIDAD VÁLIDA \n");
    } 

  } while (isalpha(prioridadAuxString[0]) || prioridadAux <=0);
  
  tarea->listaPrecedentes = createList();
  tarea->prioridad = prioridadAux;
  tarea->explorado = false;
  insertMap(mapaTareas, tarea->nombre, tarea);

  printf("\nTAREA AGREGADA CON ÉXITO\n");
}

TareaNodo* verifExiste(char *nombreTarea, Map *mapaTareas){
  TareaNodo * tareaAux = searchMap(mapaTareas, nombreTarea);
  if(tareaAux == NULL){
    printf("\nESA TAREA NO EXISTE\n");
    return NULL;
  }
  return tareaAux;
}

//Insertamos precedencia
void insertarAccionTareaPrecedente(Stack*pilaAcciones, char* accion, TareaNodo * nombreTarea, TareaNodoPrecedente * precedente) {
  TareaNodoPila * auxPila= malloc(sizeof(TareaNodoPila));

  strcpy(auxPila->nombreAccion, accion);
  auxPila->tarea = nombreTarea;
  auxPila->tareaPrecedente = precedente;
  stack_push(pilaAcciones, auxPila);
}

//Le pedimos al usuario primero la tarea que se realizara antes(por ej, Tarea A), y luego la tarea con precedente(por ej, Tarea B), y verificamos que estas 2 existan en el mapa. Luego creamos un nodoPrecedente, el cual guardara todos los datos en la tarea B e insertamos ese nodo a la lista de precedentes de la tarea A. Guardamos tambien la accion realizada en la pila de acciones.
void precedenciaTareas(Map* mapaTareas, Stack* pilaAcciones){
  char nombreTarea1[21], nombreTarea2[21];
  printf("\nINGRESE LA TAREA QUE SE DEBE REALIZAR ANTES: ");
  scanf("%20[^\n]s", nombreTarea1);
  getchar();
  
  TareaNodo *tareaAux1 = verifExiste(nombreTarea1, mapaTareas);
  if(tareaAux1 == NULL) return;
  
  printf("\nINGRESE LA TAREA CON PRECEDENTE: ");
   scanf("%20[^\n]s", nombreTarea2);
  getchar();
  
  TareaNodo *tareaAux2 = verifExiste(nombreTarea2, mapaTareas);
  if(tareaAux2 == NULL) return;

  if(strcmp(nombreTarea1, nombreTarea2) == 0){
    printf("\nNO PUEDES ASIGNARLE %s A SI MISMA\n", nombreTarea1);
    return;
  }

  TareaNodoPrecedente * tareaPrecedente = (TareaNodoPrecedente*) malloc(sizeof(TareaNodoPrecedente));
  strcpy(tareaPrecedente->nombre,nombreTarea1);
  tareaPrecedente->prioridad = tareaAux1->prioridad;
  tareaPrecedente->visitado = false;

  insertarAccionTareaPrecedente(pilaAcciones, "establecer precedencia", tareaAux2, tareaPrecedente);
  pushBack(tareaAux2->listaPrecedentes, tareaPrecedente);

  printf("\nPRECEDENCIA REGISTRADA CON ÉXITO\n");
}

