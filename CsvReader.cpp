#include "CsvReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

void CsvReader::trim(string &s) {
    while (!s.empty() && isspace(s.front())) s.erase(s.begin());
    while (!s.empty() && isspace(s.back())) s.pop_back();
}

bool CsvReader::parseFile(const string &filename, Conference &conf) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro: não foi possível abrir o ficheiro " << filename << endl;
        return false;
    }

    string line;
    string section;
    while (getline(file, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') {
            if (line.rfind("#Submissions", 0) == 0) section = "Submissions";
            else if (line.rfind("#Reviewers", 0) == 0) section = "Reviewers";
            else if (line.rfind("#Parameters", 0) == 0) section = "Parameters";
            else if (line.rfind("#Control", 0) == 0) section = "Control";
            continue;
        }

        stringstream ss(line);
        string field;
        vector<string> fields;
        while (getline(ss, field, ',')) {
            trim(field);
            fields.push_back(field);
        }

        if (section == "Submissions" && fields.size() >= 5) {
            int id = stoi(fields[0]);
            conf.submissions.emplace_back(
                    id, fields[1], fields[2], fields[3],
                    stoi(fields[4]), fields.size() >= 6 ? stoi(fields[5]) : 0
            );
        } else if (section == "Reviewers" && fields.size() >= 4) {
            int id = stoi(fields[0]);
            conf.reviewers.emplace_back(
                    id, fields[1], fields[2],
                    stoi(fields[3]), fields.size() >= 5 ? stoi(fields[4]) : 0
            );
        } else if (section == "Parameters" && fields.size() == 2) {
            string param = fields[0];
            int value = stoi(fields[1]);
            if (param == "MinReviewsPerSubmission") conf.params.minReviewsPerSubmission = value;
            else if (param == "MaxReviewsPerReviewer") conf.params.maxReviewsPerReviewer = value;
            else if (param == "PrimaryReviewerExpertise") conf.params.primaryReviewerExpertise = value;
            else if (param == "SecondaryReviewerExpertise") conf.params.secondaryReviewerExpertise = value;
            else if (param == "PrimarySubmissionDomain") conf.params.primarySubmissionDomain = value;
            else if (param == "SecondarySubmissionDomain") conf.params.secondarySubmissionDomain = value;
        } else if (section == "Control" && fields.size() >= 2) {
            string param = fields[0];
            if (param == "GenerateAssignments") conf.control.generateAssignments = stoi(fields[1]);
            else if (param == "RiskAnalysis") conf.control.riskAnalysis = stoi(fields[1]);
            else if (param == "OutputFileName") {
                string name = fields[1];
                if (!name.empty() && name.front() == '"') name.erase(name.begin());
                if (!name.empty() && name.back() == '"') name.pop_back();
                conf.control.outputFileName = name;
            }
        }
    }

    file.close();
    return true;
}

