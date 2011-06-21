#include "main.h"

//============================== Achar bola ====================================
acmPoint findBall(Mat &roiImg, int minr, int maxr, double thScore){
	int thNCirc = 10;
	int thCanny = 100;
	
	vector<acmPoint> circles;	
	houghC1(roiImg,minr,maxr,thCanny,circles,thNCirc,thScore);
	
	//TODO fazer análise de histograma para filtrar aqui
		
	return circles[0];
	
}

//============================== Ajuste da ROI ====================================
void trackBall(const Mat &imgAnt, Mat &imgAt,const Rect &ROIat,const acmPoint &ballAnt, acmPoint &newBall, Rect &newROI,
int minr, int maxr, bool firstTime){
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
