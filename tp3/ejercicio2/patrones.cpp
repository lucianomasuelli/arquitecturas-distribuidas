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

    // Get hostname and IP address
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct hostent* host = gethostbyname(hostname);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, host->h_addr_list[0], ip, INET_ADDRSTRLEN);

    // Read patterns from file
    vector<string> patterns;
    ifstream patternFile("patterns.txt");
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
    ifstream textFile("text.txt");
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
        matches[i - startPattern] = {patterns[i], to_string(count), ip};
    }

    // Gather results from all processes
    vector<array<string, 3>> allMatches(numPatterns);
    MPI_Gather(matches.data(), matches.size() * 3, MPI_CHAR, allMatches.data(), 3, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Print results from root process
    if (rank == 0) {
        for (int i = 0; i < numPatterns; i++) {
            cout << "Pattern " << allMatches[i][0] << ": " << allMatches[i][1] << " matches (searched by " << allMatches[i][2] << ")" << endl;
        }
    }

    MPI_Finalize();

    return allMatches;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    parallelPatternMatching();
    return 0;
}