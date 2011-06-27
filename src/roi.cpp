#include "main.h"

//============================== Achar bola ====================================
acmPoint findBall(Mat &roiImg, Mat &roiImgGray, int minr, int maxr, bool filtraHistograma, double thScore){
	int thNCirc = 10;
	int thCanny = 100;
	
	vector<acmPoint> circles;	
	houghC1(roiImgGray,minr,maxr,thCanny,circles,thNCirc,thScore);
	

	if(filtraHistograma) calcula_histograma(roiImg, circles);
	
	/*vector<acmPoint>::iterator it;
	for(it = circles.begin(); it != circles.end(); ++it) {
		it->calculaScore();
	}
	sort(circles.begin(), circles.end());*/

	return circles[0];
	
}

//============================== Ajuste da ROI ====================================
void trackBall(const Mat &frameAtual, Mat &frameFuturo,const Rect &ROIat,const acmPoint &ballAtual, acmPoint &newBall, Rect &newROI,
int minr, int maxr, bool firstTime, bool filtraHistograma){
	int deltaR=1;
	int limMinR = 2;
	double roiScale=3;
	Rect roiRect;
	Mat frameFuturoGray;
	
        cvtColor(frameFuturo, frameFuturoGray, CV_BGR2GRAY);
        GaussianBlur(frameFuturoGray, frameFuturoGray, Size(5,5), 1.5, 1.5);

	int limMaxR = min(frameFuturoGray.rows/2,frameFuturoGray.cols/2);

	if(firstTime){
		roiRect = Rect(0,0,frameFuturoGray.cols-1,frameFuturoGray.rows-1);
		
	}else{
		roiRect = ROIat;
		if(ballAtual.inic){
			minr = max(limMinR,ballAtual.rad-deltaR);
			maxr = min(limMaxR,ballAtual.rad+deltaR);
		}
	}
	
	Mat roiImgGray  = frameFuturoGray(roiRect);
	Mat roiImg	= frameFuturo(roiRect);
	newBall = findBall(roiImg,roiImgGray,minr,maxr,filtraHistograma);
	newBall.cx += roiRect.x; 
	newBall.cy += roiRect.y;

	Point center(newBall.cx,newBall.cy);
	int rad = newBall.rad;
	
	newROI = Rect(center.x - roiScale*rad, center.y - roiScale*rad,2*roiScale*rad,2*roiScale*rad);
	newROI = newROI & Rect(0,0,frameFuturoGray.cols-1,frameFuturoGray.rows-1);
	printf("vnorm = %lf\nvhist = %lf\nscore_final=%lf\nrad = %d\n",newBall.vnorm,newBall.vhistograma,
									newBall.score_final, newBall.rad);
	
}
