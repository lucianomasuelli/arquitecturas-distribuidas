#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;
std::mutex resultsMutex;

void printResults(const std::vector<std::array<int, 2>>& results) {
    for (const std::array<int, 2>& arr : results) {
        cout << "La subcadena " << arr[0] << " aparece " << arr[1] << " veces en el texto" << endl;
    }
}

void patternMatch(string pattern, string content,int i, std::vector<std::array<int, 2>>& vec){
    int count = 0;
    int pos = 0;
    
    while ((pos = content.find(pattern, pos)) != string::npos) {
        count++;
        pos++;
    }
    
    vec.push_back({i, count});
    
}

std::vector<std::array<int, 2>> serialPatternMatching(){
    ifstream fin("texto.txt");
    string content;
    getline(fin, content);

    ifstream patterns("patrones.txt");
    
    int i = 0;
    std::vector<std::array<int, 2>> results;

    string pattern;
    while (getline(patterns, pattern)){
        patternMatch(pattern, content, i, results);
        i++;
    }

    return results;
}

std::vector<std::array<int,2>> parallelPatternMatching() {
    std::thread arr[32];

    ifstream fin("texto.txt");
    string content;
    getline(fin, content);

    ifstream patterns("patrones.txt");
    int i = 0;
    std::vector<std::array<int, 2>> results;

    string pattern;
    while (getline(patterns, pattern)){
        arr[i] = std::thread(patternMatch, pattern, content, i, ref(results));
        i++;
    }

    for (int i = 0; i < 32; i++) {
        if (arr[i].joinable()) {
            arr[i].join();
        }
    }

    return results;
}

double get_time(timeval t1, timeval t2){
    return double(t2.tv_sec - t1.tv_sec) + double(t2.tv_usec - t1.tv_usec)/1000000;
}

int main() {
    timeval start, end;
    
    gettimeofday(&start, NULL);
    std::vector<std::array<int, 2>> results = serialPatternMatching();
    gettimeofday(&end, NULL);
    double serialTime = get_time(start, end);
    cout << "Serial time: " << serialTime << endl;

    gettimeofday(&start, NULL);
    std::vector<std::array<int, 2>> results2 = parallelPatternMatching();
    gettimeofday(&end, NULL);
    double parallelTime = get_time(start, end);
    cout << "Parallel time: " << parallelTime << endl;
    cout << "Speedup: " << serialTime / parallelTime << endl;
}