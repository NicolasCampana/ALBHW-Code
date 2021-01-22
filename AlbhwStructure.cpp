#include "AlbhwStructure.h"

void AlbhwStructure::setInstance(int cycT, std::vector<int> tTimes, int numT,
                            std::vector<int>* predecessors, std::vector<int>* sucessors,
                            int** closure, std::vector<int> tTypes , int** twTimes, std::vector<int> wCosts) {
    this->cycT = cycT;
    this->tTimes = tTimes;
    this->numT = numT;
    this->predecessors = predecessors;
    this->sucessors = sucessors;
    this->tTypes = tTypes;
    this->twTimes = twTimes;
    this->wCosts = wCosts;
    this->closure = closure;
    this->numW = (int)wCosts.size();
    this->numS = numT;
}
void AlbhwStructure::printInstance(){
    cout << "Cycle Time:" << this->cycT << endl;
    cout << "Number of Tasks:" << this->numT << endl;
    cout << "Task Caracteristics: " << endl;
    for(int i =0; i < numT; ++i) {
        cout << "Task: " << i << " : " << endl;
        cout << "Precedences: " << endl;
        for(int j = 0; j < int(predecessors[i].size()); ++j) {
            cout << predecessors[i].at(j) << " ";
        }
        cout << endl;
        cout << "Sucessors: " << endl;
        for(int j = 0; j < int(sucessors[i].size()); ++j) {
            cout << sucessors[i].at(j) << " ";
        }
        cout << endl;
        cout << "Times for this: " << twTimes[i][0] << " - " << twTimes[i][1] << " - " << twTimes[i][2] << endl;
    }
    cout << "Costs: " << wCosts[0] << " - " << wCosts[1] << " - " << wCosts[2] << endl;
}
AlbhwStructure::AlbhwStructure() {

}
AlbhwStructure::~AlbhwStructure() {
    for(int i = 0; i < numT; ++i) {
        delete twTimes[i];
        delete closure[i];
    }

    delete [] twTimes;
    delete [] closure;
    delete [] predecessors;
    delete [] sucessors;
}

int AlbhwStructure::getCyc() {
    return cycT;
}

int AlbhwStructure::getNumT() {
    return numT;
}

int** AlbhwStructure::getClo() {
    return closure;
}

void AlbhwStructure::findSucessors() {

}

void AlbhwStructure::findClosure() {
}

std::vector<int> AlbhwStructure::gettTimes() {
    return tTimes;
}

std::vector<int> AlbhwStructure::gettTypes() {
    return tTypes;
}

int** AlbhwStructure::gettwTimes() {
    return twTimes;
}

std::vector<int> AlbhwStructure::getwCosts() {
    return wCosts;
}
int AlbhwStructure::getsTotal() {
    return sTotal;
}

std::vector<int>* AlbhwStructure::getPredecessors() {
    return predecessors;
}

std::vector<int>* AlbhwStructure::getSucessors() {
    return sucessors;
}

void AlbhwStructure::readFile(std::string file) {
    this->instance = file;
    std::ifstream arquivo(this->instance, ios::in);
    std::string linha;
    int numT, cycT, numW;
    int i, j = 0;
    std::vector<int> auxtaskT;
    std::vector<int> tTypes;
    std::vector<int> wCosts;

    // Read the properties
    while(arquivo >> linha) {
        if(linha.compare("<precedence") == 0) {
            break;
        } 
        if(linha.compare("tasks>") == 0) {
            arquivo >> numT;
        }
        if(linha.compare("time>") == 0) {
            arquivo >> cycT;
        }
        /*
        if(linha.compare("Time>") == 0) {
            while(arquivo >> linha) {
                if(linha.compare("<precedence") == 0) {
                    break;
                }
                auxtaskT.push_back(atoi(linha.c_str()));
            }
        } */
    }
    // Create the datastructures of sucessors and predecessors
    std::vector<int>* predecessors = new std::vector<int>[numT];
    std::vector<int>* sucessors = new std::vector<int>[numT];
    int** closure = new int*[numT];
    for(i = 0; i < numT; i++) {
        closure[i] = new int[numT];
    }
    for(i = 0; i < numT; ++i) {
        for(j = 0; j < numT; ++j) {
            if(i == j) {
                closure[i][j] = 0;
            } else {
                closure[i][j] = -1;
            }
        }
    }
    

    while(arquivo >> linha) {
        // Read the precedences relations
        if(linha.compare("relations>") == 0) {
            int aux = 0;
            while(arquivo >> linha) {
                if(linha.compare("<type") == 0) {
                    break;
                }
                if(aux == 0) {
                    i = atoi(linha.c_str());
                    aux++;
                } else {
                    j = atoi(linha.c_str());
                    //std::cout << i << " " << j << std::endl;
                    aux = 0;
                    predecessors[j - 1].push_back(i - 1);
                    sucessors[i-1].push_back(j-1);
                    closure[i-1][j-1] = 1;
                    //std::cout << i-1 << " precede " << j-1 << std::endl;
                    //cout << linha << endl;
                }
            }
        }
        if(linha.compare("<type") == 0) break;
    }
    // Read Type of workers
    while (arquivo >> linha) {
        if(linha.compare("<task") == 0) {
            break;
        }
        if(linha.compare("workers>") == 0) {
            arquivo >> numW;
        }
    }
    int** twTimes = new int*[numT];
    for(i = 0; i < numT; ++i)
        twTimes[i] = new int[numW];        

    ////////////////////////
    // Read task types
    while(arquivo >> linha) {
        if(linha.compare("types>") == 0) {
            while(arquivo >> linha) {
                if(linha.compare("<task") == 0) {
                    break;
                }
                tTypes.push_back(atoi(linha.c_str()) - 1);
            }
        }
        if(linha.compare("<task") == 0) {
            break;
        }
    }
    while(arquivo >> linha) {
        if(linha.compare("<worker") == 0) break;
        if(linha.compare("times>") == 0 ){
            for (int row = 0; row < numT; ++row) {
                for(int col = 0; col < numW; ++col) {
                    arquivo >> linha;
                    cout << linha << " ";
                    i = atoi(linha.c_str());
                    linha.compare("INF") == 0 or i > cycT? twTimes[row][col] = NAO_PODE_EXECUTAR : twTimes[row][col] = i;
                }
            }
        }
    }
    while(arquivo >> linha) {
        if(linha.compare("costs>") == 0) {
            if(linha.compare("<end>") == 0)
                break;
            while(arquivo >> linha) {
                if(linha.compare("<end>")==0) break;
                wCosts.push_back(atoi(linha.c_str()));
            }
        }
    }
    setInstance(cycT,auxtaskT, numT, predecessors, sucessors, closure, tTypes , twTimes, wCosts);
    
}
