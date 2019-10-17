# Sistemas Operativos-2018-2 :computer: :nerd_face:
Programas entregados para la clase de Sistemas Operativos durante el semestre 2018-2 en la Facultad de Ingeniería, UNAM.

## Programa 1. Factorial con hilos
Este programa hace uso de hilos. En él, el hilo principal crea un hilo secundario, el cual se encarga de obtener el factorial de un número dado por el usuario (entre 1 y 5) a través del teclado y, el resultado es devuelto al hilo principal haciendo uso de las funciones pthread_exit() y pthread_join

## Programa 2. Intérprete de comandos UNIX
Este programa, elaborado en ANSI C funciona como un intérprete de comandos haciendo uso de llamadas al sistema en UNIX. Cabe mencionar que algunos comandos como exit, cd, logout, entre otros, no pueden ser utilizados en este Shell
debido a que no existen como comandos ejecutables. Para poder salir del Shell, basta con escribir la palabra 'salir'.

## Programa 3. Exclusión mutua con el Algoritmo de Peterson
Este programa, elaborado en ANSI C y con llamadas al sistema en UNIX, simula una cuenta de banco en la cual se pueden realizar actividades de depósito y retiro. Para ello, se utilizan dos hilos secundarios, cada uno encargado de una de
estas dos actividades mencionadas.

Para evitar que los hilos accedan a las variables compartidas al mismo tiempo, se implementó el algoritmo de Peterson visto en clase. En él, como se vió en la teoría, es muy poco probable que ocurra algún problema al acceder a la región crítica.

## Programa 4. Administración de memoria contigua
Este programa realiza la simulación de asignación de memoria contigua. Para ello, la memoria fue simulada mediante una estructura de datos, la cual inicia con dos particiones: el sistema operativo de la dirección 0 a la 1499 y un hueco inicial de tamaño 3000 que inicia en la dirección 1500. Conforme entran procesos, la memoria se va dividiendo en bloques de distintos tamaños, los cuales, al terminar el proceso, corresponden a huecos libres que pueden ser asignados a otros nuevos procesos. Esta asignación se realiza siguiendo la estrategia del peor ajuste. Para implementar esta estrategia, se lleva a cabo el registro de los huecos en una lista ordenada de mayor a menor tamaño.

## Alumna :email:
Ilse Abril Vázquez Sánchez - iabrilvzqz@gmail.com