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


//Constantes:
#define INF (1<<29)
#define MAXR (1024)
#define EPSINCCIRC 0.0000001

const double pi = acos(-1.0); //Não pode virar define?

inline bool INSIDE(int const& x, int const& y, int const& nx, int const& ny){
	if(x >=0 && x < nx && y >=0 && y < ny) return 1;
	return 0;
}


//Estruturas:

//Estrutura que representa uma possível resposta no espaço de busca.
struct acmPoint{
	double vnorm; // valor normalizado da votação
	double vhistograma; // score com base na comparação de histograma
	double score_final;	
	

	int rad,cx,cy; // raio, centro (cx,cy)

	bool inic;
	
	acmPoint(){inic = false;}
	
	acmPoint(int rad_, int cx_,int cy_, double vnorm_ = 0){
		rad = rad_; cx = cx_; cy = cy_;
		score_final = vnorm = vnorm_;
		vhistograma = 0.0;
		inic = true;
	}

	void calculaScore(){ score_final =  0.4 * vnorm + 0.6 * vhistograma; }
	
	inline bool operator< (const acmPoint &p) const{
		return (score_final < p.score_final);
	}


};


// Protótipos:
int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r,
	    pair<double,double> thetaRange = pair<double,double>(0.0,2*pi),int inc=1);

void fillPreCalcDiscP();

double calcVNorm(int **m,int x,int y,int nx,int ny,int rad);

void houghC1(Mat &gray, int minr, int maxr , int cannyt, vector<acmPoint> &output,
 		unsigned int thNCirc = 1, double thVNorm = -1);

acmPoint findBall(Mat &frame, Mat &roiImg, int minr, int maxr, bool filtraHistograma = false, double thScore = -1);

void trackBall(const Mat &frameAnterior, Mat &frame,const Rect &ROIat,const acmPoint &ballAnt,
		acmPoint &newBall, Rect &newROI, int minr=0, int maxr=0, bool firstTime = false, bool filtraHistograma = false);

void calcula_histograma(Mat &imagem, vector<acmPoint> &pontos);

void define_histograma_otimo(const Mat &imagem, acmPoint &ponto);
