#include <poscar.hpp>

using ionizing::POSCAR;

int main(int argc, char* argv[]) {
  POSCAR poscar{"./POSCAR"};

  poscar.saveAsDuplicate("POSCAR_cart", true);
  poscar.saveAsDuplicate("POSCAR_direct", false);
  return 0;
}
