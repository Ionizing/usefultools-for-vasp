#include "BinIO.hpp"

namespace ionizing {
  using namespace Eigen;
  using namespace std;

  void BinIO::_openFile(const char* FileName) {
    ifs.open(FileName, ios::binary);
    string str_stars(10, '*');
    if (!ifs.good()) {
      cerr << str_stars << " Binary File \"" << FileName << "\" open failed " << str_stars << endl;
      abort();
    }
  }

  template <typename T>
    T BinIO::_readElement() {
      
    }

};
