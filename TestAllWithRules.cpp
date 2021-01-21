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
#include <cstdlib>
using namespace std;


//Files
#include "dirent.h"


//Classes utilizadas
#include "AlbhwStructure.cpp"
#include "AlbhwModel.cpp"
#include "AlbhwHeuristic.cpp"
#include "ResultsModel.h"
#include "TestClass.cpp"

// DEFINES

#define REGRASTRAB 3
#define REGRASTAREFAS 8

int main(int argc, char* argv[] ) {
    string regraT(argv[1]);
    string regraTR(argv[2]);
    vector <string> pathS, pathM;
    AlbhwHeuristicResult *res;
    AlbhwHeuristic *heur;
    vector <string> smallInstances = Util::addSmallPaths();
    vector <string> mediumInstances = Util::addMediumPaths();
    ofstream arqS("Results/Perprof/PerprofSmall_" + regraT + "_" + regraTR + ".txt", ios::app);
    int contador = 0;
    for (int i = 0; i < 4; ++i) {
        pathS = Util::getSmallPaths(i);
        for (int j = 0; j < pathS.size(); ++j) {
            //Caminho para cada instância
            heur = new AlbhwHeuristic(smallInstances[i] + pathS[j]);
            // Monta estrutura para ler arquivo
            res = heur->Heuristic(atoi(regraT.c_str()), atoi(regraTR.c_str()));
            arqS << "Inst" + to_string(contador) + " ";
            arqS << "converged ";
            arqS << res->valorObjetivoHeur << "\n";
            contador++;
        }
    }
    arqS << "\n";
    arqS.close();
    ofstream arqM("Results/Perprof/PerprofMedium_" + regraT + "_" + regraTR + ".txt", ios::app);
    for (int i = 0; i < 4; ++i) {
        pathM = Util::getMediumPaths(i);
        for (int j = 0; j < pathM.size(); ++j) {
            //Caminho para cada instância
            heur = new AlbhwHeuristic(mediumInstances[i] + pathM[j]);
            // Monta estrutura para ler arquivo
            res = heur->Heuristic(atoi(regraT.c_str()), atoi(regraTR.c_str()));
            arqM << "Inst" + to_string(contador) + " ";
            arqM << "converged ";
            arqM << res->valorObjetivoHeur << "\n";
            contador++;
        }
    }
    arqM << "\n";
    arqM.close();
}
