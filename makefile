#Name: Raul Gonzalez, Jayvee Novilla
#RedID 824258754, 824319685
#Assignment 1
#Project: SIC-XE-Disassembler
#File: makefile

CXX=g++
CXXFLAGS := -std=c++11 -g

dissem: dissem.o
	$(CXX) $(CXXFLAGS) -o dissem $^

dissem.o: dissem.cpp

clean:
	rm -rf dissem
	rm -rf *.lis.txt
	rm -rf *.o


