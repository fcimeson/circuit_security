
CC            = g++ -D __STDC_FORMAT_MACROS -D __STDC_LIMIT_MACROS
LIB_DIR       = -L/usr/lib64 -L/usr/lib
LIB           = -lboost_regex -lboost_program_options -ligraph
INC           = -I./include -I./cb_minisat
MINISAT_OBJS  = cb_minisat/build/release/minisat/core/Solver.o  cb_minisat/build/dynamic/minisat/utils/System.o

all: main

minisat:
	cd cb_minisat &&\
	make

main: minisat obj/main.o obj/general.o obj/circuit.o obj/security.o obj/formula.o obj/subisosat.o
	$(CC) $(LDFLAGS) $(LIB_DIR) $(MINISAT_OBJS) obj/main.o obj/general.o obj/circuit.o obj/security.o obj/formula.o obj/subisosat.o -o bin/circuit_security $(LIB)

obj/main.o: src/main.cpp include/main.hpp
	$(CC) $(CFLAGS) $(INC) -c src/main.cpp -o obj/main.o

obj/general.o: src/general.cpp include/general.hpp
	$(CC) $(CFLAGS) $(INC) -c src/general.cpp -o obj/general.o 

obj/circuit.o: src/circuit.cpp include/circuit.hpp
	$(CC) $(CFLAGS) $(INC) -c src/circuit.cpp -o obj/circuit.o 

obj/security.o: src/security.cpp include/security.hpp
	$(CC) $(CFLAGS) $(INC) -c src/security.cpp -o obj/security.o 

obj/formula.o: include/formula.hpp src/formula.cpp
	$(CC) $(CFLAGS) $(INC) $(LIB_DIR) $(MINISAT_OBJS) -c src/formula.cpp -o obj/formula.o

obj/subisosat.o: minisat obj/formula.o include/subisosat.hpp src/subisosat.cpp 
	$(CC) $(CFLAGS) $(INC) $(LIB_DIR) $(MINISAT_OBJS) obj/formula.o -c src/subisosat.cpp -o obj/subisosat.o


clean:
	cd cb_minisat && make clean
	rm -rf *~
	rm -rf */*~
	rm -rf */*/*~
	rm -f obj/*
	rm -f bin/*

