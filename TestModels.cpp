
#include <gurobi_c++.h>



class TestModel {
    public:
        TestModel() {

        }
        void testPaper(string destination, string instance) {
            AlbhwModel* testModel2 = new AlbhwModel(instance,0);
            AlbhwHeuristicResult* rmodel2 = testModel2->resolveAlbhwPaper(); 
            testModel2->printResults(rmodel2,destination, false);
        }
        void testImprovement(int melhorTA, int melhorTR, int numS, string destination, string instance) {
            AlbhwHeuristic* heur = new AlbhwHeuristic(instance);
            AlbhwHeuristicResult* result = heur->Heuristic(melhorTA, melhorTR);
            AlbhwModel* testModel = new AlbhwModel(instance,0);
            AlbhwHeuristicResult* rmodel = testModel->resolveAlbhwImproved(numS); 
            for(int i = 0; i <3; ++i) cout << " " << rmodel->workers[i];
             //testModel->printResults(rmodel,destination, true);
	    testModel->printResultsTerminal(rmodel);
		cout<< "Finito!";
        }


};
