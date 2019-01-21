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
    for (size_t i=0; i!=str_vec.size(); ++i) {
      _contentVector(i) = std::move(str_vec[i]);
    }

    read_all(_contentVector);
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


  POSCAR::POSCAR(std::istream& is) : _filename{"POSCAR"} {
    init(is);
  }

  POSCAR::POSCAR(const char* file_name) : _filename{file_name} {
    std::ifstream ifs{file_name};
    if (!ifs.good()) {
      std::cerr << "\nERROR: Open POSCAR file '" << file_name << "' failed.\n" << std::endl;
    }
    init(ifs);
  }

  POSCAR::~POSCAR() {
    std::cout << "POSCAR class destructed.\n" << std::endl;
  }

  string POSCAR::read_header(const string str) {
    {
      std::cout << "\nDEBUG: " << __FILE__ << __FUNCTION__ << " str = :\n" << str << std::endl;
    }
    _header = str;
    return str;
  }

  double POSCAR::read_scale(const string str) {
    std::stringstream ss(trim_copy(str));
    ss >> _scale;
    if (ss.fail()) {
      std::cerr << "\nERROR: Invalid scale in POSCAR: " << str << " \n";
      std::abort();
    }

    return _scale;
  }

  Mat33d POSCAR::read_lattice_vectors(const VecStr& str_vec) {
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

    _recipLattVecs = _latticeCartVecs.inverse();
    return _latticeCartVecs;
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
  
  MatX3d POSCAR::read_atom_positions(const VecStr& str_vec) {
    _atomCartesianPositions.resize(_nAtoms, 3);
    _atomDirectPositions   .resize(_nAtoms, 3);
    _atomPositions         .resize(_nAtoms, 3);
    
    _atomComments.resize(_nAtoms);
    _atomSelectiveDynamics.resize(_nAtoms, 3);

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
        if ('!' != tmp[0] and 0 != tmp[0]) {
          std::cerr << "\nERROR: Invalid comment prefix: " << tmp[0] << " \n" << std::endl;
          std::abort();
        }
        _atomComments(i) = std::move(tmp);
      }
    } // end for

/*
 * Fractional coordinate --> Cartesian Coordinate
 * 
 *                 | Ax, Ay, Az |
 *  r' = [x, y, z] | Bx, By, Bz | = r * Acell
 *                 | Cx, Cy, Cz |
 *
 * Cartesian coordinate --> Fractional Coordinate
 *
 *                    | Ax, Ay, Az | -1
 *  r  = [x', y', z'] | Bx, By, Bz |    = r' * Bcell
 *                    | Cx, Cy, Cz |   
 */

    if (_isCartesian) {
      _atomDirectPositions = (_atomPositions * _latticeCartVecs);
    } else {
      _atomCartesianPositions = (_atomPositions * _recipLattVecs);
    }
    return _atomPositions;
  } // end of read_atom_positions

/*
 * Fractional coordinate --> Cartesian Coordinate
 * 
 *                 | Ax, Ay, Az |
 *  r' = [x, y, z] | Bx, By, Bz | = r * Acell
 *                 | Cx, Cy, Cz |
 *
 * Cartesian coordinate --> Fractional Coordinate
 *
 *                    | Ax, Ay, Az | -1
 *  r  = [x', y', z'] | Bx, By, Bz |    = r' * Bcell
 *                    | Cx, Cy, Cz |   
 */
  MatX3d POSCAR::convert_coordinate(const bool is_to_cart) {
    if (is_to_cart) { // to cartesian
      if (!_isCartesian) {
        _atomPositions = _atomCartesianPositions;
        _isCartesian = true;
      }
    } else {          // to direct/fractional
      if (_isCartesian) {
        _atomPositions = _atomDirectPositions;
        _isCartesian = false;
      }
    }

    return _atomPositions;
  }

  POSCAR::VecStr POSCAR::mark_atom_with_elem () {
    _elementOfEachAtom.resize(_nAtoms);
    int cnt{0};
    for (auto elem : _elemVector) {
      for (int i=0; i!=elem.Num; ++i) {
        _elementOfEachAtom(cnt++) = elem.Name;
      }
    }

    return _elementOfEachAtom;
  }

/*
 ****************************************************
 *********** END OF PRIVATE FUNCTIONS ***************
 ****************************************************
 */

  const string& POSCAR::getHeader() const {
    return _header;
  }

  const double& POSCAR::getScale() const {
    return _scale;
  }

  const Mat33d& POSCAR::getLatticVector() const {
    return _latticeCartVecs;
  }

  const POSCAR::VecElem& POSCAR::getElementVector() const {
    return _elemVector;
  }

  const bool& POSCAR::getIsSelectiveDynamics() const {
    return _isSelectiveDynamics;
  }

  const POSCAR::MatX3b& POSCAR::getSelectiveDynamicsMatrix() const {
    return _atomSelectiveDynamics;
  }

  const bool& POSCAR::getIsCartesian() const {
    return _isCartesian;
  }

  const MatX3d& POSCAR::getAtomPositions() const {
    return _atomPositions;
  }

  const MatX3d& POSCAR::getCartesianPositions() const {
    return _atomCartesianPositions;
  }

  const MatX3d& POSCAR::getDirectPositions() const {
    return _atomDirectPositions;
  }

  int POSCAR::getNAtoms() const {
    return _nAtoms;
  }

  void POSCAR::convertToCartesian() {
    convert_coordinate(true);
  }

  void POSCAR::convertToDirect() {
    convert_coordinate(false);
  }

  void POSCAR::saveAsDuplicate(const char* file_name,
                               const bool  is_cartesian) const {
    using std::setw;
    string fname;
    if (std::strlen(file_name) == 0) {
      fname = string{_filename} + "_saved.vasp";
    } else {
      fname = string{file_name};
    }

    std::stringstream ss;
    ss << _header << "\n"
       << _scale  << "\n"
       << _latticeCartVecs << "\n";
    if (!_elemVector(0).Name.empty()) {
      for (Element elem : _elemVector) {
        ss << setw(4) << elem.Name;
      } ss << "\n";
    }

    for (Element elem : _elemVector) {
      ss << setw(4) << elem.Num;
    } ss << "\n";

    if (_isSelectiveDynamics) {
      ss << "Selective Dynamics\n";
    }

    if (is_cartesian) {
      ss << "Cartesian\n";
    } else {
      ss << "Direct\n";
    }
    MatX3d atom_position = (is_cartesian) ? 
        _atomCartesianPositions : _atomDirectPositions;
    
    for (int i=0; i!=_nAtoms; ++i) {
      ss << atom_position.row(i);
      if (_isSelectiveDynamics) {
        for (int j=0; j!=3; ++j) {
          ss << setw(3) << (_atomSelectiveDynamics(i) == true ? "T" : "F");
        }
      }

      ss << "  " << _atomComments(i) << "\n";
    }
    std::ofstream ofs(fname.c_str());
    ofs << ss.str();
    std::cout << "\nInfo: POSCAR saved to '" << fname << "'.\n";
  }












}
