

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
            string aux(destination + output);
            this->output = aux;
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
                    cout << this->instance;
                    heur = new AlbhwHeuristic(this->instance);
                    result = heur->Heuristic(j, i);
                    //char c; cin >> c;
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
            // VND Test
            AlbhwModel* m = new AlbhwModel(this->instance, 0);
            AlbhwHeuristicResult* vnd = m->RunVND(result, melhorTA, melhorTR);
            //m->printResultsHeur(result, melhorTA, melhorTR, 1, destination, filename);
            //m->printResultsHeur(vnd, melhorTA, melhorTR, 0, destination, filename);
            this->numS = vnd->numS;


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
            delete m;
            delete m1;
            delete m2;
            
            return 0;
        }

};
