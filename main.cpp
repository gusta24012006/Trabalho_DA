#include <iostream>
#include "CsvReader.h"
#include "Assigner.h"

int main() {
    Conference conf;

    // Loading data
    if (CsvReader::parseFile("dataset1.csv", conf)) {
        std::cout << "Submissions: " << conf.submissions.size() << std::endl;
        std::cout << "Reviewers: " << conf.reviewers.size() << std::endl;
        std::cout << "MinReviews: " << conf.params.minReviewsPerSubmission << std::endl;
        std::cout << "OutputFile: " << conf.control.outputFileName << std::endl;


        std::cout << "\n--- Algoritmo (Max Flow) ---\n";
        Assigner::buildAndRun(conf);

    } else {
        std::cerr << "Erro ao ler o ficheiro." << std::endl;
    }

    return 0;
}

