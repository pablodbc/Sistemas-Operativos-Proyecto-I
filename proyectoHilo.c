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

struct param{
    int F;
    int C;
    int R;
    int P;
    int Nu;
};

int tablero[1002][1002];

void destruir(struct param *params){
    int i,j,fi=params->F-params->R,ff=params->F+params->R,ci=params->C-params->R,cf=params->C+params->R,N=params->Nu;
    while(fi<0) fi++;
    while(ff>N-1) ff--;
    while(ci<0) ci++; 
    while(cf>N-1) cf--; 
    for(i=fi; i<=ff; i++){
        for(j=ci; j<=cf; j++){
            if(tablero[i][j]>0){
                int res=tablero[i][j]-params->C;
                if(res<0) res=0;
                tablero[i][j]=res;
            }else if(tablero[i][j]<0){
                int res=tablero[i][j]+params->C;
                if(res>0) res=0;
                tablero[i][j]=res;
            }
        }
    }
}

void ataque(bom bombas[],int N,int B){
	int i,j,contador=0; char s_child[10];
	while(contador!=B){
		pthread_t tid;
        struct param params;
        bom b1=bombas[contador];
        params.F=b1.F;
        params.C=b1.S;
        params.R=b1.T;
        params.P=b1.C;
        params.Nu=N;
        if (pthread_create (&tid, NULL, (void *)destruir, (void *)&params)) {
            perror("ERROR: No se pudo crear un thread");
            exit(1);
        }
        pthread_join (tid, NULL);
        contador++;
    }

}


int main(){
	//HAYQEHACER UN ARCHIVO
	int N,T,B;
	bom bombas[100002];
	int ans[1002][1002];
	scii(N,T);
	int j,i,x,y,r,p;
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			tablero[i][j]=0;
			ans[i][j]=0;
		}
	}
	for(i=0; i<T; i++){
		sciii(x,y,r);
		tablero[x][y]=r;
		ans[x][y]=r;
	}
	//ans=tablero;
    sci(B);
	for(i=0; i<B; i++){
		sciii(x,y,r); sci(p);
		bom b1; b1.F=x; b1.S=y; b1.T=r; b1.C=p;
		bombas[i]=b1;
	}
    //printf("ASDAS\n");
	ataque(bombas,N,B);
    //printf("DDD\n");
	int estadofin[6];
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			if(ans[i][j]<0){
				if(!tablero[i][j]) estadofin[0]++;
				else if(tablero[i][j]==ans[i][j]) estadofin[2]++;
				else estadofin[1]++;
			}else if(ans[i][j]>0){
				if(!tablero[i][j]) estadofin[3]++;
				else if(tablero[i][j]==ans[i][j]) estadofin[5]++;
				else estadofin[4]++;
			}	
		}
	}
	printf("Objetivos Militares totalmente destruidos: %d\n",estadofin[0]);
	printf("Objetivos Militares parcialmente destruidos: %d\n",estadofin[1]);
	printf("Objetivos Militares no afectados: %d\n",estadofin[2]);
	printf("Objetivos Civiles totalmente destruidos: %d\n",estadofin[3]);
	printf("Objetivos Civiles parcialmente destruidos: %d\n",estadofin[4]);
	printf("Objetivos Civiles no afectados: %d\n",estadofin[5]);
}