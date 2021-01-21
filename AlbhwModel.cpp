//
// Created by nicolas on 16/04/19.
//
#include "AlbhwModel.h"
#include "ResultsModel.h"
#include "AlbhwStructure.h"
#include "Util.h"
#include <sstream>
#include <cstdio>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
int* AlbhwModel::improveMh(int **twTimes, std::vector<int> tTypes, int numT, int numW, int cycT) {
    int* improvedMh = new int[numW];
    for(int w = 0; w < numW; ++w) {
        GRBModel model = GRBModel(this->env);
        try {
            int i;
            GRBVar x[numT];
            GRBLinExpr expr1;
            // Variável binária Alfa_ih
            for(i = 0; i < numT; i++) {
                if(w <= tTypes[i])
                    x[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "TrabxEstacao");
            }
            model.update();
            // Função Objetivo de Minimização
            expr1 = 0;
            for(i = 0; i < numT; i++) {
                if(w <= tTypes[i])
                    expr1 += x[i];
            }
            model.setObjective(expr1, GRB_MAXIMIZE);
            model.set(GRB_StringAttr_ModelName, "ALBHW");

            // Restrição que garante que os empregados serão colocados em alguma estação
            expr1 = 0;
            for(i = 0; i < numT; ++i) {
                if(w <= tTypes[i])
                    expr1 += (twTimes[i][w] * x[i]);
            }
            model.addConstr(expr1, GRB_LESS_EQUAL, cycT, "Restrição 4 - ImproveMh");

            for(i = 0; i < numT; ++i) {
                for(int j = 0; j < numT; ++j) {
                    if(w <= tTypes[i] and w <= tTypes[j]) {
                        if(this->graph->p[i][j] >= 1 or this->graph->p[j][i] >= 1) {
                            model.addConstr((x[i] + x[j]), GRB_LESS_EQUAL, 1, "Restrição 5");
                        }
                    }
                }
            }
            //char c; cin >> c;
            model.optimize();

            improvedMh[w] = model.get(GRB_DoubleAttr_ObjVal);
            cout << "Valor encontrado: " << improvedMh[w] << " Trabalhador " << w;
        }  catch (GRBException e) {
            std::cout << "Error code = " << e.getErrorCode() << std::endl;
            model.computeIIS();
            model.write("test.ilp");
            std::cout << e.getMessage() << std::endl;
            exit(0);
        }
    }
    return improvedMh;
}

