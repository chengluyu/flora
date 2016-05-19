CC = clang++
CXX_FLAGS = --std=c++11 -DFLORA_DEBUG
OUTPUT_EXEC = test.out
OBJECTS = token.o scanner.o main.o

token:
	$(CC) $(CXX_FLAGS) -c "../../src/token.cc" -o token.o

scanner:
	$(CC) $(CXX_FLAGS) -c "../../src/scanner.cc" -o scanner.o

main:
	$(CC) $(CXX_FLAGS) -c test.cc -o main.o

clean_obj:
	rm *.o

compile: token scanner main
	$(CC) $(OBJECTS) -o $(OUTPUT_EXEC)

test: compile clean_obj

.PHONY: clean_obj compile test