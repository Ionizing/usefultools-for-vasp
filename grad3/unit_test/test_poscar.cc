#include <poscar.hpp>

using ionizing::POSCAR;

int main() {
  POSCAR poscar{"./test6/POSCAR"};

  /*
   * poscar.saveAsDuplicate("POSCAR_cart", true);
   * poscar.saveAsDuplicate("POSCAR_direct", false);
   */
  return 0;
}