int AlbhwModel::improveCycT(int **twTimes, int twTimesOrig, std::vector<int> tTypes, int cycT, int numT, int task, int w) {
    
    GRBModel model = GRBModel(this->env);
    try {
        int i;
        GRBVar x[numT];
        GRBLinExpr expr1;
        // Variável binária Alfa_ih
        for(i = 0; i < numT; i++) {
            if(w <= tTypes[i] && i != task)
                x[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "TrabxEstacao");
        }
        model.update();
        // Função Objetivo de Minimização
        expr1 = 0;
        for(i = 0; i < numT; i++) {
            if(w <= tTypes[i] && i != task)
                expr1 += x[i] * twTimes[i][w];
        }
        model.setObjective(expr1, GRB_MAXIMIZE);
        model.set(GRB_StringAttr_ModelName, "ALBHW");

        // Restrição que garante que os empregados serão colocados em alguma estação
        expr1 = 0;
        for(i = 0; i < numT; ++i) {
            if(w <= tTypes[i] && i != task)
                expr1 += (twTimes[i][w] * x[i]);
        }

        for(i = 0; i < numT; ++i) {
            for(int j = 0; j < numT; ++j) {
                if(w <= tTypes[i] and w <= tTypes[j] and i != task and j != task) {
                    if(this->graph->p[i][j] >= 1 or this->graph->p[j][i] >= 1) {
                       model.addConstr((x[i] + x[j]), GRB_LESS_EQUAL, 1, "Restrição 5");
                    }
                }
            }
        }
        model.addConstr(expr1, GRB_LESS_EQUAL, cycT - twTimesOrig, "Restrição 4");
        model.optimize();

        /*cout << "Tasks choosed with trab " << w << ": ";
        for(i = 0; i < numT; ++i) {
            if(w <= tTypes[i] and i != task and x[i].get(GRB_DoubleAttr_X) ) {
                cout << i << " ";
            }
        }*/
        return int(model.get(GRB_DoubleAttr_ObjVal));
    }  catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        model.computeIIS();
        model.write("test.ilp");
        std::cout << e.getMessage() << std::endl;
        exit(0);
    }
    return 0;
}
int AlbhwModel::minOfWorkers(int h) {
    int sum = 0;
    for(int i = 0; i < this->instanceProblem->numT; ++i) {
        if(this->instanceProblem->tTypes[i] == h) {
            sum += this->instanceProblem->twTimes[i][h];
        }
    }
    return ceil(sum/this->instanceProblem->cycT);
}
AlbhwHeuristicResult* AlbhwModel::resolveAlbhwImproved(int ubStations) {
    AlbhwHeuristicResult* result = new AlbhwHeuristicResult();
    GRBModel model = GRBModel(this->env);
    try{
        this->graph->calculateCriticalPath(ubStations);
        //Variáveis do problema
        int numT = instanceProblem->getNumT();
        int cycT = instanceProblem->getCyc();
        //std::vector<int> tTimes = instanceProblem->gettTimes(); //Tempo da atividade i feita pelo trabalhador h
        std::vector<int> tTypes = instanceProblem->gettTypes();	// Tipos das tarefas (ki)
        std::vector<int> wCosts = instanceProblem->getwCosts(); // Custo dos trabalhadores dos tipos 1,2,3
        //int** twTimes = instanceProblem->gettwTimes();
        //std::vector<int> *predecessors = instanceProblem->getPredecessors();
        int i,h,s;
        int numS = int(ubStations); // Numeros de estações
        int numW = this->instanceProblem->numW; // Quantidade de tipos de trabalhadores

        cout << "Tasks: " << numT << " Workers: " << numW << " numS: " << numS << endl;

        // Gurobi Vars
        GRBVar x[numT][numW][numS];
        GRBVar y[numW][numS];
        GRBVar nWorkers[numW];
        GRBVar v[numS];
        GRBLinExpr expr1, expr2;

        // Variavel inteira Yh
        for(h = 0; h < numW; h++) {
            nWorkers[h] = model.addVar(0, numS, 0, GRB_INTEGER, "name");
        }
        //Variavel binária Xihs
        for (i = 0; i < numT; ++i) {
            for (h = 0; h < numW; ++h) {
                for (s = 0; s < numS; ++s) {
                    x[i][h][s] = model.addVar(0, 1, 0, GRB_BINARY );
                }
            }
        }
        // Variável binária Yhs
        for(h = 0; h < numW; ++h) {
            for(s = 0; s < numS; ++s) {
                y[h][s] = model.addVar(0, 1, 0, GRB_BINARY, "TrabxEstacao");
            }
        }
        // Variavel Vs
        for(s = 0; s < numS; ++s) {
            v[s] = model.addVar(0, 1, 0, GRB_BINARY);
        }

        model.update();

        // Função Objetivo de Minimização
        expr1 = 0;
        for(h = 0; h < numW; h++ ) {
            expr1 += wCosts[h] * nWorkers[h];
        }
        model.setObjective(expr1, GRB_MINIMIZE);
        model.set(GRB_StringAttr_ModelName, "ALBHW");


        // Restrição que garante que apenas um trabalhador fará a tarefa
        for(i = 0; i < numT; i++) {
            expr1 = 0;
            for(h = 0; h <= tTypes[i]; h++) {
                for(s = 0; s < numS; s++ ) {
                    expr1 += x[i][h][s];
                }
            }
            model.addConstr(expr1 , GRB_EQUAL , 1, "Restrição 1");
        }

        // Restrição que conta a quantidade de trabalhadores no total
        for(s = 0; s < numS; s++) {
            expr1 = 0;
            for(h = 0; h < numW; h++ ) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, v[s], "Restrição 2");
        }

        // Set the earliest and latest stations = 0
        for (i = 0; i < numT; ++i) {
            expr1 = 0;
            cout << "Task: " << i << endl;
            for(h = 0; h < numW; ++h) {
                for (s = 0; s <= this->graph->tmin[i+1]-1; ++s) {
                    model.addConstr(x[i][h][s], GRB_EQUAL,0, "Restrição Set Earliest");
                }
                 for (s = this->graph->tmax[i+1]+1; s < numS; ++s) {
                    model.addConstr(x[i][h][s], GRB_EQUAL,0, "Restrição Set Latest");
                }
            }
        }
         // Set the earliest and latest stations = 0
         /*
            for (s = 0; s <= this->graph->tmin[i+1]-1; ++s) {
            for(h = 0; h < numW; ++h) {
                    model.addConstr(y[h][s], GRB_EQUAL,0, "Restrição Set Earliest");
                }
            }
                for (s = this->graph->tmax[i+1]+1; s < numS; ++s) {
                    for(h = 0; h < numW; ++h) {
                    model.addConstr(y[h][s], GRB_EQUAL,0, "Restrição Set Latest");
                    }

            }*/

        //  Verify
        //for(s = ubStations; s < numS; ++s) {
        //    model.addConstr(v[s], GRB_EQUAL, 0);
        //    for(h = 0; h < numW; ++h)
        //        model.addConstr(y[h][s], GRB_EQUAL, 0);
        //}

        int* minWorkerFType = new int[numW];
        minWorkerFType[0] = this->minOfWorkers(0);
        //model.addConstr(nWorkers[0], GRB_LESS_EQUAL, ubStations);

        //minWorkerFType[1] = ubStations-minWorkerFType[0];
        //minWorkerFType[2] = ubStations-minWorkerFType[0];
        //expr1 = 0;
        //for(s = 0; s < numS; ++s)
        //    expr1 += y[0][s];
        model.addConstr(nWorkers[0], GRB_GREATER_EQUAL, minWorkerFType[0]);
        //model.addConstr(expr1, GRB_GREATER_EQUAL, minWorkerFType[0]);
        // Primeiro - Todos
        // Segundo - dois primeiros
        //model.addConstr(nWorkers[1], GRB_LESS_EQUAL, ubStations - minWorkerFType[0]);
        //model.addConstr(nWorkers[2], GRB_LESS_EQUAL, ubStations - minWorkerFType[0]);

        // Restrição que liga Xihs a Yhs - Sugestão 3 do Manuel
        /*for(i = 0; i < numT; i++ ) {
            for(h = 0; h < numW; h++ ) {
                if(h <= tTypes[i]) {
                    for(s = 0; s < numS; s++) {
                        model.addConstr(x[i][h][s], GRB_LESS_EQUAL, y[h][s], "Restrição 3");
                    }
                } else {
                    for(s = 0; s < numS; s++) {
                        model.addConstr(x[i][h][s], GRB_EQUAL, 0, "Restrição 3");
                    }
                }
            }
        } */

        // Restrição que garante que os empregados serão colocados em alguma estação
        for(h = 0; h < numW; h++) {
            expr1 = 0;
            for(s = 0; s < numS; s++) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, nWorkers[h], "Restrição 4");
        }
        // Restrição que garante precedencia e sucessão
        for(int j = 0; j < numT; ++j) {
            for(int i = 0; i < int(graph->allPredecessors[j].size()); ++i) {
                expr1 = expr2 = 0;
                for(h = 0; h <= tTypes[graph->allPredecessors[j].at(i)]; ++h) {
                    for(s = 0; s < numS; ++s) {
                        expr1 += (s+1) * x[graph->allPredecessors[j].at(i)][h][s];
                    }
                }
                for(h = 0; h <= tTypes[j]; ++h) {
                    for(s = 0; s < numS; ++s) {
                        expr2 += (s+1)* x[j][h][s];
                    }
                }
                model.addConstr(expr2, GRB_GREATER_EQUAL, expr1 + max(this->graph->p[graph->allPredecessors[j].at(i)][j],0) , "Restrição Predecessors");
            }
        }
        //Cria o novo vetor e copia os elementos do antigo twTimes
        // Primeira Sugestão Manuel

        /*// Versão II
        int melhorado = 0;
        for(h = 0; h < numW; ++h) {
            for(i = 0; i < numT; ++i) {
                if (h <= tTypes[i]) {
                    melhorado = improvedCycT(ntwTimes, twTimes[i][h],tTypes, cycT,numT, i, h);
                    ntwTimes[i][h] = cycT - melhorado;
                }
                if(ntwTimes[i][h] != twTimes[i][h]) {
                    cout << melhorado << " - " << ntwTimes[i][h] << " " << twTimes[i][h] << endl;
                    char tt; cin >> tt;
                }
            }
        }  */
        /*for (i = 0; i < numT; ++i) {
            for(h = 0; h < numW; ++h) {
                cout << ntwTimes[i][h] << " ";
            }
            cout << endl;
        }*/
        // Restrição que garante que o tempo de dada estação não ultrapasse o tempo de ciclo
        // Segunda sugestão do Manuel
        for(s = 0; s < numS; s++) {
            expr1 = 0;
            for(i = 0; i < numT; i++) {
                for(h = 0; h <= tTypes[i]; h++) {
                    expr1 += this->ntwTimes[i][h] * x[i][h][s];
                    //expr1 += twTimes[i][h] * x[i][h][s];
                }
            }
            model.addConstr(expr1, GRB_LESS_EQUAL, cycT * v[s], "Restrição 6");
            //model.addConstr(expr1, GRB_LESS_EQUAL, cycT, "Restrição 6");
        }

        // Restrição que liga Xihs a Yhs
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < numS; s++ ) {
                expr1 = 0;
                for(i = 0; i < numT; i++) {
                    expr1 += x[i][h][s];
                }
                //aqui vai o improvedMh[h];
                model.addConstr(expr1, GRB_LESS_EQUAL, this->improvedMh[h]*y[h][s], "Restrição 3");
            }
        }

        // Restrição que garante a precedencia das estações em numero 1..2..3..
        for(s = 0; s < numS-1; s++) {
            model.addConstr(v[s], GRB_GREATER_EQUAL, v[s+1], "Restrição 7");
        }
        /*
        std::cout << "Tempo de Ciclo: " << cycT << std::endl;
        std::cout << "Numero de Tarefas: " << numT << std::endl;
        //std::cout << "Numero de Precedencias: " << instanceProblem->getNumPrec() << std::endl;
        std::cout << "Tempo da Tarefa: " << std::endl;
        //for(int i = 0; i < int(tTimes.size()); i++) {
        //    std::cout << i << " : " << tTimes[i] << std::endl;
        //}
        std::cout << "Tipo das Tarefas: " << std::endl;
        for(int i = 0; i < int(tTypes.size()); i++) {
            std::cout << i << " : " << tTypes[i] << std::endl;
        }
        //model.write("out1.sol");
        for(int i = 0; i < numT; i++) {
            std::cout << "Predecessores de " << i << " :" << std::endl;
            for(int j = 0; j < int(predecessors[i].size()); j++) {
                std::cout << predecessors[i].at(j) <<" " << std::endl;
            }
        }
        std::cout << "Custo dos Trabalhadores: " << std::endl;
        for(int i = 0; i < int(wCosts.size()); i++) {
            std::cout << i << " : " << wCosts[i] << std::endl;
        }*/
        model.getEnv().set(GRB_DoubleParam_NodeLimit, GRB_INFINITY);
        model.getEnv().set(GRB_DoubleParam_TimeLimit, 7200.0);
        model.getEnv().set(GRB_IntParam_Threads, 1);
        //model.getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-9);

        model.setCallback(result);
        // Resolve modelo
        //char c; cin >> c;
        model.optimize();

        //model.computeIIS();
        //model.write("model.ilp");
        /*
        int contS;
        for(s = 0; s < numS; ++s) {
            contS = 0;
            for(i = 0; i < numT; ++i) {
                for(h = 0; h < numW; ++h) {
                    if(x[i][h][s].get(GRB_DoubleAttr_X) == 1) {
                        cout << "A tarefa " << i << " com o trab " << h << " na estação " << s << endl;
                        contS += ntwTimes[i][h];
                    }
                }
            }
            cout << "Workstation: " << s << " Time: " << contS << endl;
        }
        for(i = 0; i < numW; i++) {
            result->workers[i] = nWorkers[i].get(GRB_DoubleAttr_X);
            cout << "Trab T " << i << " " << result->workers[i] << endl;

        }*/
        //model.write("out1.sol");
        result->status = model.get(GRB_IntAttr_Status);
        result->lower = model.get(GRB_DoubleAttr_ObjBound);
        result->time = model.get(GRB_DoubleAttr_Runtime);
        result->nodes = model.get(GRB_DoubleAttr_NodeCount);
        result->valorObjetivoModelo = model.get(GRB_DoubleAttr_ObjVal);
        if(result->valorObjetivoModelo != 0 ) {
            result->gap = model.get(GRB_DoubleAttr_MIPGap) * 100;
            for(i = 0; i < numW; i++) {
                if(nWorkers[i].get(GRB_DoubleAttr_X))
                    result->workers[i] = round(nWorkers[i].get(GRB_DoubleAttr_X));
            }
        }
    } catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
        return result;
    }
    exit(0);
}

