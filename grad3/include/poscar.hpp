#pragma once
#ifndef POSCAR_H
#define POSCAR_H

// #include <base.hpp>
#include "base.hpp"
#include "textio.hpp"
#include "stringops.hpp"

namespace ionizing {

struct Element {
  string Name;
  int    Num;
  Element(const char* name, const int num) : Name(name), Num(num) {}
};


class POSCAR {
public:
  using MatX3b  = Matrix<bool, Dynamic, 3>;
  using VecStr  = VecT<string>;
  using VecElem = VecT<Element>;

/*
 * fcc: Ni Header      (comment)                        - read_header()                      
 * 3.53                Overall scaling constant         - read_scale()
 * 0.5 0.5 0.0                                        -\
 * 0.0 0.5 0.5                                         |- read_lattice_vectors()
 * 0.5 0.0 0.5         Bravais matrix                 -/
 * Ni                  Name(s) of atomic type(s)      -|  read_element_vector()
 * 1                   Number of atoms (of each type) -|
 * Selective Dynamics  (optional: selective dynamics)   - read_selective_dynamics()
 * Cartesian           Cartesian or Direct coordinates  - read_cartesian()
 * 0 0 0 (T T T)       positions of the atoms           - read_atom_positions()
 */

  POSCAR(const char* file_name);
  POSCAR(std::istream& is);
  ~POSCAR();
  POSCAR(const POSCAR&) = delete;
  POSCAR& operator=(const POSCAR&) = delete;


private:
  void read_header             (const string  str);
  void read_scale              (const string  str);
  void read_lattice_vectors    (const VecStr& str_vec);
  bool read_element_vector     (const VecStr& str_vec);
  bool read_selective_dynamics (const string  str);
  bool read_cartesian          (const string  str);
  void read_atom_positions     (const VecStr& str_vec);

  void init                    (std::istream& is);
  void read_all                (const VecStr& str_vec);

private:
// raw data
  string  _content;
  VecStr  _contentVector;

// parsed data
  string  _header;
  double  _scale;
  Mat33d  _latticeCartVecs;        // in Cartesian coordinates
  VecElem _elemVector;
  bool    _isSelectiveDynamics;
  bool    _isCartesian;
  MatX3d  _atomPositions;
  MatX3b  _atomSelectiveDynamics;
  VecStr  _atomComments;

// processed data
private:
  int     _nAtoms;
 

};

}
#endif
