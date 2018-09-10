#include <iostream>
#include <string>
#include <vector>

#ifndef POSCAR_H
#define POSCAR_H

namespace ionizing {
class POSCAR {
public:
    POSCAR(const std::string& FileName);
private:
    std::string _strHeader;
    double _dScaleConst;
    std::vector<std::vector<double>> _BravaisMatrix;
    std::vector<std::pair<std::string, int>> _pElemInfo;
    bool _isSelDyn, _isCartesian;
    std::vector<std::vector<double>> _dmatrixAtomPos;
};
}

#endif /* POSCAR_H */