AlbhwHeuristicResult* AlbhwModel::resolveAlbhwPaper() {
    AlbhwHeuristicResult* result = new AlbhwHeuristicResult();
    GRBModel model = GRBModel(this->env);
    try{
        //Variaveis do problema:
        int numT = instanceProblem->getNumT();
        int cycT = instanceProblem->getCyc();
        //std::vector<int> tTimes = instanceProblem->gettTimes(); //Tempo da atividade i feita pelo trabalhador h
        std::vector<int> tTypes = instanceProblem->gettTypes();	// Tipos das tarefas (ki)
        std::vector<int> wCosts = instanceProblem->getwCosts(); // Custo dos trabalhadores dos tipos 1,2,3
        int** twTimes = instanceProblem->gettwTimes();
        std::vector<int> *predecessors = instanceProblem->getPredecessors();

        int i,h,s;
        int numS = numT; // Numeros de estações
        int numW = (int)wCosts.size(); // Quantidade de tipos de trabalhadores

        // Gurobi Vars
        GRBVar x[numT][numW][numS];
        GRBVar y[numW][numS];
        GRBVar nWorkers[numW];
        GRBVar v[numS];
        GRBLinExpr expr1, expr2;


        // Variavel binária Yh
        for(h = 0; h < numW; h++) {
            nWorkers[h] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER);
        }
        //Variavel binária Xihs
        for (i = 0; i < numT; i++) {
            for (h = 0; h < numW; h++) {
                for (s = 0; s < numS; s++) {
                    x[i][h][s] = model.addVar(0, 1, 0, GRB_BINARY);
                }
            }
        }
        // Variável binária Yhs
        for(h = 0; h < numW; h++) {
            for(s = 0; s < numS; s++) {
                y[h][s] = model.addVar(0, 1, 0, GRB_BINARY, "TrabxEstacao");
            }
        }
        // Variavel Vs
        for(s = 0; s < numS; s++) {
            v[s] = model.addVar(0, 1, 0, GRB_BINARY);
        }

        model.update();

        // Função Objetivo de Minimização
        expr1 = 0;
        for(h = 0; h < numW; h++ ) {
            expr1 += wCosts[h] * nWorkers[h];
        }
        model.setObjective(expr1, GRB_MINIMIZE);
        model.set(GRB_StringAttr_ModelName, "ALBHW");


        // Restrição que garante que apenas um trabalhador fará a tarefa
        for(i = 0; i < numT; i++) {
            expr1 = 0;
            for(h = 0; h <= tTypes[i]; h++) {
                for(s = 0; s < numS; s++ ) {
                    expr1 += x[i][h][s];
                }
            }
            model.addConstr(expr1 , GRB_EQUAL , 1, "Restrição 1");
        }

        // Restrição que conta a quantidade de trabalhadores no total
        for(s = 0; s < numS; s++) {
            expr1 = 0;
            for(h = 0; h < numW; h++ ) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, v[s], "Restrição 2");
        }

        // Restrição que liga Xihs a Yhs
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < numS; s++ ) {
                expr1 = 0;
                for(i = 0; i < numT; i++) {
                    expr1 += x[i][h][s];
                }
                model.addConstr(expr1, GRB_LESS_EQUAL, numT*y[h][s], "Restrição 3");
            }
        }
        // Restrição que garante que os empregados serão colocados em alguma estação
        for(h = 0; h < numW; h++) {
            expr1 = 0;
            for(s = 0; s < numS; s++) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, nWorkers[h], "Restrição 4");
        }

        // Restrição que garante precedencia e sucessão
        for(int i = 0; i < numT; i++) {
            for(int j = 0; j < int(this->graph->precedences[i].size()); j++) {
                expr1 = expr2 = 0;
                for(h = 0; h <= tTypes[this->graph->precedences[i].at(j)]; h++) {
                    for(s = 0; s < numS; s++) {
                        expr1 += (s+1) * x[this->graph->precedences[i].at(j)][h][s];
                    }
                }
                for(h = 0; h <= tTypes[i]; h++) {
                    for(s = 0; s < numS; s++) {
                        expr2 += (s+1)* x[i][h][s];
                    }
                }
                model.addConstr(expr1, GRB_LESS_EQUAL, expr2, "Restrição 5");
            }
        }
        // Restrição que garante que o tempo de dada estação não ultrapasse o tempo de ciclo
        for(s = 0; s < numS; s++) {
            expr1 = 0;
            for(i = 0; i < numT; i++) {
                for(h = 0; h <= tTypes[i]; h++) {
                    expr1 += twTimes[i][h] * x[i][h][s];
                }
            }
            model.addConstr(expr1, GRB_LESS_EQUAL, cycT, "Restrição 6");
        }

        // Restrição que garante a precedencia das estações em numero 1..2..3..
        expr1 = 0;
        for(s = 0; s < numS-1; s++) {
            model.addConstr(v[s], GRB_GREATER_EQUAL, v[s+1], "Restrição 7");
        }

        std::cout << "Tempo de Ciclo: " << cycT << std::endl;
        std::cout << "Numero de Tarefas: " << numT << std::endl;
        std::cout << "Numero de Precedencias: " << instanceProblem->getNumPrec() << std::endl;
        //std::cout << "Tempo da Tarefa: " << std::endl;
        //for(int i = 0; i < int(tTimes.size()); i++) {
        //    std::cout << i << " : " << tTimes[i] << std::endl;
       // }
        std::cout << "Tipo das Tarefas: " << std::endl;
        for(int i = 0; i < int(tTypes.size()); i++) {
            std::cout << i << " : " << tTypes[i] << std::endl;
        }
        std::cout << "Worker Times - Tempo Tarefas: " << std::endl;
        for(int i = 0; i < numT; i++) {
            for(int j = 0; j < numW; j++) {
                std::cout << i << " " << j << " : " << twTimes[i][j] << std::endl;
            }
        }
        std::cout << "Predecessores: " << std::endl;
        for(int i = 0; i < numT; i++) {
            std::cout << "Predecessores de " << i << " :" << std::endl;
            for(int j = 0; j < int(predecessors[i].size()); j++) {
                std::cout << predecessors[i].at(j) <<" " << std::endl;
            }
        }
        std::cout << "Custo dos Trabalhadores: " << std::endl;
        for(int i = 0; i < int(wCosts.size()); i++) {
            std::cout << i << " : " << wCosts[i] << std::endl;
        }
        model.getEnv().set(GRB_DoubleParam_NodeLimit, GRB_INFINITY);
        model.getEnv().set(GRB_DoubleParam_TimeLimit, 7200.0);
        model.getEnv().set(GRB_IntParam_Threads, 1);
        //model.getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-9);

        model.setCallback(result);
        // Resolve modelo
        model.optimize();
        /*
        for(s = 0; s < numS; ++s) {
            for(i = 0; i < numT; ++i) {
                for(h = 0; h < numW; ++h) {
                    if(x[i][h][s].get(GRB_DoubleAttr_X) == 1) {
                        cout << "A tarefa " << i << " com o trab " << h << " na estação " << s << endl;
                    }
                }
            }
        }*/
        result->lower = model.get(GRB_DoubleAttr_ObjBound);
        result->time = model.get(GRB_DoubleAttr_Runtime);
        result->nodes = model.get(GRB_DoubleAttr_NodeCount);
        result->valorObjetivoModelo = model.get(GRB_DoubleAttr_ObjVal);
        result->status = model.get(GRB_IntAttr_Status);
        for(i = 0; i < numW; i++) {
            if(nWorkers[i].get(GRB_DoubleAttr_X))
                result->workers[i] = round(nWorkers[i].get(GRB_DoubleAttr_X));
        }
        result->gap = model.get(GRB_DoubleAttr_MIPGap) * 100;

        //model.write("out.sol");
    } catch (GRBException e) {
        result->valorObjetivoModelo = 0;
        return result;
    }
    return result;
}

