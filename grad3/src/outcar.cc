#include <outcar.hpp>

namespace ionizing {

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
  VecStr OUTCAR::parse_elems(const VecStr& lines) const {
    VecStr out;
    for (string e : lines) {
      if (!is_start_with(e, "   VRHFIN =")) {
        throw "String doesn't contain element name"; 
      }
      string token = split(e)[1];
      token.pop_back();
      token.erase(0, 1); // erase 1 element from index 0
      out.emplace_back(std::move(token));
    }
    return out;
  }


/*
 * void parsElems(VecStr lines, string content)
 *  in: OUTCAR content in VecStr(lines) and one string(content)
 * out: String Vector of element names;
 */
  VecStr OUTCAR::parseElems(const VecStr& lines, const string& content) {
    _nElems = count_substr(content, "   VRHFIN =");
    VecStr lines_to_use;

    int cnt = 0;
    for (const string str : lines) {
      if (cnt < _nElems and is_start_with(str, "   VRHFIN =")) {
        lines_to_use.emplace_back(std::move(str));
        ++cnt;
      }

      if (cnt == _nElems) {
        break;
      } else /*     */;
    }

    return parse_elems(lines_to_use);
  }
  



}
