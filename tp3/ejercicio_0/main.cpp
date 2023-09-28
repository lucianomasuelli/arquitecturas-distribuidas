#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include "direccion_IP.cpp"

void imprimir_info(int id_proceso, int total_procesos) {
    char nombre_maquina[256];
    gethostname(nombre_maquina, 256);
    char direccion_ip[20];
    obtener_IP(direccion_ip);

    std::cout << "Hola Mundo! Soy el proceso " << id_proceso << " de " << total_procesos << " corriendo en la máquina " << nombre_maquina << " IP=" << direccion_ip << std::endl;
}

int main() {
    int total_procesos = 5; // Cambia este valor al número total de procesos que desees crear

    for (int i = 1; i <= total_procesos; i++) {
        pid_t pid = fork();

        if (pid == 0) { // Proceso hijo
            imprimir_info(i, total_procesos);
            exit(0);
        }
        else if (pid < 0) {
            std::cerr << "Error al crear el proceso hijo." << std::endl;
            exit(1);
        }
    }

    return 0;
}
