/* 
Proyecto Sistemas Operativos
Parcial 1   Paralelo #2
Autor: Oswaldo Aguilar - Matricula: 201419079
Tutor: Angel Lopez.
Fecha: 03/12/2019

Parte #2 -> twc.
Crear un aplicativo en c que permita realizar una version del word count usando hilos, debe poder recibir varios archivos y recibir banderas.
*/


//Importaciones Necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include <errno.h> //Libreria para errores
#include<sys/wait.h> 
#include <pthread.h> //Libreria para hilos
#include <sys/sysinfo.h>
#include <math.h>

/* Declaraciòn de globales */
int words = 0; //Contador de palabras
int lines= 0;  //Contador de lineas
int hilos = 4; //Numero de hilos

// Semaforo para validar los contadores
pthread_mutex_t lock_words;
pthread_mutex_t lock_lines;

//Estructura para los archivos
struct archivo 
{ 
	char * ruta; 
	FILE * file; 
	int tamanio;
	int hilos;
	pthread_t** hilos_p; //Hilos ejecutandose por archivo
	int words_a; // Contador de palabras por archivo
	int lines_a; // Contador de lineas por archivo

	//Listas de auxiliar con primeras y ultimos caracteres por hilo para validar.
	char *inicio_l;
	char *final_l;

	//Semaforos para contadores por archivo
	pthread_mutex_t lock_words_a;
	pthread_mutex_t lock_lines_a;
} * archivos;

//Puntero a proceso del hilo donde calcula y cuenta.
void *calcular( void *ptr );

//Funcion de conteo tradicional usando los contadores. y particionando el archivo mediante indices.
int* conteo(char* file, int inicio, int final, int indice, int subindice)
{
	int *conteos = (int*)malloc(2 * sizeof(int));
	int state = 0;
	int aux = 0;

	//Contadores locales
	conteos[0] = 0;
	conteos[1] = 0;
	FILE *f = fopen(file, "r");
	char c;
	fseek(f, inicio, 0);
    	while((c = fgetc(f)) != EOF && aux < final -inicio) {
		//printf("%c",c);
		if(aux == 0){
			//Ayuda de correcion de conteo 
			archivos[indice].inicio_l[subindice] = c;}
        	if (c == ' ' || c == '\n' || c == '\t')  
		    	state = 0;  
	  
		else if (state == 0)  
		{
			state = 1;  
		    	conteos[0]++; 
		}  
	  	if(c == '\n')
			conteos[1]++;
		// Move to next character  
		aux++;
		if(aux == final -inicio)
			//Ayuda de correcion de conteo
			archivos[indice].final_l[subindice] = c; 
	}
	fclose(f);
	return conteos;
}

