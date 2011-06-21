#include "main.h"

int main(int argc, char* argv[]){
	Mat gray,
	    img = imread("./arquivoTeste/testeImg6.jpeg",1);
	acmPoint ball;
	Rect roiRect;

	//Trabalhando apenas com a imagem em preto e branco.
	cvtColor(img,gray,CV_BGR2GRAY);

	//Passando o filtro gaussiano do openCV
	GaussianBlur(gray, gray, Size(5,5), 1.5, 1.5);
	
	printf("%d %d\n",img.rows,img.cols);
	
	trackBall(Mat(),gray,Rect(110,105,25,25),acmPoint(),ball,roiRect,5,30,false);
		
	//rectangle(img,Point(110,105),Point(135,130),Scalar(0,0,255));
	
	circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
	rectangle(img,Point(roiRect.x,roiRect.y),Point(roiRect.x + roiRect.width - 1,roiRect.y + roiRect.height -1),Scalar(0,255,0));
	
	imshow("img",img);
	waitKey();
}
