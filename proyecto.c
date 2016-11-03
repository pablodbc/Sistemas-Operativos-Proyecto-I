#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#define sci(x) scanf("%d",&x)
#define scii(x,y) scanf("%d %d",&x,&y)
#define sciii(x,y,z) scanf("%d %d %d",&x,&y,&z)

typedef struct bom{
	int F;
	int S;
	int T;
	int C;
} bom;

int tablero[1002][1002];

void armar_txt(int n, char s[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        s[i++] = n %10 + '0';
    } while ((n /= 10) > 0);
    if (sign < 0) s[i++] = '-';
    s[i] = '\0';

    char* j;
    int c; 
    j = s + strlen(s) - 1; 
    while (s < j) {
        c = *s;
        *s++ = *j;
        *j-- = c;
    }
}


void ataque(bom bombas[],int N,int B){
	int i,j,contador=0; char s_child[10];
	while(contador!=B){
		pid_t child=fork();
		if (child == -1){
       		perror("Error al ejecutar el fork");
        	exit(1);  
        }  
        else if (child == 0){
        	bom b1=bombas[contador];
            armar_txt(getpid(), s_child);
            //printf("%d\n",tablero[1][1]);
            //printf("%s contador %d\n",s_child,contador);
            FILE *arc = fopen(s_child, "w");
            int fi=b1.F-b1.T,ff=b1.F+b1.T,ci=b1.S-b1.T,cf=b1.F+b1.T;
            while(fi<0) fi++;
            while(ff>N-1) ff--;
            while(ci<0) ci++; 
            while(cf>N-1) cf--; 
            for(i=fi; i<=ff; i++){
            	for(j=ci; j<=cf; j++){
            		if(tablero[i][j]>0){
            			int res=tablero[i][j]-b1.C;
            			if(res<0) res=0;
            			//printf("%d %d %d ",i,j,res);
            			fprintf(arc,"%d %d %d ",i,j,res);
            		}else if(tablero[i][j]<0){
            			//printf("dd %d\n",b1.C);
            			int res=tablero[i][j]+b1.C;
            			if(res>0) res=0;
            			//printf("%d|%d%d ",i,j,res);
            			fprintf(arc,"%d %d %d ",i,j,res);
            		}
            	}
            }
            exit(0);
        }else{
        	wait(NULL);
        	contador++;
        	armar_txt(child, s_child);
        	printf("%s contador %d\n",s_child,contador);
        	FILE *resp = fopen(s_child, "r");
        	rewind(resp); char aux[10];
        	while (feof(resp) == 0){
        		int nums[4];
        		for(i=0; i<3; i++){
        			strcpy(aux, ""); 
                    fscanf(resp, "%s ", aux);
        			int n=atoi(aux);
        			nums[i]=n;
        		}
        		tablero[nums[0]][nums[1]]=nums[2];
        	}
            close(resp); 
            remove(s_child);
        }
	}

}


int main(){
	//HAYQEHACER UN ARCHIVO
	int N,T,B;
	bom bombas[100002];
	int ans[1002][1002];
	sciii(N,T,B);
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
	for(i=0; i<B; i++){
		sciii(x,y,r); sci(p);
		bom b1; b1.F=x; b1.S=y; b1.T=r; b1.C=p;
		bombas[i]=b1;
	}
	ataque(bombas,N,B);
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