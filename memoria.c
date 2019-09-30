/*
Nombre: Administración de memoria contigua
Autor: Vázquez Sánchez Ilse Abril
Fecha de realización: 11 de Mayo de 2018	*/ 

#include <stdlib.h>
#include <stdio.h>

struct bloq_memoria
{
	int inicio_bloque;
	int tamanio_bloque;
	int bloque_libre;
	int proc_asignado;
	struct bloq_memoria* siguiente;
	struct bloq_memoria* anterior;
};

struct hueco_memoria
{
	int inicio_hueco;
	int tamanio_hueco;
	struct hueco_memoria *siguiente;
};

struct elemento_cola
{
	int id_proc;
	int tamanio_proc;
	struct elemento_cola* siguiente;
	struct elemento_cola* anterior;
};

typedef struct bloq_memoria bloque;
typedef struct hueco_memoria hueco;
typedef struct elemento_cola proceso;

bloque *primer_bloque, *ultimo_bloque, *primer_bloque_usuario;
hueco *inicio_lista_huecos = NULL;
hueco *fin_lista_huecos = NULL;
proceso *inicio = NULL;
proceso *fin = NULL;

// Funciones corresponientes a la cola de procesos en espera
// Esta función verifica si la cola tiene o no elementos
int noHayProcesos(){
	if(inicio == NULL){
		return 1;
	}else{
		return 0;
	}
}

// Esta función se encarga de contar el número de procesos en cola
// Nos servirá para determinar el momento en el que se realizará compactación
int cuentaProcesos(){
	proceso *temp = inicio;
	int num_proc = 0;
	if(noHayProcesos()){
		// Si está vacía la cola, devuelve el valor 0
		return 0;
	}else{
		// Si no está vacía, comienza a contar los elementos que hay en cola
		do{
			num_proc += 1;
			temp = temp->siguiente;
		}while(temp!=NULL);
	// Devuelve el número de elementos en cola
	return num_proc;
	} 
}

// Esta función se encarga de agregar a los procesos que entrarán a la cola al final de ésta
void agregarProcesoCola(int id, int tamanio){
	proceso *nuevo;
	nuevo = (proceso *) malloc(sizeof(proceso));
	if (nuevo!= NULL){
		nuevo->id_proc = id;
		nuevo->tamanio_proc = tamanio;
		// El siguiente es nulo porque nuestro elemento nuevo va a ser el último de la cola
		nuevo->siguiente = NULL;

		// Agregamos el elemento nuevo a la cola
		// Verificamos si la cola no está vacía
		if(noHayProcesos()){
			nuevo->anterior = NULL;
			inicio = nuevo;
			fin = nuevo;
		}else{
			// El que era el último elemento apunta ahora al nuevo en su campo siguiente
			fin->siguiente = nuevo;
			// El nuevo elemento apunta al que era el último anteriormente en su campo anterior
			nuevo->anterior = fin;
			// El nuevo elemento es ahora el último de la cola
			fin = nuevo;
		}
	}
}

// Esta función elimina elementos de la cola cuando ya han sido asignados a algún área en la memoria
// Recibe como parámetro el id del proceso a eliminar
void eliminarProcesoCola(int valor){
	proceso *temp = inicio;
	if(inicio == fin){
		// Vamos a eliminar al único elemento de la cola
		inicio = NULL;
		fin = NULL;
		free(temp);
	}else{
		do{
			if(temp->id_proc == valor){
				if(temp == inicio){
					// Se va a eliminar el inicio de la cola
					inicio = temp->siguiente;
					inicio->anterior = NULL;
					free(temp);
				}else if(temp == fin){
					// Vamos a eliminar el fin de la cola
					fin = temp->anterior;
					fin->siguiente = NULL;
					free(temp);
				}else{
					// Vamos a eliminar un elemento de en medio de la cola
					// Enlazamos los elementos anterior y siguiente del proceso que vamos a eliminar
					(temp->siguiente)->anterior = temp->anterior;
					(temp->anterior)->siguiente = temp->siguiente;
					free(temp);
				}
			}else{
				// Se continua buscando el id del proceso a eliminar
				temp = temp->siguiente;
			}
			// Terminamos la búsqueda	
		}while(temp!=NULL);
	}
}

