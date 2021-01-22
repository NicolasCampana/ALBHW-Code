//
// Created by nicolas on 22/04/19.
//
#pragma once
#ifndef NICOLAS_IC_ALBHWHEURISTICRESULT_H
#define NICOLAS_IC_ALBHWHEURISTICRESULT_H


#include <vector>       // std::vector
#define QNT_HEURISTICAS 8
#define NAO_PODE_EXECUTAR 100000


/* Classe que retem as informações do ALBHW */
class AlbhwHeuristicResult : public GRBCallback {
    friend class AlbhwHeuristic;
    friend class AlbhwModel;
public:
    //Heuristica
    vector<int> *tasksA;
    vector<int> workersA;
    vector<int> stationsTasks;
    vector<int> estacoesDeletadas;
    double* qntTrabAlocados;
    int valorObjetivoHeur;
    int numS;
    int regraTR;
    int regraTA;
    string instance;
    double porTarefas;
    double* porTrab;
    AlbhwHeuristicResult();
    AlbhwHeuristicResult(std::vector<int>* tasksA, int* workstationTime, std::vector<int> workersA, std::vector<int> stationsTasks,vector<int> estacoesDeletadas, double* qntTrabAlocados, int numS, int cTotal, int regraTA, int regraTR, string instance);
    ~AlbhwHeuristicResult();
    void initializeTrabs();
    void imprimeArquivo();
    // Modelos
    int valorObjetivoModelo;
    int status;
    int *workers;
    int *workstationTime;
    double gap;
    double time;
    double lower;
    double lowerRoot;
    double nodes;
    bool flag = false;
protected:
    void callback() {
        try {
            if (where == GRB_CB_MIPNODE && !flag) {
                if(getDoubleInfo(GRB_CB_MIPNODE_NODCNT) == 0) {
                    this->lowerRoot = getDoubleInfo(GRB_CB_MIPNODE_OBJBNDC);
                } else {
                    flag = true;
                }
            }
        } catch (GRBException e) {
            cout << "Error number: " << e.getErrorCode() << endl;
            cout << e.getMessage() << endl;
        }
    }
};


#endif //NICOLAS_IC_ALBHWHEURISTICRESULT_H
