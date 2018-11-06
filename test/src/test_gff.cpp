#include "test_gff.hpp"

AGizmo::Evaluation::Stats TestHKL::TestGFF::check_gffreader(bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::GFF::GFFReader"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<GFFTest> contructor_tests{{{"test/input/annotation.gff"}, {}}};

  Evaluator eval(test_name, contructor_tests);
  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

TestHKL::TestGFF::GFFTest::GFFTest(InputGFF input, OutputGFF expected)
    : BaseTest(input, expected) {
  validate();
}
