all: node.o phi.o

node.o:
	icpc -qopenmp source/alg_1.cpp -o binary/my_alg_1 -std=c++14
phi.o:
	icpc -mmic -qopenmp source/alg_1.cpp -o binary/my_alg_1_phi -std=c++14