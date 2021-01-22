

#include "AlbhwStructure.cpp"
#include "AlbhwModel.cpp"
#include "AlbhwHeuristic.cpp"
#include "ResultsModel.h"


#define REGRASTRAB 4
#define REGRASTAREFAS 15

class TestHeuristics {
    public:
        string destination;
        string filename;
        string instance;
        string output;
        int TA;
        int TR;
        int numS;
        TestHeuristics(string filename, string destination, string instance, string output) {
            this->destination = destination;
            this->filename = filename;
            this->instance = instance;
            string aux(destination + "/" + output);
            this->output = aux;
        }
        int verifyTests(AlbhwHeuristicResult* res, Graph* gr) {
            cout << "Verify if a result is feasible and good:";
            int cycT = gr->cycT;
            cout << "Workstatiom times - Cycle time: " << cycT << endl;
            cout << "Verify Workers: " << endl;
            int* auxPrec = new int[int(gr->allSucessors.size())];

            for(int i = 0; i < res->numS; ++i) {
                cout << "Workstation: " << i+1 << " " << res->workstationTime[i] << ". Worker: " << res->workersA[i] << endl; 
                if(res->workstationTime[i] > cycT) return 0;
                for(int j = 0; j < int(res->tasksA[i].size()); ++j) {
                    cout << "Tasks " << res->tasksA[i].at(j) << " Time: " << gr->taskTimes[res->tasksA[i].at(j)][res->workersA[i]] << endl;
                    if(gr->taskTimes[res->tasksA[i].at(j)][res->workersA[i]] > cycT) {
                        cout << "Seems bad!" << gr->taskTimes[res->tasksA[i].at(j)][res->workersA[i]] << endl;
                        return 0;
                    }
                    auxPrec[res->tasksA[i].at(j)] = i;
                }
            }
            for (int i = 0; i < res->numS; ++i) {
                for(int j = 0; j < int(res->tasksA[i].size()); ++j) {
                    int task = res->tasksA[i].at(j);
                    for(int k = 0; k < int(gr->precedences[task].size()); ++k){
                        if(auxPrec[gr->precedences[task].at(k)] > i) {
                            cout << "Error: " << auxPrec[gr->precedences[task].at(k)] << " e " << i << endl;
                            return 0;
                        }
                    }
                }
            }
            return 1;            
        }

        int main() {
            AlbhwHeuristic *heur;
            AlbhwHeuristicResult *result;
            int melhorV = 500 * 500;
            int melhorTA = 0;
            int melhorTR = 0;
            int melhorStations = 500;
            for(int i = 0; i < REGRASTRAB; ++i) {
                for(int j = 0; j < REGRASTAREFAS; ++j) {
                    heur = new AlbhwHeuristic(this->instance);
                    result = heur->Heuristic(j, i);
                    if(result->valorObjetivoHeur < melhorV) {
                        melhorTA = j;
                        melhorTR = i;
                        melhorV = result->valorObjetivoHeur;
                        if(result->numS < melhorStations) {
                            melhorStations = result->numS;
                        }
                    }
                }                
            } 
            this->TR = melhorTR;
            this->TA = melhorTA;
            ofstream arq(this->output, ios::app);
            if (arq) {
                arq << this->instance << ";" << melhorStations << ";" << melhorTA << ";" << melhorTR << ";" << endl;
            }
            arq.close();

            heur = new AlbhwHeuristic(instance);
            result = heur->Heuristic(melhorTA, melhorTR);
            //char d; cin >> d;
            // VND Test
            AlbhwModel* m = new AlbhwModel(this->instance,0);
            AlbhwHeuristicResult* vnd = m->RunVND(result, melhorTA, melhorTR);
            m->printResultsHeur(result, melhorTA, melhorTR, 1, destination, filename);
            m->printResultsHeur(vnd, melhorTA, melhorTR, 0, destination, filename);
            m->printResultsTerminal(vnd);
            //this->numS = vnd->numS; 
            cout << " VND " << vnd->numS;
            int ub;
            if(heur->getNumberOfWorkers() > 1) {
                cout <<" Value: " << vnd->valorObjetivoModelo << " Valor W: " <<heur->getWorkerValue(1) << endl;
                ub = floor(float(vnd->valorObjetivoModelo)/float(heur->getWorkerValue(2)));
            } else {
                ub = floor(float(vnd->valorObjetivoModelo)/float(heur->getWorkerValue(0)));               
            }
            //this->numS = std::min(int(ceil(ub)), heur->getNumberTasks());
            this->numS = ub;
		    cout << this->filename;
            cout << "Number of ws! " << this->numS;
            //cin >> d;
            // MIP1 Test
            heur = new AlbhwHeuristic(this->instance);
            AlbhwModel* m1 = new AlbhwModel(this->instance,0);
            AlbhwHeuristicResult* result1 = heur->Heuristic(melhorTA, melhorTR);
            AlbhwHeuristicResult* mip1 = m1->LocalSearchMIP1(result1);
            m->printResultsHeur(mip1, melhorTA, melhorTR, 2, destination, filename);
            
            // MIP2 Test
            heur = new AlbhwHeuristic(this->instance);
            AlbhwModel* m2 = new AlbhwModel(this->instance,0);
            AlbhwHeuristicResult* result2 = heur->Heuristic(melhorTA, melhorTR);
            AlbhwHeuristicResult* mip2 = m2->LocalSearchMIP2(result2);
            m->printResultsHeur(mip2, melhorTA, melhorTR, 3, destination, filename);

            delete heur;
            delete m;
            delete m1;
            delete m2;
            delete vnd;
            delete mip2;
            delete mip1;
            delete result;
            delete result1;
            delete result2;
            return 0;
        }

};
