//
// Created by nicolas on 27/04/19.
//

#include "TestClass.h"
#include "AlbhwHeuristic.h"

void TestClass::RunAllHeuristics() {
    /*
   // Run Heuristics
   // Run VNN */
    vector <string> pathS, pathM;
    AlbhwHeuristicResult* res = new AlbhwHeuristicResult();
    AlbhwHeuristic* heur;
    vector<string> smallInstances = Util::addSmallPaths();
    vector<string> mediumInstances = Util::addMediumPaths();
    for (int i = 0; i < 4; ++i) {
        pathS = Util::getSmallPaths(i);
        pathM = Util::getMediumPaths(i);
        for(int j = 0; j < int(pathS[i].size()); ++j) {
            //Caminho para cada instância
            for(int regraTarefas = 0; regraTarefas < 8; ++regraTarefas) {
                for(int regraTrab = 0; regraTrab < 3; ++regraTrab ) {
                    heur = new AlbhwHeuristic(smallInstances[i] + pathS[j]);
                    // Monta estrutura para ler arquivo
                    res = heur->Heuristic(regraTarefas, regraTrab);
                }
            }
        }


    }
}


TestClass::TestClass() {

}

