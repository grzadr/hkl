#include "test_hkl.hpp"

int perform_tests(bool verbose) {
  Stats result;

  for (int i = 0; i < 2; ++i) {
    bool verbose = i;
    Stats result;

    cout << gen_framed("Evaluating HKL") << "\n";

    cout << "\n" << gen_summary(result, "Evaluation", true) << "\n";
  }

  return result.getFailed();
}

int main() { return perform_tests(true); }
