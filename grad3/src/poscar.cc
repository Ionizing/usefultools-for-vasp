#include "../include/poscar.hpp"

namespace ionizing {

  /*
   * initilize class with istream&
   */
  void POSCAR::init(std::istream& is) {
    std::stringstream buffer;
    buffer << is.rdbuf();
    _content = buffer.str();

    buffer.seekp(0);
    std::vector<string> str_vec;
    while (! buffer.eof()) {
      string line;
      std::getline(buffer, line);
      str_vec.emplace_back(line);
    }

    _contentVector.resize(str_vec.size());
    for (long i=0; i!=str_vec.size(); ++i) {
      _contentVector(i) = std::move(str_vec[i]);
    }
  }

  void POSCAR::read_all(const VecStr& str_vec) {
    int n_current_line = 0;
    read_header(str_vec(n_current_line++));
    read_scale(str_vec(n_current_line++));
    read_lattice_vectors(str_vec.segment<3>(n_current_line));
    n_current_line += 3;

    if (read_element_vector(str_vec.segment<2>(n_current_line)) ){
      n_current_line += 2;
    } else {
      n_current_line += 1;
    }

    if (read_selective_dynamics(str_vec(n_current_line))) {
      ++n_current_line;
    }

    read_cartesian(str_vec(n_current_line++));
    read_atom_positions(str_vec.segment(n_current_line, _nAtoms));
    
  }


  POSCAR::POSCAR(std::istream& is) {
    init(is);
  }

  POSCAR::POSCAR(const char* file_name) {
    std::ifstream ifs{file_name};
    init(ifs);
  }

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

  bool POSCAR::read_element_vector (const VecStr& str_vec) {
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
    _nAtoms = 0;
    for (int i=0; i!=cnt_elem_atom; ++i) {
      _elemVector(i).Name = (-1 == cnt_elem_type) ? "_" : elem_types[i];
      _elemVector(i).Num = atom_nums[i];
      _nAtoms += atom_nums[i];
    }

/*
 * if no elem type tags, return false;
 * esle return true;
 */
    return !(-1 == cnt_elem_type);
  }

  bool POSCAR::read_selective_dynamics(const string str) {
    string str_trim = trim_copy(str);
    switch (str_trim[0]) {
      case 's':
      case 'S': _isSelectiveDynamics = true; break;
      default : _isSelectiveDynamics = false; break;
    }

    return _isSelectiveDynamics;
  }

  bool POSCAR::read_cartesian(const string str) {
    string str_trim = trim_copy(str);
    switch (str_trim[0]) {
      case 'c':
      case 'C': _isCartesian = true; break;
      case 'd':
      case 'D': _isCartesian = false; break;
      default: std::cerr << "\nERROR: Invalid atom position types (cartesian or fractional ?).\n" << std::endl;
    }

    return _isCartesian;
  }
  
  void POSCAR::read_atom_positions(const VecStr& str_vec) {
    _atomPositions.resize(_nAtoms);
    _atomComments.resize(_nAtoms);
    _atomSelectiveDynamics.resize(_nAtoms);

    for (int i=0; i!=_nAtoms; ++i) {
      std::stringstream ss(str_vec(i));

      // Read positions
      for (int j=0; j!=3; ++j) {
        ss >> _atomPositions(i, j);
      }

      if (_isSelectiveDynamics) {
        // Read Selective Dynamics tags
        for (int j=0; j!=3; ++j) {
          std::string seldyn;
          ss >> seldyn;
          switch (seldyn[0]) {
            case 't':
            case 'T': _atomSelectiveDynamics(i, j) = true ; break;
            case 'f':
            case 'F': _atomSelectiveDynamics(i, j) = false; break;
            default : std::cerr << "\nERROR: Invalid selective dynamics tag: " << seldyn << " .\n" << std::endl; 
                      break;
          }
        }
      } // end if (_isSelectiveDynamics)

      // Read comments for each atom
      {
        std::string tmp;
        ss >> tmp;
        if ('!' != tmp[0]) {
          std::cerr << "\nERROR: Invalid comment prefix: " << tmp[0] << " \n" << std::endl;
          std::abort();
        }
        _atomComments(i) = std::move(tmp);
      }
    }

    



  } // end of read_atom_positions

/*
 ****************************************************
 *********** END OF PRIVATE FUNCTIONS ***************
 ****************************************************
 */







  









}
