#pragma once

#ifndef OUTCAR_H
#define OUTCAR_H

#if __cplusplus < 201103L
  #error "Deprecated compiler, use gcc4.9 or higher"
#endif

#include <base.hpp>

namespace ionizing{ 


class OUTCAR {
public:
  OUTCAR(std::istream& is);
  OUTCAR(const char* file_name);
  ~OUTCAR();


private:
  int _nAtoms,
      _nElems,
      _nIterations,
      _nSteps;

  double _tottalEnergy,
         _lastEnergy,
         _cpuTime,
         _deltaE,
         _magmom,
         _volume,
         _totaltime,
         _average,
         _maxForce,
         _accuracy;


private:

};







}
#endif
