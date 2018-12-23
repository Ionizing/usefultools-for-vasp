#include "BinIO.hpp"

namespace ionizing {
  BinIO::BinIO(const char* FileName) {
    _openFile(FileName);
    std::cout << "BinIO created successful" << std::endl;
  }

  BinIO::~BinIO() {
    ifs.close();
    std::cout << "BinIO destructed" << std::endl;
  }

  void BinIO::_openFile(const char* FileName) {
    ifs.open(FileName, std::ios::ate | std::ios::binary | std::ios::in);
    std::string str_stars(10, '*');
    if (ifs.fail()) {
      std::cerr << str_stars << " Binary File \"" << FileName << "\" open failed " << str_stars << std::endl;
      std::abort();
    }
    _fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
  }

  void BinIO::seek(const int n) {
    if (n < 0 or n >= _fileSize) {
      std::cout << "****ERROR!****  seeking an invalid file position: " << n << " !\n";
      std::abort();
    }
    ifs.seekg(n);
  }
  
  int BinIO::getFileSize() const {
    return _fileSize;
  }

}
