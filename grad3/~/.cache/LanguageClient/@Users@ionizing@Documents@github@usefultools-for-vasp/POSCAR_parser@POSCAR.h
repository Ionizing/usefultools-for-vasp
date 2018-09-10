#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// #define DEBUG

#ifndef POSCAR_H
#define POSCAR_H

namespace ionizing {
class POSCAR {
public:
    POSCAR(const std::string& FileName); ~POSCAR();
    void PrintToStdout();
    void PrintToFile(const std::string& FileName);

private:
    std::string _strHeader;
    double _dScaleConst;
    std::vector<std::vector<double>> _BravaisMatrix;
    std::vector<std::pair<std::string, std::vector<std::vector<double>>>> _pElemInfo;
    bool _isSelDyn, _isCartesian;
    std::vector<std::vector<double>> _dmatrixAtomPos; // Deprecated
    // atom pos info is already contained in _pElemInfo vector

private:
    int _nAtoms, _nElems;
    bool _isHasElemTags;
    std::ifstream _ifs;

    void readFirst5Lines(std::ifstream& ifs);
    void readElemInfo(std::ifstream& ifs);
    void readSelDyn_and_Cartesian(std::ifstream& ifs);
    void readPositons(std::ifstream& ifs);
    void print(std::ostream& ofs);
};
}

#endif /* POSCAR_H */

/*
fcc: Ni Header      (comment)
3.53                Overall scaling constant
0.5 0.5 0.0 
0.0 0.5 0.5 
0.5 0.0 0.5         Bravais matrix
Ni                  Name(s) of atomic type(s)
1                   Number of atoms (of each type)
Selective Dynamics  (optional: selective dynamics)
Cartesian           Cartesian or Direct coordinates
0 0 0 (T T T)       positions of the atoms 
*/
