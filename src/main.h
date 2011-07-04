/* Header primário do código.
   É incluído por todos os outros arquivos de forma a prover
   estruturas básicas e protótipos que são utilizados em diversas
   partes do código.
*/

#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<cstring>
#include<ctime>
#include<getopt.h>

#include<algorithm>
#include<set>
#include<vector>

#include<cv.h>
#include<highgui.h>

using namespace std;
using namespace cv;


//O histograma considerado como "ótimo" pelo código responsável pela comparação de histograma.
extern vector< vector< vector <double> > > histograma_otimo;

//Quantos níveis cada canal de cor (BGR) será subduvidido.
#define NIVEL_POR_COR (6)
#define FATOR (256.0/NIVEL_POR_COR)

//Constantes matemáticas:
#define INF (1<<29)
#define MAXR (1024)
#define EPSINCCIRC 0.0000001

const double pi = acos(-1.0); //Como const double para evitar que o programa fique chamando acos() diversas vezes.


/*
  Verifica se o ponto (x, y) está dentro do retangulo delimitado por (0, 0) e (nx-1, ny-1)
 */
inline bool pointInsideRect(int const& x, int const& y, int const& nx, int const& ny){
	if(x >=0 && x < nx && y >=0 && y < ny) return 1;
	return 0;
}


//Estruturas:

//Estrutura que representa uma possível resposta no espaço de busca.
struct acmPoint{
        double vnorm; // valor normalizado da votação de circularidade do ponto
	double vhistograma; // score com base na comparação de histograma
        double score_final; // score final com base nos dois scores anteriores
	
	int rad,cx,cy; // raio, centro (cx,cy)

        bool inic; //flag que indica se a estrutura já foi inicializada com os parametros ou não
	
	acmPoint(){inic = false;}
	
	acmPoint(int rad_, int cx_,int cy_, double vnorm_ = 0){
		rad = rad_; cx = cx_; cy = cy_;
		score_final = vnorm = vnorm_;
		vhistograma = 0.0;
		inic = true;
	}

	void calculaScore(){
		if(rad < 5)
			score_final =  0.4 * vnorm + 0.6 * vhistograma;
		else
			score_final =  0.2 * vnorm + 0.8 * vhistograma;
	}
	
	inline bool operator< (const acmPoint &p) const{
		return (score_final < p.score_final);
	}
};


// Protótipos:
int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r,
	    pair<double,double> thetaRange = pair<double,double>(0.0,2*pi),int inc=1);

void fillPreCalcDiscP();

double calcVNorm(int **m,int x,int y,int nx,int ny,int rad);

void houghC1(Mat &frame, int minr, int maxr , int cannyt, vector<acmPoint> &output,
 		unsigned int thNCirc = 1, double thVNorm = -1);

acmPoint findBall(Mat &roiImg, Mat &roiImgGray, int minr, int maxr,
                  bool filtraHistograma = false, double thScore = -1);

void trackBall(Mat &frame,const Rect &ROIat,const acmPoint &currentBall, acmPoint &newBall, Rect &newROI,
               int minr=0, int maxr=0, bool firstTime = false, bool filtraHistograma = false);

void setOptHistogram(Mat &imagem, acmPoint &ponto);
void calcHistogram(Mat &imagem, vector<acmPoint> &pontos);
void calcHistogramScore(const Mat &imagem, acmPoint &ponto);

void teste_histograma_img();
void teste_discretizazao();
void teste_gradientes();
void teste_acha_circulos();
