#include "main.h"

vector< vector< vector <double> > > histograma_otimo;

void inicializa_histograma(vector< vector< vector <double> > > &histograma) {

	histograma.clear();

	for(int i = 0; i < NIVEL_POR_COR; i++) {

		vector< vector<double> > vect1;
		histograma.push_back(vect1);

		for(int j = 0; j < NIVEL_POR_COR; j++) {

			vector<double> vect2;
			histograma[i].push_back(vect2);

			for(int k = 0; k < NIVEL_POR_COR; k++) {
				histograma[i][j].push_back(0.0);
			}
		}
	}
}

void normaliza_histograma(vector< vector< vector <double> > > &histograma, int total) {

	for(int i = 0; i < NIVEL_POR_COR; i++) {
		for(int j = 0; j < NIVEL_POR_COR; j++) {
			for(int k = 0; k < NIVEL_POR_COR; k++) {
				histograma[i][j][k] /= total;
			}
		}
	}
}


void define_histograma_otimo(Mat &imagem, acmPoint &ponto) {

	int total_pontos = 0;

	inicializa_histograma(histograma_otimo);
	for(int i = ponto.cx - ponto.rad; i <= ponto.cx + ponto.rad; ++i) {
		for(int j = ponto.cy - ponto.rad; j <= ponto.cy + ponto.rad; ++j) {
		
			if( ((ponto.cx - i) * (ponto.cx - i) + (ponto.cy - j) * (ponto.cy - j)) > (ponto.rad * ponto.rad) ) continue;
			if(!INSIDE(i, j, imagem.rows, imagem.cols)) continue;
			
			total_pontos++;
			

			int B = imagem.at<Vec3b>(j, i)[0]/FATOR;
			int G = imagem.at<Vec3b>(j, i)[1]/FATOR;
			int R = imagem.at<Vec3b>(j, i)[2]/FATOR;
			
//			imagem.at<Vec3b>(j,i)[0] = imagem.at<Vec3b>(j,i)[1] = 0;
//			imagem.at<Vec3b>(j,i)[2] = 255;

			histograma_otimo[B][G][R]++;
		}
	}
	normaliza_histograma(histograma_otimo, total_pontos);
}

void calcula_score_histograma(const Mat &imagem, acmPoint &ponto) {

	vector< vector< vector <double> > > histograma_ponto;
	int total_pontos = 0;

	//Calcula o histograma do circulo inicialmente.
	inicializa_histograma(histograma_ponto);
	for(int i = ponto.cx - ponto.rad; i <= ponto.cx + ponto.rad; ++i) {
		for(int j = ponto.cy - ponto.rad; j <= ponto.cy + ponto.rad; ++j) {
		
			if( ((ponto.cx - i) * (ponto.cx - i) + (ponto.cy - j) * (ponto.cy - j)) > (ponto.rad * ponto.rad) ) continue;
			if(!INSIDE(i, j, imagem.rows, imagem.cols)) continue;

			total_pontos++;

			int B = imagem.at<Vec3b>(j, i)[0]/FATOR;
			int G = imagem.at<Vec3b>(j, i)[1]/FATOR;
			int R = imagem.at<Vec3b>(j, i)[2]/FATOR;

			histograma_ponto[B][G][R]++;
		}
	}

	if(total_pontos > 0) normaliza_histograma(histograma_ponto, total_pontos);

	//Compara com o histograma "ótimo" e atribui um score (ainda não normalizado).
	double score = 0.0;
	for(int i = 0; i < NIVEL_POR_COR; i++) {
		for(int j = 0; j < NIVEL_POR_COR; j++) {
			for(int k = 0; k < NIVEL_POR_COR; k++) {
				double diff = histograma_ponto[i][j][k] - histograma_otimo[i][j][k];
				//if(diff < 0.0) diff *= -1.0;
				if(histograma_ponto[i][j][k] + histograma_otimo[i][j][k] != 0.0)
					score += diff * diff/(histograma_ponto[i][j][k] + histograma_otimo[i][j][k]);
			}
		}
	}
	ponto.vhistograma = score;
}

void calcula_histograma(Mat &imagem, vector<acmPoint> &pontos) {
	vector<acmPoint>::iterator it;
	double maior_score = -1;

	for(it = pontos.begin(); it != pontos.end(); ++it) {
		calcula_score_histograma(imagem, *it);
		if(maior_score < it->vhistograma || maior_score == -1) maior_score = it->vhistograma;
		//printf("%lf(%lf) ", it->vhistograma, it->vnorm);
	}
	//printf("\n%lf\n", maior_score);
	
	maior_score = 2.0;
	
	//Normaliza:
	for(it = pontos.begin(); it != pontos.end(); ++it) {
		//it->vhistograma = 1 - ((it->vhistograma - menor_score)/(maior_score - menor_score));
		it->vhistograma = 1 - (it->vhistograma/maior_score);
		it->vhistograma = (it->vhistograma - 0.9) * 10.0;
		if(it->vhistograma < 0.0) it->vhistograma = 0.0;
		it->calculaScore();
		//printf("%lf ", it->score_final);
	}
	//printf("\n");

	sort(pontos.begin(), pontos.end());
	reverse(pontos.begin(), pontos.end());
}

