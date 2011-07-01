#include "main.h"

void video_mode(char file[]) {

    int globalMaxR,globalMinR; 
    double thRestart;
    
    printf("Digite minr maxr e thRestart ou -1 para valores padrões\n");
    scanf(" %d",&globalMinR);
    if(globalMinR == -1){
    	globalMinR = 5;
    	globalMaxR = 30;
    	thRestart = 0.48;
    }else{
    	scanf(" %d %lf",&globalMaxR,&thRestart);
    }
    
    
    VideoCapture cap(file); 
    if(!cap.isOpened()) 
        exit(1);

    Mat gray,frame,gray2,grayAnt;
    
    vector<acmPoint> circles;
    bool firstFrame = true;
    Rect roiRect,newRoiRect;
    acmPoint ballAt,newBall;

    int countRestart = 0;
    
    Rect inicRoi(130,175,25,25);
    
    //Flags
    bool fAcertou = false;
    bool fPause = true;

    char ch;
    
    while(1)
    {
        
        if(!cap.grab()) break;        
        cap.retrieve(frame,0);
        
 
        if(firstFrame){
        	trackBall(grayAnt,frame,inicRoi,acmPoint(),newBall,newRoiRect,globalMinR,globalMaxR,true, fAcertou);
        }else{
        	
        	if(ballAt.score_final < thRestart){
        		countRestart++;        		
			}else countRestart = 0;
			

			if(countRestart == 5){
				trackBall(grayAnt,frame,roiRect,ballAt,newBall,newRoiRect,globalMinR,globalMaxR,true, fAcertou);
				countRestart = 0;
		    }
			else{
		    		trackBall(grayAnt,frame,roiRect,ballAt,newBall,newRoiRect, 0, 0, false, fAcertou);
		    }
        	
        }
        
		Mat framecopy = frame.clone();

		circle(frame,Point(newBall.cx,newBall.cy),newBall.rad,Scalar(255,0,0),2);
		rectangle(frame,Point(newRoiRect.x,newRoiRect.y),Point(newRoiRect.x + newRoiRect.width - 1,
		newRoiRect.y + newRoiRect.height -1),Scalar(0,255,0));
		

		imshow(file, frame);
		
		printf("cx = %d cy = %d rad = %d\n",newBall.cx,newBall.cy,newBall.rad);

		if(fAcertou && !fPause) ch = waitKey(30);
		else ch = waitKey();


        if(ch >= 0){
			if(ch == 'h'){
				define_histograma_otimo(framecopy, newBall);
				fAcertou = true;
			}

			if(ch == 'p') {
				if(fPause) fPause = false;
				else fPause = true;
			}
			
			if(ch == 'w'){
				char nomeArq[128];
				printf("Digite o nome para gravar\n");
				scanf(" %s",nomeArq);
				
				imwrite(nomeArq,framecopy);
			}
	
			if(ch == 'q') break;
        }
        
        ballAt = newBall;
        roiRect = newRoiRect;
		
		
	
		grayAnt = gray.clone();
		firstFrame = false;		
    }
}

void image_mode(char file[]) {
	Mat gray,
	    img = imread(file,1);
	acmPoint ball;
	Rect roiRect;
	
	trackBall(Mat(),img,Rect(110,105,25,25),acmPoint(),ball,roiRect,5,30,false);

	circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
	rectangle(img,Point(roiRect.x,roiRect.y),Point(roiRect.x + roiRect.width - 1,roiRect.y + roiRect.height -1),Scalar(0,255,0));
	
	imshow(file,img);
	waitKey();
}

char arquivo[1024];
int mode = -1; //0 = video, 1 = imagem, 2 = testes

void parse_options(int argc, char* argv[]) {
    int option_index, option;

    static struct option long_options[] = {
        {"video",	required_argument, 0, 'v'},
        {"image",	required_argument, 0, 'i'},
        {"tests",	no_argument	 , 0, 't'},
        {0,		0,                 0,  0 }};

    while(1)
    {
        option_index = 0;
        option = getopt_long(argc, argv, "v:i:t", long_options, &option_index);

        if(option == -1) break;

        switch(option) {
            case 'v':
		strncpy(arquivo, optarg, 1020);
		mode = 0;
                break;
            case 'i':
		strncpy(arquivo, optarg, 1020);
		mode = 1;
                break;
            case 't':
		mode = 2;
                break;
	}
    }
}

int main(int argc, char* argv[]){

    parse_options(argc, argv);

    if(mode == 0) {
        video_mode(arquivo);
    }
    else if(mode == 1) {
        image_mode(arquivo);
    }
    else if(mode == 2) {
		int teste;
		printf("Digite o número do teste (ou -1 para todos):\n");
		printf("1 - histograma\n");
		printf("2 - discretizacao\n");
		printf("3 - gradientes\n");
		printf("4 - achar circulos\n");
		
		scanf("%d", &teste);

		if(teste == 1 || teste == -1) teste_histograma_img();
        if(teste == 2 || teste == -1) teste_discretizazao();
        if(teste == 3 || teste == -1) teste_gradientes();
        if(teste == 4 || teste == -1) teste_acha_circulos();
    }
    else printf("Você precisa escolher um dos modos (video, image ou tests) para executar o programa.\n");

 
    return 0;
}


