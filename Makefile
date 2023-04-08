
all:
	g++ -g *.cpp SymbolTable/*.cpp -o MiniJava

clean:
	rm MiniJava
