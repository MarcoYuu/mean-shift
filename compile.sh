g++ -c -O3 mshift.cpp mean_shift.cpp -std=c++11
g++ mean_shift.o mshift.o -o mshift `pkg-config --cflags --libs opencv` -lboost_thread -lboost_timer
