#include <outcar.hpp>

namespace ionizing {


/*
 * TEST func
 */
  VecStr OUTCAR::test_parse_elem(std::istream& is) {
    file_to_string(is);
    string_to_vecstr(_content);
    return parseElems(_contentVector, _content);
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
 *  "   VRHFIN =Cu: d10 p1",
 *  "   VRHFIN =C: s2p2",
 *  "   VRHFIN =H: ultrasoft test"
 *  ---------
 * out: String Vector full of element names;
 *  ---------
 *  "Cu",
 *  "C",
 *  "H"
 *  ---------
 */
  VecStr OUTCAR::parse_elems(const VecStr& lines) {
    VecStr out;
    for (string e : lines) {
      if (!is_start_with(e, "   VRHFIN =")) {
        throw "String doesn't contain element name"; 
      }
      e.erase(0, 11);
      e.erase(e.find_first_of(':'), string::npos);
      out.emplace_back(std::move(e));
      /*
       * string token = split(e)[1];
       * token.pop_back();
       * token.erase(0, 1); // erase 1 element from index 0
       * out.emplace_back(std::move(token));
       */
    }
    return out;
  }


/*
 * void parsElems(VecStr lines, string content)
 *  in: OUTCAR content in VecStr(lines) and one string(content)
 * out: String Vector of element names;
 */
  VecStr OUTCAR::parseElems(const VecStr& lines, 
                            const string& content,
                            const int     startline,
                                  int     endline) {
    endline = (endline < 0) ? lines.size() : endline;
    _nElems = count_substr(content, "   VRHFIN =");
    VecStr lines_to_use;

    int cnt = 0;
    for (int i=startline; i!=endline; ++i) {
      if (cnt < _nElems and is_start_with(lines[i], "   VRHFIN =")) {
        lines_to_use.emplace_back(lines[i]);
        ++cnt;
      }
      if (cnt == _nElems) {
        break;
        __current_line = i;
      } else /**/ ;
    }

    return parse_elems(lines_to_use);
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
      sscanf(lines[i].c_str(), " A%*c = (%lf,%lf,%lf)", &out(i, 0), &out(i, 1), &out(i, 2));
    }
    
    return out;
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

    /*
     * if (13 != parsed_lines) {
     *   string str;
     *   string_printf("\tINCAR Parse failed with incorrect lines:%4d\n", parsed_lines);
     *   throw str;
     * }
     */

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







/*
 * double parse_toten(const string& line)
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
  double OUTCAR::parse_toten(const string& line) {
    double toten, toten_sigma_0;
    int flag = sscanf(line.c_str(), "  energy  without entropy= %lf  energy(sigma->0) = %lf",
        &toten, &toten_sigma_0);
    if (flag != 2 or (toten > 0 or toten_sigma_0 > 0)) {
      string str = string_printf("Parse TOTEN failed:\n\t%s\n\ttoten = %lf, toten_sigma_0 = %lf\n",
          line.c_str(), toten, toten_sigma_0);
      throw str;
      return -1;
    }
    this->tmpIteration._totalEnergy         = toten;
    this->tmpIteration._totalEnergy_sigma_0 = toten_sigma_0;
    return this->tmpIteration._totalEnergy;
  }

/*
 * double calc_delta_toten(double toten, double last_toten)
 *  In:
 *  ----------
 *  -20, 0
 *  ----------
 * Out:
 *  ----------
 *  -20
 *  ----------
 */
  double OUTCAR::calc_delta_toten(const double toten,
      const double last_toten) {
    return toten - last_toten;
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
    if (1 != flag or cpu_time > 0) {
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
    double magmom;
    int flag = sscanf(line.c_str(), " number of electron %*lf magnetization %lf",
        &magmom);
    if (1 != flag or magmom < 0) {
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
 * Out:
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
   if (lines.size() != this->_incar._NIONS) {
     string str = string_printf("Parse Iteration's atom position & forces failed:\n\tlines.size() = %3d, NIONS = %3d\n",
        static_cast<int>(lines.size()), this->_incar._NIONS);
     throw str;
     return this->tmpIteration._atom_forces;
   }
   MatX3d tmppos, tmpforce;
   tmppos.resize(lines.size(), 3);
   tmpforce.resize(lines.size(), 3);
   for (size_t i=0; i!=lines.size(); ++i) {
     sscanf(lines[i].c_str(), "%lf %lf %lf %lf %lf %lf",
         &tmppos(i, 0), &tmppos(i, 1), &tmppos(i, 2), 
         &tmpforce(i, 0), &tmpforce(i, 1), &tmpforce(i, 2));
   }

   return this->tmpIteration._atom_forces;
 }
   
   



}
