#include "POSCAR.h"

int main(int argc, char* argv[]) {
  // std::string FileName("POSCAR");
  // ionizing::POSCAR poscar(FileName);
  // poscar.PrintToStdout();
  // // poscar.PrintToFile(std::string("POSCAR_parsed.txt"));

  // FileName = std::string("POSCAR_2");
  // ionizing::POSCAR poscar_2(FileName);
  // poscar_2.PrintToStdout();
  
  if (2 == argc) {
    std::string FileName = argv[1];
    ionizing::POSCAR poscar(FileName);
    poscar.PrintToStdout();
  }
  return 0;
}
