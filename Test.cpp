//
// Created by nicolas on 16/04/19.
//
//Bibliotecas
#include <gurobi_c++.h>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <algorithm>    // std::sort
#include <iostream>
#include <math.h>
using namespace std;


//Files
#include "dirent.h"
#include "TestHeur.cpp"
#include "TestModels.cpp"

//Classes utilizadas

// DEFINES


int main(int argc, char* argv[] ) {
    string entry(argv[1]);
    string filename(argv[2]);
    string destination(argv[3]);
    string instance(entry + filename);
    string outputRulesHeuristic(argv[4]);
    TestHeuristics* t = new TestHeuristics(filename, destination, instance, outputRulesHeuristic);
    t->main();
    int numS = t->numS;
    int melhorTR = t->TR;
    int melhorTA = t->TA;
    delete t;
    TestModel* mold = new TestModel();
    mold->testImprovement(melhorTA, melhorTR, numS,destination, instance);
    delete mold;
    return 0;
}
