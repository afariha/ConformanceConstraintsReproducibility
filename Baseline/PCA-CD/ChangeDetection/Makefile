PROG = CD

UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CXX = clang++ -stdlib=libc++
else
CXX = g++
endif

FLAGS = -O0 -g3 -Wall -std=c++0x -pthread

INCS = -I"/usr/local/lib/" 
LIBS = -L"/usr/local/lib/" -lpca -larmadillo
SRCS = CD.cpp

RM = rm -f

all :
	$(CXX) $(FLAGS) $(INCS) $(SRCS) $(LIBS) -o $(PROG)
	# $(CXX) $(FLAGS)  $(SRCS)  -o $(PROG)

clean :
	$(RM) $(PROG)
