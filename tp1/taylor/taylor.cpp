#include <iostream>
#include <math.h>
#include <iomanip>
#include <sys/time.h>
#include <thread>
#include <mutex>
using namespace std;

long double linear_taylor(int num, int iters){
    long double res = 0;
    for (int i=0; i < iters; i++){
        res += (1 / (2*i + 1.0)) * float(pow((float(num-1)/float(num+1)), 2*i + 1));
    }
    return res*2;
}

long double taylor_iter(int target, int i){
    
        return 2 * (1 / (2*i + 1.0)) * float(pow((float(target-1)/float(target+1)), 2*i + 1));
    
}

void thread_function(long double * ptr, int target, int start, int end){
    for (int i = start; i <= end; i++){
        *ptr += taylor_iter(target, i);
    }
}

long double parallel_taylor(int num, int iters, int threads) {
    long double res = 0;
    std::mutex resMutex;  // Mutex to protect the shared variable res
    std::thread array[threads];

    for (int i = 0; i < threads; i++) {
        int start = (i * iters) / threads;
        int end = ((i + 1) * iters) / threads;
        array[i] = std::thread([&res, &resMutex, num, start, end]() {
            long double localSum = 0;
            for (int j = start; j <= end; j++) {
                localSum += taylor_iter(num, j);
            }
            std::lock_guard<std::mutex> lock(resMutex);
            res += localSum;
        });
    }

    for (int i = 0; i < threads; i++) {
        if (array[i].joinable()) {
            array[i].join();
        }
    }

    return res;
}

double get_time(timeval t1, timeval t2){
    return double(t2.tv_sec - t1.tv_sec) + double(t2.tv_usec - t1.tv_usec)/1000000;
}

int main(){
    int NUM = 150000000;
    int ITERS = 100000000;
    int THREADS = 8;

    timeval start, end;

    gettimeofday(&start, NULL);

    long double taylor1 = linear_taylor(NUM, ITERS);

    
    gettimeofday(&end, NULL);
    double time1 = get_time(start,end);
    cout << "Resultado 1: " << setprecision(15) << taylor1 << endl;
    cout << "Tiempo de ejecucion: " << time1 << endl;

    gettimeofday(&start, NULL);
    long double taylor2 = parallel_taylor(NUM, ITERS, THREADS);
    gettimeofday(&end, NULL);

    double time2 = get_time(start,end);
    cout << "Resultado 1: " << setprecision(15) << taylor2 << endl;
    cout << "Tiempo de ejecucion: " << time2 << endl;
    
    double speedup = time1/time2;
    cout << "Speedup using " << THREADS << " threads: " << speedup << endl;
}
