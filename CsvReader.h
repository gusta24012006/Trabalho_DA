#ifndef CSVREADER_H
#define CSVREADER_H
#include <string>      // para std::string
#include "Conference.h" // para o tipo Conference
class CsvReader {
public:
    static bool parseFile(const std::string &filename, Conference &conf);
private:
    static void trim(std::string &s);
};
#endif
