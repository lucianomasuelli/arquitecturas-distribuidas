#include <iostream>
#include <fstream>
#include <string>
#include <mpi.h>
#include "direccion_IP.cpp"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    double start_time = MPI_Wtime(); // Start time

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char direcciones_ip[size][20]; // Almacenar las direcciones IP
    char direccion_ip[20];

    obtener_IP(direccion_ip);
    

    // Juntar todas las direcciones IP en un arreglo
    MPI_Gather(direccion_ip, 20, MPI_CHAR, direcciones_ip, 20, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank < 32) {
        // Cada proceso con rango menor a 32 buscará un patrón específico

        // Leer el patrón correspondiente al rango del proceso
        std::string patron;
        std::ifstream patronesFile("patrones.txt");
        for (int i = 0; i <= rank; ++i) {
            std::getline(patronesFile, patron);

        }

        // Leer la línea de texto
        std::ifstream textoFile("texto.txt");
        std::string linea;
        std::getline(textoFile, linea);

        // Contar ocurrencias del patrón
        size_t pos = 0;
        int count = 0;
        while ((pos = linea.find(patron, pos)) != std::string::npos) {
            ++count;
            pos += patron.length();
        }

        // Enviar el resultado y la dirección IP al proceso 0
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        //MPI_Send(direccion_ip, 20, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int resultados[size] = {0}; // Inicializar resultados a 0
        //char direcciones_ip[size][20];

        for (int i = 0; i < size; ++i) {
            MPI_Recv(&resultados[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //MPI_Recv(direcciones_ip[i], 20, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Imprimir resultados ordenados
        for (int i = 0; i < size; ++i) {
            std::cout << "el patron " << i << " aparece " << resultados[i] << " veces. Buscado por " << direcciones_ip[i] << std::endl;
        }
    }

    double end_time = MPI_Wtime(); // End time
    double elapsed_time = end_time - start_time; // Elapsed time

    if (rank == 0) {
        std::cout << "Tiempo de ejecución: " << elapsed_time << " segundos" << std::endl;
    }

    MPI_Finalize();
    return 0;
}