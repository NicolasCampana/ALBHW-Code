//
// Created by nicolas on 16/04/19.
//
#pragma once
#ifndef NICOLAS_IC_ALBHWMODEL_H
#define NICOLAS_IC_ALBHWMODEL_H

using namespace std;

#include "ResultsModel.h"
#include "AlbhwHeuristicResult.h"
#include "Graph.cpp"

class AlbhwModel {
private:
    GRBEnv *env;
    string instancePath;
    AlbhwStructure* instanceProblem;
    int *improveMh(int **twTimes, std::vector<int> tTypes, int numT, int numW, int cycT);
    int improveCycT(int **twTimes, int twTimesOrig, std::vector<int> tTypes, int cycT, int numT, int task, int w);
    int *improvedMh;
    int **ntwTimes;


public:
    Graph* graph;
    int** createPrecendenceTransitive(int h);
    int calculateLowerBound1(vector<int>& tasks, int h);
    int calculateLowerBound2(vector<int>& tasks);
    AlbhwHeuristicResult* resolveAlbhwImproved(int ubStations);
    AlbhwHeuristicResult* resolveAlbhwPaper();
    AlbhwModel(string instance, int testarTudo);
    AlbhwModel();
    int minOfWorkers(int h);
    void testModels();
    void testSmall();
    void testMedium();
    void printResults(AlbhwHeuristicResult* res, string destination, bool model);
    void setInstancePath(string instance);
    string getInstancePath();
    AlbhwHeuristicResult* LocalSearchMIP1(AlbhwHeuristicResult* res);
    AlbhwHeuristicResult* LocalSearchMIP2(AlbhwHeuristicResult* res);
    AlbhwHeuristicResult* RunVND(AlbhwHeuristicResult* res, int regraT, int regraW);
    void printResultsTerminal(AlbhwHeuristicResult* res); // Just printing on the screen the results
    void printResultsHeur(AlbhwHeuristicResult* res, int regraT, int regraW, int print, string destination, string filename); // Printing on file
    void createALBHWAux();
    /*
    vector<string> addSmallPaths();
    vector<string> addMediumPaths();
    vector<string> getSmallPaths(int i);
    vector<string> getMediumPaths(int i);*/

};



#endif //NICOLAS_IC_ALBHWMODEL_H
