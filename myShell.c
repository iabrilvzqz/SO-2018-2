/*
	Nombre: Intérprete de comandos UNIX
	Elaboró: Vázquez Sánchez Ilse Abril
	Fecha de entrega: Martes 6 de Marzo de 2018	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char const *argv[]){
	char command[100], *p, *sP;
	char **sCommand = (char**)malloc(sizeof(char*)*30);
	int i;
	do{
		/*En esta parte del código se obtiene el comando escrito por el usuario*/
		printf("myShell > ");
		fgets(command, 100, stdin);

		p = strchr(command, '\n');
		/*Se quita el caracter de salto de línea y se remplaza por fin de línea*/
		if(p)
			*p = '\0';

		/*Si el usuario escribe la palabra salir, termina de ejecutarse el Shell*/
		if(strcmp(command, "salir") == 0)
			return 0;
		
		/*En esta parte del código, se separa el comando en un vector de cadenas*/
		sP = strtok(command, " ");
		i = 0;
		while(sP != NULL){
			sCommand[i] = sP;
			sP = strtok(NULL, " ");
			i++;
		}

		/*Finalmente, se crea un proceso hijo que manda ejecutar el comando y,
		el padre espera a que este proceso termine*/
		if(fork()){
			wait(NULL);
		}else{
			execvp(sCommand[0], sCommand);
			/*En caso de no existir el comando, se envía un mensaje de error y el
			proceso hijo termina su ejecución*/
			printf("\nError de comando\n\n");
			exit(0);
		}
		
		/*Se 'limpia' el contenido del vector de cadenas ya que seguirá siendo
		utilizado hasta que se mande cerrar el Shell*/
		i = 0;
		while(sCommand[i] != NULL){
			sCommand[i] = NULL;
			i++;
		}
	}while(1);
}
