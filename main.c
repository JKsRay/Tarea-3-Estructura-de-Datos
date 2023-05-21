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

void agregarTarea(Map*, TareaNodo*, Stack*);
void precedenciaTareas(Map*, Stack*);
void insertarAccionTareaPrecedente(Stack*, char*,TareaNodo* , TareaNodoPrecedente*);
void insertarAccionTareas1(Stack *, char * , TareaNodo *);
TareaNodo* verifExiste(char*, Map*);
void mostrarTareas(Map*);
void ingresarTareasHeap(Map*, List*);
void marcarVisitado(Map*, TareaNodo*);
void reiniciarBooleanos(Map*);
void marcarTareaComoHecha(Map*, Stack*);
void eliminarTarea(Map*, TareaNodo*, char*, Stack*);
void insertarAccionTareas(Stack*, char*, TareaNodo*, List*);
void clonarLista(List*, List*);
void deshacerUltimaAccion(Map*, Stack*);
void deshacerPrecedencia(Map*, TareaNodoPrecedente* , TareaNodo*);
void deshacerEliminacion(Map* , TareaNodoPila*);
void deshacerAdicion(Map *, TareaNodo*, char*);
void importarDatosTareas(Map*);
char *get_csv_field(char*, int);


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
    printf("\nELIJA UNA OPCIÓN:");
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
        agregarTarea(mapaTareas, tarea, pilaAcciones);
        break;

      case 2:
        precedenciaTareas(mapaTareas, pilaAcciones);
        break;

      case 3:
        mostrarTareas(mapaTareas);
        break;

      case 4:
        marcarTareaComoHecha(mapaTareas, pilaAcciones);
        break;

      case 5:
        deshacerUltimaAccion(mapaTareas, pilaAcciones);
        break;

      case 6:
        importarDatosTareas(mapaTareas);
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

//Al momento de agregar tarea
void insertarAccionTareas1(Stack * pilaAcciones, char * accion, TareaNodo * tarea){
  TareaNodoPila * auxPila= malloc(sizeof(TareaNodoPila));
  strcpy(auxPila->nombreAccion, accion);
  auxPila->tarea = tarea;
  stack_push(pilaAcciones, auxPila);
}

