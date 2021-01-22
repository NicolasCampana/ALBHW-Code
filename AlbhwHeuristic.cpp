//
// Created by nicolas on 22/04/19.
//
#include "AlbhwHeuristic.h"
#include "AlbhwStructure.h"
#include "AlbhwHeuristicResult.cpp"

AlbhwHeuristic::AlbhwHeuristic() {

}


AlbhwHeuristic::~AlbhwHeuristic(){
    for(int i = 0; i < this->instanceProblem->numT; ++i) {
        //delete [] closure;
        delete [] matrizEstacoes;
    }
    delete [] matrizEstacoes;
    //delete [] closure;
    delete qntLambdas;
    delete qntLambdasDraw;
    delete tasksA;
    delete totalTime;
    delete removedWorkstations;
    delete qntTrabAlocados;
    delete instanceProblem;
}
AlbhwHeuristic::AlbhwHeuristic(string instance) {
    this->instanceProblem = new AlbhwStructure();
    this->instanceProblem->readFile(instance);
    this->setProblem();
}
void AlbhwHeuristic::AlbhwHeuristicSolo(string instance) {
    this->instanceProblem = new AlbhwStructure();
    this->instanceProblem->readFile(instance);
    this->setProblem();
}
void AlbhwHeuristic::setProblem() {
    totalTime = new int[this->instanceProblem->numW];
    qntTrabAlocados = new double[this->instanceProblem->numW];
    cTotal = 0;
    gr = new Graph(instanceProblem);
    qntLambdas = new int[this->instanceProblem->numW];
    qntLambdasDraw = new int[this->instanceProblem->numW];
    for(int i =0 ; i < this->instanceProblem->numW; ++i) {
        qntLambdas[i] = 0;
        qntLambdasDraw[i] = 0;
        qntTrabAlocados[i] = 0;
    }
    for(int i = 0; i < this->instanceProblem->numT; ++i) {
        origGraph.push_back(i);
        stationsTasks.push_back(-1);
    }
    sizeOfGraph = origGraph.size();
    tasksA = new std::vector<int>[this->instanceProblem->numW];
}


