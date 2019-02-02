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
                        lines.begin() + __current_line + this->_NKPTS};
    return parse_kpoints(lines_to_use);
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
  const int& OUTCAR::parse_nkpts(const string& line) {
    if (!is_start_with(line, "   k-points           NKPTS = ")) {
      _NKPTS = -1;
      throw string_printf("Invalid line input for NKPTS parsing:\n%s", line.c_str());
      return _NKPTS;
    }

    int flag = sscanf(line.c_str(), "   k-points           NKPTS = %d", &_NKPTS);
    if (1 != flag or _NKPTS <= 0) {
      _NKPTS = -1;
      throw string_printf("Parse NKPTS failed:\n%s\n NKPTS=%d", line.c_str(), _NKPTS);
      return _NKPTS;
    }
    return _NKPTS;
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
    if (_NKPTS <= 0) {
      throw string_printf("Invalid NKPTS:%4d", _NKPTS);
      return _kpoints;
    }
    if (static_cast<int>(lines.size()) != _NKPTS) {
      throw string_printf("Inconsistent KPoint number and KPath array size: %4d != %4lld", _NKPTS, lines.size());
      return _kpoints;
    }
    _kpoints.resize(_NKPTS, 3);

    double col1, col2, col3;
    for (int i=0; i!=_NKPTS; ++i) {
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

}
