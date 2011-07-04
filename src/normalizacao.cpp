#include "main.h"

// Vetor com valores pré-calculados para acelerar a computação.
int preCalcDiscP[MAXR];

// mascara usada para ponderar votação em uma vizinhança
int vNormMask[3][3] = {
			{1,2,1},
			{2,10,2},
			{1,2,1}
		       };
int sumVNormMask = 22;


// calcula raio dos circulos discretizados para poder normalizar
void fillPreCalcDiscP(){
	for(int i=2;i<MAXR;i++)
		preCalcDiscP[i] = incCirc(NULL,2*i,2*i,i,i,i);
}

// Dado matriz de votação m com dimensão nx x ny.
// Calcula valor normalizado da votação do ponto (x,y) relativo ao raio r. Retornar algo em [0,1].
double calcVNorm(int **m,int x,int y,int nx,int ny,int rad){
	int i,j;
	double vnorm=0;

	for(i = -1; i<=1;i++){
		for(j=-1;j<=1;j++){
                        if(pointInsideRect(x + j ,y + i,nx,ny)){
				vnorm += m[y+i][x+j]*vNormMask[i+1][j+1];
			}
		}
	}
		
	vnorm = vnorm/(sumVNormMask*preCalcDiscP[rad]);
	return vnorm;
				
}
