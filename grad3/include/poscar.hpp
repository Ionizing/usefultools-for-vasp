#pragma once
#ifndef POSCAR_H
#define POSCAR_H

// #include <base.hpp>
#include "base.hpp"
#include "textio.hpp"
#include "stringops.hpp"

namespace ionizing {


class POSCAR {
public:
  using MatX3b  = Matrix<bool, Dynamic, 3>;
  using VecStr  = VecT<string>;
  using VecElem = VecT<Element>;

/*
 * fcc: Ni Header      (comment)
 * 3.53                Overall scaling constant
 * 0.5 0.5 0.0 
 * 0.0 0.5 0.5 
 * 0.5 0.0 0.5         Bravais matrix
 * Ni                  Name(s) of atomic type(s)
 * 1                   Number of atoms (of each type)
 * Selective Dynamics  (optional: selective dynamics)
 * Cartesian           Cartesian or Direct coordinates
 * 0 0 0 (T T T)       positions of the atoms 
 */

  POSCAR(const char* file_name);
  ~POSCAR();
  POSCAR(const POSCAR&) = delete;
  POSCAR& operator=(const POSCAR&) = delete;


private:
  void read_header             (const string  str);
  void read_scale              (const string  str);
  void read_lattice_vectors    (const VecStr& str_vec);
  void read_element_vector     (const VecStr& str_vec);
  void read_selective_dynamics (const string  str);
  void read_cartesian          (const string  str);
  void read_atom_positions     (const VecStr& str_vec);
  void read_atom_sel_dynamics  (const VecStr& str_vec);

  void read_all                (std::istream& is);

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

// processed data
private:
 
 

};

}
#endif
