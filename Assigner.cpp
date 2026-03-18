#include "Assigner.h"
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
using namespace std;

static void runRiskAnalysis(const Conference &conf, int baselineFlow, int mode);

/// @brief Verifica se uma submissao e um revisor sao compativeis de acordo com o modo.
/// Modo 1: apenas dominios primarios. Modo 2: dominio secundario da submissao permitido.
/// Modo 3: todos os dominios primarios e secundarios de ambos os lados.
/// @complexity O(1)
static bool isCompatible(const Submission &sub, const Reviewer &rev, int mode) {
    switch (mode) {
        case 1:
            return sub.primaryDomain == rev.primaryExpertise;
        case 2:
            return (sub.primaryDomain == rev.primaryExpertise) ||
                   (sub.secondaryDomain != 0 &&
                    sub.secondaryDomain == rev.primaryExpertise);
        case 3:
        default:
            return (sub.primaryDomain == rev.primaryExpertise) ||
                   (sub.primaryDomain == rev.secondaryExpertise) ||
                   (sub.secondaryDomain != 0 &&
                    (sub.secondaryDomain == rev.primaryExpertise ||
                     sub.secondaryDomain == rev.secondaryExpertise));
    }
}

/// @brief Devolve o dominio da submissao que originou a compatibilidade com o revisor.
/// @complexity O(1)
static int matchedDomain(const Submission &sub, const Reviewer &rev, int mode) {
    if (mode == 1)
        return sub.primaryDomain;
    if (mode == 2)
        return (sub.primaryDomain == rev.primaryExpertise)
               ? sub.primaryDomain : sub.secondaryDomain;
    if (sub.primaryDomain == rev.primaryExpertise ||
        sub.primaryDomain == rev.secondaryExpertise)
        return sub.primaryDomain;
    return sub.secondaryDomain;
}

/// @brief Escreve as atribuicoes no ficheiro CSV de saida.
/// Formato: submissao -> revisor e revisor -> submissao, ordenados, com total no fim.
/// @complexity O(S * R)
static void saveAssignments(const Graph &g, const Conference &conf,
                            const string &filename, int mode) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "Erro: nao foi possivel criar o ficheiro " << filename << endl;
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
                int subId  = conf.submissions[i].id;
                int revIdx = e.to - (1 + nSubs);
                int revId  = conf.reviewers[revIdx].id;
                int dom    = matchedDomain(conf.submissions[i], conf.reviewers[revIdx], mode);

                out << subId << ", " << revId << ", " << dom << "\n";
                reviewerAssignments[revId].push_back({subId, dom});
                total++;
            }
        }
    }

    out << "#ReviewerId,SubmissionId,Match\n";
    for (auto &[revId, subs] : reviewerAssignments) {
        sort(subs.begin(), subs.end());
        for (const auto &[subId, dom] : subs)
            out << revId << ", " << subId << ", " << dom << "\n";
    }

    out << "#Total: " << total << "\n";
    out.close();

    cout << "\nFicheiro \"" << filename << "\" criado com " << total << " atribuicoes.\n";
}

/// @brief Regista no ficheiro as submissoes que nao receberam o minimo de revisoes.
/// @complexity O(S * R)
static void saveMissingReviews(const Graph &g, const Conference &conf,
                               const string &filename) {
    const int nSubs  = conf.submissions.size();
    const int nRevs  = conf.reviewers.size();
    const int minRev = conf.params.minReviewsPerSubmission;

    vector<int> assigned(nSubs, 0);
    for (int i = 0; i < nSubs; ++i)
        for (const auto &e : g.getAdj()[1 + i])
            if (e.to > nSubs && e.to <= nSubs + nRevs && e.flow > 0)
                assigned[i]++;

    ofstream out(filename, ios::app);
    if (!out.is_open()) {
        cerr << "Erro: nao foi possivel abrir o ficheiro " << filename << endl;
        return;
    }

    out << "#SubmissionId,Domain,MissingReviews\n";
    for (int i = 0; i < nSubs; ++i) {
        int missing = minRev - assigned[i];
        if (missing > 0) {
            out << conf.submissions[i].id << ", "
                << conf.submissions[i].primaryDomain << ", "
                << missing << "\n";
            cerr << "[AVISO] Submissao " << conf.submissions[i].id
                 << " esta em falta " << missing << " revisao(oes).\n";
        }
    }
    out.close();
}

