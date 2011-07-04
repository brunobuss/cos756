#include "main.h"


/* O programa opera em modo de vídeo,
   abrindo o vídeo do arquivo file.
 */
void videoMode(char file[]) {

    int globalMaxR,globalMinR; 
    double thRestart;
    
    // Definindo parametros para o programa...
    printf("Digite minr maxr e thRestart ou -1 para valores padrões\n");
    scanf(" %d",&globalMinR);
    if(globalMinR == -1){
    	globalMinR = 5;
    	globalMaxR = 30;
    	thRestart = 0.48;
    }else{
    	scanf(" %d %lf",&globalMaxR,&thRestart);
    }
    
    //Tenta abrir o arquivo de vídeo, caso de erro sai com código 1...
    VideoCapture cap(file); 
    if(!cap.isOpened()) 
        exit(1);

    Mat frame; //Frame atual

    Rect roiRect,newRoiRect; //Retangulos que definem a ROI de busca.

    acmPoint lastBall,newBall; //Pontos que definem onde a bola estava no frame anterior e onde está agora.


    int countRestart = 0; //Contador que determina quando o algoritmo deve restartar e reiniciar a busca pela bola.
    
    //Flags de controle
    bool firstFrame = true; //Define se é o primeiro frame, pois necessita de um tratamente especial.
    bool fAcertou = false; //Define se acertou o histograma
    bool fPause = true; //Define se o programa está em estado de "pause"
    

    while(1)
    {
        
        if(!cap.grab()) break;        
        cap.retrieve(frame,0);
        
 
        if(firstFrame){
            trackBall(frame,Rect(),acmPoint(),newBall,newRoiRect,globalMinR,globalMaxR,true, fAcertou);
            firstFrame = false;
        }else{
        	
            /*Se o score da bola encontrada estiver abaixo de nosso treshhold, então é um mal sinal... caso contrário
             reiniciamos o contador. */
            if(lastBall.score_final < thRestart) countRestart++;
            else countRestart = 0;

            // Se diversas medições anteriores estavam ruins, então vamos abrir a área de busca.
            if(countRestart == 5){
                trackBall(frame,roiRect,lastBall,newBall,newRoiRect,globalMinR,globalMaxR,true, fAcertou);
                countRestart = 0;
            }else{
                trackBall(frame,roiRect,lastBall,newBall,newRoiRect, 0, 0, false, fAcertou);
            }
        	
        }
        
        //Variável auxiliar, pois iremos "sujar" o frame daqui em diante.
        Mat framecopy = frame.clone();

        //Desenhando a ROI e a bola encontrada.
        circle(frame,Point(newBall.cx,newBall.cy),newBall.rad,Scalar(255,0,0),2);
        rectangle(frame,Point(newRoiRect.x,newRoiRect.y),Point(newRoiRect.x + newRoiRect.width - 1,
        newRoiRect.y + newRoiRect.height -1),Scalar(0,255,0));

        //Mostrando tudo.
        imshow(file, frame);

        //Informações sobre a bola encontrada.
        printf("cx = %d cy = %d rad = %d\n",newBall.cx,newBall.cy,newBall.rad);

        //Parte que cuida da interação do usuário com o programa.
        char ch;
        if(fAcertou && !fPause) ch = waitKey(30);
        else ch = waitKey();


        if(ch >= 0){
            if(ch == 'h'){ //'h' define a região circular atual como a que tem o histograma ótimo.
                setOptHistogram(framecopy, newBall);
                fAcertou = true;
            }

            if(ch == 'p') { //'p' alterna o programa em modo pausado (frame a frame) ou continuo.
                if(fPause) fPause = false;
                else fPause = true;
            }

            if(ch == 'w'){ //'w' salva o frame em um arquivo especificado.
                char nomeArq[128];
                printf("Digite o nome para gravar\n");
                scanf(" %s",nomeArq);

                imwrite(nomeArq,framecopy);
            }

            if(ch == 'q') break; //'q' termina o programa.
        }
        
        //Salvando as informações para o próximo frame.
        lastBall = newBall;
        roiRect = newRoiRect;
    }
}


/* O programa opera em modo de imagem,
   lendo a figura do arquivo file.
 */
void imageMode(char file[]) {
        Mat img = imread(file,1); //Imagem a ser utilizada.
        acmPoint ball; //Bola encontrada
        Rect roiRect; //ROI identificado
	
        trackBall(img,Rect(110,105,25,25),acmPoint(),ball,roiRect,5,30,false);

        //Desenhando a ROI e a bola encontrada.
	circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
	rectangle(img,Point(roiRect.x,roiRect.y),Point(roiRect.x + roiRect.width - 1,roiRect.y + roiRect.height -1),Scalar(0,255,0));
	
        //Mostrando tudo.
	imshow(file,img);
	waitKey();
}

char arquivo[1024]; //Nome do arquivo a ser aberto.
int mode = -1; //0 = video, 1 = imagem, 2 = testes


/* Função para tratar opções passadas via linha de comando */
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
        videoMode(arquivo);
    }
    else if(mode == 1) {
        imageMode(arquivo);
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


