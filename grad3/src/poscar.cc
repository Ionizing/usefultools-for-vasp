#include "../include/poscar.hpp"

namespace ionizing {




  void POSCAR::read_header(const string str) {
    {
      std::cout << "\nDEBUG: " << __FILE__ << __FUNCTION__ << " str = :\n" << str << std::endl;
    }
    _header = str;
  }

  void POSCAR::read_scale(const string str) {
    std::stringstream ss(trim_copy(str));
    ss >> _scale;
    if (ss.fail()) {
      std::cerr << "\nERROR: Invalid scale in POSCAR: " << str << " \n";
      std::abort();
    }
  }

  void POSCAR::read_lattice_vectors(const VecStr& str_vec) {
    if (str_vec.rows() < 3) {
      std::cerr << "\nERROR: Not enough string lines provied for '" << __FUNCTION__ << "'.\n" << std::endl;
      std::abort();
    }

    for (int i=0; i!=3; ++i) {
      std::stringstream ss(trim_copy(str_vec(i)));
      ss >> _latticeCartVecs(i, 0) >> _latticeCartVecs(i, 1) >> _latticeCartVecs(i, 2);
      if (ss.fail()) {
        std::cerr << "\nERROR: Invalid cell vectors in POSCAR: " << str_vec(i) << ".\n" << std::endl;
        std::abort();
      }
    }
  }

  void POSCAR::read_element_vector (const VecStr& str_vec) {
    if (str_vec.rows() < 2) {
      std::cerr << "\nERROR: Not enough string lines provided for '" << __FUNCTION__ << "'.\n" << std::endl;
      std::abort();
    }

/*
 * eg: 
 *  Ni Mo S         <-- ss_type
 *  3  1  1         <-- ss_num
 */
    std::stringstream ss_type(str_vec(0));  // Elements type tag
    std::stringstream ss_num (str_vec(1));  // number of elements

/*
 *  cnt_elem_type = -1 : no type tags;
 *                   0 : empty line for type tags;
 *               other : valid number of types;
 */
    int cnt_elem_type = 0;
    int cnt_elem_atom = 0;

    std::vector<string> elem_types;
    while (! ss_type.eof()) {
      std::string tmp_str;
      ss_type >> tmp_str;

      if (std::isdigit(tmp_str[0])) {
        std::cout << "\nWarning: No element type tags in POSCAR." << std::endl;
        ss_num.clear();
        ss_num.str(str_vec(0));
        cnt_elem_type = -1;
        break;
      }

      elem_types.push_back(tmp_str);
      ++cnt_elem_type;
    }

    if (0 == cnt_elem_type) {
      std::cerr << "\nERROR: Empty line for element tags.\n" << std::endl;
      std::abort();
    }
    
    std::vector<int> atom_nums;
    while (! ss_num.eof()) {
      int tmp_int;
      ss_num >> tmp_int;
      if (ss_num.fail() or tmp_int <= 0) {
        std::cerr << "\nERROR: Invalid POSCAR atom number: " << tmp_int << ".\n" << std::endl;
      }
      atom_nums.push_back(tmp_int);
      ++cnt_elem_atom;
    }

    if (-1 != cnt_elem_type and (cnt_elem_type != cnt_elem_atom)) {
      std::cerr << "ERROR: Inconsistent element type list size and atom number list size.\n" << std::endl;
    }

    _elemVector.resize(cnt_elem_atom);
    for (int i=0; i!=cnt_elem_atom; ++i) {
      _elemVector(i).Name = (-1 == cnt_elem_type) ? "_" : elem_types[i];
      _elemVector(i).Num = atom_nums[i];
    }
  }

  void POSCAR::read_selective_dynamics(const string str) {
    string str_trim = trim_copy(str);
    switch (str_trim[0]) {
      case 's':
      case 'S': _isSelectiveDynamics = true; break;
      default : _isSelectiveDynamics = false; break;
    }
  }

  void POSCAR::read_cartesian(const string str) {
    string str_trim = trim_copy(str);
    switch (str_trim[0]) {
      case 'c':
      case 'C': _isCartesian = true; break;
      case 'd':
      case 'D': _isCartesian = false; break;
      default: std::cerr << "\nERROR: Invalid atom position types (cartesian or fractional ?).\n" << std::endl;
    }
  }
  
  void POSCAR::read_atom_positions(const VecStr& str_vec) {

  } 

  void POSCAR::read_atom_sel_dynamics(const VecStr& str_vec) {

  }

}
