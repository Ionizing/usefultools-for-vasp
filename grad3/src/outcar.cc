#include <outcar.hpp>

namespace ionizing {

  OUTCAR::OUTCAR(std::istream& is) {
    init(is);
  }

  OUTCAR::OUTCAR(const char* file_name) {
    std::ifstream ifs(file_name);
    init(ifs);
  }


/*
 * bool init(std::istream& is)
 *  In: file handle pointed to POSCAR
 * Parse all the things.
 * Out: is successful or not
 */
  bool OUTCAR::init(std::istream& is) {
    try {
      if (!is.good()) {
        string str = string_printf("OUTCAR class Construction failed:\n\tOpen OUTCAR failed.\n");
        throw str;
      }
      file_to_string (is);
      string_to_vecstr   (this->_content);
      parseElems         (_contentVector);
      parseLatticeVectors(_contentVector, __current_line);
      parseINCAR         (_contentVector, __current_line);
      parseKPoints       (_contentVector, __current_line);
      parseIterationVec  (_contentVector, __current_line);
      if (5 == this->_incar._IBRION) {
        parseVibration(_contentVector);
      }
    } catch (std::string msg) {
      std::cerr << msg << std::endl;
      std::abort();
    }
    return true;
  }




/*
 * string read_to_string(std::istream& is)
 *  in: std::ifstream of OUTCAR
 * out: OUTCAR file content in ONE string;
 */
  const string& OUTCAR::file_to_string(std::istream& is) {
    if (!is.good()) {
      throw "OUTCAR file open failed";
      return _content;
    }
    std::stringstream buffer;
    buffer << is.rdbuf();
    _content = (buffer.str());
    return _content;
  }

/*
 * VecStr string_to_vecstr(const string& content)
 *  in: string of whole file content
 *  ----------
 *  "1234567890\nqwertyuiop\nzxcvbnm\n"
 *  ----------
 * out: VecStr of lines of content
 *  ----------
 *  "1234567890", "qwertyuiop", "zxcvbnm"
 *  ----------
 */
  const VecStr& OUTCAR::string_to_vecstr(const string& content) {
    VecStr out = split(content, '\n');
    while (out.back() == "") {
      out.pop_back();
    }
    _contentVector = std::move(out);
    return _contentVector;
  }




/*
 * VecStr pars_elems(VecStr lines)
 *  in: String Vector that contains 'VRHFIN =[Element]...'
 *  ---------
 *  " POTCAR:    PAW_PBE O 08Apr2002                   ",
 *  " POTCAR:    PAW_PBE Ti_pv 07Sep2000               ",
 *  " POTCAR:    PAW_PBE Ba_sv 06Sep2000               "
 *  ---------
 * out: String Vector full of element names;
 *  ---------
 *  "O",
 *  "Ti",
 *  "Ba"
 *  ---------
 */
 VecStr OUTCAR::parse_elems(const VecStr& lines) {
    VecStr out;
    for (string e : lines) {
      if (!is_start_with(e, " POTCAR:")) {
        throw "String doesn't contain element name"; 
        return out;
      }
      string tmp = split(e)[2];
      if (tmp.length() > 2) {
        tmp.erase(2, string::npos);
      }
      out.emplace_back(std::move(tmp));
    }
    return out;
  }


/*
 * void parsElems(VecStr lines, string content)
 *  in: OUTCAR content in VecStr(lines) and one string(content)
 * out: String Vector of element names;
 *
 * Modifies _atomsPerElem with ions per type;
 */
  const VecStr& OUTCAR::parseElems(const VecStr& lines, 
                                   const int     startline,
                                         int     endline) {
    endline = (endline < 0) ? lines.size() : endline;
    VecStr lines_to_use;
    for (int i=startline; i!=endline; ++i) {
      if (is_start_with(lines[i], "   ions per type")) {
         VecStr str_vec = split(lines[i]);
         _nElems = str_vec.size() - 4;
         _atomsPerElem.resize(_nElems, 0);
         for (int j=0; j!=_nElems; ++j) {
           _atomsPerElem[j] = std::atoi(str_vec[j + 4].c_str());
         }
         break;
      } else {  }
    }


    int cnt = 0;
    for (int i=startline; i!=endline; ++i) {
      if (cnt < _nElems and is_start_with(lines[i], " POTCAR:")) {
        lines_to_use.emplace_back(lines[i]);
        ++cnt;
      }
      if (cnt == _nElems) {
        __current_line = i;
        break;
      } else /**/ ;
    }

   _Elems = parse_elems(lines_to_use);

   // Generate Element Type table for each atom
   VecStr elem_tab;
   for (int i=0; i!=this->_nElems; ++i) {
     VecStr consecutive_elem(_atomsPerElem[i], _Elems[i]);
     elem_tab.insert(elem_tab.end(), consecutive_elem.begin(), consecutive_elem.end());
   }
   _elem_tab = elem_tab;

    return _Elems;
  }
  
  const VecStr& OUTCAR::getElems() const {
    return this->_Elems;
  }

  const std::vector<int>& OUTCAR::getAtomsPerElem() const {
    return this->_atomsPerElem;
  }



/*
 * Mat33d parseLatticeVectors(VecStr& lines)
 *  in: OUTCAR content VecStr
 *  ---------
 *  " A1 = (   9.0750000000,  -9.0750000000,   0.0000000000)",
 *  " A2 = (   9.0750000000,   9.0750000000,   0.0000000000)",
 *  " A3 = (   0.0000000000,   0.0000000000,  29.0400000000)"
 *  ---------
 * out: Lattice Vectors in real space
 *  ---------
 *  [[   9.0750000000,  -9.0750000000,   0.0000000000],
 *   [   9.0750000000,   9.0750000000,   0.0000000000],
 *   [   0.0000000000,   0.0000000000,  29.0400000000]]
 */
  Mat33d OUTCAR::parse_lattice_vectors(const VecStr& lines) {
    Mat33d out;
    // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
    if (lines.size() != 3) {
      throw string_printf("Invalid string vector size: %lld.", lines.size());
      return out;
    } else if (std::any_of(lines.begin(), lines.end(), [](const string& str){
          return !is_start_with(str, " A");
          })) {
      throw string_printf("Invalid string input: \n%s\n%s\n%s\n", 
          lines[0].c_str(), lines[1].c_str(), lines[2].c_str());
      return out;
    }
    
    for (int i=0; i!=3; ++i) {
      // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
      sscanf(lines[i].c_str(), " A%*c = (%lf,%lf,%lf)", &out(i, 0), &out(i, 1), &out(i, 2));
    }
    
    return this->_latticeVector = out;
  }

/*
 * Mat33d parseLatticeVectors(const VecStr& lines, int startline, int endline);
 *  in: lines -> VecStr of OUTCAR content;
 *      startline -> start line number for processing
 *      endline -> endline number + 1 for processing
 */
  Mat33d OUTCAR::parseLatticeVectors(const VecStr& lines,
                                     const int     startline,
                                           int     endline) {
    endline = (endline < 0) ? lines.size() : endline;
    VecStr lines_to_use;
    for (int i=startline; i!=endline; ++i) {
      if (is_start_with(lines[i], " A1 = (")) {
        lines_to_use.push_back(lines[  i  ]);
        lines_to_use.push_back(lines[i + 1]);
        lines_to_use.push_back(lines[i + 2]);
        __current_line = i + 2;
        break;
      }
    }
    return parse_lattice_vectors(lines_to_use);
  }

