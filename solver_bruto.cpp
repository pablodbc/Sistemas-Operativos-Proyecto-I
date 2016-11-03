#include <bits/stdc++.h>
using namespace std;
#define MAXN 1001
int A[MAXN][MAXN],B[MAXN][MAXN],x,y,r,p,N,T,V,b;
bool isValid(int i, int j){return i>-1&&i<N&&j>-1&&j<N;}
int main(){
	memset(A,0,sizeof(A));
	memset(B,0,sizeof(B));
	scanf("%d",&N);
	scanf("%d",&T);
	for(int i = 0; i<T; i++){
		scanf("%d %d %d",&x,&y,&V);
		A[x][y] = B[x][y] = V;
	}
	scanf("%d",&b);
	for(int i = 0; i<b;i++){
		scanf("%d %d %d %d",&x,&y,&r,&p);
		for(int k = x-r; k<=x+r; k++){
			for(int l = y-r; l<=y+r; l++){
				if(isValid(k,l)){
					if(A[k][l] < 0) A[k][l] = min(0,A[k][l]+p);
					else if (A[k][l]>0) A[k][l] = max(0,A[k][l]-p);
				}
			}
		}
	}
	int om_no_alcanzados = 0,om_parcialmente_destruidos = 0,om_totalmente_destruidos = 0; 
	int oc_no_alcanzados = 0,oc_parcialmente_destruidos = 0,oc_totalmente_destruidos = 0; 
	for(int i = 0; i<N; i++){
		for(int j = 0; j<N; j++){
			if(B[i][j] > 0){
				if(A[i][j] == 0) oc_totalmente_destruidos++;
				else if(A[i][j] > 0 && A[i][j] < B[i][j]) oc_parcialmente_destruidos++;
				else if(A[i][j] == B[i][j]) oc_no_alcanzados++;
			}else if(B[i][j]<0){
				if(A[i][j] == 0) om_totalmente_destruidos++;
				else if(A[i][j] < 0 && A[i][j] > B[i][j]) om_parcialmente_destruidos++;
				else if(A[i][j] == B[i][j]) om_no_alcanzados++;
			}
		}
	}
	printf("Objetivos militares totalmente destruidos: %d\n",om_totalmente_destruidos);
	printf("Objetivos militares parcialmente destruidos: %d\n",om_parcialmente_destruidos);
	printf("Objetivos militares no afectados: %d\n",om_no_alcanzados);
	printf("Objetivos civiles totalmente destruidos: %d\n",oc_totalmente_destruidos);
	printf("Objetivos civiles parcialmente destruidos: %d\n",oc_parcialmente_destruidos);
	printf("Objetivos civiles no afectados: %d\n",oc_no_alcanzados);
}