void AlbhwModel::printResultsHeur(AlbhwHeuristicResult* res, int regraT, int regraW, int print, string destination, string filename) {
    int objective; string restante;
    switch (print)
    {
    case 0:
        objective = res->valorObjetivoModelo;
        restante = "_T" + std::to_string(regraT) + "_W" + std::to_string(regraW) + "_V";
        break;
    case 1:
        objective = res->valorObjetivoHeur;
        restante = "_T" + std::to_string(regraT) + "_W" + std::to_string(regraW) + "_H";
        break;
    case 2:
        objective = res->valorObjetivoModelo;
        restante = "_T" + std::to_string(regraT) + "_W" + std::to_string(regraW) + "_MIP1";
        break;
    case 3:
        objective = res->valorObjetivoModelo;
        restante = "_T" + std::to_string(regraT) + "_W" + std::to_string(regraW) + "_MIP2";
        break;
    }
    //cout << destination + "/"+ filename + restante;
    //char c; cin >> c;
    ofstream arq(destination + "/" + filename + restante);
    if(arq) {
        arq << objective << endl;
        arq << res->time << endl;
        for (int s = 0; s < res->numS ; ++s) {
            arq << s << " " << res->workersA[s] << " " << res->workstationTime[s] << " ";
            arq << "[ ";
            for(int i = 0; i < int(res->tasksA[s].size()); ++i) {
                arq << res->tasksA[s].at(i) << " ";
            }
            arq << "]" << endl;
        }
    } else {
        cout << "Failed!! Press something to go on.";
        char c; cin >> c;
    }
    arq.close();
}
// Buscas Locais MIP
AlbhwHeuristicResult* AlbhwModel::LocalSearchMIP1(AlbhwHeuristicResult* heurRes) {
    AlbhwHeuristicResult* result = new AlbhwHeuristicResult();
    GRBModel model = GRBModel(this->env);
    try{
        result->time = heurRes->time;
        int i,h,s;
        int numT = instanceProblem->getNumT();
        int cycT = instanceProblem->getCyc();
        int numW = (int)instanceProblem->wCosts.size(); // Quantidade de tipos de trabalhadores
        // Persistencia da Heurística
        vector<int>* tasksA = new vector<int>[heurRes->numS]; // tarefas em cada estação
        vector<int> stationsTasks;
        vector<int> workersA;
        int resultNumS = 0;


        // Gurobi Vars
        GRBVar x[numT][numW][heurRes->numS];
        GRBVar y[numW][heurRes->numS];
        GRBVar nWorkers[numW];
        GRBVar v[heurRes->numS];
        GRBLinExpr expr1, expr2;

        // Variavel binária Yh
        for(h = 0; h < numW; h++) {
            nWorkers[h] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER);
        }

        //Variavel binária Xihs
        for (i = 0; i < numT; i++) {
            //Cria stationstasks
            stationsTasks.push_back(-1);
            for (h = 0; h < numW; h++) {
                for (s = 0; s < heurRes->numS; s++) {
                    x[i][h][s] = model.addVar(0, 1, 0, GRB_BINARY);
                }
            }
        }
        // Variável binária Yhs
        for(h = 0; h < numW; h++) {
            for(s = 0; s < heurRes->numS; s++) {
                y[h][s] = model.addVar(0, 1, 0, GRB_BINARY, "TrabxEstacao");
            }
        }
        // Variavel Vs
        for(s = 0; s < heurRes->numS; s++) {
            v[s] = model.addVar(0, 1, 0, GRB_BINARY);
        }

        model.update();

        // Função Objetivo de Minimização
        expr1 = 0;
        for(h = 0; h < numW; h++ ) {
            expr1 += this->instanceProblem->wCosts[h] * nWorkers[h];
        }
        model.setObjective(expr1, GRB_MINIMIZE);
        model.set(GRB_StringAttr_ModelName, "ALBHW");

        /*
        for(int task = 0; task < heurRes->stationsTasks.size(); ++task) {
            expr1 =0;
            for(h = 0; h < numW; ++h) {
                expr1 += x[heurRes->stationsTasks.at(task)][];
                x[heurRes->stationsTasks.at(task)][task].set(GRB_DoubleAttr_Start,1);
                if(heurRes->stationsTasks.at(task) >= 1 && heurRes->stationsTasks.at(task) <= heurRes->numS - 2) {
                    expr1 += x[heurRes->stationsTasks.at(task) - 1][task];
                    expr1 += x[heurRes->stationsTasks.at(task) + 1][task];
                } else if (heurRes->stationsTasks.at(task) == 0) {
                    expr1 += x[heurRes->stationsTasks.at(task) + 1][task];

                } else if(heurRes->stationsTasks.at(task) == (heurRes->numS-1)) {
                    expr1 += x[heurRes->stationsTasks.at(task) - 1][task];
                }
                model.addConstr(expr1, GRB_EQUAL, 1, "Restrição da Busca local");
            }
        } */
        // Restrição da vizinhanç

        for(int task = 0; task < numT; ++task) {
            expr1 = 0;
            for (h = 0; h <= this->instanceProblem->tTypes[task]; ++h)
                expr1 += x[task][h][heurRes->stationsTasks.at(task)];

            if(heurRes->stationsTasks.at(task) >= 1 &&
               heurRes->stationsTasks.at(task) <= (heurRes->numS - 2)) {
                for (h = 0; h <= this->instanceProblem->tTypes[task]; ++h) {
                    expr1 += x[task][h][heurRes->stationsTasks.at(task) - 1];
                    expr1 += x[task][h][heurRes->stationsTasks.at(task) + 1];
                }
            } else if (heurRes->stationsTasks.at(task) == 0) {
                for (h = 0; h <= this->instanceProblem->tTypes[task]; ++h)
                    expr1 += x[task][h][heurRes->stationsTasks.at(task) + 1] ;
            } else if(heurRes->stationsTasks.at(task) == (heurRes->numS - 1)) {
                for (h = 0; h <= this->instanceProblem->tTypes[task]; ++h)
                    expr1 += x[task][h][heurRes->stationsTasks.at(task) - 1];
            }
            model.addConstr(expr1, GRB_EQUAL, 1, "Restrição da Busca local");
            //cout << expr1 << endl;
        }
        // Sets

        for (h = 0; h < numW; ++h) {
            cout <<"Trab tipo: " << h << " - " << heurRes->workers[h] << endl;
            nWorkers[h].set(GRB_DoubleAttr_Start, heurRes->workers[h]);
        }
        for (s = 0;  s < heurRes->numS ; ++s) {
            cout << "Worker: " << heurRes->workersA[s] << " Estação:  " << s << endl;
            y[heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start, 1);
            v[s].set(GRB_DoubleAttr_Start,1);
            for (i = 0; i < int(heurRes->tasksA[s].size()); ++i) {
                x[heurRes->tasksA[s].at(i)][heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start, 1);
            }
        }

        // Restrição que garante que apenas um trabalhador fará uma tarefa
        for(i = 0; i < numT; i++) {
            expr1 = 0;
            for(h = 0; h <= this->instanceProblem->tTypes[i]; h++) {
                for(s = 0; s < heurRes->numS; s++) {
                    expr1 += x[i][h][s];
                }
            }
            model.addConstr(expr1 , GRB_EQUAL , 1, "Restrição 1");
        }
        // Restrição que conta a quantidade de trabalhadores no total
        for(s = 0; s < heurRes->numS; s++) {
            expr1 = 0;
            for(h = 0; h < numW; h++ ) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, v[s], "Restrição 2");
        }


        // Restrição que liga Xihs a Yhs
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < heurRes->numS; s++ ) {
                expr1 = 0;
                for(i = 0; i < numT; i++) {
                    expr1 += x[i][h][s];
                }
                //aqui vai o improvedMh[h];
                model.addConstr(expr1, GRB_LESS_EQUAL, this->improvedMh[h]*y[h][s], "Restrição 3");
            }
        }

        // Restrição que liga Xihs a Yhs - Sugestão 3 do Manuel
        //
        /*
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < heurRes->numS; s++ ) {
                for(i = 0; i < numT; i++) {
                    model.addConstr(x[i][h][s], GRB_LESS_EQUAL, y[h][s], "Restrição 3");
                }
            }
        } */


        // Restrição que garante que os empregados serão colocados em alguma estação
        for(h = 0; h < numW; h++) {
            expr1 = 0;
            for(s = 0; s < heurRes->numS; s++) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, nWorkers[h], "Restrição 4 - Y[hs] - nWorkers - MIP1");
        }

         // Restrição que garante precedencia e sucessão
        for(int j = 0; j < numT; ++j) {
            for(int i = 0; i < int(graph->allPredecessors[j].size()); ++i) {
                expr1 = expr2 = 0;
                for(h = 0; h <= this->instanceProblem->tTypes[graph->allPredecessors[j].at(i)]; ++h) {
                    for(s = 0; s < heurRes->numS; ++s) {
                        expr1 += (s+1) * x[graph->allPredecessors[j].at(i)][h][s];
                    }
                }
                for(h = 0; h <= this->instanceProblem->tTypes[j]; ++h) {
                    for(s = 0; s < heurRes->numS; ++s) {
                        expr2 += (s+1)* x[j][h][s];
                    }
                }
                model.addConstr(expr2, GRB_GREATER_EQUAL, expr1 + max(this->graph->p[graph->allPredecessors[j].at(i)][j],0) , "Restrição Predecessors");
            }
        }

        // Restrição que garante que o tempo de dada estação não ultrapasse o tempo de ciclo
        // Segunda sugestão do Manuel
        for(s = 0; s < heurRes->numS; s++) {
            expr1 = 0;
            for(i = 0; i < numT; i++) {
                for(h = 0; h <= this->instanceProblem->tTypes[i]; h++) {
                    expr1 += this->ntwTimes[i][h] * x[i][h][s];
                    //expr1 += twTimes[i][h] * x[i][h][s];
                }
            }
            model.addConstr(expr1, GRB_LESS_EQUAL, cycT * v[s], "Restrição 6");
            //model.addConstr(expr1, GRB_LESS_EQUAL, cycT, "Restrição 6");
        }

        // Restrição que garante a precedencia das estações em numero 1..2..3..
        for(s = 0; s < heurRes->numS-1; s++) {
            model.addConstr(v[s], GRB_GREATER_EQUAL, v[s+1], "Restrição 7");
        }
        /*
        std::cout << "Tempo de Ciclo: " << this->instanceProblem->cycT << std::endl;
        std::cout << "Numero de Tarefas: " << this->instanceProblem->numT << std::endl;
        std::cout << "Numero de Precedencias: " << instanceProblem->getNumPrec() << std::endl;
        std::cout << "Tempo da Tarefa: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->tTimes.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->tTimes[i] << std::endl;
        }
        std::cout << "Tipo das Tarefas: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->tTypes.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->tTypes[i] << std::endl;
        }
        std::cout << "Worker Times - Tempo Tarefas: " << std::endl;
        for(int i = 0; i < this->instanceProblem->numT; i++) {
            for(int j = 0; j < 3; j++) {
                std::cout << i << " " << j << " : " << this->instanceProblem->twTimes[i][j] << std::endl;
            }
        }
        std::cout << "Predecessores: " << std::endl;
        for(int i = 0; i < this->instanceProblem->numT; i++) {
            std::cout << "Predecessores de " << i << " :" << std::endl;
            for(int j = 0; j < int(this->instanceProblem->predecessors[i].size()); j++) {
                std::cout << this->instanceProblem->predecessors[i].at(j) <<" " << std::endl;
            }
        }
        std::cout << "Custo dos Trabalhadores: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->wCosts.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->wCosts[i] << std::endl;
        } */

        model.getEnv().set(GRB_DoubleParam_NodeLimit, GRB_INFINITY);
        model.getEnv().set(GRB_DoubleParam_TimeLimit, 15.0);
        model.getEnv().set(GRB_IntParam_Threads, 1);
        //model.getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-9);
        //model.getEnv().set(GRB_IntParam_IntVio, 0);

        model.setCallback(result);
        // Resolve modelo
        model.optimize();

        //DEBUG

        /* cout << "Heuristica: ";
        for (s = 0; s < heurRes->numS; ++s) {
            cout << "Estação: " << s << ": ";
            for (i = 0; i < heurRes->tasksA[s].size(); ++i) {
                cout << heurRes->tasksA[s].at(i) << " ";
            }
            cout << endl;
        }
        // Verifica as estações atuais e seus trabalhadores
        for (s = 0; s < heurRes->numS; ++s) {
            cout << "Valor da estação s: " << s << " :" << v[s].get(GRB_DoubleAttr_X) << endl;
            for(h = 0; h < numW; ++h) {
                if (y[h][s].get(GRB_DoubleAttr_X) == 1) {
                }
            }
        } */

        result->time += model.get(GRB_DoubleAttr_Runtime);
        result->status = model.get(GRB_IntAttr_Status);
        result->lower = model.get(GRB_DoubleAttr_ObjBound);
        result->nodes = model.get(GRB_DoubleAttr_NodeCount);
        result->valorObjetivoModelo = model.get(GRB_DoubleAttr_ObjVal);


        for(s = 0; s < heurRes->numS; ++s) {
            for(h = 0; h < numW; ++h) {
                if(y[h][s].get(GRB_DoubleAttr_X) >= 0.5) {
                    workersA.push_back(h);
                    resultNumS++;
                    for (i = 0; i < numT; ++i) {
                        if (x[i][h][s].get(GRB_DoubleAttr_X) >= 0.5) {
                            cout << "A tarefa " << i << " com o trab " << h << " na estação " << s << endl;
                            tasksA[s].push_back(i);
                            stationsTasks[i] = s;
                        }
                    }
                }
            }
        }
        for(i = 0; i < numW; i++)
            result->workers[i] = round(nWorkers[i].get(GRB_DoubleAttr_X));

        result->workstationTime = new int[resultNumS];
        int t;
        for(s = 0; s < resultNumS; ++s) {
            t = 0;
            for(i = 0; i < int(tasksA[s].size()); ++i) {
                t += this->instanceProblem->twTimes[tasksA[s].at(i)][workersA[s]];
            }
            result->workstationTime[s] = t;
        }
        // DEBUG ESTAÇÕES
        for(s = 0; s < resultNumS; ++s) {
            cout << " Station " << s << " :";
            for (i = 0; i < int(tasksA[s].size()); ++i) {
                cout << " " << tasksA[s].at(i);
            }
            cout << endl;
        }
        //char op; cin >> op;
        cout << endl;
        cout << "Trabalhadores: ";
        for(s = 0; s < resultNumS; ++s) {
            cout << workersA.at(s) << " ";
        }
        result->gap = model.get(GRB_DoubleAttr_MIPGap) * 100;
        result->numS = resultNumS;

        // Heuristica
        result->tasksA = tasksA;
        result->stationsTasks = stationsTasks;
        result->workersA = workersA;
        return result;
    } catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
        //result = heurRes;

    }
    return new AlbhwHeuristicResult();
}

