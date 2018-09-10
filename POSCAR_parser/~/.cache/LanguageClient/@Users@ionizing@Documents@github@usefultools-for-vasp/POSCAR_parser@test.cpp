#include "POSCAR.h"

int main(int argc, char* argv[]) {
  std::string FileName("POSCAR");
  ionizing::POSCAR poscar(FileName);
  poscar.PrintToFile(std::string("POSCAR_parsed.txt"));

  return 0;
}
