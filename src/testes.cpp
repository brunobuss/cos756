
//TODO criar imagens para testes histograma
void teste_histograma_img(){
	Mat img1,img2,gray1,gray2;
	
	
	img1 = imread("",1);
	cvtColor(img1,gray1,CV_BGR2GRAY);
	GaussianBlur(gray1, gray1, Size(5,5), 1.5, 1.5);
	
	
	img2 = imread("",1);
	cvtColor(img2,gray2,CV_BGR2GRAY);
	GaussianBlur(gray2, gray2, Size(5,5), 1.5, 1.5);
	
	int minr = 5, maxr = 30;
	double thScore = -1;
	
	acmPoint b1 = findBall(img1, gray1,minr,maxr,false,thScore);
	
	define_histograma_otimo(img1, b1);
	
	acmPoint b2 = findBall(img2, gray2,minr,maxr,false,thScore);
	
	//TODO comparar histogramas
}


void teste_discretizazao(){
	int n=40,m=40,i,j;
	
	Mat img(n,m,DataType<uchar>::type);
	
	int **mat = new int*[n];
	for(i=0;i<n;i++)
		mat[i] = new int[m];
	
	memset(&mat[0][0],0,sizeof(mat));
	img = Scalar(0);
	
	while(1){
	
		int cx,cy,rad;
		pair<double,double> thetaRange(0.0,2*pi);
	
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
	
	
		imshow("",img);
		char ch = waitKey();
		if(ch == 'q') break;
	}
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
	int cannyt = 100;
	Mat img = imread("testeImg2.jpeg",1),gray,Ix,Iy,edges;	
	cvtColor(img,gray,CV_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5,5), 1.5, 1.5);
	
	Canny(gray,edges,cannyt,cannyt,3,true);
	
		
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


// Desenha ocorrencias circulares de hough na tela
void teste_acha_circulos(){
	Mat img = imread("./arquivoTeste/testeImg.jpeg",1);
	Mat gray = cvtcolor(img,gray,CV_BRG2GRAY);
	GaussianBlur(gray,gray,Size(5,5),1.5,1.5);
	
	vector<acmPoint> circles;	
	int minr = 5,maxr = 30, thCanny = 100, thNCirc = 10;
	double thScore = -1;
	
	houghC1(gray,minr,maxr,thCanny,circles,thNCirc,thScore);
	
	unsigned int i;
	for(i=0;i<circles.size();i++){
		acmPoint ball = circles[i];
		
		circle(img,Point(ball.cx,ball.cy),ball.rad,Scalar(255,0,0),2);
		printf("cx = %lf cy = %lf rad = %lf vnorm = %lf\n",ball.cx,ball.cy,ball.rad,ball.vnorm);  
	}
	
	waitKey();
}
