# Make file for the ALBHW

PLATFORM = linux64
INC      = ${GUROBI_HOME}/include/
CC       = gcc 
CPP      = g++
CARGS    = -m64 -Wall -g -std=gnu++1z -lstdc++fs #-std=c++17
CLIB     = -L${GUROBI_HOME}/lib/ -lgurobi90
CPPLIB   = -L${GUROBI_HOME}/lib/ -lgurobi_c++ -lgurobi90
ARCH = $(shell getconf LONG_BIT)
#ifeq ($(ARCH), 32)
#  LIBPATH = libxl/lib
#else
#  LIBPATH = libxl/lib64
#endif

#CFLAGS = -I libxl/include_cpp -L $(LIBPATH) -lxl -Wl,-rpath,$(LIBPATH)
# adicionar ao final de -lm

all: albhw

run: run_albhw


albhw: Test.cpp
	$(CPP) $(CARGS) -o Test Test.cpp -I$(INC) $(CPPLIB) -lm

albhwTestAll: TestAllWithRules.cpp
	$(CPP) $(CARGS) -o TestAll TestAllWithRules.cpp -I$(INC) $(CPPLIB) -lm

clean:
	rm -rf *.o *_c *_c++ *.class *.log *.rlp *.lp *.bas *.ilp
