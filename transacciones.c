/* 
Nombre: Exclusión mutua con el algoritmo de Peterson
Elaboró: Vázquez Sánchez Ilse Abril
Fecha de entrega y realización: Martes 3 de Abril de 2018	*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int proc_fav,
proc1wantIn,
proc2wantIn;
float saldo = 1000.00; // Se da un saldo inicial

/* Esta función reliza las actividades necesarias para realizar
retiros en la cuenta. Regresa la cantidad de dinero a depositar
para que en la función proc_deposito se pueda saber si ya se debe
terminar el hilo de retiros o no */
float actividades_de_retiro(){
	float deposito;
	printf("Cantidad a depositar:\t");
	scanf("%f", &deposito);
	saldo += deposito;
	printf("Saldo actualizado: %.2f\n", saldo);
	fflush(stdout);
	return deposito;
}

/* Esta función reliza las actividades necesarias para realizar
depósitos en la cuenta. Regresa la cantidad de dinero que se
desea retirar para que en la función proc_deposito se pueda 
saber si ya se debe terminar el hilo de depositos o no */
float actividades_de_deposito(){
	float retiro;
	printf("Cantidad a retirar:\t");
	scanf("%f", &retiro);
	if(saldo>=retiro){
		saldo -= retiro;
		printf("Saldo actualizado: %.2f\n", saldo);
		fflush(stdout);
	} else{
		printf("Saldo no suficiente para realizar retiro\n");
		fflush(stdout);
	}
	return retiro;
}

/* Esta función es realizada por el hilo de depósito y, en ella,
solicita entrar a la región crítica para realizar las tareas
debidas. En esta función también se encuentran las primitivas
de entrada y salida */
void *proc_deposito(void *nada){
	float deposito;
	srand(time(NULL));
	do{
		proc1wantIn = 1;
		proc_fav = 2;
		while ((proc2wantIn == 1) && (proc_fav == 2));
		deposito = actividades_de_deposito();
		proc1wantIn = 0;
		/* La linea siguiente corresponde al bloque de otras
		tareas para el hilo de depositos */
		sleep(rand() % 3 + 1); // El tiempo de sleep es random entre 1 y 3
	} while (deposito!=0.0);
 	pthread_exit(NULL);
}

void *proc_retiro(void *nada){
	float retiro;
	srand(time(NULL));
	do{
		proc2wantIn = 1;
		proc_fav = 1;
		while ((proc1wantIn == 1) && (proc_fav == 1));
		retiro = actividades_de_retiro();
		proc2wantIn  = 0;
		/* La linea siguiente corresponde al bloque de otras
		tareas para el hilo de retiros */
		sleep(rand() % 3 + 1); // El tiempo de sleep es random entre 1 y 3
	} while(retiro!=0.0);
 	pthread_exit(NULL);
}

int main( ){
	pthread_t hilo_dep, hilo_ret;
	/* La variable error es utilizada para saber si fue posible o no
	crear los hilos o si hubo algún error al ejecutar alguno de los 
	hilos*/
	int error;
	error= pthread_create(&hilo_dep, NULL, proc_deposito, NULL);
	if(error!=1){
		error= pthread_create(&hilo_ret, NULL, proc_retiro, NULL);
		if(error!=1){
			error= pthread_join(hilo_dep, NULL);
			if(error!=1){
				error= pthread_join(hilo_ret, NULL);
			}
		}
	}
	return 0;
}