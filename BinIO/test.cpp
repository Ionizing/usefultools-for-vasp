#include <iostream>
#include <fstream>
#include <Eigen/Eigen>
#include "BinIO.hpp"

using Eigen::MatrixXd;
using Eigen::Matrix;
using ionizing::BinIO;

int main(int argc, char* argv[]) {
  // BinIO io("test.bin");

  MatrixXd matrix = MatrixXd::Random(4, 5);
  long rows = matrix.rows(), cols = matrix.cols();
  
  std::cout << "matrix = \n" << matrix << std::endl;

  std::ofstream ofs("test.bin", std::ios::binary | std::ios::out | std::ios::trunc);
  ofs.write((char*) &rows, sizeof(rows));
  ofs.write((char*) &cols, sizeof(cols));
  ofs.write((char*) matrix.data(), rows*cols*sizeof(double));
  ofs.close();

  BinIO io2("test.bin");
  rows = io2.readElement<long>();
  cols = io2.readElement<long>();
  // MatrixXd matrix2 = io2.readMatrix<double> (rows, cols);
  // std::cout << "matrix2 = \n" << matrix2 << std::endl;
  std::cout << "matrix2 = \n" << std::endl;
  // for (int i=0; i!=rows; ++i) {
    // std::cout << io2.readVectorRow<double>(cols) << std::endl;
  // }
  
  matrix = io2.readMatrix<double>(rows, cols);
  std::cout << matrix << std::endl;

  return 0;
}