AlbhwHeuristicResult* AlbhwModel::LocalSearchMIP2(AlbhwHeuristicResult* heurRes) {
    AlbhwHeuristicResult* result = new AlbhwHeuristicResult();
    GRBModel model = GRBModel(this->env);
    try{
        int i,h,s;
        result->time = heurRes->time;
        cout << " Numero de estações: " << heurRes->numS;

        int numT = instanceProblem->getNumT();
        int cycT = instanceProblem->getCyc();
        int numW = (int)instanceProblem->wCosts.size(); // Quantidade de tipos de trabalhadores
        // Persistencia da Heurística
        vector<int>* tasksA = new vector<int>[heurRes->numS]; // tarefas em cada estação
        vector<int> stationsTasks;
        vector<int> workersA;
        int resultNumS = 0;


        // Gurobi Vars
        GRBVar x[numT][numW][heurRes->numS];
        GRBVar y[numW][heurRes->numS];
        GRBVar nWorkers[numW];
        GRBVar v[heurRes->numS];
        GRBVar c[heurRes->numS];
        GRBVar B[heurRes->numS];
        GRBLinExpr expr1, expr2;

        // Variavel Inteira Yh
        for(h = 0; h < numW; h++) {
            nWorkers[h] = model.addVar(0, GRB_INFINITY, 0, GRB_INTEGER);
        }
        //Variável Inteira Cs
        for(s = 0; s < heurRes->numS; ++s) {
            c[s] = model.addVar(0,GRB_INFINITY,0,GRB_INTEGER, "Var Cs");
        }
        //Variável binária Bs
        for(s = 0; s < heurRes->numS; ++s) {
            B[s] = model.addVar(0,1,0,GRB_BINARY, "Var Bs");
        }
        //Variavel binária Xihs
        for (i = 0; i < numT; i++) {
            //Cria stationstasks
            stationsTasks.push_back(-1);
            for (h = 0; h < numW; h++) {
                for (s = 0; s < heurRes->numS; s++) {
                    x[i][h][s] = model.addVar(0, 1, 0, GRB_BINARY);
                }
            }
        }
        // Variável binária Yhs
        for(h = 0; h < numW; h++) {
            for(s = 0; s < heurRes->numS; s++) {
                y[h][s] = model.addVar(0, 1, 0, GRB_BINARY, "TrabxEstacao");
            }
        }
        // Variavel Vs
        for(s = 0; s < heurRes->numS; s++) {
            v[s] = model.addVar(0, 1, 0, GRB_BINARY);
        }

        model.update();

        // Função Objetivo de Minimização
        // Aqui, como já vai ter passado pela MIP1, já o modelo e não mais a heurística
        // heurRes->valorObjetivoHeur se qiser testar solo
        // heurRes->valorObjetivoModelo se testar na VND
        expr1 = heurRes->valorObjetivoModelo;
        expr2 = 0;
        for(s = 0; s < heurRes->numS; ++s) {
            expr2 += c[s];
        }

        model.setObjective(expr1 - expr2, GRB_MINIMIZE);
        model.set(GRB_StringAttr_ModelName, "ALBHW");



        // Restrição que garante que apenas um trabalhador fará a tarefa
        for(i = 0; i < numT; i++) {
            expr1 = 0;
            for(h = 0; h <= this->instanceProblem->tTypes[i]; h++) {
                for(s = 0; s < heurRes->numS; s++ ) {
                    expr1 += x[i][h][s];
                }
            }
            model.addConstr(expr1 , GRB_EQUAL , 1, "Restrição 1");
        }

        // Restrição que conta a quantidade de trabalhadores no total
        for(s = 0; s < heurRes->numS; s++) {
            expr1 = 0;
            for(h = 0; h < numW; h++ ) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, v[s], "Restrição 2");
        }


        // Restrição que liga Xihs a Yhs
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < heurRes->numS; s++ ) {
                expr1 = 0;
                for(i = 0; i < numT; i++) {
                    expr1 += x[i][h][s];
                }
                //aqui vai o improvedMh[h];
                model.addConstr(expr1, GRB_LESS_EQUAL, this->improvedMh[h]*y[h][s], "Restrição 3");
            }
        }

        // Restrição que liga Xihs a Yhs - Sugestão 3 do Manuel
        //
        /*
        for(h = 0; h < numW; h++ ) {
            for(s = 0; s < numS; s++ ) {
                for(i = 0; i < numT; i++) {
                    model.addConstr(x[i][h][s], GRB_LESS_EQUAL, y[h][s], "Restrição 3");
                }
            }
        }*/

        // Restrição que garante que os empregados serão colocados em alguma estação
        for(h = 0; h < numW; h++) {
            expr1 = 0;
            for(s = 0; s < heurRes->numS; s++) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, nWorkers[h], "Restrição 4 - Y[hs] - nWorkers - MIP2");
        }

         // Restrição que garante precedencia e sucessão
        for(int j = 0; j < numT; ++j) {
            for(int i = 0; i < int(graph->allPredecessors[j].size()); ++i) {
                expr1 = expr2 = 0;
                for(h = 0; h <= this->instanceProblem->tTypes[graph->allPredecessors[j].at(i)]; ++h) {
                    for(s = 0; s < heurRes->numS; ++s) {
                        expr1 += (s+1) * x[graph->allPredecessors[j].at(i)][h][s];
                    }
                }
                for(h = 0; h <= this->instanceProblem->tTypes[j]; ++h) {
                    for(s = 0; s < heurRes->numS; ++s) {
                        expr2 += (s+1)* x[j][h][s];
                    }
                }
                model.addConstr(expr2, GRB_GREATER_EQUAL, expr1 + max(this->graph->p[graph->allPredecessors[j].at(i)][j],0) , "Restrição Predecessors");
            }
        }

        // Restrição que garante que o tempo de dada estação não ultrapasse o tempo de ciclo
        // Segunda sugestão do Manuel
        for(s = 0; s < heurRes->numS; s++) {
            expr1 = 0;
            for(i = 0; i < numT; i++) {
                for(h = 0; h <= this->instanceProblem->tTypes[i]; h++) {
                    expr1 += this->ntwTimes[i][h] * x[i][h][s];
                    //expr1 += twTimes[i][h] * x[i][h][s];
                }
            }
            model.addConstr(expr1, GRB_LESS_EQUAL, cycT * v[s], "Restrição 6");
            //model.addConstr(expr1, GRB_LESS_EQUAL, cycT, "Restrição 6");
        }


        // Restrição que garante a precedencia das estações em numero 1..2..3..
        for(s = 0; s < heurRes->numS-1; s++) {
            model.addConstr(v[s], GRB_GREATER_EQUAL, v[s+1], "Restrição 7");
        }
        //char ss; cin >> ss;
        // BUSCA LOCAL MIP
        // Restrição 1 - Garante que o número de tipos de trabalhadores continuará igual
        for(h = 0; h < numW; ++h) {
            expr1 = 0;
            for(s = 0; s < heurRes->numS; ++s) {
                expr1 += y[h][s];
            }
            model.addConstr(expr1, GRB_EQUAL, heurRes->workers[h], "Restrição");
        }
        //Restrição 2 - Bs, estações vazias
        for(s = 0; s < heurRes->numS; ++s) {
            expr2 = 0;
            for(i = 0; i < numT; ++i) {
                for (h = 0; h < this->instanceProblem->tTypes[i]; ++h)
                    expr2 += x[i][h][s];
            }
            expr1 = (1-B[s]);
            model.addConstr(expr1, GRB_LESS_EQUAL, expr2, "Restrição");
        }
        // Restrição 3 - Bs, estações vazias, garante a de cima
        // TODO
        for(s = 0; s < heurRes->numS; ++s) {
            expr2 =0;
            for(int i = 0; i < numT; ++i) {
                for (h = 0;  h < this->instanceProblem->tTypes[i]; ++h)
                    expr2 += x[i][h][s];
            }
            expr1 = (numT* (1-B[s])); // TODO numT ?
            model.addConstr(expr2, GRB_LESS_EQUAL, expr1, "Restrição");
        }
        // Restrição 4 - Constroi o Cs
        for(s = 0; s < heurRes->numS; ++s) {
            expr2 = 0;
            for(h = 0; h < numW; ++h) {
                expr2 += (this->instanceProblem->wCosts[h] * y[h][s]);
            }
            expr1 = c[s];
            model.addConstr(expr1, GRB_LESS_EQUAL, expr2, "Restrição");
        }
        // Restrição 5 - Constroi Cs
        /*
        int maiorV = 0;
        int maiorI = 0;
        for(h = 0; h < numW; ++h) {
            if(maiorV < this->instanceProblem->wCosts[h]) {
                maiorI = h;
                maiorV = this->instanceProblem->wCosts[h];
            }
        }*/
        for (s = 0; s < heurRes->numS; ++s) {
            expr1 = c[s];
            expr2 = (this->instanceProblem->wCosts[0] * B[s]);
            model.addConstr(expr1, GRB_LESS_EQUAL, expr2, "restrição");
        }

        //Fixa trabalhadores e tarefas
        /*for(s = 0; s < heurRes->numS; ++s) {
            y[heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start,1);
            for (int i = 0; i < heurRes->tasksA[s].size(); ++i) {
                x[heurRes->tasksA[s].at(i)][heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start,1);
                 }
        }*/

        for (h = 0; h < numW; ++h) {
            cout << "Setting: " << h << " - " << heurRes->workers[h];
            nWorkers[h].set(GRB_DoubleAttr_Start, heurRes->workers[h]);
        }

        for (s = 0;  s < heurRes->numS ; ++s) {
            y[heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start, 1);
            v[s].set(GRB_DoubleAttr_Start,1);
            for (i = 0; i < int(heurRes->tasksA[s].size()); ++i) {
                cout << "Estação " << s << " trab: " << heurRes->workersA[s] << " e tarefa: " << heurRes->tasksA[s].at(i) << endl;
                x[heurRes->tasksA[s].at(i)][heurRes->workersA[s]][s].set(GRB_DoubleAttr_Start, 1);
            }
        }


        /*
        std::cout << "Tempo de Ciclo: " << this->instanceProblem->cycT << std::endl;
        std::cout << "Numero de Tarefas: " << this->instanceProblem->numT << std::endl;
        std::cout << "Numero de Precedencias: " << instanceProblem->getNumPrec() << std::endl;
        std::cout << "Tempo da Tarefa: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->tTimes.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->tTimes[i] << std::endl;
        }
        std::cout << "Tipo das Tarefas: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->tTypes.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->tTypes[i] << std::endl;
        }
        std::cout << "Worker Times - Tempo Tarefas: " << std::endl;
        for(int i = 0; i < numT; i++) {
            for(int j = 0; j < 3; j++) {
                std::cout << i << " " << j << " : " << this->instanceProblem->twTimes[i][j] << std::endl;
            }
        }
        std::cout << "Predecessores: " << std::endl;
        for(int i = 0; i < this->instanceProblem->numT; i++) {
            std::cout << "Predecessores de " << i << " :" << std::endl;
            for(int j = 0; j < int(this->instanceProblem->predecessors[i].size()); j++) {
                std::cout << this->instanceProblem->predecessors[i].at(j) <<" " << std::endl;
            }
        }
        std::cout << "Custo dos Trabalhadores: " << std::endl;
        for(int i = 0; i < int(this->instanceProblem->wCosts.size()); i++) {
            std::cout << i << " : " << this->instanceProblem->wCosts[i] << std::endl;
        } */

        model.getEnv().set(GRB_DoubleParam_NodeLimit, GRB_INFINITY);
        model.getEnv().set(GRB_DoubleParam_TimeLimit, 15.0);
        model.getEnv().set(GRB_IntParam_Threads, 1);
        //model.getEnv().set(GRB_DoubleParam_IntFeasTol, 1e-5);

        model.setCallback(result);
        // Resolve modelo
        model.optimize();

        // Valor atual da linha
        int valorModelo = 0;
        for (h = 0; h < this->instanceProblem->numW; ++h)
            valorModelo += round(nWorkers[h].get(GRB_DoubleAttr_X)) * this->instanceProblem->wCosts[h];

        cout << resultNumS << " Velho: " << heurRes->numS;
        for(s = 0; s < heurRes->numS; ++s) {
            for(h = 0; h < numW; ++h) {
                if(y[h][s].get(GRB_DoubleAttr_X) >= 0.5) {
                    workersA.push_back(h);
                    resultNumS++;
                    for (i = 0; i < numT; ++i) {
                        if (x[i][h][s].get(GRB_DoubleAttr_X) >= 0.5 ) {
                            cout << "A tarefa " << i << " com o trab " << h << " na estação " << s << endl;
                            tasksA[s].push_back(i);
                            stationsTasks[i] = s;
                        }
                    }
                }
            }
        }
        result->workstationTime = new int[resultNumS];
        int t;
        for(s = 0; s < resultNumS; ++s) {
            t = 0;
            for(i = 0; i < int(tasksA[s].size()); ++i) {
                t += this->instanceProblem->twTimes[tasksA[s].at(i)][workersA[s]];
            }
            result->workstationTime[s] = t;
        }
        result->workers = new int[numW];
        for (i = 0; i < numW; i++) {
            result->workers[i] = round(nWorkers[i].get(GRB_DoubleAttr_X));

        }
        // DEBUG
        cout << heurRes->numS << " vs " << resultNumS << endl;
        cout << int(tasksA->size());
        cout << " Estações: " << tasksA->size() << endl;
        for (s = 0; s < resultNumS; ++s) {
            cout << " Station " << s << " :";
            for (i = 0; i < int(tasksA[s].size()); ++i) {
                cout << " " << tasksA[s].at(i);
            }
            cout << endl;
        }
        cout << endl;
        cout << "Trabalhadores: ";
        for (s = 0; s < resultNumS; ++s) {
            cout << workersA.at(s) << " ";
        }

        //int t;
        /*for (int s = 0; s < resultNumS; ++s) {
            t = 0;
            for (int i = 0; i < int(tasksA[s].size()); ++i) {
                t += this->instanceProblem->twTimes[tasksA[s].at(i)][workersA[s]];
            }
            cout << "tempo da estação " << s << ": " << t << endl;
        } */
        cout << "\n Encontrado valor: " << valorModelo;
        result->valorObjetivoModelo = valorModelo;
        result->gap = model.get(GRB_DoubleAttr_MIPGap) * 100;
        result->time += model.get(GRB_DoubleAttr_Runtime);
        result->status = model.get(GRB_IntAttr_Status);
        result->lower = model.get(GRB_DoubleAttr_ObjBound);
        result->nodes = model.get(GRB_DoubleAttr_NodeCount);
        result->numS = resultNumS;
        // Heuristica
        result->tasksA = tasksA;
        result->stationsTasks = stationsTasks;
        result->workersA = workersA;
        return result;
    } catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;
    }
    return NULL;
}

