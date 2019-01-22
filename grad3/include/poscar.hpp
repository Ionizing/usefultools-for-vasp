#pragma once
#ifndef POSCAR_H
#define POSCAR_H

// #include <base.hpp>
#include "base.hpp"
#include "textio.hpp"
#include "stringops.hpp"

namespace ionizing {

struct Element {
  using VecStr = VecT<string>;
  string Name;
  int    Num;
  MatX3d atomPos;
  MatX3d atomPosCart;
  MatX3d atomPosDire;
  VecStr comments;
  Element(const char* name, const int num) : Name(name), Num(num) {}
  Element() : Element("", -1) {}
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



/*
 * public apis
 */

  const string  & getHeader() const;
  const double  & getScale() const;
  const Mat33d  & getLatticVector() const;
  const VecElem & getElementVector() const;
  const bool    & getIsSelectiveDynamics() const;
  const MatX3b  & getSelectiveDynamicsMatrix() const;
  const bool    & getIsCartesian() const;
  const MatX3d  & getAtomPositions() const;
  const MatX3d  & getCartesianPositions() const;
  const MatX3d  & getDirectPositions() const;
        int       getNAtoms() const;
        void      convertToCartesian();
        void      convertToDirect();

  void saveAsDuplicate(const char* file_name,
                       const bool  is_cartesian) const;



  POSCAR             (const char* file_name);
  POSCAR             (std::istream& is);
  ~POSCAR            ();
  POSCAR             (const POSCAR&) = delete;
  POSCAR& operator = (const POSCAR&) = delete;

private:
  string read_header             (const string  str);
  double read_scale              (const string  str);
  Mat33d read_lattice_vectors    (const VecStr& str_vec);
  bool   read_element_vector     (const VecStr& str_vec);
  bool   read_selective_dynamics (const string  str);
  bool   read_cartesian          (const string  str);
  MatX3d read_atom_positions     (const VecStr& str_vec);

  void init                      (std::istream& is);
  void read_all                  (const VecStr& str_vec);
  MatX3d convert_coordinate      (const bool    is_to_cart);
  VecStr mark_atom_with_elem     ();
  void   sync_atompos_to_elem    ();
  void   sync_elem_to_atompos    ();

private:
// raw data
  string  _content;
  VecStr  _contentVector;

// parsed data
  string  _filename;
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
  Mat33d  _recipLattVecs;
  MatX3d  _atomCartesianPositions;
  MatX3d  _atomDirectPositions;
  VecStr  _elementOfEachAtom;

};
/*
 *
 * Note: I came up with an idea that define element struct:
 * struct Element {
 *    string Name;
 *    int    Num;
 *    MatX3d atomPos;
 *    MatX3d atomPosCart;
 *    MatX3d atomPosReci;
 *    VecStr comments;
 * }
 *
 * It may be convenient to insert atoms, supercell and etc = =
 * 
 */
}
#endif
