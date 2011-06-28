// Arquivo de teste para gradiente

#include "cv.h"
#include "highgui.h"
#include<stdio.h>
#include<math.h>
#include<time.h>

using namespace std;
using namespace cv;

double pi = acos(-1.0);


void desenhaGrad(Mat &img,Mat &Ix,Mat &Iy,Point p1){
	
	double scale=100;
	double x2 = p1.x + Ix.at<double>(p1.y,p1.x) * scale;
	double y2 = p1.y + Iy.at<double>(p1.y,p1.x) * scale;
	
	printf("%lf %lf\n",x2,y2);
	 
	Point p2(x2,y2);
	
	line(img,p1,p2,Scalar(255,0,0));
	circle(img,p2,1,Scalar(0,255,0));
}

int main(){
	int cannyt = 100;
	Mat img = imread("testeImg2.jpeg",1),gray,Ix,Iy,edges;	
//	img = img(Range(160,200),Range(130,170));
	cvtColor(img,gray,CV_BGR2GRAY);
	Canny(gray,edges,cannyt,cannyt,3,true);
	
	GaussianBlur(gray, gray, Size(5,5), 1.5, 1.5);	
	printf("%d %d\n",img.rows,img.cols);
	
	Sobel(gray,Ix,DataType<double>::depth,1,0,3,((double)1)/(8*255)); // [-0.5 - 0.5]
	Sobel(gray,Iy,DataType<double>::depth,0,1,3,((double)1)/(8*255)); // [-0.5 - 0.5]
	
	
	
	srand(time(NULL));

	
	int i,j;
	
	for(i=0;i<edges.rows;i++){
		for(j=0;j<edges.cols;j++){
			if(edges.at<uchar>(i,j) > 0 && rand()%10 < 1){
				desenhaGrad(img,Ix,Iy,Point(j,i));
			}
		}
	}
	imshow("",edges);
	imshow("img",img);
	imwrite("gradientes.jpeg",img);
	waitKey();
	
	
}
