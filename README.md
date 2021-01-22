# Assembly line balancing with hierarchical worker assignment (ALBHW)

## DISCLAIMER
This code is not complete and probably with some memory leak. The latest version has been lost.

## ---------------- SOLVER ----------------------------
The implementation uses Gurobi as mathematical solver.

## ---------------- CODE STRUCTURE --------------------
-> HEADERS .h
   - AlbhwHeuristic (
   - AlbhwHeuristicResult
   - AlbhwModel
   - AlbhwStructure
   - TestClass
   - ResultsModel
   
-> IMPLEMENTATIONS .cpp
   - AlbhwHeuristic
   - AlbhwHeuristicResult
   - AlbhwModel
   - AlbhwStructure
   - TestClass
   
-> TEST FILES
   - TestClass.cpp
   - TestAllWithRules.cpp
   - Test.cpp

## ---------------- MAKEFILE --------------------

In the folder, do "make" on terminal, it will compile for **linux** the main test file (Test.cpp).

