#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>

using namespace std;

long double calcularTermino(int n, long double x) {
    long double result = 2 * (1 / (2*n + 1.0)) * float(pow((float(x-1)/float(x+1)), 2*n + 1));
    return result;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    double start_time = MPI_Wtime(); // Start time

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int terminos = 10000000; // Número total de términos
    const long double x = 1500000.0; // Número para el que se calculará el logaritmo natural

    long double resultado = 0.0;

    int terminosPorProceso = terminos / size;
    int inicio = rank * terminosPorProceso;
    int fin = (rank + 1) * terminosPorProceso;

    for (int i = inicio; i < fin; ++i) {
        resultado += calcularTermino(i, x);
    }

    /* cout << "rank: " << rank << endl;
    cout << "resultado: " << resultado << endl; */

    long double resultadoFinal;
    MPI_Reduce(&resultado, &resultadoFinal, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << std::setprecision(15) << "El logaritmo natural de " << x << " es: " << resultadoFinal << std::endl;

        double end_time = MPI_Wtime(); // End time
        double elapsed_time = end_time - start_time; // Elapsed time

        std::cout << "Tiempo de ejecución: " << elapsed_time << " segundos" << std::endl;

        // Calcular el speedup
        double serial_time = 0.0;
        if (rank == 0) {
            double serial_start_time = MPI_Wtime(); // Start time

            long double resultadoSerial = 0.0;
            for (int i = 0; i < terminos; ++i) {
                resultadoSerial += calcularTermino(i, x);
            }

            double serial_end_time = MPI_Wtime(); // End time
            serial_time = serial_end_time - serial_start_time; // Serial time

            std::cout << "El logaritmo natural de " << x << " (serial) es: " << resultadoSerial << std::endl;
            std::cout << "Tiempo de ejecución (serial): " << serial_time << " segundos" << std::endl;
        }

        double speedup = serial_time / elapsed_time;
        std::cout << "Speedup: " << speedup << std::endl;
    }

    MPI_Finalize();
    return 0;
}