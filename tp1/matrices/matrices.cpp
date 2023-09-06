#include <iostream>
#include <math.h>
#include <vector>
#include <sys/time.h>
#include <thread>
#include <mutex>

using namespace std;


void fila(int i, int size, vector<vector<float>> A,vector<vector<float>> B,vector<vector<float>>& C){
    for (int j = 0; j < size; j++){
        float sum = 0;
        for (int k = 0; k < size; k++){
            sum += A[i][k] * B[k][j];
        }
        C[i][j] = sum;
    }
    
}

vector<vector<float>> serial_mult(int size, vector<vector<float>> A,vector<vector<float>> B){
    vector<vector<float>> C(size, vector<float>(size, 0));

    for (int i = 0; i < size; i++){
        fila(i, size, A, B, C);
    }

    return C;
}

vector<vector<float>> parallel_mult(int size, int threads, vector<vector<float>> A, vector<vector<float>> B){
    vector<vector<float>> C(size, vector<float>(size, 0));
    thread array[threads];

    for (int i = 0; i < threads; i++) {
        int start = (i * size) / threads;
        int end = ((i + 1) * size) / threads;
        array[i] = thread([start, end, size, &A, &B, &C]() {
            for (int i = start; i < end; i++){
                fila(i, size, A, B, C);
            }
        });
    }

    for (int i = 0; i < threads; i++) {
        if (array[i].joinable()) {
            array[i].join();
        }
    }

    return C;
}

vector<vector<float>> generate_random_matrix(int size){
    vector<vector<float>> matrix(size, vector<float>(size, 0));
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            matrix[i][j] = rand() % 10;
        }
    }
    return matrix;
}

vector<vector<float>> generate_matrix(int size, float value){
    vector<vector<float>> matrix(size, vector<float>(size, value));
    return matrix;
}
void print_matrix(int size, vector<vector<float>> matrix){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void print_esquinas(int size, vector<vector<float>>& matrix){
    cout << matrix[0][0] << "-" << matrix[0][size-1] << "-" << matrix[size-1][0] << "-" << matrix[size-1][size-1] << endl;
}

float get_total(int size, vector<vector<float>> matrix){
    float total = 0;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
           total += matrix[i][j];
        }
    }
    return total;
}


int main(){
    int SIZE = 300;
    int THREADS = 16;

    vector<vector<float>> A = generate_matrix(SIZE, 0.1);
    vector<vector<float>> B = generate_matrix(SIZE, 0.2);
    
    timeval start, end;

    gettimeofday(&start, NULL);
    vector<vector<float>> C = serial_mult(SIZE, A, B);
    gettimeofday(&end, NULL);
    double serialTime = double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec)/1000000;
    cout << "Serial time: " << serialTime << endl;
    
    cout << get_total(SIZE, C) << endl;
    print_esquinas(SIZE, C);


    cout << "---------------" << endl;

    gettimeofday(&start, NULL);
    vector<vector<float>> D = parallel_mult(SIZE, THREADS, A, B);
    gettimeofday(&end, NULL);
    double parallelTime = double(end.tv_sec - start.tv_sec) + double(end.tv_usec - start.tv_usec)/1000000;
    cout << "Parallel time: " << parallelTime << endl;

    cout << get_total(SIZE, D) << endl;
    print_esquinas(SIZE, D);

    cout << "---------------" << endl;
    cout << "Speedup: " << serialTime/parallelTime << endl;
    
}