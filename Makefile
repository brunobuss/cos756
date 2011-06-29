OPENCV = `pkg-config --cflags --libs opencv`
GCCOPT = -Wall -Wextra -g

all: trabalho.out

trabalho.out: hough.o discretizacao.o normalizacao.o roi.o histograma.o testes.o main.o
	@g++ -o trabalho.out $(OPENCV) $(GCCOPT) hough.o discretizacao.o normalizacao.o roi.o histograma.o testes.o main.o

hough.o: src/hough.cpp src/main.h
	@g++ -c src/hough.cpp $(OPENCV) $(GCCOPT) -o hough.o

discretizacao.o: src/discretizacao.cpp src/main.h
	@g++ -c src/discretizacao.cpp $(OPENCV) $(GCCOPT) -o discretizacao.o

normalizacao.o: src/normalizacao.cpp src/main.h
	@g++ -c src/normalizacao.cpp $(OPENCV) $(GCCOPT) -o normalizacao.o

roi.o: src/roi.cpp src/main.h
	@g++ -c src/roi.cpp $(OPENCV) $(GCCOPT) -o roi.o

histograma.o: src/histograma.cpp src/main.h
	@g++ -c src/histograma.cpp $(OPENCV) $(GCCOPT) -o histograma.o

testes.o: src/testes.cpp src/main.h
	@g++ -c src/testes.cpp $(OPENCV) $(GCCOPT) -o testes.o

main.o: src/main.cpp src/main.h
	@g++ -c src/main.cpp $(OPENCV) $(GCCOPT) -o main.o

clean:
	@rm trabalho.out hough.o discretizacao.o normalizacao.o roi.o histograma.o testes.o main.o
