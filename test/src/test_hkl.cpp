#include "test_hkl.hpp"

int TestHKL::perform_tests(bool verbose) {
  Stats result;

  for (int i = 0; i < 2; ++i) {
    bool verbose = i;
    Stats result;

    cout << gen_framed("Evaluating HKL") << "\n";

    result(TestRegion::check_region_constructors(verbose));
    result(TestRegion::check_region_formation(verbose));
    result(TestRegion::check_region_failure(verbose));

    cout << "\n" << gen_summary(result, "Evaluation", true) << "\n";
  }

  return result.getFailed();
}

int main() { return TestHKL::perform_tests(true); }
