#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <fstream>
#include <vector>

// #define DEBUG

std::string _readWholeFileToString(std::istream& is);
std::vector<std::string> _readWholeFileToStrVec(std::istream& is);
std::vector<std::string> _matchTokens(const std::string& str, const std::string pattern);
bool _compare(const std::vector<std::string> vec1, const std::vector<std::string> vec2);

int main(int argc, char* argv[]) { 
  std::ifstream poscar("POSCAR"), potcar("POTCAR");
  if (poscar.fail()) {
    std::cerr << "POSCAR open failed" << std::endl;
    std::abort();
  }

  if (potcar.fail()) {
    std::cerr << "POTCAR open failed" << std::endl;
    std::abort();
  }

  std::string POTCAR = _readWholeFileToString(potcar);
  std::vector<std::string> PotElements = _matchTokens(POTCAR, "VRHFIN =([a-zA-Z]+)");
#ifdef DEBUG
  std::cout << "DEBUG: " << __FILE__ << __LINE__ << "\n"
    << "PotElements = " ;
  for (auto e : PotElements) {
    std::cout << e << " ";
  } std::cout << "\n";
#endif

  
  std::vector<std::string> POSCAR_vec = _readWholeFileToStrVec(poscar);
  std::string ElemLine = POSCAR_vec[5];
#ifdef DEBUG
  std::cout << "DEBUG: POSCAR ElemLine is "<< ElemLine << std::endl;
#endif
  std::vector<std::string> PosElements = _matchTokens(ElemLine, "[a-zA-Z]+");
#ifdef DEBUG
  std::cout << "DEBUG: " << __FILE__ << __LINE__ << "\n"
    << "PosElements = " ;
  for (auto e : PosElements) {
    std::cout << e << " ";
  } std::cout << "\n";
#endif
  if (PosElements.size() == 0) {
    std::cout << "No element tag at line 6, POSCAR" << std::endl;
    std::abort();
  }

  if (!_compare(PosElements, PotElements)) {
    std::cout << "Compare failed." << std::endl;
  } else {
    std::cout << "OK" << std::endl;
  }
  return 0;
}

std::string _readWholeFileToString(std::istream& is) {
  if (is.fail()) {
    std::cerr << "File stream failed." << std::endl;
    std::abort();
  }
  std::stringstream buffer;
  buffer << is.rdbuf();
  return buffer.str();
}

std::vector<std::string> _readWholeFileToStrVec(std::istream& is) {
  if (is.fail()) {
    std::cerr << "File stream failed." << std::endl;
    std::abort();
  }

  std::vector<std::string> vec;
  std::string line;
  while (std::getline(is, line)) {
    vec.push_back(line);
  }

  return vec;
}

std::vector<std::string> _matchTokens(const std::string& str, const std::string pattern) {
  std::regex re(pattern); 
  std::smatch m;
  std::vector<std::string> results;
  auto it = str.cbegin();
  while (std::regex_search(it, str.cend(), m, re)) {
    if (m.size() > 1) {
      results.push_back(*(m.end()-1));
    } else {
      results.push_back(m[0]);
    }
    it = m.suffix().first;
  }
  return results;
}

bool _compare(const std::vector<std::string> PosVec, const std::vector< std::string> PotVec) {
  if (PosVec.size() > PotVec.size()) {
    std::cerr << "Insufficient PseudoPotential Elements" << std::endl;
    std::cerr << " POSCAR <<- ";

    for (auto e : PosVec) {
      std::cerr << e << " ";
    } std::cerr << "->>\n";

    std::cerr << " POTCAR <<- ";
    for (auto e : PotVec) {
      std::cerr << e << " ";
    } std::cerr << "->>\n";

    std::abort();
  }

  bool isSuccess = true;

  for (int i=0; i!=PosVec.size(); ++i) {
    const std::string strA = PosVec[i], strB = PotVec[i];
#ifdef DEBUG
  std::cout << "DEBUG:" << __FILE__ << __LINE__ << "\n"
    << "strA = " << strA << "\n"
    << "strB = " << strB << "\n";
#endif
    if (strA.length() != strB.length()) {
      isSuccess = false;
      std::cerr << "Inconsistent elements: " << strA 
        << " " << strB << std::endl;
    } else {
      for (int j=0; j!=strA.length(); ++j) {
        if (::toupper(strA[j]) != ::toupper(strB[j])) {
          isSuccess = false;
          std::cerr << "Inconsistent elements: " << strA 
            << " " << strB << std::endl;
          continue;
        }
      } // end inner for
    }
  } // end of outer for

  return isSuccess;
}
