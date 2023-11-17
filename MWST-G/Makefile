MF=     Makefile
 
CC=     g++
 
CFLAGS= -g -fopenmp -msse4.2 -fomit-frame-pointer -funroll-loops 
 
LFLAGS= -std=c++14 -O3 -DNDEBUG -lboost_iostreams -I ./libsdsl/include/ -L ./libsdsl/lib/ -lsdsl

EXE=   index 
 
SRC=    main.cpp input.cpp estimation.cpp PST.cpp property_string.cpp anchors_new.cpp minimizer_index.cpp heavy_string.cpp krfp.cpp utils.cpp minimizers.cpp

HD=     input.h PST.h estimation.h property_string.h anchors_new.h minimizer_index.h grid.h heavy_string.h krfp.h utils.h minimizers.h Makefile

# 
# No need to edit below this line 
#
 
.SUFFIXES: 
.SUFFIXES: .cpp .o 
 
OBJ=    $(SRC:.cpp=.o) 
 
.cpp.o: 
	$(CC) $(CFLAGS)-c $(LFLAGS) $< 
 
all:    $(EXE) 
 
$(EXE): $(OBJ) 
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LFLAGS) 
 
$(OBJ): $(MF) $(HD) 
 
clean: 
	rm -f $(OBJ) $(EXE) *~



