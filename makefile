CXX = clang++ 

.PHONY: all clean 

all: main 

main: main.o MST.o union_find.o Graph.o GraphGUI.o
	$(CXX) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	$(CXX) -c -o $@ $^ 

GraphGUI.o: GraphGUI.cpp
	$(CXX) -c -o $@ $^ 

MST.o: MST.cpp
	$(CXX) -c -o $@ $^ 

union_find.o: union_find.cpp
	$(CXX) -c -o $@ $^ 
	
Graph.o: Graph.cpp
	$(CXX) -c -o $@ $^ 



clean:
	rm -f *.o main 