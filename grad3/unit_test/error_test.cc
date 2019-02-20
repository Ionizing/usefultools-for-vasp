#include <error.hpp>

void foo() {
  PRINT_ERROR("test in function 'foo'");
}

int main() {

  PRINT_ERROR("test in function 'main'");

  return 0;
}
