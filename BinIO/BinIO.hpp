#pragma once

#ifndef BINIO_H
#define BINIO_H

// #define EIGEN_USE_BLAS
// #define EIGEN_USE_LAPACK

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "Eigen/Eigen"

namespace ionizing {
  class BinIO {
  public:
    BinIO(const char* FileName);
    ~BinIO();
    BinIO(const BinIO&) = delete;             // Prevent 'copying' operations.
    BinIO& operator=(const BinIO&) = delete;
    void seek(const int n);
    int getFileSize() const;

  template <typename T>
    T readElement();

  template <typename T>
    Eigen::Matrix<T, Eigen::Dynamic, 1> readVectorCol(const long size);

  template <typename T>
    Eigen::Matrix<T, 1, Eigen::Dynamic> readVectorRow(const long size);

  template <typename T>
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> readMatrix(const long nRow, const long nCol);

  private:
    std::ifstream ifs;
    std::ofstream ofs;
    int _fileSize;

  private:
    void _openFile(const char* FileName);

  }; // end of class BinIO
};


namespace ionizing {

  template <typename T>
    T BinIO::readElement() {
      long len = sizeof(T);
      T x;
      ifs.read((char*) &x, len);
      return x;
    }

  template <typename T>
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> BinIO::readMatrix(const long nRow, const long nCol) {
      Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrix(nRow, nCol);
      ifs.read((char*) matrix.data(), nRow * nCol * sizeof(T));
      return matrix;
    }

  template <typename T> 
    Eigen::Matrix<T, Eigen::Dynamic, 1> BinIO::readVectorCol(const long size) {
      return readMatrix<T>(size, 1);
    }
 
  template <typename T>
    Eigen::Matrix<T, 1, Eigen::Dynamic> BinIO::readVectorRow(const long size) {
      return readMatrix<T>(1, size);
    } 


}
#endif  // BINIO_H