AlbhwHeuristicResult* AlbhwHeuristic::Heuristic(int optRegra, int optTrab) {
    int numS = this->instanceProblem->numS;
    std::vector<int>* predAtual = new std::vector<int>[this->instanceProblem->numW];
    tasksA = new vector<int>[this->instanceProblem->numT]; // tarefas em cada estação
    std::vector<int> tasks;
    int task; // the task that will be allocated
    int newTask; // Maybe we need to find other task
    std::vector<int>* setofTasks = new vector<int>[this->instanceProblem->numW]; // Usar para escolher a melhor entre os 3 tipos de trabalhadores

    predTasks(&predOrig);
    for(int i = 0; i < this->instanceProblem->numW; ++i) {
        predTasks(&predAtual[i]);
    }
    for(int i = 0; i < this->instanceProblem->numW; ++i) {
        totalTime[i] = 0;
    }
    bool tasksAvaliables = true;
    // cout << "Lower bound: " << wsLowerBound() << " . Upper bound: " << wsUpperBound() << endl;
    for(s = 0; (s < numS) and sizeOfGraph > 0; s++) {
        // cout << "TAREFAS RESTANTES: " << sizeOfGraph << endl;
        // cout << "S: " << s << " " << predOrig.size() << " and stations: " << numS;
        for(int w = 0; w < this->instanceProblem->numW; ++w) {
            // cout << "Calculando para o próximo trab: " << predOrig.size();
            for(int x = 0; x < int(predOrig.size()); ++x) {
                predAtual[w][x] = predOrig[x];
            }

            do {
                // cout << "Conteudo de Tasks: ";
                for(int x = 0; x < int(predAtual[w].size()); x++) {
                    // cout << predAtual[w].at(x) << " ";
                }
                tasks = avaliableTasks(&predAtual[w]);
                // cout << "Tasks disponíveis: " << endl;
                for(int x = 0; x < int(tasks.size()); x++) {
                    // cout << tasks.at(x) << " ";
                }
                //char ss; cin >> ss;

                ////Escolhe tarefa
                if(int(tasks.size()) != 0){
                    task = chooseTask(optRegra, w, tasks);
                }
                // char ss; cin >> ss;
                // Its possible to choose a infeasible task to that worker
                // Then we choose other, searching for the best using of the space remaining
                if(task == -1 or (totalTime[w] + this->instanceProblem->twTimes[task][w]) > this->instanceProblem->cycT) {
                    // cout << "LAVORATORE: " << w << endl;
                    // cout << "Tinha escolhido a task: " << task << endl;
                    // cout << "Possibile: ";
                    for(int x = 0; x < int(tasks.size()); ++x) {
                        // cout << tasks.at(x) << " ";
                    }
                    // cout << endl;
                    newTask = this->GetNewTask(tasks, task, w, predAtual, totalTime[w]);
                    //Se não achar nenhuma outra tarefa, garante que vai sair do loop e não vai entrar ali no if
                    //if(totalTime[w] +  this->instanceProblem->twTimes[task][w] >  this->instanceProblem->cycT) {
                    // cout << "Escolhida final: " << newTask << endl;
                    if(newTask == -1 or newTask == task) {
                        // cout << "Deu bom não! " << endl;
                        tasksAvaliables = false;
                    } else {
                        task = newTask;
                        // cout << "Achou uma melhor!: " << task << " tempo da task: " << this->instanceProblem->twTimes[task][w] << " - WS + Task: " << (this->instanceProblem->twTimes[task][w] + totalTime[w]);
                        //char c; cin >> c;
                    }
                }
                // Se entrar aqui, escolheu uma tarefa que cabe no tempo de ciclo
                if(tasks.size() != 0 && tasksAvaliables) {
                    // cout << " ESCOLHIDA " << task << endl;
                    predAtual[w].at(task)--;
                    for(int j = 0; j < int( this->instanceProblem->sucessors[task].size()); ++j) {
                        predAtual[w].at(this->instanceProblem->sucessors[task].at(j))--;
                        // cout << " Sucessores " <<  this->instanceProblem->sucessors[task].at(j);
                    }
                    setofTasks[w].push_back(task); // Coloca a tarefa no determinado set of tasks do determinado trabalhador
                    totalTime[w] +=  this->instanceProblem->twTimes[task][w];	// Soma o tempo total, com o tempo necessitado para executar a tarefa escolhida
                    // cout << "TEMPO TOTAL DO SETOFTASKS: " << w << " => " <<  totalTime[w] << endl;

                } else {
                    tasksAvaliables = false;
                    // cout << "Esse trabalhador não pode executar mais tarefas" << endl;
                    // cout << "Tempo workstations: " << totalTime[w] << endl;
                }
            } while (tasksAvaliables && (totalTime[w] <= this->instanceProblem->cycT));
            tasksAvaliables = true;
        }
        for (int a = 0; a < this->instanceProblem->numW; ++a) {
            // cout << totalTime[a] << " ";
        }
        cout <<  "Choosing worker" << endl;
        // Opção para escolhas de trabalhador
        switch(optTrab) {
            case 0:
                tasksA[s] = this->bestOfWorkersLambda(setofTasks, predAtual, totalTime);
                break;
            case 1:
                tasksA[s] = this->bestOfWorkersUpPerTask(setofTasks, predAtual, totalTime);
                break;
            case 2:
                tasksA[s] = this->bestOfWorkersLessTime(setofTasks, predAtual, totalTime);
                break;
            case 3:
                tasksA[s] = this->bestOfWorkersTimeWorkStation(setofTasks, predAtual, totalTime);
                break;
        }
        cout << "All tasks" << endl;
        for(int w = 0; w < this->instanceProblem->numW; ++w ) {
            cout << "Worker: " << w << " : ";
            for(unsigned int i = 0; i < setofTasks[w].size(); ++i) {
                cout << setofTasks[w].at(i) << " ";
            }
          }
        for(int i = 0; i <  this->instanceProblem->numW; ++i) {
            setofTasks[i].clear();

        }

        cout << "Selected tasks " << endl;

        for(int i = 0; i < int(tasksA[s].size()); ++i) {
            stationsTasks[tasksA[s].at(i)] = s;
            cout << tasksA[s].at(i) << " ";
        }

    }
    //Seta a nova quantidade de estações
    numS = s;

    // cout << "ANTES " << cTotal << endl;
    cout << "Tarefas alocadas: " << endl;
    for(int i = 0; i < s; ++i) {
        cout << "Station: " << i <<endl;
        for(int j = 0; j < int(tasksA[i].size()); ++j) {
            cout << tasksA[i].at(j) << " ";
            cout << " trabalhador: " << workersA[i];
            cout << endl;
        }
    }
    //cin >> s;
    ///// Fim das heuristicas

    for(int i = 0; i < int(stationsTasks.size()); ++i) {
        // cout << stationsTasks[i] << " ";
    }
    // cout << "Tarefas alocadas: " << endl;
    for(int i = 0; i < s; ++i) {
        // cout << "Station: " << i << " Lavoratore: " << workersA.at(i) << " -> ";
        for(int j = 0; j < int(tasksA[i].size()); ++j) {
            // cout << tasksA[i].at(j) << " ";
        }
        // cout << endl;
    }
    // cout << "Trabalhadores escolhidos: ";
    for(int i = 0; i < int(workersA.size()); ++i) {
        // cout << workersA.at(i) << " ";
    }
    // cout << endl;
    // cout << "Tempo das estações: ";
    for(int i = 0; i < int(timesA.size()); ++i) {
        // cout << timesA.at(i) << " ";
    }
    // cout << endl;
    // cout << "PREÇO TOTAL: " << cTotal << " Foi utilizado a Regra " << optRegra << endl;result->workstationTime = new int[resultNumS];
    int t;
    int* workstationTimes = new int[numS];
    for (int s = 0; s < numS; ++s) {
        t = 0;
        for (int i = 0; i < int(tasksA[s].size()); ++i) {
            t += this->instanceProblem->twTimes[tasksA[s].at(i)][workersA[s]];
        }
        workstationTimes[s] = t;
        cout << "tempo da estação " << s << ": " << t << endl;
    }
    return new AlbhwHeuristicResult(tasksA, workstationTimes, workersA, stationsTasks,estacoesDeletadas, qntTrabAlocados, numS, cTotal, optRegra, optTrab, this->instanceProblem->instance);
}
// Best fit of task in the remaining space
int AlbhwHeuristic::GetNewTask(vector<int> tasks, int task, int worker, vector<int>* predAtual, int totalTime) {
    int betterI = -1;
    int betterV = 0;
    int auxV;
    for(int x = 0; x < int(tasks.size()); ++x) {
        auxV = this->instanceProblem->twTimes[tasks.at(x)][worker] + totalTime;
        if(task != tasks.at(x) and worker <= this->instanceProblem->tTypes[tasks.at(x)] and predAtual[worker].at(tasks.at(x)) == 0 and
           auxV > betterV and auxV <= this->instanceProblem->cycT) {
            betterI = tasks.at(x);
            betterV = auxV;
            // cout << "Achou uma substituta!! > " << betterI << " - " << betterV << endl;
        }
    }
    return betterI;
}
void AlbhwHeuristic::predTasks(std::vector<int>* pred) {
    for(int i = 0; i < int(origGraph.size()); ++i) {
        pred->push_back(this->instanceProblem->predecessors[i].size());
    }
}