//Para agregar tareas, primero verificamos de que esta no se encuentre en el mapa anteriormente. Luego inicializamos sus variables, insertamos la accion "agregar" junto la tarea dentro de la pila de acciones e ingresamos la tarea al mapa.
void agregarTarea(Map * mapaTareas, TareaNodo *tarea, Stack* pilaAcciones){
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
  insertarAccionTareas1(pilaAcciones, "agregar", tarea);
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

//Insertamos precedencia en la pila de acciones
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

//Revisamos que cada nodo de la lista de precedente haya sido visitado.
bool todasVisitadas(List* listaPrecedentes){
  TareaNodoPrecedente* aux = firstList(listaPrecedentes);

  while(aux!= NULL){
      if(aux->visitado == false) return false;
      aux = nextList(listaPrecedentes);
    }
  return true;
}

//Busca el/los nodoPrecedentes en la lista precedentes de cada tarea del mapa que sean iguales que la raiz del monticulo, y los marca como visitado.
void marcarVisitado(Map * mapaTareas, TareaNodo* auxMaximo){
  TareaNodo * tareaAux = firstMap(mapaTareas);
  while(tareaAux != NULL){
    TareaNodoPrecedente * auxPrecedente = firstList(tareaAux->listaPrecedentes);
    while(auxPrecedente != NULL){
        if(strcmp(auxPrecedente->nombre, auxMaximo->nombre) == 0){
          auxPrecedente->visitado = true;
        }
        auxPrecedente = nextList(tareaAux->listaPrecedentes);
      }
    tareaAux = nextMap(mapaTareas);
  }
}

//Esta funcion se basa principalmente en el algoritmo de Kahn. Primero creamos un montículo de mínimos, y un auxiliar que apunta al primer elemento del mapa y lo recorremos mediante un ciclo y nextMap en busca de todas las tareas que posean precedentes, y cada una de ellas las ingresamos al montículo. Luego recorremos el montículo con un ciclo y la condicion que la raiz sea diferente de NULL (heap_top), creamos la variable auxMaximo que apunta a la raiz del montículo, y eliminamos la raiz. Ingresamos el auxMaximo dentro de la lista donde estaremos guardando las tareas de forma ordenada y llamamos la funcion marcarVisitado. Luego recorremos el mapa nuevamente y preguntamos si el primero del mapa no ha sido explorado aun, si esta condicion se cumple, hacemos otra condicion, que en caso de que todos los nodosPrecedentes de la lista de la tarea correspondiente esten marcados como visitado, o simplemente la lista esté vacia, marca esa tarea como explorada y la ingresa al monticulo. Esto lo hace hasta que ya no queden elementos en el monticulo, dejando la lista de tareas ordenadas, con todas las tareas ordenadas por prioridad y precedencia.
void ingresarTareasHeap(Map* mapaTareas, List* tareasOrdenadas){

  Heap* heapTareas = createHeap();
  TareaNodo *tareaAux = firstMap(mapaTareas);
  while(tareaAux != NULL){
    if(firstList(tareaAux->listaPrecedentes) == NULL){
      tareaAux->explorado = true;
      heap_push(heapTareas, tareaAux, tareaAux->prioridad);
    }  
    tareaAux = nextMap(mapaTareas);
  }
  
  while(heap_top(heapTareas) != NULL){
    TareaNodo* auxMaximo = heap_top(heapTareas);
    heap_pop(heapTareas);
    pushBack(tareasOrdenadas, auxMaximo);
    marcarVisitado(mapaTareas, auxMaximo);
    TareaNodo * aux = firstMap(mapaTareas);
    while(aux != NULL){
      if(aux->explorado != true){
        TareaNodoPrecedente * auxPrecedente = firstList(aux->listaPrecedentes);
      
        if(todasVisitadas(aux->listaPrecedentes) == true || auxPrecedente == NULL){
          aux->explorado = true;
          heap_push(heapTareas, aux, aux->prioridad);
        }
      }
      aux = nextMap(mapaTareas);
    }
  }
}

//Reiniciamos todos los booleanos para cuando se ingresa más de una vez a la función mostrarTareas
void reiniciarBooleanos(Map* mapaTareas){
  TareaNodo * aux = firstMap(mapaTareas);

  while(aux != NULL){
    aux->explorado = false;
    if(firstList(aux->listaPrecedentes) != NULL){
      TareaNodoPrecedente * auxPrecedente = firstList(aux->listaPrecedentes);
      while(auxPrecedente != NULL){
        auxPrecedente->visitado = false;
        auxPrecedente = nextList(aux->listaPrecedentes);
      }
    }
    aux = nextMap(mapaTareas);
  }
}

//Imprimimos las tareas según el orden establecido en la función ingresarTareasHeap
void mostrarTareas(Map* mapaTareas){
  List * tareasOrdenadas = createList();
  TareaNodo * tareaAux = firstMap(mapaTareas);
  reiniciarBooleanos(mapaTareas);

  if(tareaAux == NULL){
    printf("\nNO HAY TAREAS INGRESADAS\n");
    return;
  }
  
  //Meter al heap las tareas sin precedentes
  ingresarTareasHeap(mapaTareas, tareasOrdenadas);

  int cont = 1;
  printf("TAREAS POR HACER, ORDENADA POR PRIORIDAD Y PRECEDENCIA: \n\n");

  TareaNodo * tareaAuxLista = firstList(tareasOrdenadas);

  while(tareaAuxLista != NULL){
    if(firstList(tareaAuxLista->listaPrecedentes) == NULL){
      printf(" %d. %s (Prioridad: %d) \n", cont, tareaAuxLista->nombre, tareaAuxLista->prioridad);
    }
    else{
      printf(" %d. %s (Prioridad: %d) - Precedente(s): ", cont, tareaAuxLista->nombre, tareaAuxLista->prioridad);
      TareaNodoPrecedente * auxPrecedente = firstList(tareaAuxLista->listaPrecedentes);
      while(auxPrecedente != NULL){
        printf("%s ", auxPrecedente->nombre);
        auxPrecedente = nextList(tareaAuxLista->listaPrecedentes);
      }
      
      printf("\n");
    }                       

    tareaAuxLista = nextList(tareasOrdenadas);
    cont++;
  }

}

//Creamos una lista temporal en donde guardaremos las tareas que tenian de precedencia la tarea que eliminaremos. Luego, borramos la tarea correspondiente del mapa. Buscamos aquellas tareas en el mapa que tenían como precedente la tarea que eliminamos, la agregamos a la listaTemporal y la eliminamos de su listaPrecedentes. La lista temporal nos servirá al momento de deshacer esta opción. Finalmente, guardamos la acción en la pila de acciones.
void eliminarTarea(Map* mapaTareas, TareaNodo* tarea, char *nombreTarea, Stack * pilaAcciones) {
  List* listaTemporal = createList();
  eraseMap(mapaTareas, tarea->nombre);
  
  TareaNodo *tareaAux = firstMap(mapaTareas);
  while (tareaAux != NULL) {
    TareaNodoPrecedente *auxPrecedente = firstList(tareaAux->listaPrecedentes);
    while (auxPrecedente != NULL) {
      if (strcmp(auxPrecedente->nombre, nombreTarea) == 0) {
          pushBack(listaTemporal, tareaAux);
          popCurrent(tareaAux->listaPrecedentes);   
      }
      auxPrecedente = nextList(tareaAux->listaPrecedentes);
    }
    tareaAux = nextMap(mapaTareas);
  }
  insertarAccionTareas(pilaAcciones,"eliminar", tarea, listaTemporal);
  printf("\nTAREA ELIMINADA CON ÉXITO\n");
}

//Verificamos si la tarea a eliminar existe, si existe comprobamos si tiene precedentes por hacer. En el caso de que aún tenga precedentes imprimimos un texto de advertencia y en el caso de que el usuario esté de acuerdo lo eliminamos. En caso contrario, return. Si no tiene precedentes simplemente eliminamos la tarea.
void marcarTareaComoHecha(Map * mapaTareas, Stack* pilaAcciones){
  char nombreTareaAEliminar[21];
  printf("\nINGRESE LA TAREA QUE DESEA ELIMINAR: ");
  scanf("%20[^\n]s", nombreTareaAEliminar);
  getchar();
  
  TareaNodo *tareaAEliminar = verifExiste(nombreTareaAEliminar, mapaTareas);
  if(tareaAEliminar == NULL) return;
  
  if (firstList(tareaAEliminar->listaPrecedentes) != NULL) {
    printf("\n¿ESTÁS SEGURX QUE DESEA ELIMINAR ESTA TAREA?");
    printf("\n1: SI\n2: NO\n");

    int opcion;
    char entrada[11];
    while (1) {
      scanf("%10s", entrada);
      opcion = atoi(entrada);

      if (opcion == 1) {
        eliminarTarea(mapaTareas, tareaAEliminar, nombreTareaAEliminar, pilaAcciones);
        break;
      } else if (opcion == 2) {
        return;
      } else {
        printf("\nSELECCIONE UNA OPCIÓN VÁLIDA\n\n");
      }
    }
  } else {
  // Caso en donde no tiene precedentes
  eliminarTarea(mapaTareas, tareaAEliminar, nombreTareaAEliminar, pilaAcciones);
  }
}

//Al momento de eliminar tarea
void insertarAccionTareas(Stack * pilaAcciones, char * accion, TareaNodo * tarea, List* listaTemporal){
  TareaNodoPila * auxPila= malloc(sizeof(TareaNodoPila));
  strcpy(auxPila->nombreAccion, accion);
  auxPila->tarea = tarea;
  auxPila->listaTemporal = createList();
  clonarLista(auxPila->listaTemporal, listaTemporal);
  stack_push(pilaAcciones, auxPila);
}

void clonarLista(List* listaPila, List* listaTemporal){
  TareaNodo* auxTemporal = firstList(listaTemporal);
  while(auxTemporal != NULL){
    pushBack(listaPila, auxTemporal);
    auxTemporal = nextList(listaTemporal);
  }
}

//Cuando queremos eliminar una precedencia ya hecha lo que hacemos es acceder al primer elemento de la lista de precedentes de la ultima tarea a la que se le asignó una precedencia. Luego recorremos la lista hasta encontrar la ultima precedencia agregada y la eliminamos de la lista de precedencia.
void deshacerPrecedencia(Map * mapaTareas, TareaNodoPrecedente* tareaPrecedente, TareaNodo* tarea){
  TareaNodoPrecedente * aux = firstList(tarea->listaPrecedentes);

  while(aux != NULL){
    if(strcmp(aux->nombre, tareaPrecedente->nombre) == 0){
      popCurrent(tarea->listaPrecedentes);
    }
    aux = nextList(tarea->listaPrecedentes);
  }
}

//En el caso de que queramos volver a agregar una tarea ya eliminada, lo que hacemos es crear un nodoPrecedente e ingresar la tarea a las listas de precedentes correspondientes. Finalmente, volvemos a agregar la tarea al mapa.
void deshacerEliminacion(Map * mapaTareas, TareaNodoPila* nodoPila){
  TareaNodo* aux = firstMap(mapaTareas);
  
  TareaNodoPrecedente * tareaPilaAuxPrecedente = (TareaNodoPrecedente*)   malloc(sizeof(TareaNodoPrecedente));
  strcpy(tareaPilaAuxPrecedente->nombre,nodoPila->tarea->nombre);
  tareaPilaAuxPrecedente->prioridad = nodoPila->tarea->prioridad;
  tareaPilaAuxPrecedente->visitado = false;
  
  while(aux!=NULL){
      TareaNodoPrecedente* auxPrecedente = firstList(nodoPila->listaTemporal);
      while(auxPrecedente!= NULL){
        if(strcmp(aux->nombre, auxPrecedente->nombre)==0){
          pushBack(aux->listaPrecedentes, tareaPilaAuxPrecedente);
        }
        auxPrecedente = nextList(nodoPila->listaTemporal);
      }
      aux = nextMap(mapaTareas);
    }
  insertMap(mapaTareas, nodoPila->tarea->nombre, nodoPila->tarea);
}

//En el caso de que debamos deshacer la adición de una tarea, debemos eliminarla. Por lo tanto, hacemos lo mismo que en eliminarTarea con la diferencia en que no guardamos en la pila de acciones.
void deshacerAdicion(Map* mapaTareas, TareaNodo * tarea, char * nombreTarea){
  List* listaTemporal = createList();
  eraseMap(mapaTareas, tarea->nombre);
  
  TareaNodo *tareaAux = firstMap(mapaTareas);
  while (tareaAux != NULL) {
    TareaNodoPrecedente *auxPrecedente = firstList(tareaAux->listaPrecedentes);
    while (auxPrecedente != NULL) {
      if (strcmp(auxPrecedente->nombre, nombreTarea) == 0) {
          pushBack(listaTemporal, tareaAux);
          popCurrent(tareaAux->listaPrecedentes);   
      }
      auxPrecedente = nextList(tareaAux->listaPrecedentes);
    }
    tareaAux = nextMap(mapaTareas);
  } 
}

//Verificamos si existe alguna accion a deshacer. Luego en los if vamos comparando el tipo de acción que se realizó y deshacemos según lo que corresponda. Finalmente, eliminamos la ultima accion ingresada de la pila.
void deshacerUltimaAccion(Map* mapaTareas, Stack* pila){
  if (stack_top(pila) == NULL) {
      printf("No hay acciones para deshacer.\n");
      return;
  }
  
  TareaNodoPila* ultimaAccion = stack_top(pila);
  
  if (strcmp(ultimaAccion->nombreAccion, "agregar") == 0) {
    deshacerAdicion(mapaTareas, ultimaAccion->tarea, ultimaAccion->tarea->nombre);
    
  } else if (strcmp(ultimaAccion->nombreAccion, "establecer precedencia") == 0) {
    deshacerPrecedencia(mapaTareas, ultimaAccion->tareaPrecedente, ultimaAccion->tarea);
     
  } else if (strcmp(ultimaAccion->nombreAccion, "eliminar") == 0) {
    deshacerEliminacion(mapaTareas, ultimaAccion);
     
  } else {
      printf("Acción desconocida.\n");
      return;
  }

  stack_pop(pila);
  free(ultimaAccion);
  printf("\nACCIÓN DESHECHA CON ÉXITO\n");
}

char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }


    return NULL;
}


