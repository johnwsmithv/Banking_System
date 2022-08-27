CFLAGS = -std=gnu17 -Wall -O3
CFLAGS_DEBUG = -std=gnu17 -Wall
C++FLAGS = -std=c++0x
INCLUDES = -I/home/jwsv61099/workspace/libraries/cunit/CUnit/
LIBS = -lsqlite3
TEST_LIBS = -L/home/jwsv61099/workspace/libraries/cunit/local-build/CUnit/ -lcunit
SOURCES= banking.c bankingFunctions.c
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
OUT = banking.exe
OUT_DEBUG = banking_debug.exe
OUT_TEST = bankingTest.exe

default: 
# Compiling the C files into object files
	gcc $(CFLAGS) -o bankingFunctions.o -c bankingFunctions.c
	gcc $(CFLAGS) -o banking.o -c banking.c
# Linking the object files together with the libraries  
	g++ *.o -o $(OUT) $(LIBS)
	rm -f *.o
test:
	rm -f *.o
# gcc $(CFLAGS) -o bankingFunctions.o -c bankingFunctions.c
# gcc $(CFLAGS) $(INCLUDES) -o bankingTest.o -c bankingTest.c
# g++ *.o -o $(OUT_TEST) $(TEST_LIBS)
	gcc $(CFLAGS) banking.c bankingFunctions.c $(LIBS) -o $(OUT_TEST)
debug:
	# Compiling the C, C++ files into object files
	rm -f *.o
# gcc $(CFLAGS) -o banking.o -c -g banking.c
# # Linking the object files together with the libraries  
# g++ *.o -g -o $(OUT) $(LIBS)
	gcc $(CFLAGS_DEBUG) -g banking.c bankingFunctions.c $(LIBS) -o $(OUT_DEBUG)
debug_test:
	rm -f *.o
	gcc $(CFLAGS) -o bankingFunctions.o -c -g bankingFunctions.c
	gcc $(CFLAGS) $(INCLUDES) -o bankingTest.o -c -g bankingTest.c
	g++ *.o -g -o $(OUT_TEST) $(TEST_LIBS)
clean:
	rm $(OUT)
	rm $(OUT_TEST)
	rm -f $(OBJECTS)

testSQL:
	gcc -g testSql.c -lsqlite3 -o runSQL