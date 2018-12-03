#include <hkl/gfflatter.hpp>

#include <iostream>

using std::cerr;
using std::string;
using std::vector;

int main(int argc, char* argv[]) {
  const auto args = get_arguments(argc, argv);

  return 0;
}

Args::Arguments get_arguments(int argc, char* argv[]) {
  auto args = Args::Arguments(argc, argv);

  return args;
}
