#include <fft.hpp>
#include <iostream> // For debug

namespace ionizing {
  using namespace Eigen;

  using Vecd  = VectorXd;
  using Veccd = VectorXcd;
  using Matd  = Matrix<double,               Dynamic, Dynamic, RowMajor>;
  using Matcd = Matrix<std::complex<double>, Dynamic, Dynamic, RowMajor>;
  using Cubd  = Tensor<double,                              3, RowMajor>;
  using Cubcd = Tensor<std::complex<double>,                3, RowMajor>;

  Veccd fft_1d(const Veccd& vec){
    Veccd out(vec.size());
    auto plan = fftw_plan_dft_1d(vec.size(),
        (fftw_complex *)vec.data(), (fftw_complex *)out.data(),
        FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    return out;
  }

  Veccd fft_1d(const Vecd& vec){
    Veccd tmp = vec.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return fft_1d(tmp);
  }

  Veccd ifft_1d(const Veccd& vec) {
    Veccd out(vec.size());
    auto plan = fftw_plan_dft_1d(vec.size(),
        (fftw_complex *)vec.data(), (fftw_complex *)out.data(),
        FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    out /= out.size();
    return out;
  }

  Veccd ifft_1d(const Vecd& vec){
    Veccd tmp = vec.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return ifft_1d(tmp);
  }


  Matcd  fft_2d(const Matcd& mat) {
    Matcd out(mat.rows(), mat.cols());
    auto plan = fftw_plan_dft_2d(mat.rows(), mat.cols(),
        (fftw_complex *)mat.data(), (fftw_complex *)out.data(),
        FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    return out;
  }



  Matcd  fft_2d(const  Matd& mat) {
    Matcd tmp = mat.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return fft_2d(tmp);
  }
  
  Matcd ifft_2d(const Matcd& mat) {
    Matcd out(mat.rows(), mat.cols());
    auto plan = fftw_plan_dft_2d(mat.rows(), mat.cols(),
        (fftw_complex *)mat.data(), (fftw_complex *)out.data(),
        FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    out /= out.size();
    return out;
  }

  Matcd ifft_2d(const  Matd& mat) {
    Matcd tmp = mat.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return ifft_2d(tmp);
  }

  Cubcd  fft_3d(const Cubcd& cub) {
    Cubcd out(cub.dimension(0), cub.dimension(1), cub.dimension(2));
    auto plan = fftw_plan_dft_3d(
        cub.dimension(0), cub.dimension(1), cub.dimension(2), 
        (fftw_complex *)cub.data(), (fftw_complex *)out.data(),
        FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    return out;
  }

  Cubcd  fft_3d(const  Cubd& cub) {
    Cubcd tmp = cub.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return fft_3d(tmp);
  }

  Cubcd ifft_3d(const Cubcd& cub) {
    Cubcd out(cub.dimension(0), cub.dimension(1), cub.dimension(2));
    auto plan = fftw_plan_dft_3d(
        cub.dimension(0), cub.dimension(1), cub.dimension(2), 
        (fftw_complex *)cub.data(), (fftw_complex *)out.data(),
        FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
    // std::cout << __FUNCTION__ << "out.size() == " << out.size() << std::endl;
    out /= out.constant(out.size());
    return out;
  }

  Cubcd ifft_3d(const  Cubd& cub) {
    Cubcd tmp = cub.cast<std::complex<double>>();
    // std::cout << __FUNCTION__ << ": tmp = \n" << tmp << std::endl;
    return ifft_3d(tmp);
  }




}
