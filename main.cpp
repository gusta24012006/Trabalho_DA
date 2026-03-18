#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include "CsvReader.h"
#include "Assigner.h"

using namespace std;

static void runInteractiveMenu();
static void runBatchMode(const string &inputFile, const string &outputFile);
static void printSeparator();
static void printHeader(const string &title);
static void listSubmissions(const Conference &conf);
static void listReviewers(const Conference &conf);
static void showParameters(const Conference &conf);
static void showControl(const Conference &conf);
static int  getIntInput(const string &prompt, int min, int max);

// Ponto de entrada. Modo interativo se sem argumentos,
// modo batch se chamado com: ./myProg -b input.csv output.csv
int main(int argc, char *argv[]) {
    if (argc == 4 && string(argv[1]) == "-b") {
        runBatchMode(string(argv[2]), string(argv[3]));
        return 0;
    }
    if (argc > 1) {
        cerr << "Uso:\n"
             << "  Interativo : " << argv[0] << "\n"
             << "  Batch      : " << argv[0] << " -b input.csv output.csv\n";
        return 1;
    }
    runInteractiveMenu();
    return 0;
}

// Le o ficheiro CSV, substitui o nome do ficheiro de saida pelo argumento
// da linha de comandos e executa o algoritmo de atribuicao.
static void runBatchMode(const string &inputFile, const string &outputFile) {
    Conference conf;
    if (!CsvReader::parseFile(inputFile, conf)) {
        cerr << "[ERRO] Nao foi possivel ler: " << inputFile << "\n";
        return;
    }
    conf.control.outputFileName = outputFile;
    cout << "[BATCH] " << conf.submissions.size() << " submissoes, "
         << conf.reviewers.size() << " revisores.\n";
    cout << "[BATCH] GenerateAssignments=" << conf.control.generateAssignments
         << "  RiskAnalysis=" << conf.control.riskAnalysis << "\n";
    Assigner::buildAndRun(conf);
}

//Menu interativo que permite carregar dados, consultar informacao
//e configurar e executar o algoritmo de atribuicao.
static void runInteractiveMenu() {
    Conference conf;
    bool dataLoaded = false;

    while (true) {
        printHeader("Scientific Conference Assignment Tool");
        cout << "  1. Carregar dataset (CSV)\n";
        cout << "  2. Listar submissoes\n";
        cout << "  3. Listar revisores\n";
        cout << "  4. Ver parametros\n";
        cout << "  5. Ver configuracoes de controlo\n";
        cout << "  6. Executar algoritmo de atribuicao\n";
        cout << "  7. Alterar modo de atribuicao (GenerateAssignments)\n";
        cout << "  8. Alterar nivel de analise de risco (RiskAnalysis)\n";
        cout << "  9. Alterar nome do ficheiro de saida\n";
        cout << "  0. Sair\n";
        printSeparator();

        int choice = getIntInput("Opcao", 0, 9);

        switch (choice) {

            // Carrega um novo dataset a partir de um ficheiro CSV
            case 1: {
                cout << "Nome do ficheiro CSV: ";
                string fname;
                cin >> fname;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                Conference newConf;
                if (CsvReader::parseFile(fname, newConf)) {
                    conf = newConf;
                    dataLoaded = true;
                    cout << "[OK] " << conf.submissions.size() << " submissoes e "
                         << conf.reviewers.size() << " revisores carregados de \""
                         << fname << "\".\n";
                } else {
                    cerr << "[ERRO] Nao foi possivel carregar: " << fname << "\n";
                }
                break;
            }

            case 2: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                listSubmissions(conf);
                break;
            }

            case 3: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                listReviewers(conf);
                break;
            }

            case 4: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                showParameters(conf);
                break;
            }

            case 5: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                showControl(conf);
                break;
            }

            // Executa o algoritmo com as configuracoes atuais
            case 6: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                printHeader("Executar Atribuicao");
                cout << "  Modo (GenerateAssignments) : " << conf.control.generateAssignments << "\n";
                cout << "  Analise de risco           : " << conf.control.riskAnalysis        << "\n";
                cout << "  Ficheiro de saida          : " << conf.control.outputFileName      << "\n";
                printSeparator();
                Assigner::buildAndRun(conf);
                break;
            }

            // Altera o modo de compatibilidade usado na atribuicao
            case 7: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                printHeader("Modos de Atribuicao");
                cout << "  0 - Executar sem gerar ficheiro\n";
                cout << "  1 - Dominio primario da submissao + expertise primaria do revisor\n";
                cout << "  2 - Dominios primario e secundario da submissao + expertise primaria\n";
                cout << "  3 - Todos os dominios primarios e secundarios de ambos\n";
                printSeparator();
                int mode = getIntInput("Modo", 0, 3);
                conf.control.generateAssignments = mode;
                cout << "[OK] GenerateAssignments definido para " << mode << ".\n";
                break;
            }

            // Define o nivel K da analise de risco
            case 8: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                cout << "  0 - Desativado\n";
                cout << "  1 - Ausencia de um revisor\n";
                cout << "  K - Ausencia de K revisores em simultaneo\n";
                int k = getIntInput("K", 0, (int)conf.reviewers.size());
                conf.control.riskAnalysis = k;
                cout << "[OK] RiskAnalysis definido para " << k << ".\n";
                break;
            }

            // Altera o nome do ficheiro de saida
            case 9: {
                if (!dataLoaded) { cerr << "[ERRO] Nenhum dataset carregado.\n"; break; }
                cout << "Ficheiro atual: " << conf.control.outputFileName << "\n";
                cout << "Novo nome: ";
                string fname;
                cin >> fname;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (fname.empty()) {
                    cerr << "[ERRO] O nome nao pode ser vazio.\n";
                } else {
                    conf.control.outputFileName = fname;
                    cout << "[OK] Ficheiro de saida definido para \"" << fname << "\".\n";
                }
                break;
            }

            case 0:
                cout << "Ate logo!\n";
                return;
        }

        cout << "\nPressione Enter para continuar...";
        cin.get();
    }
}