AlbhwHeuristicResult* AlbhwModel::RunVND(AlbhwHeuristicResult *heurRes, int regraT, int regraW) {
    int bestResult = NAO_PODE_EXECUTAR;
    AlbhwHeuristicResult* bestN = heurRes;
    this->printResultsTerminal(bestN);
    cout << "HEURISTICA";
    int i = 0;
    AlbhwHeuristicResult* heurImproved;
    double time =0;
    cout << "Entrando no VND: " << bestN->valorObjetivoHeur << endl;
    while(i < 2) {
        if(i == 0) {
            //cin >> c;
            heurImproved = this->LocalSearchMIP1(bestN);
            cout << "Entrando MIP1";
        } else {
             //cin >> c;
            heurImproved = this->LocalSearchMIP2(bestN);
            cout << "Entrando MIP2";
        }
        time += heurImproved->time;
        if(heurImproved->valorObjetivoModelo < bestResult) {
            cout << "\n Valor antigo: " << bestResult << ". Valor novo: " << heurImproved->valorObjetivoModelo << " TEmpo: " << heurImproved->time << endl;
            bestResult = heurImproved->valorObjetivoModelo;
            bestN = heurImproved;
            i = 0;
            this->printResultsTerminal(heurImproved);
        } else {
            i++;
        }
        this->printResultsTerminal(heurImproved);
    }
    bestN->time = time;
    printResultsTerminal(bestN);
    //printResultsHeur(bestN, regraT, regraW);
    return bestN;
}
void AlbhwModel::createALBHWAux() {
    try {
        env = new GRBEnv();
    } catch (GRBException e) {
        std::cout << "Error code = " << e.getErrorCode() << std::endl;
        std::cout << e.getMessage() << std::endl;

    }
    int i, h;
    int numW = this->instanceProblem->numW;
    int numT = this->instanceProblem->numT;
    this->improvedMh = new int[numW];
    this->ntwTimes = new int*[numT];
    for(i = 0; i < numT; ++i) {
        this->ntwTimes[i] = new int[numW];
        for(h = 0; h < numW; ++h) {
            this->ntwTimes[i][h] = this->instanceProblem->twTimes[i][h];
        }
    }

    int melhorado = 0;
    for(i = 0; i < numT; ++i) {
        for(h = 0; h < numW; ++h) {
            if(h <= this->instanceProblem->tTypes[i] and this->ntwTimes[i][h] <= this->instanceProblem->cycT) {
                melhorado = this->improveCycT(this->ntwTimes, this->instanceProblem->twTimes[i][h], this->instanceProblem->tTypes, this->instanceProblem->cycT,numT, i, h);
                if(melhorado != 0) {
                    this->ntwTimes[i][h] = this->instanceProblem->cycT - melhorado;
                }
            }
        }
    }
    this->improvedMh = this->improveMh(this->ntwTimes,this->instanceProblem->tTypes,numT,numW,this->instanceProblem->cycT);
}
void AlbhwModel::printResultsTerminal(AlbhwHeuristicResult *res) {
    cout << "Instance: " << res->instance << endl;
    cout << "O.F. value: " << res->valorObjetivoModelo << endl;
    cout << "LB: " << res->lower << ". LBRoot: " << res->lowerRoot << endl;
    cout << " #Nodes: " << res->nodes << endl;
    cout << "Workers: " << endl;
    for (int i = 0; i < 3; ++i) {
        cout << "Type " << i << " " << res->workers[i] << endl;
    }
    cout << "Gap:" << res->gap << " with time: " << res->time << endl;
}

