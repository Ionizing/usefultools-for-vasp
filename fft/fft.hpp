#pragma once
#ifndef FFT_H
#define FFT_H

#include <fftw3.h>
#include <unsupported/Eigen/CXX11/Tensor>
#include <complex>

namespace ionizing {

  using namespace Eigen;

  using Vecd  = VectorXd;
  using Veccd = VectorXcd;
  using Matd  = Matrix<double,               Dynamic, Dynamic, RowMajor>;
  using Matcd = Matrix<std::complex<double>, Dynamic, Dynamic, RowMajor>;
  using Cubd  = Tensor<double,                              3, RowMajor>;
  using Cubcd = Tensor<std::complex<double>,                3, RowMajor>;

  Veccd  fft_1d(const Veccd& vec);
  Veccd  fft_1d(const  Vecd& vec);
  Veccd ifft_1d(const Veccd& vec);
  Veccd ifft_1d(const  Vecd& vec);

  Matcd  fft_2d(const Matcd& mat);
  Matcd  fft_2d(const  Matd& mat);
  Matcd ifft_2d(const Matcd& mat);
  Matcd ifft_2d(const  Matd& mat);

  Cubcd  fft_3d(const Cubcd& cub);
  Cubcd  fft_3d(const  Cubd& cub);
  Cubcd ifft_3d(const Cubcd& cub);
  Cubcd ifft_3d(const  Cubd& cub);

}
#endif // FFT_H
