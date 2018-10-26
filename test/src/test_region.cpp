#include "test_region.hpp"

using AGizmo::Evaluation::Stats;

Stats TestHKL::TestRegion::check_region_constructors(bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::Region::Region()"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionConstructors> contructor_tests{
      {{""}, ":0"},
      {{":0"}, ":0"},
      {{"", "0", "0", "0"}, ":0"},
      {{"", "0", "0", ""}, ":0"},
      {{"", "", "", "+"}, ":0/+"},
      {{"A:1-2/+"}, "A:1-2/+"},
      {{"A", "1", "2", "+"}, "A:1-2/+"},
      {{"A", "1", "2", ""}, "A:1-2"},
      {{"", "1", "2", ""}, ":1-2"},
      {{"A"}, "A:0"},
      {{"A", "", "", "+"}, "A:0/+"},
  };

  Evaluator eval(test_name, contructor_tests);
  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

AGizmo::Evaluation::Stats
TestHKL::TestRegion::check_region_formation(bool verbose) {
  Stats result;

  sstream message, failure;

  const auto &test_name = "HKL::Region Formation"s;

  message << "\n~~~ Checking " << test_name << "\n";

  message << "\n>>> Constructing tests\n";

  ifstream input;
  string line;

  Files::open_file("test/input/constructors.tsv", input);

  string header;
  getline(input, header);

  auto header_splitted = StringDecompose::str_split(header, "\t");

  vector<string> functions{};
  std::copy(next(header_splitted.begin(), 6), header_splitted.end(),
            back_inserter(functions));

  vector<RegionFormation> formation_tests{};

  while (getline(input, line)) {
    const auto splitted = StringDecompose::str_split(line, "\t");
    auto id = stoi(splitted[0]);
    auto args =
        InputRegionArgs(splitted[1], splitted[2], splitted[3], splitted[4]);
    InputSingleRegionEval first_input{id, args, ""s};
    formation_tests.emplace_back(first_input, splitted[5]);
    for (size_t i = 0; i < functions.size(); ++i)
      formation_tests.emplace_back(
          InputSingleRegionEval{id, args, functions[i]}, splitted[i + 6]);
    //    break;
  }

  message << ">>> Constructed " << formation_tests.size() << " tests\n"
          << "\n>>> Testing\n";

  Evaluator eval(test_name, formation_tests);
  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

AGizmo::Evaluation::Stats
TestHKL::TestRegion::check_region_failure(bool verbose) {
  Stats result;

  sstream message, failure;

  const auto &test_name = "HKL::Region Failure"s;

  message << "\n~~~ Checking " << test_name << "\n";

  message << "\n>>> Constructing tests\n";

  ifstream input;
  string line;

  Files::open_file("test/input/failure.tsv", input);
  getline(input, line);

  vector<RegionFailure> failure_tests{};

  while (getline(input, line)) {
    const auto splitted = StringDecompose::str_split(line, "\t");
    failure_tests.emplace_back(
        InputRegionArgs(splitted[1], splitted[2], splitted[3], splitted[4]),
        splitted[5]);
  }

  message << ">>> Constructed " << failure_tests.size() << " tests\n"
          << "\nTesting:\n";

  Evaluator eval(test_name, failure_tests);
  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

Stats TestHKL::TestRegion::check_region_paired(bool verbose) {
  Stats result;

  vector<string> files{
      "special", "basic-normal",
      //    "basic-mixed",
      //    "basic-pure",
      //      "strand-normal",
      //    "strand-mixed", "strand-pure",
  };

  for (const auto &name : files) {
    sstream message, failure;
    const auto &test_name = "HKL::Region "s + name;

    message << "\n~~~ Checking " << test_name << "\n";

    message << "\n>>> Constructing tests\n";

    ifstream input;
    string line;

    Files::open_file("test/input/" + name + ".tsv", input);

    string header;
    getline(input, header);

    auto header_splitted = StringDecompose::str_split(header, "\t");

    vector<string> functions{};
    std::copy(next(header_splitted.begin(), 3), header_splitted.end(),
              back_inserter(functions));

    vector<RegionPaired> special_tests{};

    while (getline(input, line)) {
      const auto splitted = StringDecompose::str_split(line, "\t");
      auto id = stoi(splitted[0]);
      auto ref = InputRegionArgs(splitted[1]);
      auto query = InputRegionArgs(splitted[2]);
      for (size_t i = 0; i < functions.size(); ++i)
        special_tests.emplace_back(
            InputPairRegionEval{id, ref, query, functions[i]}, splitted[i + 3]);
    }

    message << ">>> Constructed " << special_tests.size() << " tests\n";

    message << "\n>>> Testing:\n";

    Evaluator eval(test_name, special_tests);
    result(eval.verify());

    if (verbose)
      cout << message.str() << eval.message << "\n";
    else if (eval.hasFailed())
      cout << message.str() << eval.failed << "\n";

    cout << "~~~ " << gen_summary(eval.result, "Checking " + test_name) << endl;
  }

  return result;
}

AGizmo::Evaluation::Stats
TestHKL::TestRegion::check_region_resize(bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::Region::resize"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionResize> resize_tests{
      {{{":0"}, 1, -1, false}, ":0"},
      {{{":0"}, 1, -1, true}, ":0"},

      {{{":5"}, 0, 0, false}, ":5"},
      {{{":5"}, 0, 0, true}, ":5"},
      {{{":5"}, 5, 5, false}, ":1-10"},
      {{{":5"}, 5, 5, true}, ":1-10"},
      {{{":5"}, 5, 0, false}, ":1-5"},
      {{{":5"}, 5, 0, true}, ":1-5"},
      {{{":5"}, 0, 5, false}, ":5-10"},
      {{{":5"}, 0, 5, true}, ":5-10"},
      {{{":5"}, -5, -5, false}, ":0"},
      {{{":5"}, -5, -5, true}, ":0"},
      {{{":5"}, 0, -5, false}, ":0"},
      {{{":5"}, 0, -5, true}, ":0"},
      {{{":5"}, -5, 0, false}, ":0"},
      {{{":5"}, -5, 0, true}, ":0"},

      {{{":5/-"}, 0, 0, false}, ":5/-"},
      {{{":5/-"}, 0, 0, true}, ":5/-"},
      {{{":5/-"}, 5, 5, false}, ":1-10/-"},
      {{{":5/-"}, 5, 5, true}, ":1-10/-"},
      {{{":5/-"}, 5, 0, false}, ":1-5/-"},
      {{{":5/-"}, 5, 0, true}, ":5-10/-"},
      {{{":5/-"}, 0, 5, false}, ":5-10/-"},
      {{{":5/-"}, 0, 5, true}, ":1-5/-"},
      {{{":5/-"}, -5, -5, false}, ":0/-"},
      {{{":5/-"}, -5, -5, true}, ":0/-"},
      {{{":5/-"}, 0, -5, false}, ":0/-"},
      {{{":5/-"}, 0, -5, true}, ":0/-"},
      {{{":5/-"}, -5, 0, false}, ":0/-"},
      {{{":5/-"}, -5, 0, true}, ":0/-"},

      {{{":1-5"}, -4, 5, false}, ":5-10"},
      {{{":1-5"}, -4, 5, true}, ":5-10"},
      {{{":5-10"}, 5, -5, false}, ":1-5"},
      {{{":5-10"}, 5, -5, true}, ":1-5"},
      {{{":1-10"}, -4, -5, false}, ":5"},
      {{{":1-10"}, -4, -5, true}, ":5"},

      {{{":1-5/-"}, -4, 5, false}, ":5-10/-"},
      {{{":1-5/-"}, 5, -4, true}, ":5-10/-"},
      {{{":5-10/-"}, 5, -5, false}, ":1-5/-"},
      {{{":5-10/-"}, -5, 5, true}, ":1-5/-"},
      {{{":1-10/-"}, -4, -5, false}, ":5/-"},
      {{{":1-10/-"}, -5, -4, true}, ":5/-"},
  };

  Evaluator eval(test_name, resize_tests);
  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

AGizmo::Evaluation::Stats
TestHKL::TestRegion::check_region_slice(bool verbose) {
  sstream message;

  const auto &test_name = "HKL::Region::slice"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionSlice> tests{
      {{"", 0, 0}, "None"},       {{":0", 0, 0}, "None"},
      {{":1-10", 0, 0}, ":1-10"}, {{":1-10", 0, 2}, ":1-2"},
      {{":1-10", 2, 2}, ":3-4"},  {{":1-10", 2, 0}, ":3-10"},
      {{":1-10", 2, 9}, ":3-10"}, {{":1-10", 10, 0}, "None"},
  };

  Evaluator eval(test_name, tests);
  eval.verify();

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(eval.result, "Checking " + test_name) << endl;

  return eval.result;
}

TestHKL::TestRegion::RegionConstructors::RegionConstructors()
    : BaseTest({}, ":0") {
  validate();
}

TestHKL::TestRegion::RegionConstructors::RegionConstructors(
    InputRegionArgs input, string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegion::RegionFormation::RegionFormation(
    InputSingleRegionEval input, string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegion::RegionFailure::RegionFailure(InputRegionArgs input,
                                                  string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegion::RegionPaired::RegionPaired(InputPairRegionEval input,
                                                string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegion::RegionResize::RegionResize(InputRegionResize input,
                                                string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegion::RegionSlice::RegionSlice(InputRegionSlice input,
                                              string expected)
    : BaseTest(input, expected) {
  validate();
}
