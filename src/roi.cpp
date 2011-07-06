#include "main.h"

/* Função que busca uma "bola" em 'roiImg'/'roiImgGray'.
   Primeiro calcula o score circular utilizando a transformada circular e depois
   calcula o score de histograma para formar um score final e pega o circula com
   o maior score encontrado.
 */
acmPoint findBall(Mat &roiImg, Mat &roiImgGray, int minr, int maxr, bool filtraHistograma, double thScore){
	int thNCirc = 10;
	int thCanny = 100;
	
	vector<acmPoint> circles;	

        //Transformada circular, para encontrar elementos com alta
        //circularidada na iamgem/
	houghC1(roiImgGray,minr,maxr,thCanny,circles,thNCirc,thScore);
	
        //Se o filtro por histograma estiver habilitado,
        //então calcula o score de histograma.
        if(filtraHistograma) calcHistogram(roiImg, circles);

        //Retorna o melhor candidato.
	return circles[0];
	
}

/* Função principal de localização da bola.
   Encontra a região circular mais parecida com um circulo em 'frame', mais especificamente na região
   'ROIat'.

   Retorna o novo ROI em 'newROI' e a nova "bola" em 'newBall'.
 */
void trackBall(Mat &frame,const Rect &ROIat,const acmPoint &currentBall, acmPoint &newBall, Rect &newROI,
int minr, int maxr, bool firstTime, bool filtraHistograma){
	int deltaR=1;
	int limMinR = 2;
	double roiScale=4;
	Rect roiRect;
        Mat frameGray;
	
        //Passa a imagem para tons de cinza.
        cvtColor(frame, frameGray, CV_BGR2GRAY);
        //Gaussian blur para remover ruídos e suavizar a imagem.
        GaussianBlur(frameGray, frameGray, Size(5,5), 1.5, 1.5);

        int limMaxR = min(frameGray.rows/2,frameGray.cols/2);

        if(firstTime){ //Se for a primeira vez, definimos o ROI como a imagem toda.
                roiRect = Rect(0,0,frameGray.cols,frameGray.rows);
		
	}else{
                roiRect = ROIat; //Se não, podemos utilizar o ROI anterior.
                if(currentBall.inic){ //Se temos uma bola atualmente identificado,
                                      //então vamos procupar por algo com um raio próximo
                        minr = max(limMinR,currentBall.rad-deltaR);
                        maxr = min(limMaxR,currentBall.rad+deltaR);
		}
	}
	

        Mat roiImgGray  = frameGray(roiRect);
        Mat roiImg	= frame(roiRect);

        //Encontramos a nova "bola":
	newBall = findBall(roiImg,roiImgGray,minr,maxr,filtraHistograma);

        //Ajustando coordenadas da bola em razão do ROI utilizado.
	newBall.cx += roiRect.x; 
	newBall.cy += roiRect.y;

	Point center(newBall.cx,newBall.cy);
	int rad = newBall.rad;
	
        //Define novo ROI.
	newROI = Rect(center.x - roiScale*rad -1, center.y - roiScale*rad -1,2*roiScale*rad + 1,2*roiScale*rad + 1);
        newROI = newROI & Rect(0,0,frameGray.cols,frameGray.rows);
        printf("vnorm = %lf\nvhist = %lf\nscore_final=%lf\nrad = %d\n",newBall.scoreCircular,newBall.scoreHistogram,
                                                                        newBall.scoreFinal, newBall.rad);
}
