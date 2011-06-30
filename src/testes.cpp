#include "main.h"

//TODO criar imagens para testes histograma
void teste_histograma_img(){
	printf("\n ============ Teste Histograma ====================== \n");
	
	Mat img1,img2,gray1,gray2;
	char nomeArq1[128],nomeArq2[128],ch;
	
	printf("Usar arquivos default?\n");
	scanf(" %c",&ch);
	if(ch == 's'){
		strcpy(nomeArq1,"./arquivoTeste/testeImg11.jpeg");
		strcpy(nomeArq2,"./arquivoTeste/testeImg11.jpeg");
	}else{
		printf("Digite o nome dos dois arquivos\n");
		scanf(" %s",nomeArq1);
		scanf(" %s",nomeArq2);
	}
	
	img1 = imread(nomeArq1,1);
	cvtColor(img1,gray1,CV_BGR2GRAY);
	GaussianBlur(gray1, gray1, Size(5,5), 1.5, 1.5);
	
	
	img2 = imread(nomeArq2,1);
	cvtColor(img2,gray2,CV_BGR2GRAY);
	GaussianBlur(gray2, gray2, Size(5,5), 1.5, 1.5);
	
	int minr = 5, maxr = 30;
	double thScore = -1;
	
	acmPoint b1 = findBall(img1, gray1,minr,maxr,false,thScore);
	
	define_histograma_otimo(img1, b1);
	
	for(int i = 0; i < NIVEL_POR_COR; i++) {
		for(int j = 0; j < NIVEL_POR_COR; j++) {
			for(int k = 0; k < NIVEL_POR_COR; k++) {
				printf("(B=%d,G=%d,R=%d) = %lf\n",i,j,k,histograma_otimo[i][j][k]);
			}
		}
	}

	
	acmPoint b2 = findBall(img2, gray2,minr,maxr,false,thScore);
	
	/*
	printf("============================\n\n");
	
	define_histograma_otimo(img2, b2);
	
	for(int i = 0; i < NIVEL_POR_COR; i++) {
		for(int j = 0; j < NIVEL_POR_COR; j++) {
			for(int k = 0; k < NIVEL_POR_COR; k++) {
				printf("(B=%d,G=%d,R=%d) = %lf\n",i,j,k,histograma_otimo[i][j][k]);
			}
		}
	}
	*/
	
	calcula_score_histograma(img2,b2);
	printf("vhistogram diff = %lf\n",b2.vhistograma);
	
	
	circle(img1,Point(b1.cx,b1.cy),b1.rad,Scalar(255,0,0),2);
	circle(img2,Point(b2.cx,b2.cy),b2.rad,Scalar(0,255,0),2);
	imshow("img1",img1);
	imshow("img2",img2);
	waitKey();
	
	//TODO comparar histogramas
}


void teste_discretizazao(){
	printf("\n ============ Teste Discretização Cicrular ====================== \n");
	
	int n,m,i,j;
	char ch;
	
	printf("Digite as dimensões da matriz (linha e coluna) \n");
	scanf("%d %d",&n,&m);
	
	Mat img(n,m,DataType<uchar>::type);
	
	int **mat = new int*[n];
	for(i=0;i<n;i++)
		mat[i] = new int[m];
	
	memset(&mat[0][0],0,sizeof(mat));
	img = Scalar(0);
	
	while(1){
	
		int cx,cy,rad;
		printf("Digite a variação angular (theta0 theta1) ou (0 0) para (0 2pi)\n");

		double theta0,theta1;		
		scanf("%lf %lf",&theta0,&theta1);
		if(theta0 == 0.0 && theta1 == 0.0){
			theta1 = 2*pi;
		}
		pair<double,double> thetaRange(theta0,theta1);
		
		printf("Digite cx cy e rad\n");
		scanf("%d %d %d",&cx,&cy,&rad);
		incCirc(mat,n,m,cx,cy,rad,thetaRange);
		
		
		for(i=0;i<n;i++){
			for(j=0;j<m;j++){
			
				if(mat[i][j] > 0)
					img.at<uchar>(i,j) = 255;
			
				printf("%d ",mat[i][j]);
			}
			printf("\n");
		}
		printf("\n");
	
	
		imshow("img",img);waitKey();
		printf("outra iteração? (s/n)\n");
		scanf(" %c",&ch);
		
		if(ch == 'n') break;
	}
	cvDestroyWindow("img");
}

void desenhaGrad(Mat &img,Mat &Ix,Mat &Iy,Point p1){
	
	double scale=100;
	double x2 = p1.x + Ix.at<double>(p1.y,p1.x) * scale;
	double y2 = p1.y + Iy.at<double>(p1.y,p1.x) * scale;
	
	printf("%lf %lf\n",x2,y2);
	 
	Point p2(x2,y2);
	
	line(img,p1,p2,Scalar(255,0,0));
	circle(img,p2,1,Scalar(0,255,0));
}

void teste_gradientes(){
	printf("\n ============ Teste Gradientes ====================== \n");
	
	int cannyt = 100;
	char ch,nomeArq[128];
	
	while(1){
	
		printf("Usar aquivo default?(s/n)\n");
		scanf(" %c",&ch);
		if(ch == 's'){
			strcpy(nomeArq,"./arquivoTeste/testeImg2.jpeg");
		}else{
			printf("Digite o nome do arquivo\n");
			scanf(" %s",nomeArq);
		}	
	
		Mat img = imread(nomeArq,1),gray,Ix,Iy,edges;	
		cvtColor(img,gray,CV_BGR2GRAY);
		GaussianBlur(gray, gray, Size(5,5), 1.5, 1.5);
	
		Canny(gray,edges,cannyt,cannyt,3,true);
	
		
		printf("nrow = %d ncol = %d\n",img.rows,img.cols);
	
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
		imshow("edges",edges);
		imshow("img",img);waitKey();
		imwrite("gradientes.jpeg",img);
		printf("outra iteração?(s/n)\n");
		scanf(" %c",&ch);
	
		if(ch == 'n')
			break;
	}
}


// Desenha ocorrencias circulares de hough na tela
void teste_acha_circulos(){
	printf("\n ============ Teste Achar Cirulos ====================== \n");
	char nomeArq[128],ch;
	
	while(1){
		printf("Usar aquivo default?(s/n)\n");
		scanf(" %c",&ch);
		if(ch == 's'){
			strcpy(nomeArq,"./arquivoTeste/testeImg.jpeg");
		}else{
			printf("Digite o nome do arquivo\n");
			scanf(" %s",nomeArq);
		}
		
		Mat img = imread(nomeArq,1);
		Mat gray;

		cvtColor(img,gray,CV_BGR2GRAY);
		GaussianBlur(gray,gray,Size(5,5),1.5,1.5);
	
		vector<acmPoint> circles;	
		int minr = 5,maxr = 30, thCanny = 100, thNCirc = 10;
		double thScore = -1;
	
		houghC1(gray,minr,maxr,thCanny,circles,thNCirc,thScore);
	
		unsigned int i;
		for(i=0;i<circles.size();i++){
			acmPoint ball = circles[i];
		
			circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
			printf("cx = %d cy = %d rad = %d vnorm = %lf\n",ball.cx,ball.cy,ball.rad,ball.vnorm);  
		}
	
		imshow("",img);waitKey();
		
		printf("outra iteração?(s/n)\n");
		scanf(" %c",&ch);
	
		if(ch == 'n')
			break;
	}
	
}
