CXX = g++ -g -gdwarf-4 -fprofile-arcs -ftest-coverage

.PHONY: all clean run_tests coverage

all: main 

main: main.o union_find.o Graph.o GraphGUI.o MST_graph.o MST_stats.o MST_strategy.o LeaderFollowerPool.o Pipeline.o
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

LeaderFollowerPool.o: LeaderFollowerPool.cpp
	$(CXX) -c -o $@ $^ 	

Pipeline.o: Pipeline.cpp
	$(CXX) -c -o $@ $^	

gcov_coverage: main
	./main
	gcov Graph.cpp MST_graph.cpp MST_stats.cpp main.cpp Pipeline.cpp LeaderFollowerPool.cpp union_find.cpp MST_strategy.cpp

clean:
	rm -f *.o main *.gcda *.gcno
	rm -rf coverage