// Esta función se encarga de buscar qué proceso cabe en el espacio de memoria libre
// Recibe el tamaño de espacio que será asignado
proceso* buscarProcesoCola(int valor){
	proceso *temp = inicio;
	do{
		if(temp->tamanio_proc <= valor){
			return temp;
		}else{
			temp = temp->siguiente;
		}
	}while(temp!=NULL);
	// Si el valor es nulo, entonces no se encontró ningún elemento que quepa en la memoria
	return NULL;
}

// Esta función imprime la situación de la cola
// Muestra el id del proceso y el tamaño del proceso
void imprimirCola(){
	if(noHayProcesos()){
		printf("Cola vacia\n");
	}else{
		proceso *temp = inicio;
		printf("\nId\tTamaño\n");
		do{
			printf("%d\t%d\n", temp->id_proc, temp->tamanio_proc);
			temp = temp->siguiente;
		}while(temp!=NULL);
	}
}

// Funciones corresponientes a la asignación de memoria
// Esta función nos permite saber si hay huecos disponibles
int noHayHuecos(){
	if(inicio_lista_huecos == NULL){
		return 1;
	}else{
		return 0;
	}
}

// Esta función sirve para contar el número de huecos disponibles
int cuentaHuecos(){
	hueco* temp = inicio_lista_huecos;
	int numHuecos=0;
	if(noHayHuecos()){
		return 0;
	}else{
		do{
			numHuecos += 1;
			temp = temp->siguiente;
		}while(temp!= NULL);
	}
	return numHuecos;
}

// Esta función es utilizada para borrar la lista de huecos cuando se realiza compactación
void limpiaListaHuecos(){
	hueco *actual = inicio_lista_huecos;
	hueco *siguiente = actual->siguiente;
	do{
		siguiente = actual->siguiente;
		free(actual);
		actual = siguiente;
	}while(actual!=NULL);
	inicio_lista_huecos = NULL;
	fin_lista_huecos = NULL;
}

// Esta función imprime los inicios de los huecos de la memoria y su tamaño
void imprimirListaHuecos(){
	if(noHayHuecos()){
		printf("No hay huecos disponibles\n");
	}else{
		hueco* temp = inicio_lista_huecos;
		printf("\nInicio\tTamaño\n");
		do{
			printf("%d\t%d\n", temp->inicio_hueco, temp->tamanio_hueco);
			temp = temp->siguiente;
		}while(temp!= NULL);
	}
}

// Esta función agrega un hueco nuevo a la lista. Ordena a los huecos por tamaño de mayor a menor para facilitar la asignación por la estrategia del peor ajuste
// Recibe la dirección de inicio del hueco y el tamaño del hueco
void agregaHueco(int inicio_hueco, int tamanio_hueco){
	hueco *nuevo = (hueco *) malloc(sizeof(hueco));
	if(nuevo!=NULL){
		// Se le da al hueco nuevo el tamaño y dirección de memoria recibidos
		nuevo->inicio_hueco=inicio_hueco;
		nuevo->tamanio_hueco=tamanio_hueco;
		if(noHayHuecos()){
			// La lista de huecos está vacía, tanto el fin de la lista como el inicio apuntarán al nodo nuevo
			inicio_lista_huecos = nuevo;
			fin_lista_huecos = nuevo;
		}else{
			// Si hay huecos en la lista, el hueco nuevo se agrega en el lugar corresponiente según su tamaño
			hueco *temp = inicio_lista_huecos;
			if((tamanio_hueco) > (temp->tamanio_hueco)){
				// El hueco nuevo es mayor al primer elemento de la lista, entonces lo hacemos el primer elemento de la lista
				nuevo->siguiente = temp;
				inicio_lista_huecos = nuevo;
			}else{
				hueco *tempSig;
				while(temp!=fin_lista_huecos){
					// Comenzamos a buscar aquel hueco que tenga un tamaño menor al hueco nuevo
					tempSig = temp->siguiente;
					if(nuevo->tamanio_hueco > tempSig->tamanio_hueco){
						// Cuando se encuentre un hueco con tamaño menor, el hueco nuevo se agrega antes de él
						nuevo->siguiente = tempSig;
						temp->siguiente = nuevo;
						return;
					}
					// Se avanza al siguiente elemento si el hueco tempSig no es de menor tamaño
					temp = temp->siguiente;
				}
				// Si no se encuentra un hueco con tamaño menor, se agrega al final de la lista, por ello el fin de la lista apunta el hueco nuevo
				temp->siguiente = nuevo;
				nuevo->siguiente = NULL;
				fin_lista_huecos = nuevo;
			}
		}
	}	
	return;
}

