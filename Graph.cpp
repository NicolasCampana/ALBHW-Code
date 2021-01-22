#include "Graph.h"
#include <algorithm>
#include <vector>
using namespace std;

Graph::~Graph(){
    for(int i = 0; i < this->numV; ++i) {
        delete this->p[i];
        delete this->fullClosure[i];
        //delete this->newP[i];
    }
    delete this->p;
    delete this->fullClosure;
    //delete[] tmax;
    //delete[] tmin;
    //delete this->newP;

}
Graph::Graph(AlbhwStructure* str) {
    this->precedences = str->predecessors;
    this->sucessors = str->sucessors;
    this->numV = str->numT;
    this->taskTimes = str->twTimes;
    this->simpleClosure = str->closure;
    this->cycT = str->cycT;
    this->initializeClousures();
    this->transitiveClosure();
}
void Graph::outputGraph(){
    for (int i = 0; i < int(this->edges.size()); ++i) {
        for(int j = 0; j < int(this->edges[i].size()); ++j) {
            cout << "Node: " << i << " with: " << edges[i].at(j) << endl;
        }
    }
}
void Graph::outputMatrix(int **p,int r1, int r2){
    cout << endl << "MATRIX : " << "Size: " << r1 << " x " << r2 << endl;
    for(int i = 0; i < r1; ++i) {
        for (int j = 0; j < r2; ++j) {
            cout << " " << p[i][j];
        }
        cout << endl;
    }
}
void Graph::initializeClousures() {
    // Verify if it needs everything
    this->fullClosure = new int*[this->numV];
    this->p = new int*[this->numV];
    for(int i = 0; i < this->numV; ++i) {
        //this->edges.push_back(vector<int>());
        this->fullClosure[i] = new int[this->numV];
        this->p[i] = new int[this->numV];
        this->paths.push_back(vector<vector<int>>());
        this->allPredecessors.push_back(vector<int>());
        this->allSucessors.push_back(vector<int>());
        for(int j = 0; j < this->numV; ++j) {
            this->paths[i].push_back(vector<int>());
            this->fullClosure[i][j] = 0;
            this->p[i][j] = 0;
        }
    }
}
void Graph::transitiveClosure() {
    for(int k = 0; k <  this->numV; ++k) {
        for(int i = 0; i <  this->numV; ++i) {
            for(int j = 0; j <  this->numV; ++j) {
                if((this->simpleClosure[i][j] == 1) or (this->simpleClosure[i][k] == 1 and this->simpleClosure[k][j] == 1 )) {
                    this->fullClosure[i][j] = 1;
                    /*if((this->simpleClosure[i][k] == 1 and this->simpleClosure[k][j] == 1 )) {
                        this->paths[i][j].insert(k);
                    }*/
                }
            }
        }
    }
    /*
    // Calculated with Floyd
    cout << "Calculado com floyd" << endl;
    for (int i = 0; i < this->numV; ++i) {
        for(int j = 0; j < this->numV; ++j) {
            if(this->paths[i][j].size() != 0) {
                cout << "Tasks: " << i << " " << j << endl;
                for (auto itr = this->paths[i][j].begin(); itr != this->paths[i][j].end(); ++itr) { 
                    cout << '\t' << *itr; 
                } 
                cout << endl;
            }
        }
    } */
    // Transitive closure - all sucessors
    for(int i = 0 ; i < this->numV; ++i) {
        for(int j = 0; j < this->numV; ++j) {
            if(this->fullClosure[i][j] == 1) {
                cout << i << " - " << j;
                this->allSucessors[i].push_back(j);
                this->allPredecessors[j].push_back(i);
            } else {
                cout << " 0 ";
            }
        }
        cout << endl;
    }
    // Calculated with the transitive clousure
    cout << "Calculado com TC" << endl;
    for (int i = 0; i < this->numV; ++i) {
        for (int j = 0; j < this->numV; ++j) {
            if(i != j) {
                if (this->fullClosure[i][j] == 1) {
                    this->paths[i][j].push_back(i);
                    set_intersection(this->allSucessors[i].begin(), this->allSucessors[i].end(),
                                    this->allPredecessors[j].begin(), this->allPredecessors[j].end(),
                                    back_inserter(this->paths[i][j]));
                    this->paths[i][j].push_back(j);
                    this->p[i][j] = calculateLowerBound1(this->paths[i][j]) - 1;
                } else {
                    this->p[i][j] = -1;

                }
            } else {
                this->p[i][j] = 0;
            }
        }
    }
}
void Graph::DFSUtil(int v, bool visited[]) 
{ 
    // Mark the current node as visited and 
    // print it 
    visited[v] = true; 
    cout << v << " "; 
  
    // Recur for all the vertices adjacent 
    // to this vertex 
    vector<int>::iterator i; 
    for (i = edges[v].begin(); i != edges[v].end(); ++i) 
        if (!visited[*i]) 
            DFSUtil(*i, visited); 
} 
  
