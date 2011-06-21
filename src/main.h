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


// Protótipos:
int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r,
	    pair<double,double> thetaRange = pair<double,double>(0.0,2*pi),int inc=1);

void fillPreCalcDiscP();

double calcVNorm(int **m,int x,int y,int nx,int ny,int rad);

void houghC1(Mat &gray, int minr, int maxr , int cannyt, vector<acmPoint> &output,
 		unsigned int thNCirc = 1, double thVNorm = -1);

acmPoint findBall(Mat &roiImg, int minr, int maxr, double thScore = -1);

void trackBall(const Mat &imgAnt, Mat &imgAt,const Rect &ROIat,const acmPoint &ballAnt,
		acmPoint &newBall, Rect &newROI, int minr=0, int maxr=0, bool firstTime = false);