// Esta función se manda llamar al inicio del programa para dar a la memoria los valores iniciales necesarios
void inicializaMemoria(){
	bloque *SO, *hueco_inicial;
	SO = (bloque *) malloc(sizeof(bloque));
	hueco_inicial = (bloque *) malloc(sizeof(bloque));
	if((SO != NULL) && (hueco_inicial != NULL)){
		// Inicializamos el bloque de memoria corresponiente al sistema operativo
		SO->inicio_bloque = 0;
		SO->tamanio_bloque = 1500;
		SO->bloque_libre = 1;
		SO->proc_asignado = 1;

		// Inicializamos el bloque de memoria correspondiente al hueco inicial
		hueco_inicial->inicio_bloque = 1500;
		hueco_inicial->tamanio_bloque = 3000;
		hueco_inicial->bloque_libre = 1;
		hueco_inicial->proc_asignado = 0;
		// El apuntador al inicio de la memoria apunta al SO y el apuntador al último bloque de memoria apunta al hueco inicial
		// Su anterior es NULL porque el SO es el primer bloque
		SO->anterior = NULL;
		SO->siguiente = hueco_inicial;

		// El apuntador siguiente de hueco apunta a NULL porque es el único bloque hasta el momento
		hueco_inicial->siguiente = NULL;
		hueco_inicial->anterior = SO;

		primer_bloque = SO;
		ultimo_bloque = hueco_inicial;
		primer_bloque_usuario = hueco_inicial;

		// Inicializamos la lista de huecos con el hueco inicial marcado anteriormente
		hueco *inicio_lista = (hueco *) malloc(sizeof(hueco));
		if(inicio_lista != NULL){
			inicio_lista->inicio_hueco = 1500;
			inicio_lista->tamanio_hueco = 3000;
			inicio_lista->siguiente = NULL;
			inicio_lista_huecos = inicio_lista;
			fin_lista_huecos = inicio_lista;
		}
	}
}

// Esta función es utilizada simplemente para imprimir la situación de la memoria desde el bloque inicial corresponiente al sistema operativo hasta el último bloque
// Nos muestra la dirección de inicio, el tamaño del bloque, si está ocupado y el proceso que lo ocupa
void imprimirMemoria(){
	bloque *temp = primer_bloque;
	printf("Inicio\tTamaño\tOcupado\tProceso\n");
	printf("---------------------------------\n");
	do{
		printf("%d\t%d\t%d\t%d\n", temp->inicio_bloque, temp->tamanio_bloque, temp->bloque_libre, temp->proc_asignado);
		temp = temp->siguiente;
	}while(temp!=NULL);
}

// Funciones para realizar operaciones sobre la memoria y sobre los huecos
void asignarEnMemoria(int id, int tamanio, hueco *hueco_asignado){
	int espacio_libre;
	bloque *temp = primer_bloque_usuario;
	do{
		if(hueco_asignado->inicio_hueco == temp->inicio_bloque){
			//Indicamos que el bloque de memoria está ya asignado al proceso
			temp->bloque_libre = 1;
			temp->proc_asignado = id;
			// Calculamos si el proceso asignado dejó espacio libre
			espacio_libre = (hueco_asignado->tamanio_hueco)-(tamanio);
			temp->tamanio_bloque = tamanio;

			if(espacio_libre > 0){
				// Creamos un hueco nuevo
				bloque *nuevo = (bloque *) malloc(sizeof(bloque));
				if (nuevo!= NULL){
					nuevo->inicio_bloque = (temp->inicio_bloque)+(temp->tamanio_bloque);
					nuevo->tamanio_bloque = espacio_libre;
					nuevo->bloque_libre = 0;
					nuevo->proc_asignado = 0;
					agregaHueco(nuevo->inicio_bloque, nuevo->tamanio_bloque);
					nuevo->siguiente = temp->siguiente;
					nuevo->anterior = temp;
					temp->siguiente = nuevo;
					if(temp == ultimo_bloque){
						// Si el bloque en donde se asignó el proceso fue el último de la memoria, se cambia este apuntador al hueco nuevo
						ultimo_bloque = nuevo;
					}
					return;
				}
			}
		}
		temp = temp->siguiente;
	}while(temp!=NULL);
}

