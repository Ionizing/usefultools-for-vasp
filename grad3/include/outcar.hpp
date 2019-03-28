#pragma once

#ifndef OUTCAR_H
#define OUTCAR_H

#if __cplusplus < 201103L
  #error "DEPRECATED COMPILER, USE GCC4.9 OR HIGHER"
#endif

#include <base.hpp>
#include <poscar.hpp>
#include <stringops.hpp>
#include <incar.hpp>
#include <constants.hpp>

#include <sys/stat.h>     // mkdir
#include <sys/types.h>     // mkdir

#if (defined(_WIN32) || defined(__WIN32__))
#  define mkdir(A, B) mkdir(A)  // Compromise for MinGW
#endif

namespace ionizing{ 

#ifdef UNIT_TEST
#  define private public  // Just for unit_test
#endif

class OUTCAR {
public:
  OUTCAR(std::istream&  is);
  OUTCAR(const char*    file_name = "OUTCAR");
  ~OUTCAR() = default;
private:
  const string& file_to_string(std::istream& is);
  const VecStr& string_to_vecstr(const string& content);
  bool  init(std::istream& is);


/*
 * Parse type of Pseudo Potentials
 */
public:
  const VecStr& parseElems(const VecStr& lines, 
                           const int     startline =  0,
                                 int     endline   = -1);
  const VecStr& getElems()                   const;
  const std::vector<int>& getAtomsPerElem () const;
private:
  int    _nElems;
  VecStr _Elems;
  std::vector<int> 
          _atomsPerElem;
  VecStr  parse_elems(const VecStr& lines);

/*
 * Parse Lattice Vectors
 */
public:
  Mat33d parseLatticeVectors(const VecStr& lines, 
                             const int     startline =  0, 
                                   int     endline   = -1);
  const Mat33d& getLatticeVectors() const;
private:
  Mat33d _latticeVector;    // initial lattice from POSCAR
  Mat33d parse_lattice_vectors(const VecStr& lines);

/*
 * Parse Atom initial Positions
 */

public:
  bool parseInitialPositions(const VecStr& lines,
                             const int     startline =  0,
                                   int     endline   = -1);
  const MatX3d& getInitialPositions_Cartesian() const;
  const MatX3d& getInitialPositions_Direct() const;
private:
  MatX3d _initialPosition_cart,
         _initialPosition_dire;
  MatX3d parse_init_pos(const VecStr& lines);

/*
 * Parse mini-INCAR
 */
public:
  INCAR parseINCAR(const VecStr& lines,
                   const int     startline =  0,
                         int     endline   = -1);
  const INCAR& getINCAR()  const;
private:
  INCAR _incar;
  void parse_incar(const VecStr& lines);

  double parse_ediff        (const string& line);
  double parse_ediffg       (const string& line);
  double parse_encut        (const string& line);
  int    parse_ibrion       (const string& line);
  int    parse_isif         (const string& line);
  int    parse_ispin        (const string& line);
  int    parse_lnoncollinear(const string& line);
  int    parse_lorbit       (const string& line);
  int    parse_lsorbit      (const string& line);
  int    parse_nbands       (const string& line);
  int    parse_nelmin       (const string& line);
  int    parse_nions        (const string& line);
  int    parse_nsw          (const string& line);
  int    parse_nkpts        (const string& line);
  int    parse_nwrite       (const string& line);

/*
 * Parse K-Point Path
 */
public:
  MatX3d parseKPoints(const VecStr& lines,
                      const int     startline =  0,
                            int     endline   = -1);
  const MatX3d& getKPoints() const ;
private:
  MatX3d     _kpoints;
  const MatX3d& parse_kpoints(const VecStr& lines);



/*
 * Parse Ion Iterations
 */
public:
  struct IonIteration{
    int    _nSCF;
    int    _maxIndex;
    string _maxAtomElem;
    char   _maxDirection;
    double _totalEnergy;
    double _totalEnergy_sigma_0;      // free energy without entropy when sigma -> 0
    double _cpuTime;
    double _deltaE;
    double _magmom;
    double _volume;
    double _averageF;
    double _maxForce;
    MatX3d _atom_forces_dirs;
    MatX3d _atom_positions;           // Cartesian coordinates
    Vecd   _atom_forces;
    Mat33d _lattice_vector;

