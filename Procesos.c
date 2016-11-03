#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC   */
#include <unistd.h>			/* int pipe() */
#include <string.h>			/* memcpy() */
#include <sys/wait.h>		/* waitip() */
#include "bombas_objetivos.h"
#define sci(x) scanf("%d",&x)
#define scii(x,y) scanf("%d %d",&x,&y)
#define sciii(x,y,z) scanf("%d %d %d",&x,&y,&z)
#define SEM_NAME "/name_same"

sem_t *tablero_lock;
int indice,N,B,T,n;
int *indice_bom;

param *tablero;
param *ans;
bom *bombas;

void bombardear(sem_t *bombas_lock,int *indice){
	bom b1;
	//printf("BOM %d\n",b1.C);
	//printf("YES\n");
	while(1){
		//printf("%d %d\n",*indice,n);
		sem_wait(bombas_lock);
		if((*indice)==B){
			sem_post(bombas_lock);
			return;
		}
		memcpy(&b1, &(bombas[*indice]), sizeof(bom));
		(*indice)++;
		sem_post(bombas_lock);
		int i;
		for(i=0; i<T; i++){
			if(tablero[i].F<=b1.F+b1.T && tablero[i].F>=b1.F-b1.T && tablero[i].C<=b1.S+b1.T && tablero[i].C>=b1.S-b1.T){
				sem_wait(&(tablero_lock[i]));
				if(tablero[i].R>0){
				    int res=tablero[i].R-b1.C;
				    if(res<0) res=0;
				    tablero[i].R=res;
				}else if(tablero[i].R<0){
				    int res=tablero[i].R+b1.C;
				    if(res>0) res=0;
				    tablero[i].R=res;
				}
				sem_post(&(tablero_lock[i]));
			}
		}
	}


}


