#include "test_hkl.hpp"

int TestHKL::perform_tests(bool verbose) {
  Stats result;

  cout << gen_framed("Evaluating HKL") << "\n";

  result(TestRegion::check_region_constructors(verbose));
  result(TestRegion::check_region_formation(verbose));
  result(TestRegion::check_region_failure(verbose));
  result(TestRegion::check_region_paired(verbose));
  result(TestRegion::check_region_resize(verbose));
  result(TestRegion::check_region_slice(verbose));
  result(TestRegionSeq::check_basic(verbose));
  result(TestRegionSeq::check_get_seq(verbose));
  result(TestRegionSeq::check_fasta_reader(verbose));

  cout << "\n" << gen_summary(result, "Evaluation", true) << "\n";

  return result.getFailed();
}

int main() { return TestHKL::perform_tests(false); }