    IonIteration() :                  // Initialization
      _nSCF               { 0},
      _maxIndex           { 0},
      _maxAtomElem        {""},
      _maxDirection       {'a'},
      _totalEnergy        {.0},
      _totalEnergy_sigma_0{.0},
      _cpuTime            {.0},
      _deltaE             {.0},
      _magmom             {.0},
      _volume             {.0},
      _averageF           {.0},
      _maxForce           {.0},
      _atom_forces_dirs   {  },
      _atom_positions     {  },
      _atom_forces        {  },
      _lattice_vector     {  } {}

    bool operator==(const IonIteration& rhs) const {
      return (
        (this->_nSCF                == rhs._nSCF               ) and
        (this->_maxIndex            == rhs._maxIndex           ) and
        (this->_maxAtomElem         == rhs._maxAtomElem        ) and
        (this->_maxDirection        == rhs._maxDirection       ) and
        (this->_totalEnergy         == rhs._totalEnergy        ) and
        (this->_totalEnergy_sigma_0 == rhs._totalEnergy_sigma_0) and
        (this->_cpuTime             == rhs._cpuTime            ) and
        (this->_deltaE              == rhs._deltaE             ) and
        (this->_magmom              == rhs._magmom             ) and
        (this->_volume              == rhs._volume             ) and
        (this->_averageF            == rhs._averageF           ) and
        (this->_maxForce            == rhs._maxForce           ) and
        (this->_atom_forces_dirs    == rhs._atom_forces_dirs   ) and
        (this->_atom_positions      == rhs._atom_positions     ) and
        (this->_atom_forces         == rhs._atom_forces        ) and
        (this->_lattice_vector      == rhs._lattice_vector     ) );
    }
  };
  using VecIt = std::vector<IonIteration>;
  const VecIt & parseIterationVec  (const VecStr& lines,
                                    const int     startline =  0,
                                          int     endline   = -1);
  const VecIt & getIterationVec    () const ;
private:
  IonIteration tmpIteration;
  VecIt _iterationVec;
  /*
   * int    _nIterations;
   * int    _nSteps; use this->_iterationVec.size() instead
   */
  double _lastEnergy;

  IonIteration  parse_iteration      (const VecStr& lines);
        double  parse_magmom         (const string& lines);
        double  parse_lattice_volume (const string& line);
  const Mat33d& parse_lattice        (const VecStr& lines);
  const MatX3d& parse_atom_force_pos (const VecStr& lines);
        double  parse_toten          (const string& line);
        double  parse_toten_0        (const string& line);
        double  parse_cpu_time       (const string& line);

  const Vecd  & calc_atom_force      (const MatX3d& atom_force_dirs);


public:
  bool saveAsMolden  (const VecIt&    it_vec,
                      const char*     file_name =  "animate.molden",
                      const int       skip      =  0);
                     
public:              
  bool saveAsPoscar  (const VecIt&    it_vec,
                      const char*     file_prefix = "POSCAR_frame",
                      const char*     folder      = "poscar_frames/",
                      const bool      is_direct   = true);
private:
  bool save_one_frame_as_poscar(const IonIteration&   iteration,
                      const char*           file_name,
                      const bool            is_direct = true);

public:
  struct Vibration {
    double  _freq_THz,
            _freq_meV,
            _freq_cm1;
    bool    _is_imag;
    MatX3d  _dxdydz;
  };
  using VecVib = std::vector<Vibration>;
  VecVib parseVibration (const VecStr&    lines,
                         const int        startline =  0,
                               int        endline   = -1);

  bool   saveAsXsf      (const VecVib&    vibs,
                         const char*      prefix    = "vib_",
                         const char*      folder    = "vib_modes",
                         const int        mode_ind  = 0,
                         const double     scale     = 1.0) const;

  bool   saveAsMol      (const VecVib&    vibs,
                         const char*      file_name = "modes.mol") const;
  const VecVib& getVibrationVec() const ;
private:
  int       _dof;
  VecVib    _vibrations;
  Vibration parse_vib_mode        (const VecStr&    lines);
  bool      save_one_mode_as_xsf  (const Vibration& vib,
                                   const char*      file_name,
                                   const double     scale)  const;
  

/*
 * public:
 * // POSCAR info without selective dynamics
 *   struct mini_POSCAR {
 *     using VecElem = VecT<Element>;
 * 
 *     string Header;
 *     Mat33d LatticeVectors;    
 *     VecElem ElementVector;
 * 
 *     void print_to_file();
 *   };
 */

private:
// raw text
  string _content;
  VecStr _contentVector;

// parsed data
  string _filename;
  
// INCAR involved parameters
  int __current_line;

// Element Table corresponding to each atom
  VecStr _elem_tab;

};







}
#endif
