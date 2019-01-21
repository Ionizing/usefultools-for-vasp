#include "../include/textio.hpp"

namespace ionizing {

TextIO::TextIO(const char* file_name) : _is{nullptr} {
  _ifs.open(file_name, std::ios::ate);
  if (! _ifs.good()) {
    std::cerr << "\n***** Open text file " << file_name << " failed. *****\n" << std::endl;
    std::abort();
  } else {
    std::cout << "\nTextIO created successfully!\n" << std::endl;
  }
  _fileSize = _ifs.tellg();
  _ifs.seekg(0);
  _is.rdbuf(_ifs.rdbuf());

  {
    std::cout << "\nDEBUG: " << __FILE__ << __FUNCTION__ <<" filesize of " << file_name << " is " << _fileSize << std::endl;
  }
}

TextIO::TextIO(std::istream& is) : _is{is.rdbuf()} {
  std::cout << "\nUsing redirected filestream.\n" << std::endl;
}

/*
 * TextIO::TextIO(const string& str) : _is(nullptr) {
 *   std::stringstream ss(str);
 *   _is.rdbuf(ss.rdbuf());
 * }
 */

TextIO::~TextIO() {
  std::cout << "\nTextIO destructed successfully!\n" << std::endl; 
} 

long TextIO::getFileSize() const {
  return _fileSize;
}

}
