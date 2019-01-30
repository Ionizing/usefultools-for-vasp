#include <error.hpp>

void foo() {
  PRINT_ERROR("test in function 'foo'");
}

int main(int argc, char* argv[]) {

  PRINT_ERROR("test in function 'main'");

  return 0;
}
