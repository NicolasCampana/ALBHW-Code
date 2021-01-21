#include <iostream>
#include <vector>
#include <set>
using namespace std;
class Node {
    public:
        vector<Node> adjacents;
        double* cycTime;

};
class Graph {
    friend class AlbhwModel;
    public:
        // Variable
        vector<vector<int>> edges;
        vector<vector<vector<int>>> paths;
        int** criticalPath;
        int** simpleClosure;
        int** fullClosure; 
        int** taskTimes;
        int** newP;
        int* tmax;
        int* tmin;
        vector<vector<int>> allSucessors;
        vector<vector<int>> allPredecessors;
        int numV;
        int cycT;
        int** p;
        vector<int>* precedences;
        vector<int>* sucessors;
        // Methods
        ~Graph();
        Graph(AlbhwStructure* str);
        vector<int> getPossibleNodes(int taski, int taskj, int h);        
        int calculateLowerBound1(vector<int>& tasks);
        int calculateLowerBound2();

        void updatePrecedences();
        void outputMatrix(int **p, int r1, int r2);
        void createPrecendenceTransitive();        
        void addVertex(int w, int v);
        void createGraph();
        void transitiveClosure();
        void createPaths();
        void outputGraph();
        void DFSUtil(int v, bool visited[]);
        void DFS(int v);
        void initializeClousures();
        void calculateCriticalPath(int ubStations);
};