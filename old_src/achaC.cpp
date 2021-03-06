// Arquivo de teste para achar bola na imagem

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
- pensar em parâmetros para aplicar reiniciar o tracking

*/

/* Convenções:
	- No opencv o sistema de coordenadas (até das funções geométricas) é sempre igual ao de matrizes.
	Ou seja o ponto (x,y) é coluna x e linha y, sendo a coluna crescente da esquerda para a direita e a linha crescente
	do topo para baixo. Essa convenção é usada no código, intercambiando-se com a convenção (i,j) para linha i e coluna j.

*/

//================== Stuff ========================================================
#define INF (1<<29)
#define MAXR (1024)

double pi = acos(-1.0);

// Diz se o ponto (x,y) está dentro do Retangulo [0,nx) x [0,ny)
#define INSIDE(x,y,nx,ny) (((x) >=0 && (x) < (nx) && (y)>=0 && (y) < (ny))?1:0)

// Estrutura que representa uma possível resposta no espaço de busca.
struct acmPoint{
	double vnorm; // valor normalizado da votação
	int rad,cx,cy; // raio, centro (cx,cy)
	bool inic;
	
	acmPoint(){inic = false;}
	
	acmPoint(int rad_, int cx_,int cy_, double vnorm_ = 0){
		rad = rad_; cx = cx_; cy = cy_;
		vnorm = vnorm_;
		inic = true;
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

double epsIncCirc = 0.0000001;

int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r,
pair<double,double> thetaRange = pair<double,double>(0.0,2*pi),int inc=1){

	double theta = thetaRange.first;
	if(theta == 0.0) theta += epsIncCirc;
	
	double h = sqrt(r*r + 0.35);
	double step = acos(r/h);
 
//	printf("%lf\n",step);
	double x,y;
	int ix,iy,antx=-INF,anty=-INF;
	int pcount=0;
	
	
	for(;theta < thetaRange.second ; theta += step){
		x = cx + r*cos(theta);
		y = cy + r*sin(theta);
		
		ix = (int)x;iy = (int)y;
		if(ix == antx && iy == anty) continue;
		pcount++;
		
		if(m != NULL && INSIDE(ix,iy,ncol,nrow))
			m[iy][ix] += inc;
			
		antx = x;anty = y;
	}
	
	return pcount;

}

//=================================================================================


//================== Calculo do valor normalizado =================================

// calcula raio dos circulos discretizados para poder normalizar
int preCalcDiscP[MAXR];
bool inicPreCalcDiscP=false;

void fillPreCalcDiscP(){
	for(int i=2;i<MAXR;i++)
		preCalcDiscP[i] = incCirc(NULL,2*i,2*i,i,i,i);
	inicPreCalcDiscP=true;
}


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
		
	vnorm = vnorm/(sumVNormMask*preCalcDiscP[rad]);
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
		
	Canny(gray,edges,cannyt,cannyt,3,true);	
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
	
	if(!inicPreCalcDiscP)
		fillPreCalcDiscP();
	
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


//============================== Achar bola ====================================

acmPoint findBall(Mat &roiImg, int minr, int maxr, double thScore = -1){
	int thNCirc = 10;
	int thCanny = 100;
	
	vector<acmPoint> circles;	
	houghC1(roiImg,minr,maxr,thCanny,circles,thNCirc,thScore);
	
	//TODO fazer análise de histograma para filtrar aqui
		
	return circles[0];
	
}


//=================================================================================


//============================== Ajuste da ROI ====================================


void trackBall(const Mat &imgAnt, Mat &imgAt,const Rect &ROIat,const acmPoint &ballAnt, acmPoint &newBall, Rect &newROI,
int minr=0, int maxr=0, bool firstTime = false){
	int deltaR=1;
	int limMinR = 2;
	int limMaxR = min(imgAt.rows/2,imgAt.cols/2);
	double roiScale=3;
	Rect roiRect;
	
	if(firstTime){
		roiRect = Rect(0,0,imgAt.cols-1,imgAt.rows-1);
		
	}else{
		roiRect = ROIat;
		if(ballAnt.inic){
			minr = max(limMinR,ballAnt.rad-deltaR);
			maxr = min(limMaxR,ballAnt.rad+deltaR);
		}
	}
	
	Mat roiImg = imgAt(roiRect);
	newBall = findBall(roiImg,minr,maxr);
	newBall.cx += roiRect.x; 
	newBall.cy += roiRect.y;

	Point center(newBall.cx,newBall.cy);
	int rad = newBall.rad;
	
	newROI = Rect(center.x - roiScale*rad, center.y - roiScale*rad,2*roiScale*rad,2*roiScale*rad);
	newROI = newROI & Rect(0,0,imgAt.cols-1,imgAt.rows-1);
	printf("vnorm = %lf\n rad = %d\n",newBall.vnorm,newBall.rad);
	
} 

//=================================================================================





//================================== Fim ==========================================




int main(){

	int i;
	
	Mat img = imread("./arquivoTeste/testeImg6.jpeg",1),gray;

	cvtColor(img,gray,CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5,5), 1.5, 1.5);
	
	printf("%d %d\n",img.rows,img.cols);
	
	acmPoint ball;
	Rect roiRect;
	
	trackBall(Mat(),gray,Rect(110,105,25,25),acmPoint(),ball,roiRect,5,30,false);
	
	
//	rectangle(img,Point(110,105),Point(135,130),Scalar(0,0,255));
	
	
	circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
	rectangle(img,Point(roiRect.x,roiRect.y),Point(roiRect.x + roiRect.width - 1,roiRect.y + roiRect.height -1),Scalar(0,255,0));
	
	
	
	imshow("img",img);
	waitKey();
	
	
}