/*La función se encarga de leer y almacenar datos de tareas desde un archivo csv. Se ingresa el nombre del archivo a importar. El archivo lo abrimos en modo de lectura y se verifica si se pudo abrir correctamente.Se lee la primera línea del archivo y se descarta, ya que son los títulos. Se lee las demás líneas del archivo y se procesa. Para cada línea, se crea una estructura TareaNodo y se asigna memoria.
Se extraen el nombre,la prioridad y si es que tiene, precedentes.
El nombre y la prioridad se asignan a la tarea actual.
Si es que hay precedentes, se buscan en el mapa y se crean TareaNodoPrecedente con los datos que correspondan.
Luego, se agregan las tareas precedentes a la lista de precedentes de la tarea actual.
La tarea actual se inserta en el mapa de tareas utilizando su nombre como clave.
Una vez que se procesan todas las líneas, se cierra el archivo.
Se muestra un mensaje indicando que los datos se cargaron correctamente.*/
void importarDatosTareas(Map*mapaTareas) {
  char nombreArchivo[101];
  printf("\nINGRESE NOMBRE DEL ARCHIVO A IMPORTAR LOS JUGADORES:\n");
  scanf("%100[^\n]s", nombreArchivo);
  
  FILE* archivoTareas = fopen(nombreArchivo, "r");
  if (archivoTareas == NULL) {
      printf("Error al abrir el archivo\n");
      return;
  }

  char linea[1024];
  int i;
  char delimit[]=" \t\r\n\v\f";
  fgets (linea, 1023, archivoTareas);
  
  while (fgets (linea, 1023, archivoTareas) != NULL) { 
    TareaNodo * tarea = (TareaNodo*) malloc(sizeof(TareaNodo));
    tarea->explorado = false;
    tarea->listaPrecedentes = createList();
    char *precedentes;
    
      for(i=0;i<3;i++){

        char *aux = get_csv_field(linea, i); 
        if(i == 0){
          strcpy(tarea->nombre, aux);
        }

        if(i == 1){
          int prioridad = atoi(aux);
          tarea->prioridad = prioridad;
        }
        
        if(i == 2){
          if(strlen(aux) > 1){
            
            precedentes = strtok(aux,delimit);
           
            while(precedentes != NULL){
              TareaNodoPrecedente * tareaPrecedente = (TareaNodoPrecedente *) malloc(sizeof(TareaNodoPrecedente));

              TareaNodo* aux2= searchMap(mapaTareas, precedentes);
              strcpy(tareaPrecedente->nombre, aux2->nombre);
              tareaPrecedente->prioridad= aux2->prioridad;
              tareaPrecedente->visitado = false;

              if (tareaPrecedente != NULL) pushBack(tarea->listaPrecedentes, tareaPrecedente);
              precedentes = strtok(NULL,delimit);

            }
          }  
        }
      }
    insertMap(mapaTareas, tarea->nombre, tarea);
  }
  fclose(archivoTareas);
  printf("\nDATOS CARGADOS CON ÉXITO\n");
}