// DFS traversal of the vertices reachable from v. 
// It uses recursive DFSUtil() 
void Graph::DFS(int v) 
{ 
    // Mark all the vertices as not visited 
    bool *visited = new bool[numV]; 
    for (int i = 0; i < numV; i++) 
        visited[i] = false; 
  
    // Call the recursive helper function 
    // to print DFS traversal 
    DFSUtil(v, visited); 
} 
/*
vector<int> Graph::getPossibleNodes(int taski, int taskj, int h) {
    set<int> copy = set<int>(this->paths[taski][taskj]);
    vector<int> aux;
    for (auto itr = this->paths[taski][taskj].begin(); itr != this->paths[taski][taskj].end(); ++itr) { 
        if(taskTimes[*itr][h] >= 1000) {
            cout << "Cleaned: " << *itr << endl;
            set_difference(copy.begin(),copy.end(),
            this->edges[*itr].begin(), this->edges[*itr].end(), std::inserter(aux, aux.end()));        
        }
    } 
    vector<int> aux2;
    return aux;
}*/
void Graph::addVertex(int v, int w) {
    this->edges[v].push_back(w);
}
void Graph::calculateCriticalPath(int ubStations){
    int newVertices = this->numV+2;
    vector< vector<int> > newP;
    int i,j;
    
    for(i = 0; i < newVertices; ++i) {
        tmin.push_back(0);
        tmax.push_back(ubStations);
        newP.push_back(vector<int>(newVertices));
    }
    for(i = 1; i < newVertices-1; ++i) {
        for(j=1; j < newVertices-1; ++j) {
            newP[i][j] = this->p[i-1][j-1];
        }
    }
    newP[0][0] = 0;
    newP[0][newVertices-1] = -1;
    newP[newVertices-1][0] = -1;
    newP[newVertices-1][newVertices-1] = 0;

    //outputMatrix(newP, newVertices, newVertices);
    // Precedence / Sucessors copy
    for(int i = 1; i < newVertices-1; ++i) {
        cout << "Task: " << i <<  " Precedoress: " << this->precedences[i-1].size() << " Sucessors: " << this->sucessors[i-1].size() << endl;
        if(int(this->precedences[i-1].size()) == 0) {
            cout << " No predecessors! " << i << endl;
            newP[0][i] = 0;
            newP[i][0] = -1;
        } else {
            newP[0][i] = -1;
            newP[i][0] = -1;
        }
        if(int(this->sucessors[i-1].size()) == 0) {
            cout << " No sucessors! " << i << " - First " << newP[i-1][newVertices-1] << endl;
            //if(i != newVertices-1)
            newP[i][newVertices-1] = 0;
            newP[newVertices-1][i] = -1;
        } else {
            newP[i][newVertices-1] = -1;
            newP[newVertices-1][i] = -1;
        }
    }
    //this->outputMatrix(newP, newVertices, newVertices);
    
    int maior = 0;
    for(int k = 0; k < newVertices; ++k) {
        maior = 0;
        for(int i = 0; i < newVertices; ++i) {
            if(newP[i][k] != -1 and i != k) {
                if( maior < tmin[i]+newP[i][k]) {
                    maior = tmin[i]+newP[i][k];
                }
            }
            cout << newP[i][k] << " ";            
        }
        tmin[k] = maior;
        //cout << tmin[k] << " ";
        tmax[k] = tmin[k];
    }
    //this->outputMatrix(newP, newVertices, newVertices);
   // cout <<"End of Early Workstation" << endl;

    tmax[newVertices-1] = ubStations;
    for(size_t i = 0; i < tmax.size(); ++i) {
        cout << "I: " << i << "  " << tmax.at(i) << endl; 
    }    
    int menor;
    cout << newVertices-2 << " And " << newVertices-1 << " " << newVertices;
    for (int k = newVertices-2; k > 0; --k) {
        menor = 99999;
        for(int i = newVertices-1; i > 0; --i) {
            if(newP[k][i] != -1 and i != k) {                
                if( menor > tmax[i]-newP[k][i]) {
                    menor = tmax[i]-newP[k][i];
                    cout << "Achou um melhor!" << "Valor da precedencia: " << newP[k][i] << " - " << k << " -> " << i << endl;
                }
            }  
       }
        cout << "Para " << k << " Menor: " << menor << " ";
        if(menor != 9999) {
            tmax[k] = menor;
        }
    }

    cout << "Tasks: " << endl;
    for(int i = 1; i < newVertices-1; ++i){
        cout << "(" << tmin[i] << ", " << tmax[i] << ")" << endl;
    }
    
    /*for (int i = 0; i < newVertices; ++i) {   
        delete newP[i];
    }
    delete[] newP;
    
    
    cout << "Graph: " << endl;
    cout << "Faz parte do critical path: ";
    for(int i = 0; i < newVertices; ++i) {
        if (tmin[i] == tmax[i]) {
            cout << i << " ";
        }
    } */

}
void Graph::updatePrecedences() {
    //this->outputMatrix(this->p, this->numV, this->numV);
     for(int k = 0; k <  this->numV; ++k) {
        for(int i = 0; i <  this->numV; ++i) {
            for(int j = 0; j <  this->numV; ++j) {
                if(i != j and this->p[i][j] != -1 and this->p[i][k] != -1 and this->p[k][j] != -1) {
                    if(this->p[i][j] < this->p[i][k] + this->p[k][j]){
                        cout << "entre " << i << " e " << j << " tem: " << k;
                        this->p[i][j] = this->p[i][k]+this->p[k][j];
                    }          
                }
            }
        }
    }
    /*
    int maior;
    for (int k = 0; k < this->numV; ++k) {
        for(int i = 0; i < this->numV; ++i) {
            for(int j = 0; j < this->numV; ++j) {
                    if(i!=j)
                        this->p[i][j] = max(this->p[i][j], this->p[i][k] +this->p[k][j]);
                }
            }
    } */
    //this->outputMatrix(this->p);

}
void Graph::createPrecendenceTransitive() {
    int tasks = this->numV;
    cout << "Aqui acabou também! " << tasks << endl;
    vector<vector< vector<int> >> t;
    for(int i = 0; i < tasks; ++i) {
        t.push_back(vector<vector<int>>());
        for (int j = 0; j < tasks; ++j) {
            t[i].push_back(vector<int>());
            if(i != j) {
                //cout << "Tarefas: " << i << " and: " << j << endl;
                for(auto it = paths[i][j].begin(); it != paths[i][j].end(); ++it) { 
                    t[i][j].push_back(*it);
                    //cout << " " << *it;
                }
                t[i][j].push_back(i);
                t[i][j].push_back(j);
                //cout << "Tasks per calcolare: ";
                for(auto it = t[i][j].begin(); it != t[i][j].end(); ++it) { 
                    cout << *it << " ";
                }
                //gr.DFS(i);
                cout << endl;
                //matrixOfPrecedenceTransitive[i][j] = calculateLowerBound1(t[i][j]) - 1;
                
            } else {
                //matrixOfPrecedenceTransitive[i][j] = 0;
            }
        }
    }
    /*
    cout << "Conjuntos: ";
    for(size_t i = 0; i < t.size(); ++i) {
        for (size_t j = 0; j < t[i].size(); ++j) {
            if(t[i][j].size() > 2) {
                cout << "Tarefa " << i << " - " << j << " " << endl;
                for(size_t k = 0; k < t[i][j].size(); ++k) {
                    cout << t[i][j][k] << " ";
                }
             }
        }
    }*/
    
    //return matrixOfPrecedenceTransitive;
} 
int Graph::calculateLowerBound2() {
    vector<int> aux;
    for(int j = 0; j < int(this->allPredecessors.size()); ++j) {
        if(int(this->allPredecessors[j].size()) == 0) {
            for(int i = 0; i < int(this->allSucessors.size()); ++i) {
                if(int(this->allSucessors[i].size()) == 0) {
                    aux.clear();
                    set_intersection(this->allSucessors[j].begin(), this->allSucessors[j].end(),
                                                this->allPredecessors[i].begin(), this->allPredecessors[i].end(),
                                                back_inserter(aux));
                
                    /*for(int k = 0; k < int(aux.size(); ++k) {
                        cout << aux.at(k) << " - " << this->p[j][aux.at(k)] << endl;
                    }*/
                    if(int(aux.size()) !=0)
                        if (this->p[i][j] < 1)
                            this->p[i][j] = 1; 
                    cout << "Finito: " << j << " " << i << endl;
                } 
            }
        }
    }
    return 0;
    
}
int Graph::calculateLowerBound1(vector<int>& tasks) {
    double value = 0;
    for(auto& x: tasks)
        value += this->taskTimes[x][0];

    //cout << " PORRA: " << value << endl;
    //char c; cin >> c;
    return ceil(value/this->cycT);

}