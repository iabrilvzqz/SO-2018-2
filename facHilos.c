/*	Nombre: Obtención del factorial con hilos
	Programa elaborado por: Vázquez Sánchez Ilse Abril
	Fecha de entrega: 17/03/2018  */

#include <pthread.h>
#include <stdio.h>

/*Esta función será utilizada únicamente por el hilo secundario*/
void * factorial(void *numero){
	int i;
	/*La variable que contendrá el resultado que será devuelto
	mediante la función exit() es fac, por ello se declara estática*/
	static int fac = 1;
	// Inicia la obtención del factorial del número
	for(i = 1; i <= *(int *)numero; i++)
		fac *= i;
	/*Finalmente, se manda el resultado haciendo un casteo de la variable
	de tipo entero a apuntador a void*/
	pthread_exit((void *)&fac);
}

void main()
{
	pthread_t hilo;
	int numero = 0;
	int *resultado;

	//Mediante este while se valida que el numero que dé el usuario está entre 1 y 5
	while(numero<1 || numero>5){
		printf("\nEscriba un número del 1 al 5 para obtener su factorial: ");
		scanf("%d", &numero);
	}

	/*En esta parte se crea el hilo y se indica que utilizará la función
	factorial y que el parámetro que recibirá la función es la variable numero*/
	pthread_create(&hilo, NULL, factorial, &numero);
	/*El hilo principal espera a que termine el hilo secundario, el resultado
	obtenido en la función factorial ejecutada por el hijo será guardado en la
	variable resultado*/
	pthread_join(hilo, (void **)&resultado);
	printf("\nEl resultado del factorial es: %d\n", *resultado);
}