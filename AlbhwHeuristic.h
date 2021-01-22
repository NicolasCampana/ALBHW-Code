//
// Created by nicolas on 22/04/19.
//
#pragma once
#ifndef NICOLAS_IC_ALBHWHEURISTIC_H
#define NICOLAS_IC_ALBHWHEURISTIC_H

#include "AlbhwHeuristicResult.h"
#include "AlbhwStructure.h"

/* Classe Heuristicas */
class AlbhwHeuristic {
    friend class AlbhwHeuristicResult;
private:
    //Variáveis
    AlbhwStructure* instanceProblem;
    int** closure;
    int* qntLambdas;
    int* qntLambdasDraw;
    int s;
    int regraTR;
    int regraTA;
   /*
    int numW;
    int numT;
    int cycT;
    int numW;
    string instance;
    std::vector<int> tTimes; //Tempo da atividade i feita pelo trabalhador h
    std::vector<int> tTypes;	// Tipos das tarefas (ki)
    std::vector<int> wCosts; // Custo dos trabalhadores dos tipos 1,2,3
    int** twTimes;
    vector<int>* sucessores;
    vector<int>* predecessors;
    int nT;
    int opt;
    int folder; // Para contar qual célula ira ser armazenado no arquivo xls
    int contador; // Saber em qual arquivo esta
    */
    vector<int>* transferAux;
    vector<int> estacoesDeletadas;
    void transitiveClosure();
    // Para heuristicas
    std::vector<int> origGraph;
    std::vector<int> workersA; // trabalhadores em cada estação
    std::vector<int> timesA; // tempo de cada estação
    std::vector<int>* tasksA; // Tarefas em cada estação
    std::vector<int> predOrig; // Predecessores originais
    std::vector<int> stationsTasks; // Local onde as tasks estão
    int sizeOfGraph;
    int cTotal;
    int* totalTime;
    int melhorSolucao; // Heuristics
    int* removedWorkstations;
    double* qntTrabAlocados;
    // Busca local
    //map<int,int> mudancas; // Vetor para saber quais as mudanças feitas em um problema
    // Funções e métodos
    int MaxF(int trab, vector<int> tasks); // DONE
    int MaxIF(int trab, vector<int> tasks); // DONE
    int MaxTimeP(int trab,  vector<int> tasks); // DONE
    int MaxTimeM(int trab,  vector<int> tasks); // DONE
    int MinTimeP(int trab,  vector<int> tasks); // DONE
    int MinTimeM(int trab,  vector<int> tasks); // DONE
    int MaxPWP(int trab,  vector<int> tasks); // DONE
    int MaxPWM(int trab, vector<int> tasks); // DONE
    int MaxPW(int trab, vector<int> tasks);
    int MaxIFTime(int trab, vector<int> tasks);
    int MinTimeTrab(int trab, vector<int> tasks);
    int MaxTimeTrab(int trab, vector<int> tasks);
    int MaxFTime(int trab, std::vector<int> tasks);
    int MaxTimeTrabWorker(int trab, std::vector<int> tasks);
    int MinTimeTrabWorker(int trab, std::vector<int> tasks);
    //int MaxTasks(int trab, std::vector<int> tasks);
    //bool checkIfCanChange(int set);
    //void tryToTransfer(int set, int opFac);
    int wsLowerBound(); // DONE
    int wsUpperBound(); // DONE
    void UpdateChoosedWorker(int workerIndex,std::vector<int>* predAtual);
    int GetNewTask(vector<int> tasks, int task, int worker, vector<int>* predAtual, int totalTime);
    std::vector<int> bestOfWorkersLambda(std::vector<int>* setOfTasks,std::vector<int>* predAtual ,int* totalTime);
    std::vector<int> bestOfWorkersUpPerTask(std::vector<int>* setOfTasks,std::vector<int>* predAtual ,int* totalTime);
    std::vector<int> bestOfWorkersLessTime(std::vector<int>* setOfTasks,std::vector<int>* predAtual ,int* totalTime);
    std::vector<int> bestOfWorkersTimeWorkStation(std::vector<int>* setOfTasks,std::vector<int>* predAtual ,int* totalTime);
    std::vector<int> avaliableTasks(std::vector<int>* tasks);
    void predTasks(std::vector<int>* pred);
    int chooseTask(int opt, int trab,  std::vector<int> tasks);
    //void buscaLocal(int opTransf, int opFac);
    //void buscaLocalBound(int opFac, int bound);
    //void buscaLocalGenetica(int opFac, int maxTentativas);
    //void defineEstacaoComparativo(int estacaoMaisCedo, int estacaoMaisTarde, int set, int tarefa, int* menor);
    //void defineEstacaoComparativoII(int estacaoMaisCedo, int estacaoMaisTarde, int set, int tarefa, int* menor);
    //void defineEstacaoGulosoI(int estacaoMaisCedo, int estacaoMaisTarde, int set, int task, int tarefa,int* tarefaBackup, int* menor);
    //void defineEstacaoGulosoII(int estacaoMaisCedo, int estacaoMaisTarde, int set, int task, int tarefa,int* tarefaBackup, int* menor);
    void initializeMatrix();

public:
    //void setInstance(string inst,int folder, int contador);
    Graph* gr;
    void setProblem();
    AlbhwHeuristic();
    void AlbhwHeuristicSolo(string s );
    AlbhwHeuristic(string instance);
    ~AlbhwHeuristic();
    AlbhwHeuristicResult* Heuristic(int optRegra, int optTrab);
    //int testaHeuristica(int op);
    //void writeHeuristicResults();
    //void writeHeuristicLambdas();
    //string itoa(int number);
    //void setFiles(int i);
    int getWorkerValue(int workerId);
    int getNumberTasks();
    int getNumberOfWorkers();
    int GetTotal();
    //int calculaEstacaoMaisTarde(int tarefa, int estacao);
    //int calculaEstacaoMaisCedo(int tarefa, int estacao);
    void RunHeuristic();
    AlbhwHeuristicResult* LocalSearchMIP1(string instance, int opRegra, int opTrab);
    AlbhwHeuristicResult* LocalSearchMIP2(std::string instance,int opRegra = -1, int opTrab = 1);
    void setInstancePath(string instance);
};


#endif //NICOLAS_IC_ALBHWHEURISTIC_H
