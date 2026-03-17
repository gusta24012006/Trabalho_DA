#ifndef ASSIGNER_H
#define ASSIGNER_H
#include "Conference.h"
#include "graph.h"
#include <map>
#include <string>
class Assigner {
public:
    static void buildAndRun(const Conference &conf);
};
#endif