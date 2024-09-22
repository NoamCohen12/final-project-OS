#include <queue>
#include <iostream>
#include "MST_stats.hpp"
using namespace std;


// this class implement Ledar and Follower
class LF {
    const int NUM_OF_THREADS = 4;
    /*
     each therde will take care about one of the following:
      1. longes path
      2. shortest path
      3. average path
      4. total weight
      when client ask for one of the above, the server will send the request to the right thread
    */
   // queue of threads' the leader follower wi be the first one, if it is not busy. if it is busy, the request will be sent to the next thread
    // this function will be called by the server to get the longest path
    //create queue for 4 threads
    queue<thread> threads;
    // create 4 threads
    thread t1;
    thread t2;
    thread t3;
    thread t4;
    // create 4 mutexes
    mutex m1;
    mutex m2;
    mutex m3;
    mutex m4;
    // create 4 condition variables
    condition_variable cv1;
    condition_variable cv2;
    condition_variable cv3;
    condition_variable cv4;
    // create 4 booleans to check if the thread is busy or not
    bool busy1 = false;
    bool busy2 = false;
    bool busy3 = false;
    bool busy4 = false;
    // create 4 booleans to check if the thread is done or not
    bool done1 = false;
    bool done2 = false;
    bool done3 = false;
    bool done4 = false;
  
// the function is in Mst_stats.hpp
// according to the request, the function will call the right function from the class MST_stats, we send the thered to comlete the mission, each thered can be any calculate



   


}