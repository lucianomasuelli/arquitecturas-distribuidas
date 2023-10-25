#include <mpi.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    double start_time = MPI_Wtime(); // Start time

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Leer n
    int n;
    if (rank == 0) {
        cout << "Ingrese n: ";
        cin >> n;
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);


    //Encontrar primos menores a raiz de n.
    int raiz = sqrt(n);
    std::vector<int> primos_menores;
    for (int i = 2; i <= raiz; ++i) {
        bool es_primo = true;
        for (int j = 2; j < i; ++j) {
            if (i % j == 0) {
                es_primo = false;
                break;
            }
        }
        if (es_primo) {
            primos_menores.push_back(i);
        }
    }

    // Dividir los números entre raiz de n y n entre los procesos
    int numeros_por_proceso = (n - raiz) / size;
    int inicio = raiz + rank * numeros_por_proceso;
    int fin = inicio + numeros_por_proceso;


    // Contar primos y guardarlos
    int count = 0;
    vector<int> primos = {};
    for (int i = inicio; i <= fin; ++i) {
        bool es_primo = true;
        for (int primo : primos_menores) {
            if (i % primo == 0) {
                es_primo = false;
                break;
            }
        }
        if (es_primo) {
            ++count;
            primos.push_back(i);
        }
    }
    


    // Enviar el resultado al proceso 0
    if(rank != 0){
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&primos [0], primos.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    

    // Recibir los resultados de los otros procesos
    if (rank == 0) {
        int total_count = count + primos_menores.size();
        vector<int> total_primos = primos_menores;
        for (int i = 1; i < size; ++i) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += count;

            // Recibir primos
            int primos[count];
            MPI_Recv(&primos[0], count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Agregarlos a la lista de primos
            for (int j = 0; j < count; ++j) {
                total_primos.push_back(primos[j]);
            }
        }

        // Ordenar los primos
        sort(total_primos.begin(), total_primos.end());

        // Imprimir los 10 mayores
        cout << "Hay " << total_count << " primos menores a n." << endl;
        cout << "Los 10 mayores son: ";
        for (int i = total_primos.size() - 10; i < total_primos.size(); ++i) {
            cout << total_primos[i] << " ";
        }
        cout << endl;

        double end_time = MPI_Wtime(); // End time
        double elapsed_time = end_time - start_time; // Elapsed time

        cout << "Tiempo de ejecución: " << elapsed_time << " segundos" << endl;

        // Calcular el speedup
        double serial_time = 0.0;
        if (rank == 0) {
            double serial_start_time = MPI_Wtime(); // Start time
            std::vector<int> primos_menores;
            for (int i = 2; i <= raiz; ++i) {
                bool es_primo = true;
                for (int j = 2; j < i; ++j) {
                    if (i % j == 0) {
                        es_primo = false;
                        break;
                    }
                }
                if (es_primo) {
                    primos_menores.push_back(i);
                }
            }

            int count = primos_menores.size();
            vector<int> total_primos = primos_menores;

            for (int i = raiz + 1; i <= n; ++i) {
                bool es_primo = true;
                for (int primo : primos_menores) {
                    if (i % primo == 0) {
                        es_primo = false;
                        break;
                    }
                }
                if (es_primo) {
                    ++count;
                    total_primos.push_back(i);
                }
            }

            sort(total_primos.begin(), total_primos.end());

            double serial_end_time = MPI_Wtime(); // End time
            serial_time = serial_end_time - serial_start_time; // Serial time
        }

        double speedup = serial_time / elapsed_time;
        cout << "Speedup: " << speedup << endl;
    }

    MPI_Finalize();
    return 0;
}