#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <Eigen/Eigen>

namespace ionizing {
  using namespace Eigen;
  using namespace std;

  class BinIO {
  public:
    BinIO(const char* FileName);
    ~BinIO() = default;
    BinIO(const BinIO&) = delete;             // Prevent 'copying' operations.
    BinIO& operator=(const BinIO&) = delete;

  template <typename T>
    T readElement();

  template <typename T>
    Matrix<T, Dynamic, 1> readVectorCol(const int size);

  template <typename T>
    Matrix<T, 1, Dynamic> readVectorRow(const int size);

  template <typename T>
    Matrix<T, Dynamic, Dynamic> readMatrix(const int nRow, const int nCol);

  private:
    ifstream ifs;
    ofstream ofs;

    void _openFile(const char* FileName);
  template <typename T>
    T _readElement();
  };
};
