#include <iostream>
#include <math.h>
#include <deque>
#include <sys/time.h>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

using namespace std;

mutex resultsMutex;

bool isPrime(long long int n){
    long long int end = sqrt(n);

    for (int i = 2; i <= end; i++){
        if (n % i == 0){
            return false;
        }
    }

    return true;
}

// Serial version
pair<deque<long long int>, long long int> serial_primes(long long int n){
    deque<long long int> primes;
    long long int count = 0;

    for (long long int i = 2; i <= n; i++){
        if (isPrime(i)){
            primes.push_front(i);
            count += 1;
            if (primes.size() > 10){
                primes.pop_back();
            }
        }
    }
    return make_pair(primes, count);
}

// Parallel iteration
void parallel_primes_iter(long long int start, long long int end, vector<deque<long long int>>& results){

    deque<long long int> primes;
    long long int count = 0;
    for (long long int i = start; i < end; i++){
        if (isPrime(i)){
            primes.push_front(i);
            count++;
            if (primes.size() > 10){
                primes.pop_back();
            }
        }
    }
    primes.push_front(count);

    lock_guard<mutex> lock(resultsMutex);
    results.push_back(primes);
    
}
// Parallel version
pair<deque<long long int>, long long int> parallel_primes(long long int n, int thr){
    long long int count = 0;
    thread array[thr];
    vector<deque<long long int>> results;

    long long int start = 2;
    long long int end = sqrt(n);
    long long int step = (n - end) / (thr - 1);
    array[0] = thread(parallel_primes_iter, start, end, ref(results));

    for (int i = 1; i < thr; i++){
        start = end;
        if (i == thr - 1){
            end = n;
        }
        else{
            end += step;
        }

        array[i] = thread(parallel_primes_iter, start, end, ref(results));
    }

    for (int i = 0; i < thr; i++){
        if (array[i].joinable()){
            array[i].join();
        }
    }

    for (int i = 0; i < thr; i++){
        count += results[i][0];
        results[i].pop_front();
    }

    // Merge results
    deque<long long int> merged;

    for (int i = 0; i < thr; i++){
        for (int j = 0; j < results[i].size(); j++){
            merged.push_front(results[i][j]);
        }
    }

    // Sort merged deque
    sort(merged.begin(), merged.end(), greater<long long int>());

    // Get the last 10 elements of merged
    merged.resize(10);

    return make_pair(merged, count);


      
}

// Print deque
ostream& operator<<(ostream& os, const deque<long long int>& v){
    os << "[";
    for (int i = 0; i < v.size(); i++){
        os << v[i];
        if (i != v.size() - 1){
            os << ", ";
        }
    }
    os << "]";
    return os;
}



int main() {

    long long int n = 10000000;
    int threads = 16;
    
    timeval start, end;

    gettimeofday(&start, NULL);
    pair<deque<long long int>, long long int> primes1 = serial_primes(n);
    gettimeofday(&end, NULL);

    double t1 = double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec)/1000000;

    cout << "Serial time: " << t1 << endl;
    cout << primes1.first << endl;
    cout << primes1.second << endl;

    cout << "-------------------" << endl;


    gettimeofday(&start, NULL);
    pair<deque<long long int>, long long int> primes2 = parallel_primes(n, threads);
    gettimeofday(&end, NULL);

    double t2 = double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec)/1000000;

    cout << "Parallel time: " << t2 << endl;
    cout << primes2.first << endl;
    cout << primes2.second << endl;
    
    cout << "-------------------" << endl;

    cout << "Speedup usando " << threads << " hilos: " << t1/t2 << endl;
    return 0;
}