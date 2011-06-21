OPENCV = `pkg-config --cflags --libs opencv`

all: trabalho.out

trabalho.out: hough.o discretizacao.o normalizacao.o roi.o main.o
	@g++ -o trabalho.out $(OPENCV) -Wall -Wextra hough.o discretizacao.o normalizacao.o roi.o main.o

hough.o: src/hough.cpp src/main.h
	@g++ -c src/hough.cpp $(OPENCV) -Wall -Wextra -o hough.o

discretizacao.o: src/discretizacao.cpp src/main.h
	@g++ -c src/discretizacao.cpp $(OPENCV) -Wall -Wextra -o discretizacao.o

normalizacao.o: src/normalizacao.cpp src/main.h
	@g++ -c src/normalizacao.cpp $(OPENCV) -Wall -Wextra -o normalizacao.o

roi.o: src/roi.cpp src/main.h
	@g++ -c src/roi.cpp $(OPENCV) -Wall -Wextra -o roi.o

main.o: src/main.cpp src/main.h
	@g++ -c src/main.cpp $(OPENCV) -Wall -Wextra -o main.o

clean:
	@rm trabalho.out hough.o discretizacao.o normalizacao.o roi.o main.o
