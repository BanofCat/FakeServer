g++ test_somefunction.cpp -I include/ src/shm_module.cpp src/timer.c -o test_somefunction -lrt -lpthread
g++ test_w.cpp -I include/ src/shm_module.cpp src/timer.c -o test_w -lrt -lpthread
g++ test_r.cpp -I include/ src/shm_module.cpp src/timer.c -o test_r -lrt -lpthread
