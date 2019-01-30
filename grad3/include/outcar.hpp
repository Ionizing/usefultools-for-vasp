#pragma once

#ifndef OUTCAR_H
#define OUTCAR_H

#if __cplusplus < 201103L
  #error "DEPRECATED COMPILER, USE GCC4.9 OR HIGHER"
#endif

#include <base.hpp>
#include <poscar.hpp>

namespace ionizing{ 

#ifdef UNIT_TEST
#define private public  // Just for unit_test
#endif

class OUTCAR {
public:
  using VecStr = VecT<string>;

public:
  OUTCAR(std::istream& is);
  OUTCAR(const char* file_name);
  ~OUTCAR();

  VecStr parsePPs();

public:
// POSCAR info without selective dynamics
  struct mini_POSCAR {
    using VecElem = VecT<Element>;

    string Header;
    Mat33d LatticeVectors;    
    VecElem ElementVector;

    void print_to_file();
  };

private:
// raw text
  string _content;
  VecStr _contentVector;

// parsed data
  string _filename;
  
// INCAR involved parameters
  int _EDIFF;
  int _EDIFFG;
  int _ENCUT;
  int _IBRION;
  int _ISIF;
  int _ISPIN;
  int _LNONCOLLINEAR;
  int _LORBIT;
  int _LSORBIT;
  int _NBANDS;
  int _NELMIN;
  int _NIONS;
  int _NKPTS;
  int _NSW;

  int _nElems;
  int _nIterations;
  int _nSteps;

  double _tottalEnergy;
  double _lastEnergy;
  double _cpuTime;
  double _deltaE;
  double _magmom;
  double _volume;
  double _totaltime;
  double _average;
  double _maxForce;
  double _accuracy;

private:

};







}
#endif