void AlbhwModel::printResults(AlbhwHeuristicResult *res, string destination, bool model) {
    string filename;

    std::stringstream ss(destination);
    std::string token;
    while (std::getline(ss, token, '/')) {
    }
    if(model)
        filename = destination + "/" + token + "_MCM";
    else
        filename = destination + "/" + token + "_MSY";
    //std::string test(res->lower);
    cout << to_string(3);
    ofstream arq(filename, ios::app);
    if(arq) {
        arq << this->getInstancePath() << ";";
        arq << to_string(res->valorObjetivoModelo) << ";";
        arq << to_string(res->lower) << ";";
        arq << to_string(res->lowerRoot) << ";";
        arq << to_string(res->nodes) << ";";
        for (int i = 0; i < this->instanceProblem->numW; ++i) {
            arq << res->workers[i] << ";";
        }
        arq << to_string(res->gap) << ";";
        arq << to_string(res->time) << ";";
        arq << res->status << endl;
    }
    arq.close();
}

void AlbhwModel::setInstancePath(string instance) {
    this->instancePath = instance;
}


string AlbhwModel::getInstancePath() {
    return this->instancePath;
}
AlbhwModel::AlbhwModel(string instance, int testarTudo) {
    this->setInstancePath(instance);
    this->instanceProblem = new AlbhwStructure();
    this->instanceProblem->readFile(instance);
    //if(testarTudo == 1) this->testModels();
    this->graph = new Graph(this->instanceProblem);
    this->graph->updatePrecedences();
    this->createALBHWAux();
}

AlbhwModel::AlbhwModel() {
    this->instanceProblem = new AlbhwStructure();
    //this->testSmall();
    //this->testMedium();
}
