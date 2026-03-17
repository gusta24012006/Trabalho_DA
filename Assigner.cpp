#include "Assigner.h"
#include <fstream>
#include <iostream>
#include <map>
using namespace std;


static void runRiskAnalysis(const Conference &conf, int baselineFlow);

//Criaçao do ficheiro CSV
static void saveAssignments(const Graph &g, const Conference &conf, const string &filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "Erro: não foi possivel criar o ficheiro " << filename << endl;
        return;
    }

    const int nSubs = conf.submissions.size();
    const int nRevs = conf.reviewers.size();

    out << "#SubmissionId,ReviewerId,Match\n";

    int total = 0;
    map<int, vector<pair<int, int>>> reviewerAssignments;


    for (int i = 0; i < nSubs; ++i) {
        for (const auto &e : g.getAdj()[1 + i]) {
            if (e.to > nSubs && e.to <= nSubs + nRevs && e.flow > 0) {
                int subId = conf.submissions[i].id;
                int revIdx = e.to - (1 + nSubs);
                int revId = conf.reviewers[revIdx].id;
                int matchDomain = conf.submissions[i].primaryDomain;

                out << subId << "," << revId << "," << matchDomain << "\n";
                reviewerAssignments[revId].push_back({subId, matchDomain});
                total++;
            }
        }
    }

    out << "#Total: " << total << "\n";

    //Reviewer para Submission
    out << "#ReviewerId,SubmissionId,Match\n";
    for (const auto &[revId, subs] : reviewerAssignments) {
        for (const auto &[subId, match] : subs)
            out << revId << "," << subId << "," << match << "\n";
    }

    out << "#Total: " << total << "\n";
    out.close();

    cout << "\nFicheiro \"" << filename << "\" criado com " << total << " atribuicoes.\n";
}

// Contruçao do  grafo e execucao  do Max Flow
void Assigner::buildAndRun(const Conference &conf) {
    int nSubs = conf.submissions.size();
    int nRevs = conf.reviewers.size();

    int source = 0;
    int sink = nSubs + nRevs + 1;
    Graph g(sink + 1);

    int minReviews = conf.params.minReviewsPerSubmission;
    int maxReviews = conf.params.maxReviewsPerReviewer;

    // SOURCE para Submissions
    for (int i = 0; i < nSubs; i++)
        g.addEdge(source, 1 + i, minReviews);

    // Submissions para Reviewers (verifica compatibilidade)
    for (int i = 0; i < nSubs; ++i) {
        const auto &sub = conf.submissions[i];
        for (int j = 0; j < nRevs; ++j) {
            const auto &rev = conf.reviewers[j];
            bool match = (sub.primaryDomain == rev.primaryExpertise) ||
                         (sub.primaryDomain == rev.secondaryExpertise) ||
                         (sub.secondaryDomain != 0 &&
                          (sub.secondaryDomain == rev.primaryExpertise ||
                           sub.secondaryDomain == rev.secondaryExpertise));
            if (match)
                g.addEdge(1 + i, 1 + nSubs + j, 1);
        }
    }

    for (int j = 0; j < nRevs; j++)
        g.addEdge(1 + nSubs + j, sink, maxReviews);

    // Execucao Edmonds–Karp (Max Flow)
    int maxFlow = g.edmondsKarp(source, sink);
    cout << "Fluxo maximo (numero total de atribuicoes): " << maxFlow << "\n";

    // Guarda resultados no ficheiro no CSV
    saveAssignments(g, conf, conf.control.outputFileName);

    if (conf.control.riskAnalysis == 1) {
        cout << "\n--- Risk Analysis (R = 1) ---\n";
        runRiskAnalysis(conf, maxFlow);
    }
}

//Risk analysis k=1
static void runRiskAnalysis(const Conference &conf, int baselineFlow) {
    int nSubs = conf.submissions.size();
    int nRevs = conf.reviewers.size();

    int source = 0;
    int sink = nSubs + nRevs + 1;

    vector<int> criticalReviewers;
    int minRequiredFlow = nSubs * conf.params.minReviewsPerSubmission;

    for (int missing = 0; missing < nRevs; ++missing) {
        Graph g(sink + 1);


        for (int i = 0; i < nSubs; i++)
            g.addEdge(source, 1 + i, conf.params.minReviewsPerSubmission);

        for (int i = 0; i < nSubs; ++i) {
            const auto &sub = conf.submissions[i];
            for (int j = 0; j < nRevs; ++j) {
                if (j == missing) continue;
                const auto &rev = conf.reviewers[j];
                bool match = (sub.primaryDomain == rev.primaryExpertise) ||
                             (sub.primaryDomain == rev.secondaryExpertise) ||
                             (sub.secondaryDomain != 0 &&
                              (sub.secondaryDomain == rev.primaryExpertise ||
                               sub.secondaryDomain == rev.secondaryExpertise));
                if (match)
                    g.addEdge(1 + i, 1 + nSubs + j, 1);
            }
        }

        // Reviewers (exceto ausente)
        for (int j = 0; j < nRevs; j++) {
            if (j == missing) continue;
            g.addEdge(1 + nSubs + j, sink, conf.params.maxReviewsPerReviewer);
        }

        int newFlow = g.edmondsKarp(source, sink);

        if (newFlow < minRequiredFlow)
            criticalReviewers.push_back(conf.reviewers[missing].id);
    }

    if (criticalReviewers.empty()) {
        cout << "#Risk Analysis: M (none)" << endl;
    } else {
        cout << "#Risk Analysis: M ";
        for (size_t i = 0; i < criticalReviewers.size(); ++i) {
            cout << criticalReviewers[i];
            if (i < criticalReviewers.size() - 1) cout << ",";
        }
        cout << endl;
    }
}


