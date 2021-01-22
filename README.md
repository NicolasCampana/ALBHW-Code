# Assembly line balancing with hierarchical worker assignment (ALBHW)
This code was developed in the undergraduate period, working in the ALBHW problem ([Paper link](https://www.sciencedirect.com/science/article/abs/pii/S0278612514000983)). The results of the work can be seen at [Arxvix](https://arxiv.org/abs/2004.13396).

# DISCLAIMER 
This is not the latest code. It has been lost unfortunately.

# USING 
First run the make file.
```console
make
```
After, use:
```console
./Test <instancepath> <filename> <output-folder> <output-rules-filename>
```
## An example of run with output

```console
./Test Instances/Small/Instance1.1-0.7/ S292_0.7.txt Results Rules.csv 
```
## An example of run with empty output

```console
./Test Instances/Small/Instance1.1-0.7/ S292_0.7.txt "" ""
```

## Instances files
The instance files are located in this [repo](https://github.com/NicolasCampana/ALBHW-Instances), as well as the respectively results.

# OUTPUT 
If the parameters "output-folder" and "output-rules-filename" are provided,
the code generates five files as output:
- Exact value output for the instance (MCMI)
- Assembly line configuration for the best heuristic
- Assembly line configuration for the MIP1 
- Assembly line configuration for the MIP2
- Assembly line configuration for the VND
- The best work and task priority rules

# SOLVER 
The implementation uses Gurobi as mathematical solver.

# CODE STRUCTURE 
- HEADERS .h
   - AlbhwHeuristic (
   - AlbhwHeuristicResult
   - AlbhwModel
   - AlbhwStructure
   - TestClass
   - ResultsModel
   
- IMPLEMENTATIONS .cpp
   - AlbhwHeuristic
   - AlbhwHeuristicResult
   - AlbhwModel
   - AlbhwStructure
   - TestClass
   
- TEST FILES
   - TestClass.cpp
   - TestAllWithRules.cpp
   - Test.cpp

# MAKEFILE

In the folder, do "make" on terminal, it will compile for **linux** the main test file (Test.cpp).

