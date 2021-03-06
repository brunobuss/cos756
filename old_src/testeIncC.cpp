// Arquivo de testa do incremento/discretização de arco circular no espaço acumulador
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<algorithm>
#include<string.h>
#include<time.h>

#include<cv.h>
#include<highgui.h>

#include<set>


using namespace std;
using namespace cv;

//================== Inicio Tranformada Circular ==================================

/* TODO

- filtrar picos próximos
- restringir busca em região de interesse 
- fazer análise de histograma
- usar magnitude do gradiente ( a pensar)

*/

/* Convenções:
	- No opencv o sistema de coordenadas (até das funções geométricas) é sempre igual ao de matrizes.
	Ou seja o ponto (x,y) é coluna x e linha y, sendo a coluna crescente da esquerda para a direita e a linha crescente
	do topo para baixo. Essa convenção é usada no código, intercambiando-se com a convenção (i,j) para linha i e coluna j.

*/

//================== Stuff ========================================================
double pi = acos(-1.0);

#define INF (1<<29)

// Diz se o ponto (x,y) está dentro do Retangulo [0,nx) x [0,ny)
#define INSIDE(x,y,nx,ny) (((x) >=0 && (x) < (nx) && (y)>=0 && (y) < (ny))?1:0)



// Estrutura que representa uma possível resposta no espaço de busca.
struct acmPoint{
	double vnorm; // valor normalizado da votação
	int rad,cx,cy; // raio, centro (cx,cy)
	
	acmPoint(int rad_, int cx_,int cy_, double vnorm_ = 0){
		rad = rad_; cx = cx_; cy = cy_;
		vnorm = vnorm_;
	}
	
	inline bool operator< (const acmPoint &p) const{
		return (vnorm < p.vnorm);
	}
};


// dado vetor gradiente (dx,dy) e variação angular delta calcula o intervalo de ângulo de tamanho delta
// centrado na direção do vetor gradiente. Assume delta > 0.
pair<double,double> calcThetaRange(double dx,double dy,double delta = 2*pi){
	double theta = atan2(dy,dx);
	double d = delta/2;
	
	double ti = theta-d;
	double tf = theta+d;
	
	
	return pair<double,double>(ti,tf);
}

//=================================================================================


//================== Acumulador de votos ==========================================

// Estrutura que representa o espaço acumulador
struct acumulator{
	int nrad,nrow,ncol; // numero de raios, linhas e colunas
	int*** m; // matriz com os valores das votações
	int minr,maxr; // menor e maior raio considerados
	
	acumulator(int nrad_,int nrow_, int ncol_, int minr=5,int maxr=5){
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
//=================================================================================

//================= Discretização e Incremento de arco circular ===================

// Dado matriz (2d) de votação m com dimensão nrow x ncol.
// Percorre o arco-circular centrado em (cx,cy) de raio r no intervalo thetaRange incrementando a votação do
// valor inc

double eps = 0.0000001;

int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r,
pair<double,double> thetaRange = pair<double,double>(0.0,2*pi),int inc=1){

	double theta = thetaRange.first;
	if(theta == 0.0)theta += eps;
	
	double h = sqrt(r*r + 0.35);
	double step = acos(r/h);
 
//	printf("%lf\n",step);
	double x,y;
	int ix,iy,antx=-INF,anty=-INF;
	
	int pcount =0;
	int count2=0;
	
	for(;theta < thetaRange.second ; theta += step){		
		count2++;
		x = cx + r*cos(theta);
		y = cy + r*sin(theta);
		
		ix = (int)x;iy = (int)y;
		if(ix == antx && iy == anty) continue;
		pcount++;
		printf("%d %d %lf\n",iy,ix,theta);
		
		if(INSIDE(ix,iy,ncol,nrow))
			m[iy][ix] += inc;
			
		antx = x;anty = y;
	}
	
	printf("%d %d razao=%lf\n",pcount,count2,pcount/(2*pi*r));
	return pcount;

}

//=================================================================================


//================== Calculo do valor normalizado =================================


// mascara usada para ponderar votação em uma vizinhança
int vNormMask[3][3] = 
{
{1,2,1},
{2,10,2},
{1,2,1}
};
int sumVNormMask = 22;

// Dado matriz de votação m com dimensão nx x ny.
// Calcula valor normalizado da votação do ponto (x,y) relativo ao raio r. Retornar algo em [0,1].
double calcVNorm(int **m,int x,int y,int nx,int ny,int rad){
	int i,j;
	double vnorm=0;
	
	for(i = -1; i<=1;i++)
		for(j=-1;j<=1;j++){
			if(INSIDE(x + j ,y + i,nx,ny)){
				vnorm += m[y+i][x+j]*vNormMask[i+1][j+1];
			}
		}
		
	vnorm = vnorm/(sumVNormMask*2*pi*rad);
	return vnorm;
				
}
//=================================================================================

//================== Core Tranformada =============================================


// Dado imagem em escala de cinza gray, menor raio a ser buscado minr, maior raio a ser buscado maxr,
// treshold usado para Canny cannyt. Preenche o vetor output com os picos achados no espaço acumulador.
// São retornados os maiores thNCirc picos que tem valor normalizado acima de thVNorm. 

void houghC1(Mat &gray, int minr, int maxr , int cannyt, vector<acmPoint> &output, unsigned int thNCirc = 1, double thVNorm = -1){
	Mat edges,Ix,Iy;
	double deltaTheta = 0.79; // 45 graus
	int i,j,rad;
	pair<double,double> dTheta;
	set<acmPoint> outputMinHeap;
	
	output.clear();
		
	Canny(gray,edges,cannyt,cannyt-3,3,true);	
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
	
	for(rad=0;rad<acm.nrad;rad++){
//		printf("rad = %d\n",rad + minr);
		for(i=0;i<acm.nrow;i++){
			for(j=0;j<acm.ncol;j++){
				vnorm = calcVNorm(acm.m[rad],j,i,acm.ncol,acm.nrow,rad + minr);

				if(vnorm > thVNorm){
					if(outputMinHeap.size() < thNCirc || outputMinHeap.begin()->vnorm < vnorm){
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
//=================================================================================

//================================== Fim ==========================================



int main(){
	int n=40,m=40,i,j;
	
	Mat img(n,m,DataType<uchar>::type);
	
	int **mat = new int*[n];
	for(i=0;i<n;i++)
		mat[i] = new int[m];
	
	memset(&mat[0][0],0,sizeof(mat));
	img = Scalar(0);
	
	while(1){
	
		int cx,cy,rad;
		pair<double,double> thetaRange(0.0,2*pi);
	
		scanf("%d %d %d",&cx,&cy,&rad);
		incCirc(mat,n,m,cx,cy,rad,thetaRange);
	
		for(i=0;i<n;i++){
			for(j=0;j<m;j++){
			
				if(mat[i][j] > 0)
					img.at<uchar>(i,j) = 255;
			
				printf("%d ",mat[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	
	
		imshow("",img);
		waitKey();
	}
	
	return 0;
}
