#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#define sci(x) scanf("%d",&x)
#define scii(x,y) scanf("%d %d",&x,&y)
#define sciii(x,y,z) scanf("%d %d %d",&x,&y,&z)

typedef struct bom{
	int F;
	int S;
	int T;
	int C;
} bom;

typedef struct param{
    int F;
    int C;
    int R;
} param;

pthread_mutex_t bombas_lock;
pthread_mutex_t tablero_lock;
int indice,N,B,T,n;

param tablero[100002],ans[100002];
bom bombas[100002];


void *destruir(void *arg __attribute__((unused))){
	while(1){
		pthread_mutex_lock(&bombas_lock);
		if(indice>=B){
			pthread_mutex_unlock(&bombas_lock);
			break;
		}
		bom b1=bombas[indice];
		indice++;
		pthread_mutex_unlock(&bombas_lock);
		int i,j;
		for(i=0; i<T; i++){
			if(tablero[i].F<=b1.F+b1.T && tablero[i].F>=b1.F-b1.T && tablero[i].C<=b1.S+b1.T && tablero[i].C>=b1.S-b1.T){
				pthread_mutex_lock(&tablero_lock);
				if(tablero[i].R>0){
				    int res=tablero[i].R-b1.C;
				    if(res<0) res=0;
				    tablero[i].R=res;
				}else if(tablero[i].R<0){
				    int res=tablero[i].R+b1.C;
				    if(res>0) res=0;
				    tablero[i].R=res;
				}
				pthread_mutex_unlock(&tablero_lock);
			}
		}

	}
}

void ataque(){
	pthread_t *tid;
	tid = (pthread_t *)calloc(n, sizeof(pthread_t));
	int i;
	for ( i = 0; i < n; i++)
	{
	    if (pthread_create(&(tid[i]), NULL, &destruir, NULL) != 0)
	    {
	    	printf("Error creando thread[%d]", i);
	    	exit(-1);
	    }
	}
	
	destruir(NULL);

	for (int i = 0; i < n; ++i)
	{
		pthread_join(tid[i], NULL);
	}

}


int main(){
	//HAYQEHACER UN ARCHIVO
	scii(N,T);
	int j,i,x,y,r,p;
	for(i=0; i<T; i++){
		param p1;
		sciii(p1.F,p1.C,p1.R);
		tablero[i]=p1;
		ans[i]=p1;
	}
	//ans=tablero;
    sci(B);
	for(i=0; i<B; i++){
		sciii(x,y,r); sci(p);
		bom b1; b1.F=x; b1.S=y; b1.T=r; b1.C=p;
		bombas[i]=b1;
	}
    if (pthread_mutex_init(&(bombas_lock), NULL) != 0)
    {
        printf("Fallo inicializando\n");
        exit(-1);
    }
    if (pthread_mutex_init(&(tablero_lock), NULL) != 0)
    {
        printf("Fallo inicializando\n");
        exit(-1);
    }
    n=2;
    indice=0;
	ataque();

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
	return 0;
}