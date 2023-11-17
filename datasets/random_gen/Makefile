MF=     Makefile
 
CC=     g++
 
CFLAGS= -g -fopenmp -msse4.2 -fomit-frame-pointer -funroll-loops 
 
LFLAGS= -std=c++11 -O3 -DNDEBUG -Wl,-rpath=$(PWD)/libsdsl/lib

EXE=   index 
 
SRC=    main.cpp input.cpp estimation.cpp property_string.cpp

HD=     estimation.h input.h property_string.h Makefile

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

clean-all:
	rm -f $(OBJ) $(EXE) *~
	rm -r libsdsl
	rm -r sdsl-lite


