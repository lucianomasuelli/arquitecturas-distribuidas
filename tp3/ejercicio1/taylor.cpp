#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>

long double calcularTermino(int n, long double x) {
    return pow(-1, n) * pow(x - 1, n + 1) / (n + 1);
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int terminos = 10000000; // Número total de términos
    const long double x = 1.5e6; // Número para el que se calculará el logaritmo natural

    long double resultado = 0.0;

    int terminosPorProceso = terminos / size;
    int inicio = rank * terminosPorProceso;
    int fin = (rank + 1) * terminosPorProceso;

    for (int i = inicio; i < fin; ++i) {
        resultado += calcularTermino(i, x);
    }

    long double resultadoFinal;
    MPI_Reduce(&resultado, &resultadoFinal, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << std::setprecision(15) << "El logaritmo natural de " << x << " es: " << resultadoFinal << std::endl;
    }

    MPI_Finalize();
    return 0;
}
