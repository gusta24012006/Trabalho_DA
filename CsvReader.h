#ifndef CSVREADER_H
#define CSVREADER_H
 
#include <string>
#include "Conference.h"
 
/// @brief Classe responsavel pela leitura e parsing do ficheiro CSV de entrada.
class CsvReader {
public:
    /// @brief Le e interpreta o ficheiro CSV e preenche a estrutura conf.
    /// Devolve true em caso de sucesso, false se o ficheiro nao puder ser aberto.
    /// @complexity O(L) onde L = numero de linhas do ficheiro
    static bool parseFile(const std::string &filename, Conference &conf);
 
private:
    /// @brief Remove espacos em branco no inicio e no fim da string s.
    static void trim(std::string &s);
};
 
#endif