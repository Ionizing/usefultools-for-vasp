#pragma once
#ifndef TEXTIO_H
#define TEXTIO_H

#include "base.hpp"

namespace ionizing {

class TextIO {
  public:
    TextIO            (const char*         file_name);
    TextIO            (      std::istream& is);
    /*
     * TextIO            (const string&       str);
     */
    ~TextIO           ();
    TextIO            (const TextIO&       ) = delete;
    TextIO& operator= (const TextIO&       ) = delete;

    long getFileSize() const;
    
    template <typename T>
      T readElement();
    template <typename T>
      VecT<T> readVector(const long size);
    template <typename T>
      MatT<T> readMatrix(const long rows, const long cols);

  private:
    // ifs is just a temporary filestream.
    // I didn't put ifs in construct cause it would be
    // destroyed automately when constructor returns.
    std::ifstream _ifs;  
    std::istream  _is;
    long _fileSize;
};







// template implements
template <typename T>
  T TextIO::readElement() {
    T out;
    _is >> out;
    if (_is.fail()) {
      std::string tmp;
      _is >> tmp;
      std::cerr << "\nERROR: cannot read " << tmp << " into "
        << typeid(T).name() << " \n" << std::endl;
      std::abort();
    }
    return out;
  }

template <typename T>
MatT<T> TextIO::readMatrix(const long rows, const long cols) {
  if (rows <= 0 or cols <= 0) {
    std::cerr << "\nERROR: Invalid rows or cols for reading matrix: " << rows << "x" << cols << " !\n" << std::endl;
    std::abort();
  }
  
  MatT<T> out(rows, cols);
  for (long i=0; i!=rows; ++i) {
    for (long j=0; j!=cols; ++j) {
      out(i, j) = readElement<T>();
    }
  }
  return out;
}

template <typename T>
VecT<T> TextIO::readVector(const long size) {
  if (size <= 0) {
    std::cerr << "\nERROR: Invalid size for reading vector: " << size << " !\n" << std::endl;
    std::abort();
  }
  return readMatrix<T>(size, 1);
}


}

#endif
