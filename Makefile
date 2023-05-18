
all:
	g++ *.cpp SymbolTable/*.cpp -o MiniJava

run:
	gcc runtime/boot.c runtime/demo.s -o out
clean:
	rm MiniJava out runtime/demo.s