void asignacionHueco(int id, int tamanio){
	hueco *hueco_actual = inicio_lista_huecos;
	if(hueco_actual!= NULL){
		// Hay huecos disponibles
		if(tamanio <= hueco_actual->tamanio_hueco){
			// Si el proceso cabe en el primer hueco, se lo asignamos
			asignarEnMemoria(id, tamanio, hueco_actual);
			// Eliminamos el primer hueco
			inicio_lista_huecos = hueco_actual->siguiente;
			free(hueco_actual);
			return;
		}
	}
	// El proceso no puede ser asignado a ningún hueco porque no hay huecos disponibles o no cabe en ninguno
	agregarProcesoCola(id, tamanio);
	printf("\nNo cabe el proceso con id: %d\n", id);
	printf("\nSituacion actual de la memoria: \n");
	imprimirMemoria();
	printf("\nSituacion actual de la cola de procesos en espera\n");
	imprimirCola();
}

void atiendeCola(){
	hueco *temp = inicio_lista_huecos;
	proceso *proc;
	while(noHayProcesos() == 0){
		// Busca si hay algún proceso que quepa en el hueco
		proc = buscarProcesoCola(temp->tamanio_hueco);
		if(proc!=NULL){
			asignacionHueco(proc->id_proc, proc->tamanio_proc);
			eliminarProcesoCola(proc->id_proc);
		}else{
			// Ningún proceso de la cola cabe en el hueco
			return;
		}
		temp = temp->siguiente;
	}
}

void liberaMemoria(int id){
	bloque *temp = primer_bloque_usuario;
	while(temp!=NULL){
		if(temp->proc_asignado == id){
			// Se encontró el proceso que terminó
			// Marcamos el bloque como libre y sin proceso asignado
			temp->bloque_libre = 0;
			temp->proc_asignado = 0;
			agregaHueco(temp->inicio_bloque, temp->tamanio_bloque);
			break;
		}
		temp = temp->siguiente;
	}
	atiendeCola();
}

void compactaMemoria(){
	int tamanio = 0;
	int inicio;
	if(noHayHuecos() == 0){
		printf("\nSe realizara compactacion\n");
		bloque *temp = primer_bloque_usuario;
		bloque *borrar;
		while(temp!=ultimo_bloque){
			if(temp->bloque_libre == 1){
				// Si no está disponible el bloque, el inicio del siguiente depende del tamaño del actual y de su dirección de inicio
				(temp->siguiente)->inicio_bloque = (temp->tamanio_bloque) + (temp->inicio_bloque);
				temp = temp->siguiente;
			}else{
				// Si está disponible, se asigna su dirección al siguiente
				tamanio += temp->tamanio_bloque;
				(temp->siguiente)->inicio_bloque = temp->inicio_bloque;
				(temp->siguiente)->anterior = temp->anterior;
				(temp->anterior)->siguiente = temp->siguiente;
				// Vamos a eliminar el bloque
				borrar = temp;
				// Vamos al siguiente
				temp = temp->siguiente;
				free(borrar);
			}
		}
		ultimo_bloque->tamanio_bloque += tamanio;
		limpiaListaHuecos();
		agregaHueco(ultimo_bloque->inicio_bloque, ultimo_bloque->tamanio_bloque);
		atiendeCola();
		printf("\nSituacion actual de la memoria:\n");
		imprimirMemoria();
		printf("\nSituacion actual de la cola de procesos en espera\n");
		imprimirCola();
	}else{
		printf("\nNo hay huecos disponibles, no se puede realizar compactacion\n");
	}
}

// Función main del programa. Recibe por línea de comandos el nombre del arcivo que contiene la lista de procesos con sus respectivos tamaños
int main(int argc, char *argv[]){
	char linea[100];
	int id, tamanio;
	FILE *archivo;
	inicializaMemoria();
	if(argc == 2){
		printf("%s\n", argv[1]);
		archivo = fopen(argv[1], "r");
		if(archivo!=NULL){
			while(fgets(linea, 100, archivo)!=NULL){
				sscanf(linea,"%d %d", &id, &tamanio);
				if(tamanio!= 0){
					asignacionHueco(id, tamanio);
					if(cuentaProcesos()>=5){
						compactaMemoria();
					}
				}else{
					liberaMemoria(id);
					printf("\nEl proceso con id %d termino\n", id);
					printf("Lista de huecos disponibles:\n");
					imprimirListaHuecos();
				}
			}
			printf("\nSituacion final de la memoria:\n");
			imprimirMemoria();
			printf("\nSituacion final de la cola de procesos en espera:\n");
			imprimirCola();
		}
	}fclose(archivo);
}