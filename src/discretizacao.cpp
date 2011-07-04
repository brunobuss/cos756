#include "main.h"

//================= Discretização e Incremento de arco circular ===================

// Dado matriz (2d) de votação m com dimensão nrow x ncol.
// Percorre o arco-circular centrado em (cx,cy) de raio r no intervalo thetaRange incrementando a votação do
// valor inc


int incCirc(int **m,int nrow,int ncol, int cx,int cy,int r, pair<double,double> thetaRange,int inc){

	double theta = thetaRange.first;
	
	if(theta == 0.0) theta += EPSINCCIRC ;
	
	double h = sqrt(r*r + 0.35);
	double step = acos(r/h);
 
//	printf("%lf\n",step);
	double x,y;
	int ix,iy,antx=-INF,anty=-INF;
	int pcount=0;
	
	
	for(;theta < thetaRange.second ; theta += step){
		x = cx + r*cos(theta);
		y = cy + r*sin(theta);
		
		ix = (int)x;iy = (int)y;
		if(ix == antx && iy == anty) continue;
		pcount++;
		
                if(m != NULL && pointInsideRect(ix,iy,ncol,nrow))
			m[iy][ix] += inc;
			
		antx = x;anty = y;
	}
	
	return pcount;

}