/// @brief Constroi o grafo de fluxo e executa o algoritmo de Edmonds-Karp.
/// Respeita o valor de GenerateAssignments para escolher o modo de compatibilidade
/// e decidir se o ficheiro de saida e gerado. Corre a analise de risco se configurada.
/// @complexity O(V * E^2) onde V = S+R+2, E = S*R+S+R
void Assigner::buildAndRun(const Conference &conf) {
    int nSubs = conf.submissions.size();
    int nRevs = conf.reviewers.size();

    if (nSubs == 0) { cerr << "[ERRO] Nenhuma submissao carregada.\n"; return; }
    if (nRevs == 0) { cerr << "[ERRO] Nenhum revisor carregado.\n";    return; }

    int source = 0;
    int sink   = nSubs + nRevs + 1;
    Graph g(sink + 1);

    int minReviews = conf.params.minReviewsPerSubmission;
    int maxReviews = conf.params.maxReviewsPerReviewer;

    // Quando GenerateAssignments == 0 usa modo 1 mas nao escreve o ficheiro
    int mode = (conf.control.generateAssignments == 0)
               ? 1 : conf.control.generateAssignments;

    // Source -> Submissoes
    for (int i = 0; i < nSubs; i++)
        g.addEdge(source, 1 + i, minReviews);

    // Submissoes -> Revisores (so se forem compativeis)
    for (int i = 0; i < nSubs; ++i) {
        const auto &sub = conf.submissions[i];
        for (int j = 0; j < nRevs; ++j) {
            const auto &rev = conf.reviewers[j];
            if (isCompatible(sub, rev, mode))
                g.addEdge(1 + i, 1 + nSubs + j, 1);
        }
    }

    // Revisores -> Sink
    for (int j = 0; j < nRevs; j++)
        g.addEdge(1 + nSubs + j, sink, maxReviews);

    int maxFlow    = g.edmondsKarp(source, sink);
    int minRequired = nSubs * minReviews;

    cout << "Fluxo maximo (numero total de atribuicoes): " << maxFlow << "\n";
    cout << "Fluxo minimo necessario                  : " << minRequired << "\n";

    if (maxFlow >= minRequired)
        cout << "Resultado: atribuicao completa.\n";
    else
        cout << "Resultado: atribuicao parcial (algumas submissoes sem revisoes suficientes).\n";

    if (conf.control.generateAssignments != 0) {
        saveAssignments(g, conf, conf.control.outputFileName, mode);
        if (maxFlow < minRequired)
            saveMissingReviews(g, conf, conf.control.outputFileName);
    }

    if (conf.control.riskAnalysis >= 1) {
        cout << "\n--- Analise de Risco (K=" << conf.control.riskAnalysis << ") ---\n";
        runRiskAnalysis(conf, maxFlow, mode);
    }
}

/// @brief Analise de risco K=1: remove cada revisor individualmente e verifica
/// se o fluxo minimo ainda e alcancavel. Lista os revisores criticos.
/// @complexity O(R * V * E^2) onde R = numero de revisores
static void runRiskAnalysis(const Conference &conf, int baselineFlow, int mode) {
    int nSubs = conf.submissions.size();
    int nRevs = conf.reviewers.size();

    int source = 0;
    int sink   = nSubs + nRevs + 1;

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
                if (isCompatible(sub, rev, mode))
                    g.addEdge(1 + i, 1 + nSubs + j, 1);
            }
        }

        for (int j = 0; j < nRevs; j++) {
            if (j == missing) continue;
            g.addEdge(1 + nSubs + j, sink, conf.params.maxReviewsPerReviewer);
        }

        int newFlow = g.edmondsKarp(source, sink);

        if (newFlow < minRequiredFlow)
            criticalReviewers.push_back(conf.reviewers[missing].id);
    }

    sort(criticalReviewers.begin(), criticalReviewers.end());

    // Escreve resultado no stdout e no ficheiro de saida
    string riskLine = "#Risk Analysis: " + to_string(conf.control.riskAnalysis) + "\n";
    string reviewerLine = "";
    if (!criticalReviewers.empty()) {
        for (size_t i = 0; i < criticalReviewers.size(); ++i) {
            reviewerLine += to_string(criticalReviewers[i]);
            if (i < criticalReviewers.size() - 1) reviewerLine += ", ";
        }
        reviewerLine += "\n";
    }

    cout << riskLine;
    if (criticalReviewers.empty())
        cout << "(none)\n";
    else
        cout << reviewerLine;

    // Escreve no ficheiro CSV
    ofstream out(conf.control.outputFileName, ios::app);
    if (out.is_open()) {
        out << riskLine;
        if (!criticalReviewers.empty())
            out << reviewerLine;
        out.close();
    }
}