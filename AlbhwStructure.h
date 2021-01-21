#pragma once
#ifndef NICOLAS_IC_ALBHWSTRUCTURE_H
#define NICOLAS_IC_ALBHWSTRUCTURE_H

#define NAO_PODE_EXECUTAR 100000
/* Classe ALBHW */
class AlbhwStructure {
    public:
		// Set's
		std::vector<int> tTimes;
		std::vector<int> tTypes;
		std::vector<int> wCosts;
		int** twTimes;
		std::vector<int> *predecessors;
		std::vector<int> *sucessors;
		//Variables
		int cycT;
		int numT;
		int numW;
		int numS; // da pra melhorar?
		int numPrec;
		int **closure;
		int sTotal;
        string instance;

        //Constructors
        void setInstance(int cycT, std::vector<int> tTimes, int numT, int numPrec, std::vector<int>* predecessors, std::vector<int>* sucessors, int** closure, std::vector<int> tTypes , int** twTimes, std::vector<int> wCosts);
		~AlbhwStructure();
        AlbhwStructure();
		// Gets
		int getCyc();
		int getNumT();
		int getNumPrec();
		int** getClo();
		int getsTotal();
		std::vector<int> gettTimes();
		std::vector<int> gettTypes();
		std::vector<int> getwCosts();
		int** gettwTimes();
		std::vector<int>* getPredecessors();
		std::vector<int>* getSucessors();
		// Methods and Functions
		void readFile(std::string file);
		void findSucessors();
		void findClosure();
		void printInstance();
};

#endif // ALBHWSTRUCTURE