  const Mat33d& OUTCAR::getLatticeVectors() const {
    return this->_latticeVector;
  }


/*
 * int getMiniINCAR()
 */
  INCAR OUTCAR::parseINCAR(const VecStr& lines,
                           const int     startline,
                                 int     endline) {
    endline = (-1 == endline) ? lines.size() : endline;
    int start_line = startline,
        end_line = endline; // INCAR part in OUTCAR

    for (int i=startline; i!=endline; ++i) {
      if (is_start_with(lines[i], " Dimension of arrays:")) {
        start_line = i;
      }
      if (is_start_with(lines[i], " Orbital magnetization")) {
        end_line = __current_line = i + 3;
        break;
      }
    }

    VecStr INCAR_part(lines.begin() + start_line, lines.begin() + end_line);
    parse_incar(INCAR_part);
    return _incar;
  }

  
/*
 * int parse_incar(const VecStr& lines);
 */
  void OUTCAR::parse_incar(const VecStr& lines) {
    int parsed_lines = 0;
    for (const string& line : lines) {
      if (       is_start_with(line, "   EDIFF  =")) {
        parse_ediff(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   EDIFFG =")) {
        parse_ediffg(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   ENCUT  =")) {
        parse_encut(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   IBRION =")) {
        parse_ibrion(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   ISIF   =")) {
        parse_isif(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   ISPIN  =")) {
        parse_ispin(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   LNONCOLLINEAR =")) {
        parse_lnoncollinear(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   LORBIT =")) {
        parse_lorbit(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   LSORBIT =")) {
        parse_lsorbit(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   k-points     ")) {
        parse_nkpts(line);
        parse_nbands(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   NELM   =")) {
        parse_nelmin(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   number of dos   ")) {
        parse_nions(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   NSW    =")) {
        parse_nsw(line);
        ++parsed_lines;
      } else if (is_start_with(line, "   DQ     =")) {
        break;
      }
    }
  } // end of function

  const INCAR& OUTCAR::getINCAR() const {
    return this->_incar;
  }


/*
 * double parse_ediff(string& line)
 *  In: 
 *  ----------
 *  "   EDIFF  = 0.1E-05   stopping-criterion for ELM"
 *  ----------
 * Out: 
 *  ----------
 * 0.000001
 *  ----------
*/
  double OUTCAR::parse_ediff(const string& line) {
    double tmp;
    int flag = sscanf(line.c_str(), "   EDIFF  = %lf", &tmp);
    if (1 != flag) {
      string str = string_printf("Parse EDIFF failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._EDIFF = -1;
    }
    return this->_incar._EDIFF = tmp;
  }

/*
 * double parse_ediffg(string& line)
 *  In: 
 *  ----------
 *  "   EDIFFG = -.3E-01   stopping-criterion for IOM"
 *  ----------
 * Out: 
 *  ----------
 * -0.03
 *  ----------
 */
  double OUTCAR::parse_ediffg(const string& line) {
    double tmp;
    int flag = sscanf(line.c_str(), "   EDIFFG = %lf", &tmp);
    if (1 != flag) {
      string str = string_printf("Parse EDIFFG failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._EDIFFG = -1;
    }
    return this->_incar._EDIFFG = tmp;
  }

/*
 * double parse_encut(string& line)
 *  In: 
 *  -----------
 *  "   ENCUT  =  500.0 eV  36.75 Ry    6.06 a.u.  30.47 23.11 54.70*2*pi/ulx,y,z "
 *  -----------
 * Out:
 *  -----------
 *  500.0
 *  -----------
 */
  double OUTCAR::parse_encut(const string& line) {
    double tmp;
    int flag = sscanf(line.c_str(), "   ENCUT  = %lf eV", &tmp);
    if (1 != flag) {
      string str = string_printf("Parse ENCUT failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._ENCUT = -1;
    }
    return this->_incar._ENCUT = tmp;
  }

/*
 * int parse_ibrion(string& line)
 *  In: 
 *  -----------
 *  "   IBRION =      1    ionic relax: 0-MD 1-quasi-New 2-CG"
 *  -----------
 * Out:
 *  -----------
 *  1
 *  -----------
 */
  int OUTCAR::parse_ibrion(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   IBRION = %d", &tmp);
    if (1 != flag) {
      string str = string_printf("Parse IBRION failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._IBRION = -1;
    }
    return this->_incar._IBRION = tmp;
  }

/*
 * int parse_isif(stirng& line)
 *  In:
 *  ----------
 *  "   ISIF   =      3    stress and relaxation"
 *  ----------
 * Out:
 *  ----------
 *  3
 *  ----------
 */
  int OUTCAR::parse_isif(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   ISIF   = %d", &tmp);
    if (1 != flag or (tmp < 0 or tmp > 3)) {
      string str = string_printf("Parse ISIF failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._ISIF = -1;
    }
    return this->_incar._ISIF = tmp;
  }

/*
 * int parse_ispin(stirng& line)
 *  In:
 *  ----------
 * "   ISPIN  =      1    spin polarized calculation?"
 *  ----------
 * Out:
 *  ----------
 *  1
 *  ----------
 */
  int OUTCAR::parse_ispin(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   ISPIN  = %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse ISIF failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._ISPIN = -1;
    }
    return this->_incar._ISPIN = tmp;
  }


/*
 * int parse_lnoncollinear(stirng& line)
 *  In:
 *  ----------
 *  "   LNONCOLLINEAR =      F non collinear calculations"
 *  ----------
 * Out:
 *  ----------
 *  0
 *  ----------
 */
  int OUTCAR::parse_lnoncollinear(const string& line) {
    char tmp;
    int flag = sscanf(line.c_str(), "   LNONCOLLINEAR =      %c", &tmp);
    if (1 != flag or (tmp != 'F' and tmp != 'T')) {
      string str = string_printf("Parse LNONCOLLINEAR failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._LNONCOLLINEAR = -1;
    }
    return this->_incar._LNONCOLLINEAR = ('T' == tmp);
  }


/*
 * int parse_lsorbit(stirng& line)
 *  In:
 *  ----------
 *  "   LSORBIT =      F    spin-orbit coupling"
 *  ----------
 * Out:
 *  ----------
 *  0
 *  ----------
 */
  int OUTCAR::parse_lsorbit(const string& line) {
    char tmp;
    int flag = sscanf(line.c_str(), "   LSORBIT =      %c", &tmp);
    if (1 != flag or (tmp != 'F' and tmp != 'T')) {
      string str = string_printf("Parse LSORBOT failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._LSORBIT = -1;
    }
    return this->_incar._LSORBIT = ('T' == tmp);
  }


/*
 * int parse_lorbit(stirng& line)
 *  In:
 *  ----------
 *  "   LORBIT =      0    0 simple, 1 ext, 2 COOP (PROOUT)"
 *  ----------
 * Out:
 *  ----------
 *  0
 *  ----------
 */
  int OUTCAR::parse_lorbit(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   LORBIT =      %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse LORBOT failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._LORBIT = -1;
    }
    return this->_incar._LORBIT = tmp;
  }


/*
 * int parse_nbands(stirng& line)
 *  In:
 *  ----------
 *  "   k-points           NKPTS =      9   k-points in BZ     NKDIM =      9   number of bands    NBANDS=     98"
 *  ----------
 * Out:
 *  ----------
 *  98
 *  ----------
 */
  int OUTCAR::parse_nbands(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   k-points           NKPTS = %*d   k-points in BZ     NKDIM = %*d   number of bands    NBANDS=     %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse NBANDS failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._NBANDS = -1;
    }
    return this->_incar._NBANDS = tmp;
  }


/*
 * int parse_nelmin(stirng& line)
 *  In:
 *  ----------
 *  "   NELM   =     60;   NELMIN=  6; NELMDL= -5     # of ELM steps"
 *  ----------
 * Out:
 *  ----------
 *  6
 *  ----------
 */
  int OUTCAR::parse_nelmin(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   NELM   = %*d;   NELMIN= %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse NELMIN failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._NELMIN = -1;
    }
    return this->_incar._NELMIN = tmp;
  }


/*
 * int parse_nions(stirng& line)
 *  In:
 *  ----------
 *  "   number of dos      NEDOS =    301   number of ions     NIONS =     27"
 *  ----------
 * Out:
 *  ----------
 *  27
 *  ----------
 */
  int OUTCAR::parse_nions(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   number of dos      NEDOS = %*d   number of ions     NIONS = %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse NIONS failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._NIONS = -1;
    }
    return this->_incar._NIONS = tmp;
  }


/*
 * int parse_nsw(stirng& line)
 *  In:
 *  ----------
 *  "   NSW    =    200    number of steps for IOM"
 *  ----------
 * Out:
 *  ----------
 *  200
 *  ----------
 */
  int OUTCAR::parse_nsw(const string& line) {
    int tmp;
    int flag = sscanf(line.c_str(), "   NSW    = %d", &tmp);
    if (1 != flag or tmp < 0) {
      string str = string_printf("Parse NIONS failed:\n\t%s\n", line.c_str());
      throw str;
      return this->_incar._NSW = -1;
    }
    return this->_incar._NSW = tmp;
  }

/*
 * const int& parse_nkpts(const string& line);
 *  In: string with "NKPTS = XX"
 *  ----------
 *  "   k-points           NKPTS =     20   k-points in BZ     NKDIM =     20   number of bands    NBANDS=     81"
 *  ----------
 * Out: NKPTS in integer
 *  ----------
 *  20
 *  ----------
 */
  int OUTCAR::parse_nkpts(const string& line) {
    if (!is_start_with(line, "   k-points           NKPTS = ")) {
      _incar._NKPTS = -1;
      throw string_printf("Invalid line input for NKPTS parsing:\n%s", line.c_str());
      return _incar._NKPTS;
    }

    int flag = sscanf(line.c_str(), "   k-points           NKPTS = %d", &(_incar._NKPTS));
    if (1 != flag or _incar._NKPTS <= 0) {
      _incar._NKPTS = -1;
      throw string_printf("Parse NKPTS failed:\n%s\n NKPTS=%d", line.c_str(), _incar._NKPTS);
      return _incar._NKPTS;
    }
    return _incar._NKPTS;
  }




/*
 * MatX3d parseKPoints(const VecStr& lines,
 *                     const int     startline,
 *                           int     endline);
 *  In: VecStr& of OUTCAR
 * Out: MatX3d of KPoint coordinates
 */
  MatX3d OUTCAR::parseKPoints(const VecStr& lines,
                              const int     startline,
                                    int     endline) {
    endline = (-1 == endline) ? lines.size() : endline;
    for (int i=startline; i!=endline; ++i) {
      if (is_start_with(lines[i], "   k-points     ")) {
        parse_nkpts(lines[i]);
        __current_line = i + 1;
        break;
      }
    }

    for (int i=__current_line; i!=endline; ++i) {
      if (is_start_with(lines[i], " k-points in reciprocal latt")) {
        __current_line = i + 1;
        break;
      }
    }

    VecStr lines_to_use{lines.begin() + __current_line, 
                        lines.begin() + __current_line + this->_incar._NKPTS};
    return parse_kpoints(lines_to_use);
  }


  const MatX3d& OUTCAR::getKPoints() const {
    return this->_kpoints;
  }


/*
 * const MatX3d& parse_kpoints(VecStr& lines)
 * ***** CAUTION: REQUIRE(lines.size() == _NKPTS) *****
 *  In: VecStr with KPoints cooridinates
 *  -----------
 *     "0.06250000  0.06250000  0.06250000       0.016",
 *     "0.18750000  0.06250000  0.06250000       0.047",
 *     "0.31250000  0.06250000  0.06250000       0.047",
 *     ...
 *  -----------
 * Out: MatX3d of KPoints coordinates
 *  -----------
 *  [[0.06250000, 0.06250000, 0.06250000],
 *   [0.18750000, 0.06250000, 0.06250000],
 *   [0.31250000, 0.06250000, 0.06250000],
 *                                    ...]
 *  -----------
 */
  const MatX3d& OUTCAR::parse_kpoints(const VecStr& lines) {
    if (_incar._NKPTS <= 0) {
      throw string_printf("Invalid NKPTS:%4d", _incar._NKPTS);
      return _kpoints;
    }
    if (static_cast<int>(lines.size()) != _incar._NKPTS) {
      throw string_printf("Inconsistent KPoint number and KPath array size: %4d != %4lld", _incar._NKPTS, lines.size());
      return _kpoints;
    }
    _kpoints.resize(_incar._NKPTS, 3);

    double col1, col2, col3;
    for (int i=0; i!=_incar._NKPTS; ++i) {
      if (3 != sscanf(lines[i].c_str(), "%lf%lf%lf", 
            &col1, &col2, &col3 )) {
        throw string_printf("Parsing KPath failed:\n%s", lines[i].c_str());
        return _kpoints;
      } else {
        _kpoints(i, 0) = col1;
        _kpoints(i, 1) = col2;
        _kpoints(i, 2) = col3;
      }
    }
    return _kpoints;
  }


  double OUTCAR::parse_toten(const string& line) {
    double toten;
    int flag = sscanf(line.c_str(), "  free  energy   TOTEN  = %lf", &toten);
    if (1 != flag or toten > 0 ) {
      string str = string_printf("Parse TOTEN failed:\n\t%s\n\ttoten = %lf", toten);
      throw str;
      return -1;
    }
    return this->tmpIteration._totalEnergy = toten;
  }




/*
 * double parse_toten_0(const string& line)
 *  read TOTEN into _totalEnergy and _totalEnergy_sigma_0;
 *
 *  In:
 *  ----------
 *  "  energy  without entropy=    -1059.00022771  energy(sigma->0) =    -1059.00022771"
 *  ----------
 * Out:
 *  ----------
 *  -1059.00022771
 *  ----------
 */
  double OUTCAR::parse_toten_0(const string& line) {
    double toten_sigma_0;
    int flag = sscanf(line.c_str(), "  energy  without entropy= %*f  energy(sigma->0) = %lf",
        &toten_sigma_0);
    if (flag != 1 or toten_sigma_0 > 0) {
      string str = string_printf("Parse TOTEN failed:\n\t%s\n\ttoten_sigma_0 = %lf\n",
          line.c_str(), toten_sigma_0);
      throw str;
      return -1;
    }
    return this->tmpIteration._totalEnergy_sigma_0 = toten_sigma_0;
  }


/*
 * double parse_cpu_time(const string& line)
 *  In:
 * ----------
 *  "     LOOP+:  cpu time  428.5440: real time  468.0720"
 * ----------
 * Out:
 * ----------
 *  428.5440
 * ----------
 */
  double OUTCAR::parse_cpu_time(const string& line) {
    double cpu_time;
    int flag = sscanf(line.c_str(), "     LOOP+:  cpu time %lf", &cpu_time);
    if (1 != flag or cpu_time < 0) {
      string str = string_printf("Parse cpu time failed:\n\t%s\n\tcpu_time = %lf sec",
          line.c_str(), cpu_time);
      throw str;
      return -1;
    }
    return this->tmpIteration._cpuTime = cpu_time;
  }


/*
 * double parse_magmom(string& line)
 *  In:
 *  ----------
 *  " number of electron     310.0000000 magnetization      75.0000000"
 *  ----------
 * Out:
 *  ----------
 *  75.0
 *  ----------
 */
  double OUTCAR::parse_magmom(const string& line) {
    if (2 != this->_incar._ISPIN) {
      string str = string_printf("Parse magmom failed:\n\tWrong ISPIN in INCAR: ISPIN = %2d\n\tCorrect ISPIN = 2\n", this->_incar._ISPIN);
      throw str;
      return .0;
    }

    double magmom;
    int flag = sscanf(line.c_str(), " number of electron %*lf magnetization %lf",
        &magmom);
    if (1 != flag) {
      string str = string_printf("Parse magmom failed:\n\t%s\n\tmagmom = %lf\n",
          line.c_str(), magmom);
      throw str;
      return -1;
    }
    return this->tmpIteration._magmom = magmom;
  }


/*
 * double parse_lattice_volume(string& line)
 *  In:
 *  -----------
 *  "  volume of cell :      425.26"
 *  -----------
 * Out:
 *  -----------
 *  425.26
 *  -----------
 */
  double OUTCAR::parse_lattice_volume(const string& line) {
    double volume;
    int flag = sscanf(line.c_str(), "  volume of cell : %lf", &volume);
    if (1 != flag or volume < 0) {
      string str = string_printf("Parse cell volume failed:\n\t%s\n\tvolume = %lf\n", line.c_str(), volume);
      throw str;
      return -1;
    }
    return this->tmpIteration._volume = volume;
  }

/*
 * Mat33d parse_lattice(VecStr& lines)
 *  In:
 * ----------
 * ["     7.519999981  0.000000000  0.000000000     0.132978724  0.000000000  0.000000000",
 *  "     0.000000000  7.519999981  0.000000000     0.000000000  0.132978724  0.000000000",
 *  "     0.000000000  0.000000000  7.519999981     0.000000000  0.000000000  0.132978724"]
 * ----------
 * Out:
 * ----------
 * [[7.519999981, 0.000000000, 0.000000000],
 *  [0.000000000, 7.519999981, 0.000000000],
 *  [0.000000000, 0.000000000, 7.519999981]]
 * ----------
 */
  const Mat33d& OUTCAR::parse_lattice(const VecStr& lines) {
    Mat33d out;
    if (3 != lines.size()) {
      throw "Parse Lattice in Iteration failed:\n\tinput lines quantity != 3";
      return this->tmpIteration._lattice_vector;
    }
    for (int i=0; i!=3; ++i) {
      int flag = sscanf(lines[i].c_str(), "%lf %lf %lf",
          &out(i, 0), &out(i, 1), &out(i, 2));
      if (3 != flag) {
        string str = string_printf("Parse Lattice in Iteration failed: Invalid string to parse\n\t%s\n", lines[i].c_str());
        return this->tmpIteration._lattice_vector;
      }
    }
    this->tmpIteration._lattice_vector = std::move(out);
    return this->tmpIteration._lattice_vector;
  }
  

/*
 * MatX3d& parse_atom_force_pos(VecStr& lines)
 *  In:
 * ----------
 *["     0.52935      0.52935      0.52935         0.000716      0.000716      0.000716",
 * "     6.96491      3.21778      4.27649        -0.000716     -0.000716      0.000716",
 * "     3.21778      4.27649      6.96491        -0.000716      0.000716     -0.000716",
 * "     4.27649      6.96491      3.21778         0.000716     -0.000716     -0.000716",
 * "     6.15005      2.40292      5.09135         0.000716      0.000716     -0.000716",
 * "     1.34421      1.34421      1.34421        -0.000716     -0.000716     -0.000716",
 * "     2.40292      5.09135      6.15005         0.000716     -0.000716      0.000716",
 * "     5.09135      6.15005      2.40292        -0.000716      0.000716      0.000716",
 * "     2.40292      6.15005      5.09135         0.000716      0.000716     -0.000716",
 * "     6.15005      5.09135      2.40292         0.000716     -0.000716      0.000716",
 * "     5.09135      2.40292      6.15005        -0.000716      0.000716      0.000716",
 * "     3.21778      6.96491      4.27649        -0.000716     -0.000716      0.000716",
 * "     6.96491      4.27649      3.21778        -0.000716      0.000716     -0.000716",
 * "     4.27649      3.21778      6.96491         0.000716     -0.000716     -0.000716",
 * "     0.52935      4.27649      4.27649         0.000716      0.000716      0.000716",
 * "     6.96491      6.96491      0.52935        -0.000716     -0.000716      0.000716",
 * "     3.21778      0.52935      3.21778        -0.000716      0.000716     -0.000716",
 * "     6.15005      6.15005      1.34421         0.000716      0.000716     -0.000716",
 * "     1.34421      5.09135      5.09135        -0.000716     -0.000716     -0.000716",
 * "     2.40292      1.34421      2.40292         0.000716     -0.000716      0.000716",
 * "     2.40292      2.40292      1.34421         0.000716      0.000716     -0.000716",
 * "     6.15005      1.34421      6.15005         0.000716     -0.000716      0.000716",
 * "     3.21778      3.21778      0.52935        -0.000716     -0.000716      0.000716",
 * "     6.96491      0.52935      6.96491        -0.000716      0.000716     -0.000716",
 * "     4.27649      0.52935      4.27649         0.000716      0.000716      0.000716",
 * "     0.52935      6.96491      6.96491         0.000716     -0.000716     -0.000716",
 * "     5.09135      1.34421      5.09135        -0.000716     -0.000716     -0.000716",
 * "     1.34421      6.15005      6.15005        -0.000716      0.000716      0.000716",
 * "     1.34421      2.40292      2.40292        -0.000716      0.000716      0.000716",
 * "     0.52935      3.21778      3.21778         0.000716     -0.000716     -0.000716",
 * "     4.27649      4.27649      0.52935         0.000716      0.000716      0.000716",
 * "     5.09135      5.09135      1.34421        -0.000716     -0.000716     -0.000716"]
 * ----------
 * Out: (Writes 2 matrix, Returns the latter)
 * ----------
 *  Positions:
 * [[ 0.52935, 0.52935, 0.52935], [ 6.96491, 3.21778, 4.27649],
 *  [ 3.21778, 4.27649, 6.96491], [ 4.27649, 6.96491, 3.21778],
 *  [ 6.15005, 2.40292, 5.09135], [ 1.34421, 1.34421, 1.34421],
 *  [ 2.40292, 5.09135, 6.15005], [ 5.09135, 6.15005, 2.40292],
 *  [ 2.40292, 6.15005, 5.09135], [ 6.15005, 5.09135, 2.40292],
 *  [ 5.09135, 2.40292, 6.15005], [ 3.21778, 6.96491, 4.27649],
 *  [ 6.96491, 4.27649, 3.21778], [ 4.27649, 3.21778, 6.96491],
 *  [ 0.52935, 4.27649, 4.27649], [ 6.96491, 6.96491, 0.52935],
 *  [ 3.21778, 0.52935, 3.21778], [ 6.15005, 6.15005, 1.34421],
 *  [ 1.34421, 5.09135, 5.09135], [ 2.40292, 1.34421, 2.40292],
 *  [ 2.40292, 2.40292, 1.34421], [ 6.15005, 1.34421, 6.15005],
 *  [ 3.21778, 3.21778, 0.52935], [ 6.96491, 0.52935, 6.96491],
 *  [ 4.27649, 0.52935, 4.27649], [ 0.52935, 6.96491, 6.96491],
 *  [ 5.09135, 1.34421, 5.09135], [ 1.34421, 6.15005, 6.15005],
 *  [ 1.34421, 2.40292, 2.40292], [ 0.52935, 3.21778, 3.21778],
 *  [ 4.27649, 4.27649, 0.52935], [ 5.09135, 5.09135, 1.34421]]
 *  Forces: (RETURNS THIS)
 * [[  0.000716,  0.000716,  0.000716], [ -0.000716, -0.000716,  0.000716],
 *  [ -0.000716,  0.000716, -0.000716], [  0.000716, -0.000716, -0.000716],
 *  [  0.000716,  0.000716, -0.000716], [ -0.000716, -0.000716, -0.000716],
 *  [  0.000716, -0.000716,  0.000716], [ -0.000716,  0.000716,  0.000716],
 *  [  0.000716,  0.000716, -0.000716], [  0.000716, -0.000716,  0.000716],
 *  [ -0.000716,  0.000716,  0.000716], [ -0.000716, -0.000716,  0.000716],
 *  [ -0.000716,  0.000716, -0.000716], [  0.000716, -0.000716, -0.000716],
 *  [  0.000716,  0.000716,  0.000716], [ -0.000716, -0.000716,  0.000716],
 *  [ -0.000716,  0.000716, -0.000716], [  0.000716,  0.000716, -0.000716],
 *  [ -0.000716, -0.000716, -0.000716], [  0.000716, -0.000716,  0.000716],
 *  [  0.000716,  0.000716, -0.000716], [  0.000716, -0.000716,  0.000716],
 *  [ -0.000716, -0.000716,  0.000716], [ -0.000716,  0.000716, -0.000716],
 *  [  0.000716,  0.000716,  0.000716], [  0.000716, -0.000716, -0.000716],
 *  [ -0.000716, -0.000716, -0.000716], [ -0.000716,  0.000716,  0.000716],
 *  [ -0.000716,  0.000716,  0.000716], [  0.000716, -0.000716, -0.000716],
 *  [  0.000716,  0.000716,  0.000716], [ -0.000716, -0.000716, -0.000716]]
 * ----------
 */
 const MatX3d& OUTCAR::parse_atom_force_pos(const VecStr& lines) {
   // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << " " << lines.size() << std::endl;
   if (static_cast<int>(lines.size()) != this->_incar._NIONS) {
     string str = string_printf("Parse Iteration's atom position & forces failed:\n\tlines.size() = %3d, NIONS = %3d\n",
        static_cast<int>(lines.size()), this->_incar._NIONS);
     throw str;
     return this->tmpIteration._atom_forces_dirs;
   }
   MatX3d tmppos, tmpforce;
   tmppos.resize(lines.size(), 3);
   tmpforce.resize(lines.size(), 3);
   for (size_t i=0; i!=lines.size(); ++i) {
     int flag = sscanf(lines[i].c_str(), "%lf %lf %lf %lf %lf %lf",
           &tmppos(i, 0), &tmppos(i, 1), &tmppos(i, 2), 
           &tmpforce(i, 0), &tmpforce(i, 1), &tmpforce(i, 2));
     if (6 != flag) {
       string str = string_printf("Parse Iteration Pos & Force failed:\n\tNo. %3d line of input VecStr:\n\t%s\n",
           static_cast<int>(i), lines[i].c_str());
     }
   }
   this->tmpIteration._atom_forces_dirs = std::move(tmpforce);
   this->tmpIteration._atom_positions   = std::move(tmppos);
   return this->tmpIteration._atom_forces_dirs;
 }
   
   
/*
 * Vecd& calc_atom_force(MatrX3d atom_force_dirs)
 *  In:
 * ----------
 *  [[a1, b1, c1],
 *   [a2, b2, c2],
 *   [a3, b3, c3],
 *           ...]
 * ----------
 * Out:
 * ----------
 *  [sqrt(a1^2 + b1^2 + c1^2),
 *   sqrt(a2^2 + b2^2 + c2^2),
 *   sqrt(a3^2 + b3^2 + c3^2),
 *                        ...]
 * ----------
 */
 const Vecd& OUTCAR::calc_atom_force(const MatX3d& atom_force_dirs) {
   Vecd out;
   out.resize(atom_force_dirs.rows());
   for (int i=0; i!=atom_force_dirs.rows(); ++i) {
     out(i) = atom_force_dirs.row(i).norm();
   }
   this->tmpIteration._atom_forces = std::move(out);
   return this->tmpIteration._atom_forces;
 }

/*
 * IonIteration parse_iteration(VecStr& lines)
 *  In: Lines start with "------------------ Iteration", 
 *             ends with "    LOOP+"
 * Out:
 *  IonIteration structure
 */
 OUTCAR::IonIteration OUTCAR::parse_iteration(const VecStr& lines) {
   static const string IT_START_PREFIX = 
     "----------------------------------------- Iteration";
   static const string IT_START_PREFIX_2 = 
     "--------------------------------------- Iteration";

   static const string IT_END_PREFIX = "     LOOP+";
   if (!is_start_with(lines.front(), IT_START_PREFIX) and
       !is_start_with(lines.front(), IT_START_PREFIX_2)) {
     string str = string_printf(
"Parse IonIteration failed:\n\
\tinput VecStr not started with \"----- Iteration\":\n\
\t%s\n", lines.front().c_str());
     throw str;
     return IonIteration{};
   }
   if (!is_start_with(lines.back(), IT_END_PREFIX)) {
     throw "Parse IonIteration failed:\n\tinput VecStr not ends with \"    LOOP+ ....\"";
     return IonIteration{};
   }

   this->tmpIteration = IonIteration();
   int i_line = 0;
   const int end_of_lines = lines.size();
   const int nions = this->_incar._NIONS;

   int parsed_parts = 0;

   for (; i_line < end_of_lines; ++i_line) {
     if (is_start_with(lines[i_line], " number of electron")) {
       this->tmpIteration._nSCF += 1;
       if (2 == this->_incar._ISPIN) {
         parse_magmom(lines[i_line]);
       }
       continue;
     }

     if (is_start_with(lines[i_line], "------------------------ aborting loop")) {
     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       break;
     }
   }

     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
     int line_backup = i_line;
     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ 
       // << " line_backup = "<< line_backup << std::endl;
   for (; i_line < end_of_lines; ++i_line) {
/*
 *  BUG HERE: When ISIF = 2, lattice may not change, so the lattice vectors
 *  may not exist in each ion iterations. In this situation, this loop will
 *  ends with `i_line` reached the end of file, causing the following parse
 *  funcs not working.
 */
     if (is_start_with(lines[i_line], " VOLUME and BASIS")) {
       parse_lattice_volume(lines[i_line + 3]);
       
     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       int latt_start  = i_line + 5;
       int latt_end    = i_line + 8;
       VecStr latt_lines {
         lines.begin() + latt_start, lines.begin() + latt_end };
       parse_lattice(latt_lines);

     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       ++parsed_parts;
       i_line += 8;
       // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       break;
     }
   } // end for
   if (i_line == end_of_lines) {
     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
     i_line = line_backup;
     this->tmpIteration._lattice_vector = this->_latticeVector;
     this->tmpIteration._volume         = this->_latticeVector.determinant();
     ++parsed_parts;
   }

   line_backup = i_line;
   for (; i_line < end_of_lines; ++i_line) {
     /*
      * BUG HERE: Same as parsing lattice_vectors
      */
     if (is_start_with(lines[i_line], " POSITION")) {
       VecStr pos_force_lines{
         lines.begin() + i_line + 2, lines.begin() + i_line + 2 + nions};
       parse_atom_force_pos(pos_force_lines);
       i_line += 2 + nions;

       ++parsed_parts;
       // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       break;
     }
   } // end for
   if (i_line == end_of_lines) {
     i_line = line_backup;
     this->tmpIteration._atom_forces_dirs = 
       this->_iterationVec.back()._atom_forces_dirs;
     this->tmpIteration._atom_positions   =
       this->_iterationVec.back()._atom_positions;
     // std::cout << __FILE__ << __LINE__ << __FUNCTION__ 
       // << "_atom_positions.size() = " << this->tmpIteration._atom_positions.size() << std::endl;
     ++parsed_parts;
   }

   for (; i_line < end_of_lines; ++i_line) {
     if (is_start_with(lines[i_line], "  energy  without"))  {
       parse_toten_0(lines[i_line]);
       parse_toten(lines[i_line - 2]);
       ++parsed_parts;
       ++i_line;
       // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       break;
     }
   }

   for (; i_line < end_of_lines; ++i_line) {
     if (is_start_with(lines[i_line], "     LOOP+")) {
       parse_cpu_time(lines[i_line]);

       ++parsed_parts;
       // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
       break;
     }
   }
   // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;

/*
 * // For DEBUG use only
 *    if (4 != parsed_parts) {
 *      string str = string_printf("Parse Iteration result not complete:\n\t%2d/%2d parsed.\n",
 *          parsed_parts, 4);
 *      std::cerr << str;
 *      std::abort();
 *    }
 */

   calc_atom_force(this->tmpIteration._atom_forces_dirs);
   this->tmpIteration._maxForce = this->tmpIteration._atom_forces.maxCoeff();
   this->tmpIteration._averageF = this->tmpIteration._atom_forces.sum() / this->_incar._NIONS;
   this->tmpIteration._deltaE   = this->tmpIteration._totalEnergy - this->_lastEnergy;
   this->_lastEnergy            = this->tmpIteration._totalEnergy;

   for (int i=0; i!=this->_incar._NIONS; ++i) {
     if (this->tmpIteration._maxForce - this->tmpIteration._atom_forces(i)
         < 1e-5) {
       this->tmpIteration._maxIndex = i;
       this->tmpIteration._maxAtomElem = this->_elem_tab[i];
       break;
     }
   }
   
   const Vecd tmp_pos = 
     this->tmpIteration._atom_forces_dirs
     .row(this->tmpIteration._maxIndex);
   for (int i=0; i!=3; ++i) {
     if ( std::abs(tmp_pos(i) - tmp_pos.maxCoeff()) < 1e-5 ) {
      this->tmpIteration._maxDirection = "xyz"[i];
      break;
     }
   }

   // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl;
   return this->tmpIteration;
 }


/*
 * VecIt parse_iteration_vec(VecStr& lines, int startline, int endline)
 *  In: OUTCAR Iteration Part, start with "----- Iteration", ends with "     LOOP+:"
 *  ----------
 *  ["----------------------------------------- Iteration    1(   1)  ---------------------------------------",
 *  ...
 *  ...
 *  END OF FILE]
 *  ----------
 * Out:
 *  ----------
 *  IonIteration Vector structure
 *  ----------
 */
 const OUTCAR::VecIt& OUTCAR::parseIterationVec(const VecStr& lines,
                                                const int     startline,
                                                      int     endline) {
   endline = (-1 == endline) ? lines.size() : endline;
   static const string IT_START_PREFIX = 
     "----------------------------------------- Iteration";
   static const string IT_START_PREFIX_2 = 
     "--------------------------------------- Iteration";
   static const string IT_END_PREFIX = "     LOOP+";
   _iterationVec.clear();
   _iterationVec.reserve(10);

   this->_lastEnergy    = .0;

   int it_start, it_end;
   bool is_in_iteration = false;
   for (int i=startline; i < endline; ++i) {
     if (!is_in_iteration and 
         (is_start_with(lines[i], IT_START_PREFIX) or 
          is_start_with(lines[i], IT_START_PREFIX_2))) {
       it_start = i;
       is_in_iteration = true;
       continue;
     }
     if (is_in_iteration and is_start_with(lines[i], IT_END_PREFIX)) {
       it_end = i + 1;
       is_in_iteration = false;
       VecStr iteration_lines(lines.begin() + it_start, lines.begin() + it_end);
       // std::cout << __FILE__ << __LINE__ << __FUNCTION__ << " lines.size() = " 
         // << iteration_lines.size() << " at line " << i << std::endl;
       parse_iteration(iteration_lines);
       _iterationVec.push_back(tmpIteration);
       continue;
     }
   }

   this->_lastEnergy    = .0;
   return _iterationVec;
 }


 const OUTCAR::VecIt& OUTCAR::getIterationVec() const {
   return this->_iterationVec;
 }


/*
 * bool saveAsMolden(IonIteration it_vec, char* file_name, int skip)
 * Save IonIteration vector as Molden Format
 * 
 * ************************************************
 * *                 MOLDEN FORMAT                *
 * ************************************************
 * "[Molden Format]"
 * "[Title]"
 * ""
 * "[GEOCONV]"
 * "energy"
 * [TOTEN]
 * "max-force"
 * [MaxForce]
 * "rms-force"
 * [AvgForce]
 * "[GEOMETRIES] XYZ"
 * for (int i=0; i!=ItVec.size(); ++i) {
 *  NIONS
 *
 *  for (int j=0; j!=NIONS; ++j) {
 *    ElementName[j]     x[i][j] y[i][j] z[i][j]
 *  }
 * }
 *
 */

 bool OUTCAR::saveAsMolden(  const VecIt& it_vec,
                             const char*  file_name,
                             const int    skip) {
   if (skip < 0 or static_cast<size_t>(skip) >= it_vec.size()) {
     string str = string_printf("Save as molden failed:\n\tSkip steps invalid:\n\
\tnumber of iterations = %4lld, skip steps = %4d\n",
      it_vec.size(), skip);
     throw str;
     return false;
   }

   std::stringstream ss;
   ss << "[Molden Format]\n" 
      << "[Title]\n\n" 
      << "[GEOCONV]\n" 
      << "energy\n";

   for (size_t i=skip; i!=it_vec.size(); ++i) {
     ss << it_vec[i]._totalEnergy << "\n";
   }
   ss << "max-force\n";
   for (size_t i=skip; i!=it_vec.size(); ++i) {
     ss << it_vec[i]._maxForce << "\n";
   }
   ss << "rms-force\n";
   for (size_t i=skip; i!=it_vec.size(); ++i) {
     ss << it_vec[i]._averageF << "\n";
   }

   ss << "[GEOMETRIES] XYZ\n";
   for (size_t i=skip; i!=it_vec.size(); ++i) {
     ss << this->_incar._NIONS << "\n\n";
     for (int j=0; j!=this->_incar._NIONS; ++j) {
       string line = string_printf("%8s %12.6f %12.6f %12.6f\n", 
           _elem_tab[j].c_str(), 
           it_vec[i]._atom_positions(j, 0), 
           it_vec[i]._atom_positions(j, 1), 
           it_vec[i]._atom_positions(j, 2));
       ss << line;
     }
   }

   std::ofstream ofs(file_name);
   if (!ofs.good()) {
     string str = string_printf("Save as molden failed:\n\
\tOpen %s file failed.\n", file_name);
     throw str;
     return false;
   }
   ofs << ss.str() << "\n";
   ofs.close();
   return true;
 }


/*
 * bool saveAsPoscar(VecIt& it_vec, char* file_prefix, char* folder)
 */
bool OUTCAR::saveAsPoscar(const VecIt& it_vec,
                          const char*  file_prefix,
                          const char*  folder,
                          const bool   is_direct) {
  static auto create_dir = [](const char* path) -> bool {
    const int status = mkdir(path, 0755);
    return 0 == status;
  };
  static auto check_if_dir_exists = [](const char* path) -> bool {
    struct stat info;
    if (0 == stat(path, &info) and S_ISDIR(info.st_mode)) {
      return true;
    } else {
      return false;
    }
  };

  if (!check_if_dir_exists(folder)) {  // Make sure folder exists
    if (!create_dir(folder)) {
      string str = string_printf("Create folder failed:\n\
Cannot create %s .\n", folder);
      throw str;
      return false;
    } else {  }
  } else {  }

  const int nframes = this->_iterationVec.size();
  for (int i=0; i!=nframes; ++i) {
    string folder_(folder);
    while (folder_.back() == '/') {
      folder_.pop_back();
    }
    string file_name = string_printf("%s/%s_%03d.vasp",
        folder_.c_str(), file_prefix, i);
    if (!save_one_frame_as_poscar(it_vec[i], file_name.c_str(), is_direct)) {
      string str = string_printf("Saving POSCAR frames failed:\n\
Saving to single frame failed.\n");
      throw str;
      return false;
    }
  }
  return true;
}

/*
 * bool save_one_frame_as_poscar
 * *******************************************
 * *             POSCAR FORMAT               *
 * *******************************************
 *
 * Title
 * scale
 * [lattice 3x3]
 * [Element names]
 * [ions per element]
 * [direct or cartesian]
 * [Positions]
 */
bool OUTCAR::save_one_frame_as_poscar(const IonIteration&  iteration,
                            const char*          file_name,
                            const bool           is_direct) {
  std::stringstream ss;
  ss << "Title\n" 
     << "1.0\n";
  
  for (int i=0; i!=3; ++i) {
    string line = string_printf("%12.6f %12.6f %12.6f\n",
        iteration._lattice_vector(i, 0),
        iteration._lattice_vector(i, 1),
        iteration._lattice_vector(i, 2));
    ss << line;
  }

  for (const string& e : this->_Elems) {
    ss << std::setw(6) << e;
  } ss << "\n";

  for (int e : this->_atomsPerElem) {
    ss << std::setw(6) << e;
  } ss << "\n";

  if (is_direct) {
    ss << "Direct\n";
    Mat33d b_cell = iteration._lattice_vector.inverse();
    MatX3d frac_pos = iteration._atom_positions * b_cell;

    for (int i=0; i!=frac_pos.rows(); ++i) {
      string line = string_printf("  %12.7f  %12.7f  %12.7f  ! %5s \n",
          frac_pos(i, 0), frac_pos(i, 1), frac_pos(i, 2), 
          _elem_tab[i].c_str() );
      ss << line;
    }
  } else {
    ss << "Cartesian\n";
    const MatX3d cart_pos = iteration._atom_positions;
    
    for (int i=0; i!=cart_pos.rows(); ++i) {
      string line = string_printf("  %12.7f  %12.7f  %12.7f  ! %5s \n",
          cart_pos(i, 0), cart_pos(i, 1), cart_pos(i, 2),
          _elem_tab[i].c_str() );
      ss << line;
    }
  }

  std::ofstream ofs(file_name);
  if (!ofs.good()) {
    string str = string_printf("Save one frame failed:\n\
\tOpen %s file failed.\n", file_name);
    throw str;
    return false;
  } else {
    ofs << ss.str() << "\n";
  }

  ofs.close();
  return true;
}


/******************************************************************
 *                   Viberation involved stuff
 *  This part requires IBRION == 5
 *  `grep 'f  =' OUTCAR` shows the vibrations modes, 
 *    or `grep DOF OUTCAR`
 *  This part ignores the atom positions because they are same for 
 *    all the vibration modes. We use the positions from the result
 *    of structure optmization.
 *  'XSF' and 'mol' formats are supported. 'XSF' files are stored
 *    in seperated files and each file stores one mode. 'mol' file
 *    stores all the modes. You can use VESTA to view XSF files,
 *    or use 'jmol'/'molden' to view the 'mol' file.
 ******************************************************************/


/*
 *  VecVib parseVibration (VecStr& lines, int startline = 0, int endline = -1)
 *  In: "   Degrees of freedom DOF   =           54"
 *          ...
 *          ...
 *      "     1 f  =   60.520970 THz   380.264471 2PiTHz 2018.762208 cm-1   250.294704 meV
 *      "             X         Y         Z           dx          dy          dz
 *      "     10.279364 10.279364  0.016558            0           0           0  
 *      "      0.001283  2.570806  0.016223            0           0           0  
 *      "      0.001283  5.140194  0.016223            0           0           0  
 *      "     10.279364  7.712956  0.016558            0           0           0  
 *
 */
OUTCAR::VecVib OUTCAR::parseVibration(const VecStr& lines,
                                      const int     startline,
                                            int     endline) {
  VecVib out;
  endline = (-1 == endline) ? lines.size() : endline;

  if (5 != this->_incar._IBRION) {
    string str = string_printf(
        "Parse Vibrations failed:\n\
\t IBRION != 5\n\
\t IBRION == %d in this OUTCAR\n", this->_incar._IBRION);
    throw str;
    return out;
  }

  // parse 'DOF'
  this->_dof = 0;
  for (int i=0; i!=endline; ++i) {
    if (is_start_with(lines[i], "   Degrees")) {
      sscanf(lines[i].c_str(), "   Degrees of freedom DOF   = %d", &this->_dof);
      break;
    }
  }
  if (0 == this->_dof) {
    string str = string_printf(
        "Parse Vibrations failed:\n\
\t Degrees of freedom not found.\n");
    throw str;
    return out;
  }

  // search for lines start with ' Eigenvectors'
  for (int i=startline; i!=endline; ++i) {
    if (is_start_with(lines[i], " Eigenvectors")) {
      __current_line = i + 4;
      break;
    }
  }

  int cnt_of_parsed_mode = 0;
  for (int i=__current_line; i!=endline; ++i) {
    if (cnt_of_parsed_mode == this->_dof) {
      break;
    } else { /* */ }
    VecStr vib_mode {
      lines.begin() + i, lines.begin() + i + this->_incar._NIONS + 2 };
    out.push_back(parse_vib_mode(vib_mode));   
    i += this->_incar._NIONS + 2;
    ++cnt_of_parsed_mode;
  }

  if (this->_dof != static_cast<int>(out.size())) {
    string str = string_printf(
        "Parse Vibrations failed:\n\
\t Degrees of freedom not consistent with VecVib from OUTCAR\n\
\t DOF = %d, VecVib.size() = %llu\n", this->_dof, out.size());
    throw str;
    return out;
  }

  return this->_vibrations = out;
}




/*
 * Vibration parse_vib_mode(const VecStr& lines)
 *  In: lines.size() == NIONS + 2
 *   ["   1 f  =   60.520970 THz   380.264471 2PiTHz 2018.762208 cm-1   250.294704 meV",
 *    "             X         Y         Z           dx          dy          dz",
 *    "      3.855500  1.274152  5.434607     0.000000    0.000485    0.000691",
 *    "      3.855500  3.855500  5.534659     0.000025    0.000000   -0.014237",
 *    "      3.855500  6.436848  5.434607     0.000116   -0.000524    0.000669",
 *    "      3.855500  8.996166  5.437054     0.000008    0.000018   -0.000268",
 *    "      6.426896  1.284104  5.433393     0.000168   -0.000154   -0.000169",
 *    "      6.436848  3.855500  5.434607    -0.000494   -0.000023    0.000626",
 *    "      6.426896  6.426896  5.433393     0.000185    0.000159   -0.000180",
 *    "      6.425240  8.996166  5.435514    -0.000059    0.000008   -0.000174",
 *    "      8.996166  1.285760  5.435514     0.000006    0.000052   -0.000135",
 *    "      8.996166  3.855500  5.437054    -0.000013    0.000020   -0.000254",
 *    "      8.996166  6.425240  5.435514     0.000003   -0.000037   -0.000121",
 *    "      8.996166  8.996166  5.436557    -0.000007    0.000017   -0.000186",
 *    "      3.855500  3.855500  7.374391     0.000215    0.000160    0.768259",
 *    "      3.855500  3.855500  8.530953    -0.000003    0.000019   -0.639978"]
 * Out: 
 *        250.294704 ,
 *   [[   0.000000,   0.000485,   0.000691],
 *    [   0.000025,   0.000000,  -0.014237],
 *    [   0.000116,  -0.000524,   0.000669],
 *    [   0.000008,   0.000018,  -0.000268],
 *    [   0.000168,  -0.000154,  -0.000169],
 *    [  -0.000494,  -0.000023,   0.000626],
 *    [   0.000185,   0.000159,  -0.000180],
 *    [  -0.000059,   0.000008,  -0.000174],
 *    [   0.000006,   0.000052,  -0.000135],
 *    [  -0.000013,   0.000020,  -0.000254],
 *    [   0.000003,  -0.000037,  -0.000121],
 *    [  -0.000007,   0.000017,  -0.000186],
 *    [   0.000215,   0.000160,   0.768259],
 *    [  -0.000003,   0.000019,  -0.639978]]
 *
 */
OUTCAR::Vibration OUTCAR::parse_vib_mode(const VecStr& lines) {
  /*
   * std::cout << __FILE__ << __LINE__ << __FUNCTION__ << std::endl
   *           << "lines.size() = " << lines.size() << std::endl
   *           << lines.front() << std::endl << lines.back() << std::endl;
   */

  Vibration out;
  if (static_cast<int>(lines.size()) != this->_incar._NIONS + 2) {
    string str = string_printf(
        "Parse Vibration Mode failed:\n\
\tVibration mode matrix.rows != NIONS\n\
\tmatrix.rows = %d, NIONS = %d\n", lines.size() - 2, this->_incar._NIONS);
    throw str;
    return out;
  } else {/*  */}
  
  // parse freq
  int flag = sscanf(lines[0].c_str(), "%*d f  = %lf THz %*lf 2PiTHz %lf cm-1 %lf meV", 
      &out._freq_THz, &out._freq_cm1, &out._freq_meV);
  if (3 != flag) {
    string str = string_printf(
        "Parse Vibration Mode failed:\n\
\tParse frequency failed:\n\
\t\"%s\"", lines[0].c_str());
    throw str;
    return out;
  }

  out._dxdydz.resize(lines.size() - 2, 3);

  for (int i=2; i!=static_cast<int>(lines.size()); ++i) {
    flag = sscanf(lines[i].c_str(), " %*s %*s %*s  %lf %lf %lf",
        &out._dxdydz(i-2, 0), &out._dxdydz(i-2, 1), &out._dxdydz(i-2, 2));
    if (3 != flag) {
      string str = string_printf(
          "Parse Vibration Mode failed:\n\
\tParse dxdydz failed:\n\
\t\"%s\"\n", lines[i].c_str());
    }
  }
  return out;
}


/*
 *  bool save_one_mode_as_xsf(const Vibration& vib, const char* file_name, double scale)
 *  scale will be used when saving the _dxdydz
 */
bool OUTCAR::save_one_mode_as_xsf(const Vibration& vib,
                                  const char*      file_name,
                                  const double     scale) const {
  if (scale <= 0) {
    string str = string_printf("Save one mode as xsf failed:\n\
\t scale parameter invalid:\
\t scale = %lf\n", scale);
    throw str;
    return false;
  }

  std::stringstream ss;
  ss << "CRYSTAL\nPRIMVEC\n";
  for (int i=0; i!=3; ++i) {
    ss << string_printf("%21.16f %21.16f %21.16f\n",
        this->_latticeVector(i, 0),
        this->_latticeVector(i, 1),
        this->_latticeVector(i, 2));
  }
  ss << "PRIMCOORD\n" << string_printf("%3d 1\n", this->_incar._NIONS);

  const MatX3d& atom_pos = this->_iterationVec.front()._atom_positions;
  for (int i=0; i!=this->_incar._NIONS; ++i) {
    ss << string_printf("%-3s %21.16f %21.16f %21.16f %21.16f %21.16f %21.16f\n",
        this->_elem_tab[i].c_str(),
        atom_pos(i, 0),
        atom_pos(i, 1),
        atom_pos(i, 2),
        vib._dxdydz(i, 0) * scale / AU_TO_A,
        vib._dxdydz(i, 1) * scale / AU_TO_A,
        vib._dxdydz(i, 2) * scale / AU_TO_A);
  }

  std::ofstream ofs(file_name);
  if (!ofs.good()) {
    string str = string_printf("Saving one mode as xsf failed:\n\
\t Creating output file failed.\
\t file_name = %s", file_name);
    throw str;
    return false;
  }

  ofs << ss.str();
  ofs.close();
  return true;
}


/*
 * bool saveAsXsf(VecVib& vibs, const char* prefix, const int mode_ind)
 *
 * Saving all the modes in vibs when mode_ind == 1
 *
 * mode_ind starts from 1
 */
bool OUTCAR::saveAsXsf(const VecVib& vibs,
                       const char*   prefix,
                       const int     mode_ind,
                       const double  scale) const {
  if (scale <= 0) {
    string str = string_printf("Save one mode as xsf failed:\n\
\t scale parameter invalid:\
\t scale = %lf\n", scale);
    throw str;
    return false;
  }

  try {

    if (0 == mode_ind) {
      for (int i=0; i!=static_cast<int>(vibs.size()); ++i) {
        string file_name = string_printf("%s_%03d.xsf", prefix, i + 1);
        bool flag = save_one_mode_as_xsf(vibs[i], file_name.c_str(), scale);
        if (!flag) {
        }
      }
    } else {
      if (mode_ind > static_cast<int>(vibs.size()) or mode_ind < 0) {
          string str = string_printf("Saving Vibrations as XSF failed:\n\
\t mode_ind out of range.\n\
\t mode_ind = %d\n", mode_ind);
          throw str;
          return false;
      }
      string file_name = string_printf("%s_%03d.xsf", prefix, mode_ind);
      save_one_mode_as_xsf(vibs[mode_ind - 1], file_name.c_str(), scale);
    }

  } catch (string msg) {
      string str = string_printf("Saving Vibrations as XSF failed:\n\
\t %s\n", msg.c_str());
      throw str;
      return false;
  }

  return true;
}


/*
 * bool saveAsMol (VecVib& vibs, const char* file_name)
 */
bool OUTCAR::saveAsMol(const VecVib& vibs, 
                       const char* file_name) const {
  std::stringstream ss;
  ss << "[Molden Format]\n" << "[FREQ]\n";

  for (const auto& e : vibs) {
    ss << string_printf("%10.4f\n", e._freq_cm1);
  }

  ss << "[FR-COORD]\n";
  const MatX3d& atom_pos = this->_iterationVec.front()._atom_positions;
  for (int i=0; i!=this->_incar._NIONS; ++i) {
    const string elem_type = this->_elem_tab[i];
    ss << string_printf("%-3s %13.5f %13.5f %13.5f\n",
        this->_elem_tab[i].c_str(),
        atom_pos(i, 0) / AU_TO_A,
        atom_pos(i, 1) / AU_TO_A, 
        atom_pos(i, 2) / AU_TO_A);
  }

  ss << "[FR-NORM-COORD]\n";
  for (int i=0; i!=static_cast<int>(vibs.size()); ++i) {
    ss << "vibration " << i + 1 << "\n";
    for (int j=0; j!=this->_incar._NIONS; ++j) {
      ss << string_printf(" %13.5f %13.5f %13.5f\n",
          vibs[i]._dxdydz(j, 0) / AU_TO_A, 
          vibs[i]._dxdydz(j, 1) / AU_TO_A, 
          vibs[i]._dxdydz(j, 2) / AU_TO_A);
    }
  }

  std::ofstream ofs(file_name);
  if (!ofs.good()) {
    string str = string_printf("Saving as mol file failed:\n\
\t Cannot open destination file.\n\
\t %s open failed.\n", file_name);
    throw str;
    return false;
  }

  ofs << ss.str();
  ofs.close();
  return true;
}




} // namepsace ionizing
