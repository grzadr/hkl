#include "test_regionseq.hpp"

AGizmo::Evaluation::Stats TestHKL::TestRegionSeq::check_basic(bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::RegionSeq::RegionSeq"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionSeqConstructors> constructors_tests{
      {{"", "", ""}, "IDX=;LEN=0;LOC=:0;SEQ="},
      {{"A", "", ""}, "IDX=A;LEN=0;LOC=:0;SEQ="},
      {{"A", "ATCG", ""}, "IDX=A;LEN=4;LOC=:1-4;SEQ=ATCG"},
      {{"A", "ATCG", ":10-13"}, "IDX=A;LEN=4;LOC=:10-13;SEQ=ATCG"},
  };

  Evaluator eval(test_name, constructors_tests);

  result(eval.verify());

  if (verbose)
    cout << message.str() << eval.message << "\n";
  else if (eval.hasFailed())
    cout << message.str() << eval.failed << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

AGizmo::Evaluation::Stats TestHKL::TestRegionSeq::check_get_seq(bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::RegionSeq::getSeq"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionSeqGetSeq> contructor_tests{
      {{{"TEST", "0123456789", ""}, 0}, "0"},
      {{{"TEST", "0123456789", ""}, -10}, "0"},
      {{{"TEST", "0123456789", ""}, 10}, "ooferror"},
      {{{"TEST", "0123456789", ""}, 9}, "9"},
      {{{"TEST", "0123456789", ""}, -1}, "9"},

      {{{"TEST", "0123456789", ""}, pair<int, size_t>{0, 2}}, "01"},
      {{{"TEST", "0123456789", ""}, pair<int, size_t>{8, 2}}, "89"},
      {{{"TEST", "0123456789", ""}, pair<int, size_t>{8, 0}}, "89"},
      {{{"TEST", "0123456789", ""}, pair<int, size_t>{8, 3}}, "89"},
      {{{"TEST", "0123456789", ""}, pair<int, size_t>{10, 3}}, ""},

      {{{"TEST", "0123456789", ""}, ":1"}, "0"},
      {{{"TEST", "0123456789", ""}, ":1-2"}, "01"},
      {{{"TEST", "0123456789", ""}, ":9-10"}, "89"},
      {{{"TEST", "0123456789", ""}, ":9-11"}, "89"},
      {{{"TEST", "0123456789", ""}, ":11"}, ""},

      {{{"TEST", "0123456789", ""}, "1:1"}, "0"},
      {{{"TEST", "0123456789", ""}, "1:1-2"}, "01"},
      {{{"TEST", "0123456789", ""}, "1:9-10"}, "89"},
      {{{"TEST", "0123456789", ""}, "1:9-11"}, "89"},
      {{{"TEST", "0123456789", ""}, "1:11"}, ""},

      {{{"TEST", "0123456789", "1:10-19"}, ":10"}, "0"},
      {{{"TEST", "0123456789", "1:10-19"}, ":10-11"}, "01"},
      {{{"TEST", "0123456789", "1:10-19"}, ":18-19"}, "89"},
      {{{"TEST", "0123456789", "1:10-19"}, ":18-20"}, "89"},
      {{{"TEST", "0123456789", "1:10-19"}, ":20"}, ""},

      {{{"TEST", "0123456789", "1:10-19"}, "1:10"}, "0"},
      {{{"TEST", "0123456789", "1:10-19"}, "1:10-11"}, "01"},
      {{{"TEST", "0123456789", "1:10-19"}, "1:18-19"}, "89"},
      {{{"TEST", "0123456789", "1:10-19"}, "1:18-20"}, "89"},
      {{{"TEST", "0123456789", "1:10-19"}, "1:20"}, ""},
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

AGizmo::Evaluation::Stats TestHKL::TestRegionSeq::check_fasta_reader(
    bool verbose) {
  Stats result;

  sstream message;

  const auto &test_name = "HKL::RegionSeq::getSeq"s;

  message << "\n~~~ Checking " << test_name << "\n";

  ++result;

  FASTAReader parser{"test/input/sequences.fa"};

  const auto reads = parser.readFile(true);

  result.addFailure(reads != vector<RegionSeq>{
                                 RegionSeq("SEQ1"),
                                 RegionSeq("SEQ2", "AAAATTTT"),
                                 RegionSeq("SEQ3"),
                                 RegionSeq("SEQ4", "GGGGCCCC"),
                             });

  parser.open("test/input/sequences.fa");

  for (const auto &ele : reads) {
    ++result;
    if (const auto &seq = parser(true)) result.addFailure(*seq != ele);
  }

  if (verbose || result.hasFailed()) cout << message.str() << "\n";

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

TestHKL::TestRegionSeq::RegionSeqConstructors::RegionSeqConstructors(
    InputRegionSeq input, string expected)
    : BaseTest(input, expected) {
  validate();
}

TestHKL::TestRegionSeq::RegionSeqGetSeq::RegionSeqGetSeq(
    InputRegionSeqGetSeq input, string expected)
    : BaseTest(input, expected) {
  validate();
}
