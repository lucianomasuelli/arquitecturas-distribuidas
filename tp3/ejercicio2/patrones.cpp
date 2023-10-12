#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <thread>
#include <mutex>
#include <vector>
#include <array>
#include <netdb.h>
#include <arpa/inet.h>
#include <mpi.h>

using namespace std;

vector<array<string, 3>> parallelPatternMatching() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get hostname
    char* hostname = getenv("HOSTNAME");
    if (hostname == NULL) {
        hostname = getenv("HOST");
    }
    if (hostname == NULL) {
        hostname = (char*)"unknown";
    }

    // Read patterns from file
    vector<string> patterns;
    ifstream patternFile("patrones.txt");
    string pattern;
    while (getline(patternFile, pattern)) {
        patterns.push_back(pattern);
    }
    patternFile.close();

    // Divide patterns among processes
    int numPatterns = patterns.size();
    int patternsPerProcess = numPatterns / size;
    int extraPatterns = numPatterns % size;
    int startPattern = rank * patternsPerProcess + min(rank, extraPatterns);
    int endPattern = startPattern + patternsPerProcess + (rank < extraPatterns ? 1 : 0);

    // Read text from file
    ifstream textFile("texto.txt");
    string text((istreambuf_iterator<char>(textFile)), istreambuf_iterator<char>());
    textFile.close();

    // Count matches for each pattern
    vector<array<string, 3>> matches(endPattern - startPattern);
    for (int i = startPattern; i < endPattern; i++) {
        int count = 0;
        size_t pos = text.find(patterns[i]);
        while (pos != string::npos) {
            count++;
            pos = text.find(patterns[i], pos + 1);
        }
        if (i - startPattern < matches.size()) {
            matches[i - startPattern] = {patterns[i], to_string(count), hostname};
        } else {
            cout << "Error: matches index " << i - startPattern << " out of bounds" << endl;
        }
    }

    //print results
    cout << "Printing results from process " << rank << ":" << endl;
    for (int i = 0; i < matches.size(); i++) {
        cout << "El patron " << matches[i][0] << " aparece " << matches[i][1] << " veces. Buscado por " << matches[i][2] << endl;
    }

    // Gather results from all processes
    vector<array<string, 3>> allMatches(numPatterns);
    MPI_Gather(matches.data(), matches.size() * 3, MPI_CHAR, allMatches.data(), 50, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Print results from root process
    if (rank == 0) {
        cout << "Printing results:" << endl;
        if (allMatches.size() == numPatterns) {
            for (int i = 0; i < numPatterns; i++) {
                cout << "El patron " << i << " aparece " << allMatches[i][1] << " veces. Buscado por " << allMatches[i][2] << endl;
            }
        } else {
            cout << "Error: allMatches has size " << allMatches.size() << ", expected " << numPatterns << endl;
        }
    }

    MPI_Finalize();

    return allMatches;
}

int main(int argc, char** argv) {
    cout << "Parallel pattern matching" << endl;
    MPI_Init(&argc, &argv);
    parallelPatternMatching();
    return 0;
}