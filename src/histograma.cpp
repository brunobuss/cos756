#include "main.h"

//O histograma considerado como "ótimo" pelo código responsável pela comparação de histograma.
vector< vector< vector <double> > > histograma_otimo;

/* Prepara um novo histograma, apagando qualquer conteúdo anterior
   e inicializando tudo com 0 */
void initHistogram(vector< vector< vector <double> > > &histograma) {

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


// Normaliza todo o conteúdo do histograma para valores entre [0, 1]
void normalizeHistogram(vector< vector< vector <double> > > &histograma, int total) {

    for(int i = 0; i < NIVEL_POR_COR; i++) {
        for(int j = 0; j < NIVEL_POR_COR; j++) {
            for(int k = 0; k < NIVEL_POR_COR; k++) {
                histograma[i][j][k] /= total;
            }
        }
    }
}


/* Define o histograma_ótimo, como sendo o histograma
   relacionado ao circuoa centrado em 'ponto' na 'imagem'.
 */
void setOptHistogram(Mat &imagem, acmPoint &ponto) {

    int total_pontos = 0;

    initHistogram(histograma_otimo);
    for(int i = ponto.cx - ponto.rad; i <= ponto.cx + ponto.rad; ++i) {
        for(int j = ponto.cy - ponto.rad; j <= ponto.cy + ponto.rad; ++j) {

            if( ((ponto.cx - i) * (ponto.cx - i) + (ponto.cy - j) * (ponto.cy - j)) > (ponto.rad * ponto.rad) ) continue;
            if(!pointInsideRect(i, j, imagem.rows, imagem.cols)) continue;

            total_pontos++;

            int B = imagem.at<Vec3b>(j, i)[0]/FATOR;
            int G = imagem.at<Vec3b>(j, i)[1]/FATOR;
            int R = imagem.at<Vec3b>(j, i)[2]/FATOR;

            histograma_otimo[B][G][R]++;
        }
    }

    normalizeHistogram(histograma_otimo, total_pontos);
}

/* Calcula o score do histograma do circulo centrado em 'ponto'
   na imagem 'imagem', em relação ao histograma_otimo já definido
   anteriormente.
 */
void calcHistogramScore(const Mat &imagem, acmPoint &ponto) {

    vector< vector< vector <double> > > histograma_ponto;
    int total_pontos = 0;

    //Calcula o histograma do circulo inicialmente.
    initHistogram(histograma_ponto);
    for(int i = ponto.cx - ponto.rad; i <= ponto.cx + ponto.rad; ++i) {
        for(int j = ponto.cy - ponto.rad; j <= ponto.cy + ponto.rad; ++j) {
            if( ((ponto.cx - i) * (ponto.cx - i) + (ponto.cy - j) * (ponto.cy - j)) > (ponto.rad * ponto.rad) ) continue;
            if(!pointInsideRect(i, j, imagem.rows, imagem.cols)) continue;

            total_pontos++;

            int B = imagem.at<Vec3b>(j, i)[0]/FATOR;
            int G = imagem.at<Vec3b>(j, i)[1]/FATOR;
            int R = imagem.at<Vec3b>(j, i)[2]/FATOR;

            histograma_ponto[B][G][R]++;
        }
    }

    if(total_pontos > 0) normalizeHistogram(histograma_ponto, total_pontos);

    //Compara com o histograma "ótimo" e atribui um score (ainda não normalizado).
    double score = 0.0;
    for(int i = 0; i < NIVEL_POR_COR; i++) {
        for(int j = 0; j < NIVEL_POR_COR; j++) {
            for(int k = 0; k < NIVEL_POR_COR; k++) {
                double diff = histograma_ponto[i][j][k] - histograma_otimo[i][j][k];
                if(histograma_ponto[i][j][k] + histograma_otimo[i][j][k] != 0.0)
                        score += diff * diff/(histograma_ponto[i][j][k] + histograma_otimo[i][j][k]);
            }
        }
    }
    ponto.vhistograma = score;
}


/* Calcula o score de histograma para todos os circulos centrados em
   'pontos' em relação a imagem 'imagem'.
 */
void calcHistogram(Mat &imagem, vector<acmPoint> &pontos) {
    vector<acmPoint>::iterator it;

    for(it = pontos.begin(); it != pontos.end(); ++it) {
        calcHistogramScore(imagem, *it);
        if(maior_score < it->vhistograma || maior_score == -1) maior_score = it->vhistograma;
    }

    //Normaliza:
    double maior_score = 2.0;

    for(it = pontos.begin(); it != pontos.end(); ++it) {
        it->vhistograma = 1 - (it->vhistograma/maior_score);
        if(it->vhistograma < 0.0) it->vhistograma = 0.0;
        it->calculaScore();
    }

    sort(pontos.begin(), pontos.end());
    reverse(pontos.begin(), pontos.end());
}

