#include "test_regionseq.hpp"

// General::Stats EvalRegionSeq::check_get_seq(bool verbose) {
//  Stats stats;

//  sstream message;

//  int width = 67;

//  cout << "~~~ Checking Sequence Getters" << endl;

//  auto test = RegionSeq();

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting first character";

//  if (test.at(0) == '0')
//    message << passed_str << "\n";
//  else {
//    cout << test.at(0) << endl;
//    message << failed_str << "\n";
//    ++stats.failed;
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting last character";

//  if (test.at(9) == '9')
//    message << passed_str << "\n";
//  else {
//    cout << test.at(0) << endl;
//    message << failed_str << "\n";
//    ++stats.failed;
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting character out of scope";

//  try {
//    test.at(10);
//    ++stats.failed;
//    message << failed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    message << passed_str << "\n";
//    message << "Error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting First Character By Region";

//  if (test.at(Region("1", 20)) == '0')
//    message << passed_str << "\n";
//  else {
//    cout << test.at(0) << endl;
//    message << failed_str << "\n";
//    ++stats.failed;
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Last Character By Region";

//  if (test.at(Region("1", 29)) == '9')
//    message << passed_str << "\n";
//  else {
//    cout << test.at(0) << endl;
//    message << failed_str << "\n";
//    ++stats.failed;
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Character Out Of Region";

//  try {
//    test.at(Region("1", 30));
//    ++stats.failed;
//    message << failed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    message << passed_str << "\n";
//    message << "Error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Character with wider Region";

//  try {
//    test.at(Region("1", 30, 31));
//    ++stats.failed;
//    message << failed_str << "\n";
//  } catch (const std::runtime_error &ex) {
//    message << passed_str << "\n";
//    message << "Error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Sequence Slice";

//  try {
//    if (const auto &slice = test.getSeqSlice(2, 3); slice != "234") {
//      ++stats.failed;
//      message << failed_str << "\n";
//    } else
//      message << passed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    ++stats.failed;
//    message << failed_str << "\n";
//    message << "Generated error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Sequence Slice with Region";

//  try {
//    if (const auto &slice = test.getSeqSlice(Region(22, 24)); *slice != "234")
//    {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << std::quoted(*slice) << endl;
//    } else
//      message << passed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    ++stats.failed;
//    message << failed_str << "\n";
//    message << "Generated error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Sequence Slice with Region";

//  try {
//    if (const auto &slice = test.getSlice(Region(22, 24)); *slice != "234") {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << *slice << endl;
//    } else
//      message << passed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    ++stats.failed;
//    message << failed_str << "\n";
//    message << "Generated error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Sequence Slice with Chunk of Region";

//  try {
//    if (const auto &slice = test.getSlice(Region(19, 24)); *slice != "01234")
//    {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << *slice << endl;
//    } else if (const auto &slice = test.getSlice(Region(25, 30));
//               *slice != "56789") {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << *slice << endl;
//    } else
//      message << passed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    ++stats.failed;
//    message << failed_str << "\n";
//    message << "Generated error message:\n" << ex.what() << "\n";
//  }

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Getting Sequence Slice with Wrong Region";

//  try {
//    if (const auto &slice = test.getSlice(Region(19, 19))) {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << *slice << endl;
//    } else if (const auto &slice = test.getSlice(Region(30, 30))) {
//      cout << *slice << endl;
//      ++stats.failed;
//      message << failed_str << "\n";
//      cout << *slice << endl;
//    } else
//      message << passed_str << "\n";
//  } catch (const std::out_of_range &ex) {
//    ++stats.failed;
//    message << failed_str << "\n";
//    message << "Generated error message:\n" << ex.what() << "\n";
//  }

//  test = RegionSeq("TEST", "ATCGATCGC");

//  if (test.countGC() != 5 || test.calcGCRatio() != 0.5) {
//    ++stats.failed;
//    message << failed_str << "\n";
//  } else
//    message << passed_str << "\n";

//  if (verbose or stats.failed)
//    cout << message.str() << endl;

//  cout << "~~~ " << gen_summary(stats, "Check") << "\n" << endl;

//  return stats;
//}

// General::Stats EvalRegionSeq::check_read_fasta(bool verbose) {
//  Stats stats;

//  sstream message;

//  int width = 67;

//  cout << "~~~ Checking FASTA Reader" << endl;

//  message << std::right << setw(3) << ++stats << std::left << std::setw(width)
//          << ") Reading FASTA file";

//  const auto result = FASTAReader::readFASTAFile("Materials/test.fa", true);

//  if (result == vector<RegionSeq>{
//                    RegionSeq("SEQ1"),
//                    RegionSeq("SEQ2", "AAAATTTT"),
//                    RegionSeq("SEQ3"),
//                    RegionSeq("SEQ4", "GGGGCCCC"),
//                })
//    message << passed_str << "\n";
//  else {
//    ++stats.failed;
//    message << failed_str << "\n";
//  }

//  FASTAReader parser{"Materials/test.fa"};

//  for (const auto &ele : result) {
//    cout << ele << endl;
//    if (const auto &seq = parser.readFASTASeq(true))
//      cout << (ele == *seq) << endl;
//  }

//  if (verbose or stats.failed)
//    cout << message.str() << endl;

//  cout << "~~~ " << gen_summary(stats, "Check") << "\n" << endl;

//  return stats;
//}

AGizmo::Evaluation::Stats TestHKL::TestRegionSeq::check_basic(bool verbose) {
  Stats result;

  sstream message, failure;

  const auto &test_name = "HKL::RegionSeq::RegionSeq"s;

  message << "\n~~~ Checking " << test_name << "\n";

  vector<RegionSeqConstructors> constructors_tests{
      {{"", "", ""}, "IDX=;LEN=0;LOC=:0;SEQ="},
      {{"A", "", ""}, "IDX=A;LEN=0;LOC=:0;SEQ="},
      {{"A", "ATCG", ""}, "IDX=A;LEN=4;LOC=:1-4;SEQ=ATCG"},
      {{"A", "ATCG", ":10-13"}, "IDX=A;LEN=4;LOC=:10-13;SEQ=ATCG"},
  };

  Evaluator eval(test_name, constructors_tests);

  result(eval.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

  cout << "~~~ " << gen_summary(result, "Checking " + test_name) << endl;

  return result;
}

AGizmo::Evaluation::Stats TestHKL::TestRegionSeq::check_get_seq(bool verbose) {
  Stats result;

  sstream message, failure;

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

  result(eval.verify(message, failure));

  message << "\n";

  if (verbose)
    cout << message.str();
  else if (result.hasFailed())
    cout << failure.str();

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