//Imprime uma linha separadora.
static void printSeparator() {
    cout << string(55, '-') << "\n";
}

//Imprime um cabecalho com titulo.
static void printHeader(const string &title) {
    cout << "\n";
    printSeparator();
    cout << "  " << title << "\n";
    printSeparator();
}

//Lista todas as submissoes em formato de tabela.
static void listSubmissions(const Conference &conf) {
    printHeader("Submissoes (" + to_string(conf.submissions.size()) + " total)");
    cout << left
         << setw(6)  << "ID"
         << setw(35) << "Titulo"
         << setw(22) << "Autores"
         << setw(8)  << "Primario"
         << setw(8)  << "Secundario"
         << "\n";
    printSeparator();
    for (const auto &s : conf.submissions) {
        string title  = s.title.size()   > 33 ? s.title.substr(0,30)   + "..." : s.title;
        string author = s.authors.size() > 20 ? s.authors.substr(0,17) + "..." : s.authors;
        cout << left
             << setw(6)  << s.id
             << setw(35) << title
             << setw(22) << author
             << setw(8)  << s.primaryDomain
             << setw(8)  << (s.secondaryDomain != 0 ? to_string(s.secondaryDomain) : "-")
             << "\n";
    }
}

//Lista todos os revisores em formato de tabela.
static void listReviewers(const Conference &conf) {
    printHeader("Revisores (" + to_string(conf.reviewers.size()) + " total)");
    cout << left
         << setw(6)  << "ID"
         << setw(28) << "Nome"
         << setw(28) << "Email"
         << setw(10) << "Primario"
         << setw(10) << "Secundario"
         << "\n";
    printSeparator();
    for (const auto &r : conf.reviewers) {
        string name  = r.name.size()  > 26 ? r.name.substr(0,23)  + "..." : r.name;
        string email = r.email.size() > 26 ? r.email.substr(0,23) + "..." : r.email;
        cout << left
             << setw(6)  << r.id
             << setw(28) << name
             << setw(28) << email
             << setw(10) << r.primaryExpertise
             << setw(10) << (r.secondaryExpertise != 0 ? to_string(r.secondaryExpertise) : "-")
             << "\n";
    }
}

//Mostra os parametros do algoritmo.
static void showParameters(const Conference &conf) {
    printHeader("Parametros");
    cout << "  MinReviewsPerSubmission    : " << conf.params.minReviewsPerSubmission    << "\n";
    cout << "  MaxReviewsPerReviewer      : " << conf.params.maxReviewsPerReviewer      << "\n";
    cout << "  PrimaryReviewerExpertise   : " << conf.params.primaryReviewerExpertise   << "\n";
    cout << "  SecondaryReviewerExpertise : " << conf.params.secondaryReviewerExpertise << "\n";
    cout << "  PrimarySubmissionDomain    : " << conf.params.primarySubmissionDomain    << "\n";
    cout << "  SecondarySubmissionDomain  : " << conf.params.secondarySubmissionDomain  << "\n";
}

//Mostra as configuracoes de controlo atuais.
static void showControl(const Conference &conf) {
    printHeader("Configuracoes de Controlo");
    cout << "  GenerateAssignments : " << conf.control.generateAssignments << "\n";
    cout << "  RiskAnalysis        : " << conf.control.riskAnalysis        << "\n";
    cout << "  OutputFileName      : " << conf.control.outputFileName      << "\n";
}

//Pede um inteiro ao utilizador no intervalo [min, max].
//Repete ate receber um valor valido.
static int getIntInput(const string &prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt << " [" << min << "-" << max << "]: ";
        if (cin >> value && value >= min && value <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cerr << "[ERRO] Introduza um numero entre " << min << " e " << max << ".\n";
    }
}
