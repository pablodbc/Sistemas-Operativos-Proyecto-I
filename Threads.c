#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "bombas_objetivos.h"
#include "errors.h"
int min(int a, int b){return a<b ? a : b;}
int max(int a, int b){return a>b ? a : b;}



pthread_mutex_t bombas_lock;
pthread_mutex_t *objetivo_lock; //tablero_lock
int indice,N,B,T,n_threads;
objetivo* before,*after; //ans y tablero
bomb* bombas;


void *destruir(){
	while(true)
	{
		pthread_mutex_lock(&bombas_lock);
		if(indice >= B)
		{
			pthread_mutex_unlock(&bombas_lock);
			break;
		}
		bomb b = bombas[indice];
		indice++;
		pthread_mutex_unlock(&bombas_lock);
		for(int i = 0; i < T; i++)
		{
			if(after[i].F <= b.F + b.R && after[i].F >= b.F - b.R && after[i].C <= b.C + b.R && after[i].C >= b.C - b.R)
			{
				pthread_mutex_lock(&objetivo_lock[i]);
				if(after[i].R > 0)
				{
					after[i].R = max(0,after[i].R - b.P);
				}else if(after[i].R<0)
				{
					after[i].R = min(0,after[i].R + b.P);
				}
				pthread_mutex_unlock(&objetivo_lock[i]);
			}
		}

	}
	return NULL;
}


void solve_with_threads(char* path, int helpers){
	/*Lectura del archivo para pasarlo por stdin.*/
	if(freopen(path,"r",stdin) == NULL)
	{
		printf(FILE_ERROR);
		exit(-1);
	}
	/* N = Size of the grid
	   T = Number of objectives
	*/
	scanf("%d %d",&N,&T);
	/*
		Se leen los objetivos del stdin y se almacenan en 2 tablas
		para poder comparar la tabla modificada por los threads y
		la tabla original.
	*/
	after  			= (objetivo*) calloc(T,sizeof(objetivo));
	before 			= (objetivo*) calloc(T,sizeof(objetivo));
	objetivo_lock  	= (pthread_mutex_t *)calloc(T, sizeof(pthread_mutex_t));
	for(int i = 0; i < T; i++)
	{
		scanf("%d %d %d", &after[i].F, &after[i].C, &after[i].R);
		before[i] = after[i];
		if (pthread_mutex_init(&(objetivo_lock[i]),NULL) != 0)
		{
			printf(OBJECTIVE_MUTEX_ERROR,i);
			exit(-1);
		}
	}
	/*
		Se guarda la informacion de las bombas que seran lanzadas y se inicializa el semaforo.
	*/
    scanf("%d",&B);
    bombas = (bomb*) calloc(B,sizeof(bomb));
	for(int i = 0; i < B; i++)
	{
		scanf("%d %d %d %d",&bombas[i].F, &bombas[i].C, &bombas[i].R, &bombas[i].P);
	}
	if (pthread_mutex_init(&(bombas_lock),NULL) != 0)
	{
		printf(BOMBS_MUTEX_ERROR);
		exit(-1);
	}
	/*
		Se decide el numero de threads con los que se va a trabajar
		(No tiene sentido crear mas threads que bombas), se ejecutan
		sobre la funcion "destruir" y posteriormente se espera a que 
		todos terminen de ejecutarse.
	*/
	pthread_t *tid;
    indice    = 0;
    n_threads = min(helpers,B);
	tid 	  = (pthread_t *)calloc(n_threads, sizeof(pthread_t));
	for(int i = 0; i < n_threads; i++)
	{
	    if (pthread_create(&(tid[i]), NULL, &destruir, NULL) != 0)
	    {
	    	printf(THREAD_INIT_ERROR, i);
	    	exit(-1);
	    }
	}
	
	destruir(NULL);

	for (int i = 0; i < n_threads; i++)
	{
		pthread_join(tid[i], NULL);
	}
	/*
		Una vez los threads terminan de trabajar, se procede a destruirlos
		y a analizar la situacion posterior al bombardeo.
	*/
	int estado_final[6];
	memset(estado_final,0,sizeof(estado_final));
	for(int i = 0; i < T; i++)
	{
		pthread_mutex_destroy(&(objetivo_lock[i]));
		if(before[i].R<0)
		{
			if(!after[i].R)
			{ 
				estado_final[0]++;
			}
			else if(after[i].R==before[i].R)
			{ 
				estado_final[2]++;
			}
			else
			{ 
				estado_final[1]++;
			}
		}
		else if(before[i].R>0)
		{
			if(!after[i].R)
			{
				estado_final[3]++;
			}

			else if(before[i].R==after[i].R)
			{ 
				estado_final[5]++;
			}
			else
			{ 
				estado_final[4]++;
			}
		}	
	}
	printf("Objetivos Militares totalmente destruidos: %d\n",estado_final[0]);
	printf("Objetivos Militares parcialmente destruidos: %d\n",estado_final[1]);
	printf("Objetivos Militares no afectados: %d\n",estado_final[2]);
	printf("Objetivos Civiles totalmente destruidos: %d\n",estado_final[3]);
	printf("Objetivos Civiles parcialmente destruidos: %d\n",estado_final[4]);
	printf("Objetivos Civiles no afectados: %d\n",estado_final[5]);
}
int main(){
	char* path = "p";
	int help = 1000;
	solve_with_threads(path,help);
	return 0;
}