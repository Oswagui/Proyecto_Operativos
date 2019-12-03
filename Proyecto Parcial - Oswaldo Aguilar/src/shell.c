/* 
Proyecto Sistemas Operativos
Parcial 1   Paralelo #2
Autor: Oswaldo Aguilar - Matricula: 201419079
Tutor: Angel Lopez.
Fecha: 03/12/2019

Parte #1 -> Shell.
Crear un aplicativo en c que permita simular una version simplificada del shell
de Linux con dos principales funciones exit and cd implementadas usando las llamadas del sistema necesarias. Adicionalmente la creacion de un subproceso que incie un ejecutable sin perder el principal.
*/

//Importaciones Necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include<unistd.h>
#include <errno.h> // Ayuda a validar errores
#include<sys/wait.h> // Necesaria para realizar las pausas
int main()
{
	char instruccion[1000]; //Variable que almacenara la instruccion
   	char *token; //Puntero que nos ayudara a realizar el split
	char *com;  //Comando primera parte de la instruccion
	char **argumentos; //Argumentos lo que queda despues del comando.
	char s[100]; //Almacenara la ruta actual para mostrar en el shell.
	while(1)
	{
		// Presentar shell y recibir instruccion
		printf("\nSO<201419079>[%s]sh%% ",getcwd(s, 100));
		fgets(instruccion, 1000, stdin);
		
		//Separar la instruccion en comando y argumentos.
		instruccion[strcspn(instruccion, "\n")] = 0;
		token = strtok(instruccion, " \t");
		int i = 0;
		argumentos = (char **) malloc(1 * sizeof(char) * 100);
	    	while (token) {
			if(i == 0){
				com = (char*) malloc((strlen(token) +1)* sizeof(char) );
				com[strlen(token)] = '\0';
				strcpy(com, token);
			}
			else{
				argumentos[i-1] = (char *)malloc(100 *sizeof(char));
				strcpy(argumentos[i-1], token);
				argumentos = realloc(argumentos,(i+1) * sizeof(char*));
				
			
			}
			i++;
			token = strtok(NULL, " \t");
			
	    	}

		//En caso que el comando sea exit
		if(strcmp(com, "exit") == 0)
		{
			exit(1); //Llamada al Sistema
		}
		
		//En caso que el comando sea cd pero se ingrese sin argumentos
		else if(strcmp(com, "cd") == 0 && i == 1)
		{
			chdir(getenv("HOME"));  //Llamada al Sistema
		}
		
		//En caso que el comando sea cd y tenga los argumentos necesarios
		else if(strcmp(com, "cd") == 0 && i == 2)
		{
			int z = chdir(argumentos[0]);
			if(z == -1)
				fprintf(stderr, "Error in cd command: %s\n", strerror( errno ));
		}
		
		//En caso que el comando sea cd pero se ingrese sin argumentos
		else if(strcmp(com, "cd") == 0 && i > 2)
		{
			printf("bash: cd: too many arguments"); 
		}

		//En caso que el sea un ejecutable
		else{
			
			int pid;
			int status;
			//Creamos el hijo
			if (pid= fork()== 0)
			{
				//Creamos el arreglo necesario
				char *y = (char*) malloc(100 * sizeof(char));
				token = strtok(instruccion, "/");
				while (token){
					token = strtok(NULL, "/");
					if(token != NULL)
						strcpy(y, token);
				}
				char *argv[i+1];
				if(strlen(y)==0){
					argv[0] = com;}
				else{
					argv[0] = y;}
				for(int j=0;j< i -1; j++)
					argv[j+1] = argumentos[j];
				
				argv[i] = NULL; 
				
				//Ejectuamos el comando
				int w = execv(com, argv);

				//Validamos
				if(w == -1)
				fprintf(stderr, "Error in cd command: %s\n", 	strerror( errno ));
				exit(1); 
				
			}
			else
			{  
				//Esperamos el hijo termine.
				waitpid(pid, &status, 0);
			} 
			
		}
		//Liberamos memoria.
		free(argumentos);
		free(com);
	}
	
}