int AlbhwHeuristic::chooseTask(int opt,int trab, std::vector<int> tasks) {
    switch(opt) {
        case 0:
            // Bigger number of followers
            return MaxF(trab,tasks);
        case 1:
            // Bigger number of imediate followers
            return MaxIF(trab,tasks);
        case 2:
            // The greatest time in the smallest ones
            return MaxTimeM(trab,tasks);
        case 3:
            // The greatest time in the biggest ones
            return MaxTimeP(trab,tasks);
        case 4:
            // The greatest time in the smallest ones
            return MaxPWM(trab, tasks);
        case 5:
            // The greatest time in the biggest ones
            return MaxPWP(trab, tasks);
        case 6:
            // The greatest time with the worker "trab"
            return MaxPW(trab, tasks);
        case 7:
            // The smallest time in the smallest ones
            return MinTimeM(trab, tasks);
        case 8:
            // The smallest time in the biggest ones
            return MinTimeP(trab, tasks);
        case 9:
            // The greatest time / number of imediate followers
            return MaxIFTime(trab, tasks);
        case 10:
            // The greatest time followers / number of followers
            return MaxFTime(trab,tasks);
        case 11:
            // Smallest time for the worker "trab"
            return MinTimeTrab(trab, tasks);
        case 12:
            // Biggest time for the worker "trab"
            return MaxTimeTrab(trab, tasks);
        case 13:
            // In the tasks available that is the type worker "trab", choose the biggest one
            return MaxTimeTrabWorker(trab, tasks);
        case 14:
            // In the tasks available that is the type worker "trab", choose the smallest one
            return MinTimeTrabWorker(trab, tasks);

    }
    return -1;
}
void AlbhwHeuristic::initializeMatrix() {
    matrizEstacoes = new bool*[this->instanceProblem->numT];
    for(int i = 0; i <  this->instanceProblem->numT; i++) {
        matrizEstacoes[i] = new bool[s];
        for(int j =0; j < s; ++j) {
            matrizEstacoes[i][j] = true;
            //// cout << matrizEstacoes[i][j];
        }
        //// cout << endl;
    }
}
/*
void AlbhwHeuristic::transitiveClosure() {
    // MAtriz inicial direta
    for(int k = 0; k <  this->instanceProblem->numT; ++k) {
        for(int i = 0; i <  this->instanceProblem->numT; ++i) {
            for(int j = 0; j < this->instanceProblem->numT; ++j) {
                if((closure[i][j] == 1) or (closure[i][k] == 1 and closure[k][j] == 1 )) {
                    this->closure[i][j] = 1;
                }
            }
        }
    }

    // Transitive closure - all sucessors
    for(int i = 0 ; i <  this->instanceProblem->numT; ++i) {
        for(int j = 0; j <  this->instanceProblem->numT; ++j) {
            if(this->closure[i][j] == 1) {
                // cout << i << " - " << j;
                this->gr->allSucessors[i].push_back(j);
            } else {
                // cout << " 0 ";
            }
        }
        // cout << endl;
    }
    // Per fare interseccione tra predecessor and sucessor
    vector<int>** p = new vector<int>*[this->instanceProblem->numT];
    std::vector<int>::iterator it;
    for(int i = 0 ; i < this->instanceProblem->numT; ++i) {
        for(int j = 0; j < this->instanceProblem->numT; ++j) {
            p[i][j] = new vector<int>(this->instanceProblem->numT);
            it=std::set_intersection (this->instanceProblem->sucessors[j],
                    this->instanceProblem->sucessors[j].size(), this->instanceProblem->predecessors[i],
                    this->instanceProblem->predecessors[i].size(), p[i][j].begin());
            p[i][j].resize(it-p[i][j].begin());

        }
    }
    //DEBUG
    for (int i =0 ; i < this->instanceProblem->numT; ++i) {
        for (int j = 0; j < p[i]->size(); ++j) {
            cout<< " " << this->gr->allSucessors[i][j] << " ";
        }
} */
int AlbhwHeuristic::MaxF(int trab, std::vector<int> tasks) {
    // MaxF
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        // First choose the bigger
        if(int(this->gr->allSucessors[tasks.at(i)].size()) > bigger) {
            bigger = this->gr->allSucessors[tasks.at(i)].size();
            choosed = tasks.at(i);
        } else if(int(this->gr->allSucessors[tasks.at(i)].size()) == bigger) {
            // If the task i is equal, get the task with less time
            if(this->instanceProblem->twTimes[tasks.at(i)][trab] <  this->instanceProblem->twTimes[choosed][trab]) {
                bigger = this->gr->allSucessors[tasks.at(i)].size();
                choosed = tasks.at(i);
            } else if( this->instanceProblem->twTimes[tasks.at(i)][trab] ==  this->instanceProblem->twTimes[choosed][trab]) {
                // If is equal, choose lexicographly
                if(tasks.at(i) < choosed) {
                    bigger = this->gr->allSucessors[tasks.at(i)].size();
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
int AlbhwHeuristic::MaxIF(int trab, std::vector<int> tasks) {
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        // First choose the bigger
        if(int( this->gr->sucessors[tasks.at(i)].size()) > bigger) {
            bigger =  this->instanceProblem->sucessors[tasks.at(i)].size();
            choosed = tasks.at(i);
        } else if(int( this->gr->sucessors[tasks.at(i)].size()) == bigger) {
            // If the task i is equal, get the task with less time
            if(this->instanceProblem->twTimes[tasks.at(i)][trab] < this->instanceProblem->twTimes[choosed][trab]) {
                bigger =  this->instanceProblem->sucessors[tasks.at(i)].size();
                choosed = tasks.at(i);
            } else if(this->instanceProblem->twTimes[tasks.at(i)][trab] ==  this->instanceProblem->twTimes[choosed][trab]) {
                    // If is equal, choose lexicographly
                    if(tasks.at(i) < choosed) {
                    bigger =  this->instanceProblem->sucessors[tasks.at(i)].size();
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
// Biggest time in minimum time
// MaxTime-
// First try
int AlbhwHeuristic::MaxTimeM(int trab, std::vector<int> tasks) {
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        if( this->instanceProblem->twTimes[tasks.at(i)][0] > bigger) {
            bigger =  this->instanceProblem->twTimes[tasks.at(i)][0];
            choosed = tasks.at(i);
        } else if( this->instanceProblem->twTimes[tasks.at(i)][0] == bigger) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                bigger =  this->instanceProblem->twTimes[tasks.at(i)][0];
                choosed = tasks.at(i);
            } else if(this->gr->allSucessors[tasks.at(i)].size() == this->gr->allSucessors[choosed].size()) {
                if(tasks.at(i) < bigger) {
                    bigger =  this->instanceProblem->twTimes[tasks.at(i)][0];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
//Biggest time in maximum time
//MaxTime+
int AlbhwHeuristic::MaxTimeP(int trab, std::vector<int> tasks) {
    //int ultCol = this->instanceProblem->wCosts.size() - 1; // Ultima coluna da matriz de tempos
    int lastWorker; // Get the last worker that can execute the task
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); i++) {
        lastWorker = this->instanceProblem->tTypes[tasks.at(i)];
        if( this->instanceProblem->twTimes[tasks.at(i)][lastWorker] > bigger) {
            bigger = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
            choosed = tasks.at(i);
            //Se for ==, pega o que tem maior número de sucessores se não, lexicográfico
        } else if( this->instanceProblem->twTimes[tasks.at(i)][lastWorker] == bigger) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                bigger =  this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
                choosed = tasks.at(i);
            } else if(this->gr->allSucessors[tasks.at(i)].size() == this->gr->allSucessors[choosed].size()) {
                if(tasks.at(i) < choosed) {
                    bigger = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
// Smallest time in the minimum time
// MinTime-
int AlbhwHeuristic::MinTimeM(int trab, std::vector<int> tasks) {
    int smaller = NAO_PODE_EXECUTAR;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i){
        if( this->instanceProblem->twTimes[tasks.at(i)][0] < smaller) {
            smaller =  this->instanceProblem->twTimes[tasks.at(i)][0];
            choosed = tasks.at(i);
            } else if(this->instanceProblem->twTimes[tasks.at(i)][0] == smaller) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                smaller =  this->instanceProblem->twTimes[tasks.at(i)][0];
                choosed = tasks.at(i);
            } else if(this->gr->allSucessors[tasks.at(i)].size() == this->gr->allSucessors[choosed].size()) {
                if(tasks.at(i) < choosed) {
                    smaller =  this->instanceProblem->twTimes[tasks.at(i)][0];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
//Smallest time in maximum time
//MinTime+
int AlbhwHeuristic::MinTimeP(int trab, std::vector<int> tasks) {
    int lastWorker;
    int smaller = NAO_PODE_EXECUTAR;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        lastWorker = this->instanceProblem->tTypes[tasks.at(i)];
        if( this->instanceProblem->twTimes[tasks.at(i)][lastWorker] < smaller) {
            smaller = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
            choosed = tasks.at(i);
        } else if(this->instanceProblem->twTimes[tasks.at(i)][lastWorker] == smaller) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                smaller = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
                choosed = tasks.at(i);

            } else if( this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size() ) {
                if(tasks.at(i) < choosed) {
                    smaller = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
int AlbhwHeuristic::MaxPW(int trab, vector<int> tasks) {
    int aux = 0;
    int* parcialResults = new int[int(tasks.size())];
    for(int i = 0; i < int(tasks.size()); ++i) {
        aux = this->instanceProblem->twTimes[tasks.at(i)][trab];
        for(int j = 0; j < int(this->gr->allSucessors[tasks.at(i)].size()); ++j) {
            aux += this->instanceProblem->twTimes[this->gr->allSucessors[tasks.at(i)].at(j)][trab];
        }
        parcialResults[i] = aux;
    }
    int choosed = -1; // Index
    int results = -1; // value
    for( int i = 0; i < int(tasks.size()); ++i) {
        if(parcialResults[i] > results) {
            results = parcialResults[i];
            choosed = tasks.at(i);
        } else if(parcialResults[i] == results) {
            if(this->instanceProblem->twTimes[tasks.at(i)][trab] <
               this->instanceProblem->twTimes[choosed][trab]) {
                results = parcialResults[i];
                choosed = tasks.at(i);
            } else if (this->instanceProblem->twTimes[tasks.at(i)][trab]  ==
                       this->instanceProblem->twTimes[choosed][trab]) {
                if(tasks.at(i) < choosed) {
                    results = parcialResults[i];
                    choosed = tasks.at(i);
                }
            }
        }
    }

    return choosed;
}
// Bigger in the minimum pw times
// MaxPW-
int AlbhwHeuristic::MaxPWM(int trab, std::vector<int> tasks) {
    int aux = 0;
    int* parcialResults = new int[int(tasks.size())];
    for(int i = 0; i < int(tasks.size()); ++i) {
        aux = this->instanceProblem->twTimes[tasks.at(i)][0];
        for(int j = 0; j < int(this->gr->allSucessors[tasks.at(i)].size()); ++j) {
            aux += this->instanceProblem->twTimes[this->gr->allSucessors[tasks.at(i)].at(j)][0];
        }
        parcialResults[i] = aux;
    }
    int choosed = -1; // Index
    int results = -1; // value
    for( int i = 0; i < int(tasks.size()); ++i) {
        if(parcialResults[i] > results) {
            results = parcialResults[i];
            choosed = tasks.at(i);
        } else if(parcialResults[i] == results) {
            if(this->instanceProblem->twTimes[tasks.at(i)][0] <
               this->instanceProblem->twTimes[choosed][0]) {
                results = parcialResults[i];
                choosed = tasks.at(i);
            } else if (this->instanceProblem->twTimes[tasks.at(i)][0]  ==
                       this->instanceProblem->twTimes[choosed][0]) {
                if(tasks.at(i) < choosed) {
                    results = parcialResults[i];
                    choosed = tasks.at(i);
                }
            }
        }
    }

    return choosed;
}
// Biggest pw in the maximums times
// MaxPW+
int AlbhwHeuristic::MaxPWP(int trab, std::vector<int> tasks) {
    int lastWorker;
    int aux = 0;
    int* parcialResults = new int[int(tasks.size())];
    for(int i = 0; i < int(tasks.size()); ++i) {
        lastWorker = this->instanceProblem->tTypes[tasks.at(i)];
        aux = this->instanceProblem->twTimes[tasks.at(i)][lastWorker];
        for(int j = 0; j < int(this->gr->allSucessors[tasks.at(i)].size()); ++j) {
            aux += this->instanceProblem->twTimes[this->gr->allSucessors[tasks.at(i)].at(j)][lastWorker];
        }
        parcialResults[i] = aux;
    }
    int choosed = -1; // Indice
    int results = -1; // Valor
    for(int i = 0; i < int(tasks.size()); ++i) {
        lastWorker = this->instanceProblem->tTypes[tasks.at(i)];
        if(parcialResults[i] > results) {
            results = parcialResults[i];
            choosed = tasks.at(i);
        } else if(parcialResults[i] == results) {
            if(this->instanceProblem->twTimes[tasks.at(i)][lastWorker] <
               this->instanceProblem->twTimes[choosed][lastWorker]) {
                results = parcialResults[i];
                choosed = tasks.at(i);
            } else if (this->instanceProblem->twTimes[tasks.at(i)][lastWorker]  == this->instanceProblem->twTimes[choosed][lastWorker]) {
                if(tasks.at(i) < choosed) {
                    results = parcialResults[i];
                    choosed = tasks.at(i);
                }
            }
        }
    }

    return choosed;
}

//Max number of immediate followers per time
//MaxIFTime
int AlbhwHeuristic::MaxIFTime(int trab, std::vector<int> tasks) {
    double bigger = -1;
    int choosed = -1;
    double tot;
    for(int i = 0; i < int(tasks.size()); ++i) {
        tot = this->gr->sucessors[tasks.at(i)].size()/this->instanceProblem->twTimes[tasks.at(i)][trab];
        if((tot <= this->instanceProblem->cycT) and (tot > bigger)) {
            bigger = tot;
            choosed = tasks.at(i);
        } else if(tot == bigger) {
            if(this->gr->sucessors[tasks.at(i)].size() > this->gr->sucessors[choosed].size()) {
                bigger = tot;
                choosed = tasks.at(i);
            } else {
                if(tasks.at(i) < choosed) {
                    bigger = tot;
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
//Max number of followers per time
//MaxFTime
int AlbhwHeuristic::MaxFTime(int trab, std::vector<int> tasks) {
    double bigger = -1;
    int choosed = -1;
    double tot;
    for(int i = 0; i < int(tasks.size()); ++i) {
        tot = this->gr->allSucessors[tasks.at(i)].size()/this->instanceProblem->twTimes[tasks.at(i)][trab];
        if((tot <= this->instanceProblem->cycT) and (tot > bigger)) {
            bigger = tot;
            choosed = tasks.at(i);
        } else if(tot == bigger) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                bigger = tot;
                choosed = tasks.at(i);
            } else {
                if(tasks.at(i) < choosed) {
                    bigger = tot;
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}

//Min time task to worker
//MinTimeTrab
int AlbhwHeuristic::MinTimeTrab(int trab, std::vector<int> tasks) {
    int smaller = NAO_PODE_EXECUTAR;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        if( this->instanceProblem->twTimes[tasks.at(i)][trab] < smaller) {
            smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
            choosed = tasks.at(i);
        } else if(this->instanceProblem->twTimes[tasks.at(i)][trab] == smaller) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
                choosed = tasks.at(i);
            } else if( this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size() ) {
                if(tasks.at(i) < choosed) {
                    smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}

//Max time task to worker
//MaxTimeTrab
int AlbhwHeuristic::MaxTimeTrab(int trab, std::vector<int> tasks) {
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        if( this->instanceProblem->twTimes[tasks.at(i)][trab] <= this->instanceProblem->cycT and
            this->instanceProblem->twTimes[tasks.at(i)][trab] > bigger) {
            bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
            choosed = tasks.at(i);
        } else if(this->instanceProblem->twTimes[tasks.at(i)][trab] == bigger) {
            if(this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size()) {
                bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
                choosed = tasks.at(i);
            } else if( this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size() ) {
                if(tasks.at(i) < choosed) {
                    bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}


//Max time task that the work can execute
//MaxTimeTrabWorker
int AlbhwHeuristic::MaxTimeTrabWorker(int trab, std::vector<int> tasks) {
    int bigger = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        if(this->instanceProblem->tTypes[tasks.at(i)] == trab and this->instanceProblem->twTimes[tasks.at(i)][trab] > bigger) {
            bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
            choosed = tasks.at(i);
        }
        /*else if(this->instanceProblem->twTimes[tasks.at(i)][trab] == bigger) {
            if(int(this->gr->allSucessors[tasks.at(i)].size()) > int(this->gr->allSucessors[choosed].size())) {
                bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
                choosed = tasks.at(i);
            } else if( this->gr->allSucessors[tasks.at(i)].size() > this->gr->allSucessors[choosed].size() ) {
                if(tasks.at(i) < choosed) {
                    bigger = this->instanceProblem->twTimes[tasks.at(i)][trab];
                    choosed = tasks.at(i);
                }
            }
        }*/
    }
    return choosed;
}
//Min time task that the work can execute
//MinTimeTrabWorker
int AlbhwHeuristic::MinTimeTrabWorker(int trab, std::vector<int> tasks) {
    int smaller = -1;
    int choosed = -1;
    for(int i = 0; i < int(tasks.size()); ++i) {
        if(this->instanceProblem->tTypes[tasks.at(i)] == trab and this->instanceProblem->twTimes[tasks.at(i)][trab] < smaller) {
            smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
            choosed = tasks.at(i);
        } else if(this->instanceProblem->twTimes[tasks.at(i)][trab] == smaller) {
            if(int(this->gr->allSucessors[tasks.at(i)].size()) > int(this->gr->allSucessors[choosed].size())) {
                smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
                choosed = tasks.at(i);
            } else if( int(this->gr->allSucessors[tasks.at(i)].size()) > int(this->gr->allSucessors[choosed].size()) ) {
                if(tasks.at(i) < choosed) {
                    smaller = this->instanceProblem->twTimes[tasks.at(i)][trab];
                    choosed = tasks.at(i);
                }
            }
        }
    }
    return choosed;
}
// PODE SER UTILIZADO
int AlbhwHeuristic::wsLowerBound() {
    //BOUNDS ESTAÇÕES
    int aux = 0;
    int menor = NAO_PODE_EXECUTAR;
    for(int i = 0; i < this->instanceProblem->numT; i++) {
        for(int h = 0; h < this->instanceProblem->numW; h++) {
            if(h <= this->instanceProblem->tTypes[i]) {
                for(int j = 0; j < h;j++) {
                    aux = this->instanceProblem->twTimes[i][0];
                    if(this->instanceProblem->twTimes[i][j] < aux) {
                        aux = this->instanceProblem->twTimes[i][j];
                    }
                }
            }
        }
        menor += aux;
    }
    return int(menor/this->instanceProblem->cycT);
}
int AlbhwHeuristic::wsUpperBound() {
    return std::min(2* wsLowerBound(), this->instanceProblem->numT);
}
std::vector<int> AlbhwHeuristic::avaliableTasks(std::vector<int>* pred) {
    std::vector<int> avaliables;
    for(int i = 0; i < int(pred->size()); ++i) {
        if(pred->at(i) == 0) {
            avaliables.push_back(i);
        }
    }
    return avaliables;
}
// Look ahead worker rule
std::vector<int> AlbhwHeuristic::bestOfWorkersLambda(std::vector<int>* setOfTasks,std::vector<int>* predAtual, int* totalTime) {

    std::vector<int>* conjN = new std::vector<int>[this->instanceProblem->numW];
    int* auxLambda = new int[this->instanceProblem->numW];
    int* realLambda = new int[this->instanceProblem->numW];
    int aux = 0; //tempo de cada set de tasks
    // conjN, tem todas as tarefas que ainda não foram alocadas
    for ( int h = 0; h < this->instanceProblem->numW; ++h) {
        for(int i = 0; i < this->instanceProblem->numT; ++i) {
            if(predAtual[h].at(i) > -1 and this->instanceProblem->tTypes[i] == h) {
                conjN[h].push_back(i);
            }
        }
    }
    // cout << endl;
    // cout << " ------ Conjuntos N ------- " << endl;
    for(int h = 0; h < this->instanceProblem->numW; ++h) {
        // cout << "Conjunto De Tarefas " << h << endl;
        for (int j = 0; j < int(conjN[h].size()); ++j) {
            // cout << conjN[h].at(j) << " " << " do trab: " << this->instanceProblem->tTypes[conjN[h].at(j)] << endl;
        }
        // cout << endl;
    }
    int k;
    std::vector<int> n;
    for(int w = 0; w < this->instanceProblem->numW; ++w) {
        auxLambda[w] = 0;
        realLambda[w] = 0;
        for(int x = w+1; x <= this->instanceProblem->numW - 1 + w; ++x) {
            aux = 0;
            k = x % this->instanceProblem->numW;
            n.clear();
            for(int j = 0; j < int(conjN[k].size()); ++j) {
                aux += this->instanceProblem->twTimes[conjN[k].at(j)][k];
            }
            auxLambda[w] += ceil(aux / this->instanceProblem->cycT) * this->instanceProblem->wCosts[k];
            // cout << " valor desse puto: " << auxLambda[w] << " trabaiado: " << w << " calculado: " << k  << endl;

        }
        realLambda[w] = int(cTotal + this->instanceProblem->wCosts[w] + auxLambda[w]);

        // cout << "----------------------------" << endl;
    }



    // cout << "Valor dos lambdas: " << endl;
    for(int h = 0; h < this->instanceProblem->numW; ++h) {
        // cout << realLambda[h] << " ";
    }
    int menorV = realLambda[0];
    int menorI = 0;
    for(int h = 0; h < this->instanceProblem->numW; ++h) {
        if(int(setOfTasks[h].size()) != 0 and realLambda[h] < menorV) {
            menorV = realLambda[h];
            menorI = h;
        } else if (realLambda[h] == menorV){
            if(this->instanceProblem->wCosts[h] < this->instanceProblem->wCosts[menorI]) {
                menorV = realLambda[h];
                menorI = h;
            }
            if(h != menorI)
                qntLambdasDraw[menorI] +=1;
        }
    }

    sizeOfGraph -= int(setOfTasks[menorI].size());
    this->UpdateChoosedWorker(menorI, predAtual);

    return setOfTasks[menorI];
}

// Cost/Time of tasks
std::vector<int> AlbhwHeuristic::bestOfWorkersLessTime(std::vector<int>* setOfTasks, std::vector<int>* predAtual, int* totalTime) {
    int menorI = 0;
    double menorV = NAO_PODE_EXECUTAR; // Numero muito alto
    double sum;
    double divisao;

    for(int w = 0; w < this->instanceProblem->numW; ++w ) {
        sum = 0;
        if(int(setOfTasks[w].size()) != 0) {
            for(int i = 0; i < int(setOfTasks[w].size()); ++i) {
                sum += this->instanceProblem->twTimes[setOfTasks[w].at(i)][w];
            }
            divisao = (double)(this->instanceProblem->wCosts[w]/sum);
            cout << "Worker " << w << " " << this->instanceProblem->wCosts[w] << "/" << sum << " == " << divisao << endl;

            if(divisao < menorV) {
                menorV = divisao;
                menorI = w;
            } else if (divisao == menorV) {
                if(this->instanceProblem->wCosts[w] < this->instanceProblem->wCosts[menorI]) {
                    menorV = divisao;
                    menorI = w;
                }
            }
        }
    }

    sizeOfGraph -= int(setOfTasks[menorI].size());
    this->UpdateChoosedWorker(menorI, predAtual);
    return setOfTasks[menorI];
}
std::vector<int> AlbhwHeuristic::bestOfWorkersTimeWorkStation(std::vector<int>* setOfTasks, std::vector<int>* predAtual, int* totalTime) {
    int maiorI = 0;
    double maiorV = 0;

    for(int w = 0; w < this->instanceProblem->numW; ++w ) {
        if(int(setOfTasks[w].size()) != 0) {
            if(totalTime[w] > maiorV) {
                maiorV = totalTime[w];
                maiorI = w;
            } else if (totalTime[w] == maiorV) {
                if(this->instanceProblem->wCosts[w] < this->instanceProblem->wCosts[maiorI]) {
                    maiorV = totalTime[w];
                    maiorI = w;
                }
            }
        }
    }

    sizeOfGraph -= int(setOfTasks[maiorI].size());
    this->UpdateChoosedWorker(maiorI, predAtual);
    return setOfTasks[maiorI];
}
// Cost/QntTasks
std::vector<int> AlbhwHeuristic::bestOfWorkersUpPerTask(std::vector<int>* setOfTasks, std::vector<int>* predAtual, int* totalTime) {
    int menorI = 0;
    double menorV = NAO_PODE_EXECUTAR;
    double divisao;
    for(int  w = 0; w < this->instanceProblem->numW; ++w) {
        if(int(setOfTasks[w].size()) != 0) {
            divisao = (double)(this->instanceProblem->wCosts[w]/int(setOfTasks[w].size()));
            if(divisao < menorV) {
                menorV = divisao;
                menorI = w;
            } else if(divisao == menorV) {
                if(this->instanceProblem->wCosts[w] < this->instanceProblem->wCosts[menorI] ) {
                    menorV = divisao;
                    menorI = w;
                }

            }
        }
    }
    sizeOfGraph -= int(setOfTasks[menorI].size());
    this->UpdateChoosedWorker(menorI, predAtual);

    //std::// cout << "Quantidade de tarefas escolhidas: " << setOfTasks[menorI].size() << endl;
    // cout << "Tarefas escolhidas: " << endl;
    for(int i = 0; i < int(setOfTasks[menorI].size()); ++i) {
        // cout << setOfTasks[menorI].at(i) << " " << endl;
    }
    return setOfTasks[menorI];
}
void AlbhwHeuristic::UpdateChoosedWorker(int workerIndex,std::vector<int>* predAtual) {
    // Updating choosed worker
    // cout << "Worker Choosed: " << workerIndex << endl;
    this->cTotal += this->instanceProblem->wCosts[workerIndex];
    this->workersA.push_back(workerIndex);
    this->timesA.push_back(totalTime[workerIndex]);
    this->qntTrabAlocados[workerIndex]++;
    for(int i = 0; i < int(predAtual[workerIndex].size()); ++i) {
        this->predOrig[i] = predAtual[workerIndex][i];
    }
    for (int i = 0; i < this->instanceProblem->numW; ++i) {
        totalTime[i] = 0;
    }
}
int AlbhwHeuristic::GetTotal() {
    return 0;
}

void AlbhwHeuristic::RunHeuristic() {

}
