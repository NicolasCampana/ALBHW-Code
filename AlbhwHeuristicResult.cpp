//
// Created by nicolas on 22/04/19.
//
#include "AlbhwHeuristicResult.h"

AlbhwHeuristicResult::AlbhwHeuristicResult() {
    this->workers = new int[3];
    this->time = 0;
    for(int i = 0; i < 3;++i)
        this->workers[i] = 0;
    this->valorObjetivoHeur = 0;
    this->valorObjetivoModelo = 0;
    this->nodes = 0;
    this->gap = 100;
    this->nodes = 0;
    this->lowerRoot = 0;
    this->lower = 0;
    this->status = 9;
}
AlbhwHeuristicResult::AlbhwHeuristicResult(std::vector<int>* tasksA, int* workstationTime, std::vector<int> workersA, std::vector<int> stationsTasks, vector<int> estacoesDeletadas, double* qntTrabAlocados, int numS, int cTotal, int regraTA, int regraTR, string instance) {
    this->tasksA = tasksA;
    this->workersA = workersA;
    this->stationsTasks = stationsTasks;
    this->numS = numS;
    this->valorObjetivoHeur = cTotal;
    this->valorObjetivoModelo = cTotal;
    this->estacoesDeletadas = estacoesDeletadas;
    this->qntTrabAlocados = qntTrabAlocados;
    this->regraTA = regraTA;
    this->regraTR = regraTR;
    this->instance = instance;
    this->workstationTime = workstationTime;
    cout << "Instance: " << instance << endl;
    cout << "O.F. value: " << cTotal << endl;
    cout << "Workers: " << endl;
    for (int i = 0; i < numS; ++i) {
        cout << "Worker " << i << " " << workersA[i] << endl;
    }
    this->workers = new int[3];
    this->time = 0;
    for(int i = 0; i <3;++i)
        this->workers[i] = 0;
    for(int i=0; i < int(this->workersA.size()); ++i) {
        this->workers[this->workersA[i]]++;
    }
}

AlbhwHeuristicResult::~AlbhwHeuristicResult() {
    delete tasksA;
}
void AlbhwHeuristicResult::initializeTrabs() {
    porTrab = new double[2];

}

void AlbhwHeuristicResult::imprimeArquivo() {
    string restante = "_T" + std::to_string(this->regraTA) + "_W" + std::to_string(this->regraTR) + "_H";
    string s = this->instance.substr(10, this->instance.length()-14) + restante;
    ofstream arq("Results/VND/" + s, ios::app);
    cout << "Results/VND/" + s;
    if(arq) {

        arq << this->valorObjetivoHeur << endl;
        arq << this->time << endl;
        for (int s = 0; s < this->numS ; ++s) {
            arq << "S " << s << " Time: " << this->workstationTime << " Trab " << this->workersA[s] << " ";
            arq << "[ ";
            for(int i = 0; i < int(this->tasksA[s].size()); ++i) {
                arq << this->tasksA[s].at(i) << " ";
            }
            arq << "]" << endl;
        }
    } else {
        cout << " Failed!!";
    }
    arq.close();
}