//Metodo principal
int main(int argc, char **argv){
	
	//Validando entradas
	if(argc == 1){
		printf("No ha ingresado ningun archivo\n");
		exit(0);
	}
	int l = 0; //Bandera para -l
	int w = 0; //Bandera para -w
	int suma = 0; //Acumulador de tamaños de archivos.
	
	int* contador = (int*) malloc(sizeof(int));
	*contador = 0;
	//Inicializacion de semaforos globales.
	 if (pthread_mutex_init(&lock_words, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&lock_lines, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}

	//Obtener el numero de procesadores disponibles.
	int procesadores_disponibles =  get_nprocs();
	
	//Filtrar argumentos para obtener archivos
	char ** argv_filtrado = (char ** ) malloc(sizeof(char*));
	if(procesadores_disponibles > hilos){
		hilos = procesadores_disponibles;
	}
	archivos = (struct archivo *) malloc((1)* sizeof(struct archivo)); 
	for(int i=1; i< argc; i++){
		if(strcmp(argv[i], "-l") == 0)
			l=1;
		else if(strcmp(argv[i], "-w") == 0)
			w=1;
		else{
			argv_filtrado = realloc(argv_filtrado,((*contador)+1) * sizeof(char*));
			argv_filtrado[(*contador)] =argv[i];
			(*contador)++;}
	}
	
	//Recorrer archivos y creacion de objetos con la estructura de archivos
	for(int i=1; i< (*contador)+1; i++){
		printf("%d", (*contador));
		FILE *f = fopen(argv_filtrado[i-1],"r");
		//Validar los archivos
		if(f == NULL)
		{
			printf("Uno de los archivos no es vàlido o no se encuentra\n");   
		     	exit(0);             
		}
		
		archivos[i-1].ruta = argv_filtrado[i-1];
		archivos[i-1].file = f;
		//Contar el tamaño del archivo
		fseek(f, 0, SEEK_END); // seek to end of file
		int size = ftell(f); // get current file pointer
		fseek(f, 0, SEEK_SET);
		fclose(archivos[i-1].file);
		archivos[i-1].tamanio = size;
		archivos[i-1].words_a = 0;
		archivos[i-1].lines_a = 0;
		suma = suma + size;

	}
	int aux = 0;
	
	for(int i=1; i< (*contador)+1; i++){
		
		// Calcular numero de hilos para cada archivo
		float c = (float) archivos[i-1].tamanio / (float) suma;
		c = c *(float)hilos;
		int h = round(c);
		aux = aux + h;
		archivos[i-1].hilos = h;
		
		//Inicializar semaforos de contadores por archivo
		if (pthread_mutex_init(&(archivos[i-1].lock_words_a), NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}
	if (pthread_mutex_init(&(archivos[i-1].lock_lines_a), NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}
		//Inicializar variables de archivos
		archivos[i-1].inicio_l = (char*) calloc(h,sizeof(char));
		archivos[i-1].final_l = (char*) calloc(h,sizeof(char));
		archivos[i-1].hilos_p = (pthread_t**) malloc(h * sizeof(pthread_t*));

		//Creacion  de hilos y division de archivos.
		for(int j=0; j< h; j++){
			int* valores = (int*) malloc(4 * sizeof(int));
			int aux2 = round((float)archivos[i-1].tamanio/(float)archivos[i-1].hilos);
			valores[0] = j * aux2;
			valores[1] = j*aux2 + aux2;
			valores[2] = i-1;
			valores[3] = j;
			if(valores[1] > archivos[i-1].tamanio)
				valores[1] = archivos[i-1].tamanio;
			archivos[i-1].hilos_p[j] = (pthread_t*) malloc(sizeof(pthread_t));
			pthread_create(archivos[i-1].hilos_p[j], NULL, calcular, (void*) valores);

			pthread_join( *(archivos[i-1].hilos_p[j]), NULL);
		}
	}
	
	//Correcion de conteo
	for(int j=0; j< (*contador); j++){
		for(int i=0; i< archivos[j].hilos -1; i++){
			if((archivos[j].final_l[i] != '\n' && archivos[j].final_l[i] != ' ' && archivos[j].final_l[i] != '\t') &&  (archivos[j].inicio_l[i+1] != '\n' && archivos[j].inicio_l[i+1] != ' ' && archivos[j].inicio_l[i+1] != '\t')){
				pthread_mutex_lock(&(archivos[j].lock_words_a));
				archivos[j].words_a--;
			pthread_mutex_unlock(&(archivos[j].lock_words_a));
				pthread_mutex_lock(&lock_words);
				words--;
				pthread_mutex_unlock(&lock_words);

			}
		}	
	}
	
	
		//Presentacion de Resultados
		if(l==1 && w==1)
		{
			for(int i=0; i<(*contador); i++){
			printf(" %d  %d  %s\n",archivos[i].lines_a, archivos[i].words_a, archivos[i].ruta);	
			}
		}
		else if(l==1){
			for(int i=0; i<(*contador); i++){
			printf("%d  %s\n", archivos[i].lines_a, archivos[i].ruta);	
			}
		}
		else if(w==1){
			for(int i=0; i<(*contador); i++){
			printf("%d  %s\n", archivos[i].words_a, archivos[i].ruta);	
			}
		}
		if((*contador) > 1){
			if((l==1 && w==1)|| (l==0 && w==0))
				printf("%d  %d  total\n", lines, words);	
		
			else if(l==1)
				printf("%d   total\n", lines);
			else if(w==1)
				printf("%d   total\n", words);
		}
	
	return 1;	
}

//Funcion para los punteros
void *calcular( void *ptr ){
	
	int* valores;
	valores = (int*) ptr;
	//Usa el contero
	int * prueba = conteo(archivos[valores[2]].ruta,valores[0], valores[1], valores[2], valores[3]);

	//Zona critica global
	pthread_mutex_lock(&lock_words);
	words = words + prueba[0];
	pthread_mutex_unlock(&lock_words);
	pthread_mutex_lock(&lock_lines);
	lines = lines +prueba[1];
	pthread_mutex_unlock(&lock_lines);

	//Zona critica por archivos
	pthread_mutex_lock(&(archivos[valores[2]].lock_words_a));
	archivos[valores[2]].words_a = archivos[valores[2]].words_a + prueba[0];
	pthread_mutex_unlock(&(archivos[valores[2]].lock_words_a));
	pthread_mutex_lock(&(archivos[valores[2]].lock_lines_a));
	archivos[valores[2]].lines_a = archivos[valores[2]].lines_a + prueba[1];
	pthread_mutex_unlock(&(archivos[valores[2]].lock_lines_a));
}

