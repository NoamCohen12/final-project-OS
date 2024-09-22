CXX = clang++ 

.PHONY: all clean 

all: main 

main: main.o union_find.o Graph.o GraphGUI.o MST_graph.o MST_stats.o MST_strategy.o ThreadPool.o ThreadPool.o
	$(CXX) -o $@ $^ -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp
	$(CXX) -c -o $@ $^ 

GraphGUI.o: GraphGUI.cpp
	$(CXX) -c -o $@ $^ 

MST_graph.o: MST_graph.cpp
	$(CXX) -c -o $@ $^ 

MST_stats.o: MST_stats.cpp
	$(CXX) -c -o $@ $^

MST_strategy.o: MST_strategy.cpp
	$(CXX) -c -o $@ $^

union_find.o: union_find.cpp
	$(CXX) -c -o $@ $^ 
	
Graph.o: Graph.cpp
	$(CXX) -c -o $@ $^ 

ThreadPool.o: ThreadPool.cpp
	$(CXX) -c -o $@ $^ 	



clean:
	rm -f *.o main 