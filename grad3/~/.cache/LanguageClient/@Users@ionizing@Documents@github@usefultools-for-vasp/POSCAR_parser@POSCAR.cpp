#include "POSCAR.h"

namespace ionizing {

// some functions for trimming strings, copied from
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}



  POSCAR::POSCAR(const std::string& FileName) : 
    _strHeader("DefaultHeader"),
    _dScaleConst(.0),
    _BravaisMatrix(3, std::vector<double>(3, .0)),
    _pElemInfo(0),
    _isSelDyn(false), _isCartesian(false),
    _dmatrixAtomPos(0),
    _nAtoms(0), _nElems(0),
    _isHasElemTags(false),
    _ifs(FileName.c_str()) {
      if (_ifs.fail()) {
        std::cout << "   Invalid FileName : " << FileName << std::endl;
        std::exit(1);
      }
      readFirst5Lines(_ifs);
      readElemInfo(_ifs);
      readSelDyn_and_Cartesian(_ifs);
      readPositons(_ifs);
  }

  POSCAR::~POSCAR() {
    _ifs.close();
  }

  void POSCAR::readFirst5Lines(std::ifstream& ifs) {
// read first 5 lines of POSCAR with format as follows
/*
  fcc: Ni Header      (comment)
  3.53                Overall scaling constant
  0.5 0.5 0.0 
  0.0 0.5 0.5 
  0.5 0.0 0.5         Bravais matrix
*/
    std::getline(ifs, _strHeader);
#ifdef DEBUG
    std::cout << "@Debug _strHeader = " << _strHeader << std::endl;
#endif
    
    std::string tmp;
    std::getline(ifs, tmp);     // ignore the '\n' at ending
    std::istringstream iss(tmp);
    iss >> _dScaleConst;
#ifdef DEBUG
    std::cout << "@Debug _dScaleConst = " << _dScaleConst << std::endl;
#endif

    // _BravaisMatrix is already initialized to 3x3 matrix in constructor
    for (int i=0; i!=3; ++i) {
      std::getline(ifs, tmp);
      iss.str(tmp); iss.clear();  // resetting iss string stream;
      for (int j=0; j!=3; ++j) {
        iss >> _BravaisMatrix[i][j];
      }
    } // end for

#ifdef DEBUG
    std::cout << "@Debug _BravaisMatrix = " << std::endl;
    for (int i=0; i!=3; ++i) {
      for (int j=0; j!=3; ++j) {
        std::cout << _BravaisMatrix[i][j] << ' ';
      } std::cout << std::endl;
    }
#endif
  } // end of readFirst5Lines()
  
  void POSCAR::readElemInfo(std::ifstream& ifs) {
// read the element infos with format as follows
/*
Ni                  Name(s) of atomic type(s)
1                   Number of atoms (of each type)
*/

    std::string tmp;
    std::getline(ifs, tmp);
    trim(tmp);
    _nAtoms = 0;
    if ( std::isdigit(tmp[0]) ) { // no element tags
      std::istringstream iss(tmp);
      int natom_per_elem;
      while ( !iss.eof() ) {
        iss >> natom_per_elem;
        if (0 == natom_per_elem) { // end of valid number data
          break;
        }
        _pElemInfo.push_back(std::pair<std::string, std::vector<std::vector<double>>>(
              "_", std::vector<std::vector<double>>(
                natom_per_elem, std::vector<double>(3, -10)))); 
        // initialize natom_.. * 3 array
        _nAtoms += natom_per_elem;
      }
    } else if ( std::isalpha(tmp[0]) ) {
      std::string tmp_elem_tags(tmp);
      std::string tmp_natom_per_elem;
      std::getline(ifs, tmp_natom_per_elem);
      trim(tmp_natom_per_elem);
      
      std::istringstream iss_tags(tmp_elem_tags), iss_num(tmp_natom_per_elem);
      std::string tmp_tag;
      int tmp_num;
      while (!iss_tags.eof() and !iss_num.eof()) {
        iss_tags >> tmp_tag;
        iss_num >> tmp_num;
        if (0 == tmp_num) { // end of valid number data (aka. comments)
          break;
        }
        _pElemInfo.push_back(std::pair<std::string, std::vector<std::vector<double>>>(
              tmp_tag, std::vector<std::vector<double>>(
                tmp_num, std::vector<double>(3, -10)))); 
              // initialize tmp_num * 3 array
        _nAtoms += tmp_num;
      } 
    } else {
      std::cout << "  Invalid POSCAR file.  Element tags / Atom nums error  " << std::endl;
      std::exit(1);
    }
    _nElems = _pElemInfo.size();

#ifdef DEBUG
    std::cout << "@Debug Element Info = " << std::endl;
    std::cout << "  " << _nElems << " elements, and " << _nAtoms << " atoms. " 
      << std::endl << " Detailed info is listed as following. " << std::endl;
    for (const auto& e : _pElemInfo) {
      std::cout << e.first << " " << e.second.size() << std::endl;
    }
#endif

  } // end of readElemInfo()

  void POSCAR::readSelDyn_and_Cartesian(std::ifstream& ifs) {
    std::string tmpstr;
    for (int i=0; i!=2; ++i) {
      std::getline(ifs, tmpstr);
      trim(tmpstr);
      if ('C' == std::toupper(tmpstr[0])) {
        _isCartesian = true;
        break;
      } else if ('R' == std::toupper(tmpstr[0])) {
        _isCartesian = false;
        break;
      } else if ('S' == std::toupper(tmpstr[0])) {
        _isSelDyn = true;
      } else {
        std::cout << "  Invalid POSCAR file.  Did you write lattice format? (Cartesian/Reciprocal)  " << std::endl;
        std::exit(1);
      }
    } // end for
  } // end of function readSelDyn_and_Cartesian();

  void POSCAR::readPositons(std::ifstream& ifs) {
    _dmatrixAtomPos.resize(_nAtoms, std::vector<double>(3, .0));

    std::string tmpstr;
    std::istringstream iss;
    // for (int i=0; i!=_nAtoms; ++i) {
      // std::getline(ifs, tmpstr);
      // trim(tmpstr);
      // iss.str(tmpstr); iss.clear();

      // for (int j=0; j!=3; ++j) {
        // iss >> _dmatrixAtomPos[i][j];
      // }
    // }
    
    for (auto& e : _pElemInfo) {
      const int size = e.second.size();
      for(int i=0; i!=size; ++i) {
        std::getline(ifs, tmpstr);
        trim(tmpstr);
        iss.str(tmpstr);  iss.clear();

        for(int j=0; j!=3; ++j) {
          iss >> e.second[i][j];
        }
      }
    } // end for

#ifdef DEBUG
    std::cout << "@Debug Atom positions = "  << std::endl;
    // for (int i=0; i!=_nAtoms; ++i) {
      // for (int j=0; j!=3; ++j) {
        // std::cout << _dmatrixAtomPos[i][j] << " ";
      // } std::cout << std::endl;
    // }
    for (const auto& e : _pElemInfo) {
      const int size = e.second.size();
      for (int i=0; i!=size; ++i) {
        for (int j=0; j!=3; ++j) {
          std::cout << e.second[i][j] << " ";
        } std::cout << std::endl;
      }
    } // end for
#endif
  } // end of function readPositions();

  void POSCAR::print(std::ostream& ofs) {
    ofs << std::fixed << std::setprecision(8) << std::setfill(' ');

    ofs << _strHeader << std::endl
      << _dScaleConst << std::endl;
    for (int i=0; i!=3; ++i) {
      for (int j=0; j!=3; ++j) {
        ofs << std::setw(12)  << _BravaisMatrix[i][j];
      } ofs << std::endl;
    } // end for

    std::ostringstream oss_tag, oss_nums;
    for (const auto& e : _pElemInfo) {
      oss_tag << std::setw(5) << std::setfill(' ') << e.first;
      oss_nums << std::setw(5) << std::setfill(' ') << e.second.size();
    }
    ofs << oss_tag.str() << std::endl << oss_nums.str() << std::endl;
    
    if (_isSelDyn) {
      ofs << "Selective Dynamics" << std::endl;
    }

    ofs << ((_isCartesian) ? "Cartesian" : "Reciprocal") << std::endl;

    // bool isFirst = true;
    // for (int i=0; i!=_nAtoms; ++i) {
      // for (int j=0; j!=3; ++j) {
        // ofs << std::setw(12) << _dmatrixAtomPos[i][j];
      // } 
      // if (isFirst) {
        // std::cout << "  ! " << _pElemInfo[i].first << _pElemInfo[i].second;
      // }
      // ofs << std::endl;
    // }

    for (const auto& e : _pElemInfo) {
      bool isFirst = true;
      int size = e.second.size();
      for (int i=0; i!=size; ++i) {
        for (int j=0; j!=3; ++j) {
          ofs << std::setw(12) << e.second[i][j];
        }
        if (isFirst) {
          isFirst = false;
          ofs << "  ! " << e.first << " " << e.second.size();
        }

        ofs << std::endl;
      }
    } // end for

    std::cout << " POSCAR print success\n\n" << std::endl;
  } // end of function print()

  // void POSCAR::PrintToStdout() {
    // print(std::cout);
  // }

  void POSCAR::PrintToFile(const std::string& FileName) {
    std::ofstream ofs(FileName.c_str());
    print(ofs);
    ofs.close();
  }

  void POSCAR::PrintToStdout() {
    print(std::cout);
  }
    
} // end of namespace ionizing
