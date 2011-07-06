#include "main.h"

/* Convenções:
	- No opencv o sistema de coordenadas (até das funções geométricas) é sempre igual ao de matrizes.
	Ou seja o ponto (x,y) é coluna x e linha y, sendo a coluna crescente da esquerda para a direita e a linha crescente
	do topo para baixo. Essa convenção é usada no código, intercambiando-se com a convenção (i,j) para linha i e coluna j.

*/

// Estrutura que representa o espaço acumulador
struct acumulator{
	int nrad,nrow,ncol; // numero de raios, linhas e colunas
	int*** m; // matriz com os valores das votações
	int minr,maxr; // menor e maior raio considerados
	
	acumulator(int nrad_,int nrow_, int ncol_, int minr_ =5,int maxr_ =5){
		int i,j,k;
		nrad = nrad_;
		nrow = nrow_;
		ncol = ncol_;
		m = new int**[nrad];
		for(i=0;i<nrad;i++){
			m[i] = new int*[nrow];
			for(j=0;j<nrow;j++){
				m[i][j] = new int[ncol];
				for(k=0;k<ncol;k++)
					m[i][j][k] = 0;
			}
		}
		minr = minr_;
		maxr = maxr_;
	}
	
	~acumulator(){
		int i,j;
		for(i=0;i<nrad;i++){
			for(j=0;j<nrow;j++){
				delete[] m[i][j];				
			}
			delete[] m[i];
		}
		delete[] m;
	}
};

bool inicPreCalcDiscP=false;

// dado vetor gradiente (dx,dy) e variação angular delta calcula o intervalo de ângulo de tamanho delta
// centrado na direção do vetor gradiente. Assume delta > 0.
pair<double,double> calcThetaRange(double dx,double dy,double delta = 2*pi){
	double theta = atan2(dy,dx);
	double d = delta/2;
	
	double ti = theta-d;
	double tf = theta+d;
	
	
	return pair<double,double>(ti,tf);
}

// Dado imagem em escala de cinza gray, menor raio a ser buscado minr, maior raio a ser buscado maxr,
// treshold usado para Canny cannyt. Preenche o vetor output com os picos achados no espaço acumulador.
// São retornados os maiores thNCirc picos que tem valor normalizado acima de thVNorm. 

void houghC1(Mat &gray, int minr, int maxr , int cannyt, vector<acmPoint> &output, unsigned int thNCirc, double thVNorm){
	Mat edges,Ix,Iy;
	double deltaTheta = 0.79/2; // 45/2 graus
	int i,j,rad;
	pair<double,double> dTheta;
	set<acmPoint> outputMinHeap;
	
	output.clear();
		
	Canny(gray,edges,cannyt,cannyt/(1.2),3,true);	
	Sobel(gray,Ix,DataType<double>::depth,1,0,3,((double)1)/(8*255)); // [-0.5, 0.5]
	Sobel(gray,Iy,DataType<double>::depth,0,1,3,((double)1)/(8*255)); // [-0.5, 0.5]
	
	imshow("edges",edges);
	
	acumulator acm(maxr - minr + 1, edges.rows, edges.cols,minr,maxr);
	
	for(rad = minr; rad <= maxr; rad++){
		for(i=0;i<edges.rows;i++){
			const uchar* edgesl = edges.ptr<uchar>(i);
			const double* Ixl = Ix.ptr<double>(i);
			const double* Iyl = Iy.ptr<double>(i);
			
			for(j=0;j<edges.cols;j++){				
				if(edgesl[j] > 0){
				
				    dTheta = calcThetaRange(Ixl[j],Iyl[j],deltaTheta);
					incCirc(acm.m[rad-minr],acm.nrow,acm.ncol,j,i,rad,dTheta);
				}
			}
		}
	}
	
	double vnorm;
	
	if(!inicPreCalcDiscP){
		fillPreCalcDiscP();
		inicPreCalcDiscP = true;
	}
	
	for(rad=0;rad<acm.nrad;rad++){
//		printf("rad = %d\n",rad + minr);
		for(i=0;i<acm.nrow;i++){
			for(j=0;j<acm.ncol;j++){
				vnorm = calcVNorm(acm.m[rad],j,i,acm.ncol,acm.nrow,rad + minr);

				if(vnorm > thVNorm){
                                        if(outputMinHeap.size() < thNCirc || outputMinHeap.begin()->scoreCircular < vnorm){
						// add acmPoint
						outputMinHeap.insert(acmPoint(rad + minr , j, i, vnorm));
						
						if(outputMinHeap.size() > thNCirc){
							outputMinHeap.erase(outputMinHeap.begin());
						}
					} 
				}
				
//				printf("%d(%.4lf)  ",acm.m[rad][i][j],vnorm);
			}
//			printf("\n");
		}
//		printf("\n");
	}
	
	set<acmPoint>::reverse_iterator rit;
	for(rit = outputMinHeap.rbegin(); rit != outputMinHeap.rend(); ++rit){
		output.push_back(*rit);
	}
	
}
