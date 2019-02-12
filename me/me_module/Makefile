# Copyright(C) by MY Capital Inc. 2007-2999
# Used to compile strategy library
#

OS := $(shell uname -s)

Target = libtrader.so

Top = ../..
Src :=$(wildcard *.cpp )
Src :=$(filter-out main.cpp ,$(Src))
Inc = -I. 
Lib = -lpthread -ldl -lrt -lm
Bin = 


CC = gcc
CXX = g++
Flags = --std=c++11 -w -g3 -O1
Macro = -D _TEST=1
Dynamic = -fPIC -shared 
RPATH  = -Wl,-rpath,./

ifneq ($(OS), Darwin)
	Dynamic += -Wl,--no-undefined 
else
	Dynamic += -Wl,-undefined,error
endif
AR = ar -rc
RANK = ranlib


all: $(Target)

$(Target):
	# @echo $(Dynamic), $(OS)
	$(CXX) $(Flags) $(Dynamic) -o $(Target) $(Src) $(Inc)

test:
	$(CXX) $(Flags) $(RPATH) -o test main.cpp -L ./ -ltrader
	@test -f ./test && ./test

ctest:
	rm -f test
	
clean:
	rm -f $(Target)
	rm -f test


