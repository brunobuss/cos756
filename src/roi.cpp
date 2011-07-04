#include "main.h"

//============================== Achar bola ====================================
acmPoint findBall(Mat &roiImg, Mat &roiImgGray, int minr, int maxr, bool filtraHistograma, double thScore){
	int thNCirc = 10;
	int thCanny = 100;
	
	vector<acmPoint> circles;	
	houghC1(roiImgGray,minr,maxr,thCanny,circles,thNCirc,thScore);
	
        if(filtraHistograma) calcHistogram(roiImg, circles);
	
	/*vector<acmPoint>::iterator it;
	for(it = circles.begin(); it != circles.end(); ++it) {
		it->calculaScore();
	}
	sort(circles.begin(), circles.end());*/

	return circles[0];
	
}

//============================== Ajuste da ROI ====================================
void trackBall(Mat &frame,const Rect &ROIat,const acmPoint &currentBall, acmPoint &newBall, Rect &newROI,
int minr, int maxr, bool firstTime, bool filtraHistograma){
	int deltaR=1;
	int limMinR = 2;
	double roiScale=4;
	Rect roiRect;
        Mat frameGray;
	
	
        cvtColor(frame, frameGray, CV_BGR2GRAY);
        GaussianBlur(frameGray, frameGray, Size(5,5), 1.5, 1.5);

        int limMaxR = min(frameGray.rows/2,frameGray.cols/2);

	if(firstTime){
                roiRect = Rect(0,0,frameGray.cols,frameGray.rows);
		
	}else{
		roiRect = ROIat;
                if(currentBall.inic){
                        minr = max(limMinR,currentBall.rad-deltaR);
                        maxr = min(limMaxR,currentBall.rad+deltaR);
		}
	}
	
        Mat roiImgGray  = frameGray(roiRect);
        Mat roiImg	= frame(roiRect);
	newBall = findBall(roiImg,roiImgGray,minr,maxr,filtraHistograma);
	newBall.cx += roiRect.x; 
	newBall.cy += roiRect.y;

	Point center(newBall.cx,newBall.cy);
	int rad = newBall.rad;
	
	newROI = Rect(center.x - roiScale*rad -1, center.y - roiScale*rad -1,2*roiScale*rad + 1,2*roiScale*rad + 1);
        newROI = newROI & Rect(0,0,frameGray.cols,frameGray.rows);
	printf("vnorm = %lf\nvhist = %lf\nscore_final=%lf\nrad = %d\n",newBall.vnorm,newBall.vhistograma,
									newBall.score_final, newBall.rad);
	
}
