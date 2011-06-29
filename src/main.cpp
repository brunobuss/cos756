#include "main.h"

int main(int argc, char* argv[]){

    teste_histograma_img();
    teste_discretizazao();
    teste_gradientes();
    teste_acha_circulos();
 
    return 0; //Temporário :P


    VideoCapture cap(/*"./arquivoTeste/pass.flv"*/   "./arquivoTeste/embx.flv"); 
    if(!cap.isOpened()) 
        return -1;

    Mat gray,frame,gray2,grayAnt;
    
    vector<acmPoint> circles;
    bool firstFrame = true;
    Rect roiRect,newRoiRect;
    acmPoint ballAt,newBall;
    
    double thRestart = 0.22; //0.35;
    int countRestart = 0;
    
    int globalMinR=5,globalMaxR=30;
    Rect inicRoi(130,175,25,25);

    bool fAcertou = false;
    char ch;
    
    
    for(;;)
    {
                        
        cap >> frame;
 
        if(firstFrame){
        	trackBall(grayAnt,frame,inicRoi,acmPoint(),newBall,newRoiRect,globalMinR,globalMaxR,false, fAcertou);
        }else{
        	
        	if(ballAt.score_final < thRestart){
        		countRestart++;        		
			}else
				countRestart = 0;
			
			if(countRestart == 10){
				trackBall(grayAnt,frame,roiRect,ballAt,newBall,newRoiRect,globalMinR,globalMaxR,true, fAcertou);
				countRestart = 0;
        	}else{
        		trackBall(grayAnt,frame,roiRect,ballAt,newBall,newRoiRect, 0, 0, false, fAcertou);
        	}
        	
        }
        
        /*
		houghC1(gray,5,30,80,circles,1);
		
		for(unsigned int i=0; i<circles.size();i++){
			printf("%lf %d %d %d\n",circles[i].vnorm,circles[i].rad,circles[i].cx,circles[i].cy);
			circle(frame,Point(circles[i].cx,circles[i].cy),circles[i].rad,Scalar(255,0,0),3,8,0);
		}
		*/
		

		Mat framefu = frame.clone();

		circle(frame,Point(newBall.cx,newBall.cy),newBall.rad,Scalar(255,0,0),2);

		rectangle(frame,Point(newRoiRect.x,newRoiRect.y),Point(newRoiRect.x + newRoiRect.width - 1,
		newRoiRect.y + newRoiRect.height -1),Scalar(0,255,0));
		
        imshow("video", frame);
        
		if(fAcertou /*&& newBall.vhistograma > 0.0*/) ch = waitKey(0);
		else ch = waitKey();

        if(ch >= 0){
	    	/*if(ch == 'a'){         		
	    		imwrite("testeImg7.jpeg",gray2);
	    		imwrite("verificador7.jpeg",frame);
	    	}*/
			if(ch == 's'){
				define_histograma_otimo(framefu, newBall);
				fAcertou = true;
			}
		    if(ch == 'q')
		    	break;
        }
        
        ballAt = newBall;
        roiRect = newRoiRect;
		
	
		grayAnt = gray.clone();
		firstFrame = false;		
    }
    
    return 0;




/*	Mat gray,
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
	waitKey();*/
}