int main(){
	//HAYQEHACER UN ARCHIVO
	char *path="/home/pablodbc/p";
	scii(N,T);
	int j,i,x,y,r,p;

	sem_t *bombas_lock = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
	if(bombas_lock==SEM_FAILED){
		printf("FALLO\n");
		exit(-1);
	}
	sem_unlink(SEM_NAME); 
	key_t clave_tablero;
	int id_tablero;
	clave_tablero = ftok(path,255);
	id_tablero = shmget(clave_tablero, sizeof(param) * T, 0644 | IPC_CREAT);
	if (id_tablero < 0)
	{
        printf("error1\n");
		sem_close(bombas_lock); 
	    sem_destroy(bombas_lock); 
        exit (-1);
    }

    tablero = (param *)shmat(id_tablero, NULL, 0); 

    key_t clave_tablero_lock;
	int id_tablero_lock;
	clave_tablero_lock = ftok(path,254);
	id_tablero_lock = shmget(clave_tablero_lock, sizeof(sem_t) * T, 0644 | IPC_CREAT);
	if (id_tablero_lock < 0)
	{
        printf("error2\n");
		sem_close(bombas_lock); 
	    sem_destroy(bombas_lock);
	    shmdt(tablero);
    	shmctl(id_tablero, IPC_RMID, 0); 
        exit (-1);
    }

    tablero_lock = (sem_t *)shmat(id_tablero_lock, NULL, 0); 
    ans = (param*) calloc(T,sizeof(param));

	for(i=0; i<T; i++){
		param p1;
		sciii(p1.F,p1.C,p1.R);
		tablero[i]=p1;
		ans[i]=p1;
		sem_init(&(tablero_lock[i]), 1, 1);

	}
    sci(B);

    key_t clave_bomba;
	int id_bomba;
	clave_bomba = ftok(path,253);
	id_bomba = shmget(clave_bomba, sizeof(bom) * B, 0644 | IPC_CREAT);
	if (id_bomba < 0)
	{
        printf("error3\n");
		sem_close(bombas_lock); 
	    sem_destroy(bombas_lock);
	    shmdt(tablero);
    	shmctl(id_tablero, IPC_RMID, 0);
    	shmdt(tablero_lock);
	    shmctl(id_tablero_lock, IPC_RMID, 0);
        exit (-1);
    }

    bombas = (bom *)shmat(id_bomba, NULL, 0); 

	for(i=0; i<B; i++){
		sciii(x,y,r); sci(p);
		bom b1; b1.F=x; b1.S=y; b1.T=r; b1.C=p;
		bombas[i]=b1;
	}
	n=2; //CABLEADO PA ARREGLALO
	//n= min(n,B);
	pid_t *childs;
	childs = (pid_t *)calloc(n,sizeof(pid_t));

    key_t indice = ftok(path, 252); 
	int id_indice; 
	id_indice = shmget(indice, sizeof(int), 0644 | IPC_CREAT);
	if (id_indice < 0)
	{
        printf("Error4\n");
	    sem_close(bombas_lock); 
	    sem_destroy(bombas_lock);
		shmdt(tablero);
	    shmctl(id_tablero, IPC_RMID, 0);
		shmdt(tablero_lock);
	    shmctl(id_tablero_lock, IPC_RMID, 0);
		shmdt(bombas);
	    shmctl(clave_bomba, IPC_RMID, 0);
        exit (-1);
    }


    indice_bom = (int *)shmat(id_indice, NULL, 0); 
    *indice_bom=0;

	for(i=0; i<n; i++){
		childs[i]=fork();
		if(childs[i]<0){
			printf("ERROR5\n");
		    sem_close(bombas_lock); 
		    sem_destroy(bombas_lock);
			shmdt(tablero);
		    shmctl(id_tablero, IPC_RMID, 0);
			shmdt(tablero_lock);
		    shmctl(id_tablero_lock, IPC_RMID, 0);
			shmdt(bombas);
		    shmctl(clave_bomba, IPC_RMID, 0);
		    shmdt(indice_bom);
		    shmctl(id_indice, IPC_RMID, 0);
		    exit(-1);
		}
		else if(childs[i]==0){
			free(childs);
    		bombardear(bombas_lock,indice_bom);
    		sem_close(bombas_lock);
    		shmdt(tablero);
			shmdt(tablero_lock);
    		shmdt(indice_bom);
    		shmdt(bombas);
    		exit(0);
		}
	}

	//ESPERO A MIS HIJOS
	for (int i = 0; i < n; ++i)
    {
    	int status;    
    	waitpid(childs[i], &status,0);

    }

    //for(i=0; i<T; i++) printf("%d\n",tablero[i].R );
	int estadofin[6];
	memset(estadofin,0,sizeof(estadofin));
	for(i=0; i<T; i++){
		if(ans[i].R<0){
			if(!tablero[i].R) estadofin[0]++;
			else if(tablero[i].R==ans[i].R) estadofin[2]++;
			else estadofin[1]++;
		}else if(ans[i].R>0){
			if(!tablero[i].R) estadofin[3]++;
			else if(tablero[i].R==ans[i].R) estadofin[5]++;
			else estadofin[4]++;
		}	
	}
	printf("Objetivos Militares totalmente destruidos: %d\n",estadofin[0]);
	printf("Objetivos Militares parcialmente destruidos: %d\n",estadofin[1]);
	printf("Objetivos Militares no afectados: %d\n",estadofin[2]);
	printf("Objetivos Civiles totalmente destruidos: %d\n",estadofin[3]);
	printf("Objetivos Civiles parcialmente destruidos: %d\n",estadofin[4]);
	printf("Objetivos Civiles no afectados: %d\n",estadofin[5]);


    sem_close(bombas_lock); 
    sem_destroy(bombas_lock);
	shmdt(tablero);
    shmctl(id_tablero, IPC_RMID, 0);
	shmdt(tablero_lock);
    shmctl(id_tablero_lock, IPC_RMID, 0);
	shmdt(bombas);
    shmctl(clave_bomba, IPC_RMID, 0);
    shmdt(indice_bom);
    shmctl(id_indice, IPC_RMID, 0);
    return 0;
}