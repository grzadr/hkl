#include "test.h"

#include <fstream>

using std::getline;
using std::ifstream;
using std::stringstream;
using std::to_string;

using namespace General;
using namespace EvalRegion;

void open_file(string file_name, string &line, std::ifstream &input,
               vector<string> &header) {
  input = std::ifstream(file_name);

  if (!input.is_open())
    throw std::runtime_error{"Can't open \"" + file_name + "\"\n"};

  getline(input, line);
  header = str_split(line);
}

string get_result(const Region &ref, const Region &query, string test) {

  if (test == "getChrom")
    return ref.getChrom();
  else if (test == "getFirst")
    return to_string(ref.getFirst());
  else if (test == "getCenter")
    return to_string(ref.getCenter());
  else if (test == "getLast")
    return to_string(ref.getLast());
  else if (test == "getStrand")
    return string(1, ref.getStrand() ? ref.getStrand() : '0');
  else if (test == "getRange") {
    int first, last;
    std::tie(first, last) = ref.getRange();
    return to_string(first) + ";" + to_string(last);
  } else if (test == "getLength")
    return to_string(ref.getLength());
  else if (test == "isEmpty")
    return to_string(ref.isEmpty());
  else if (test == "isPos")
    return to_string(ref.isPos());
  else if (test == "isRange")
    return to_string(ref.isRange());
  else if (test == "Ope=")
    return to_string(ref == query);
  else if (test == "Ope>")
    return to_string(ref > query);
  else if (test == "Ope<")
    return to_string(ref < query);
  else if (test == "sameChrom")
    return to_string(ref.sameChrom(query));
  else if (test == "sameRange")
    return to_string(ref.sameRange(query));
  else if (test == "sameStrand")
    return to_string(ref.sameStrand(query));
  else if (test == "sharesChrom")
    return to_string(ref.sharesChrom(query));
  else if (test == "sharesRange")
    return to_string(ref.sharesRange(query));
  else if (test == "sharesStrand")
    return to_string(ref.sharesStrand(query));
  else if (test == "shares")
    return to_string(ref.shares(query));
  else if (test == "around")
    return to_string(ref.around(query));
  else if (test == "next")
    return to_string(ref.next(query));
  else if (test == "inside")
    return to_string(ref.inside(query));
  else if (test == "covers")
    return to_string(ref.covers(query));
  else if (test == "upstream")
    return to_string(ref.upstream(query));
  else if (test == "downstream")
    return to_string(ref.downstream(query));
  else if (test == "getDist")
    return ntos(ref.dist(query));
  else if (test == "getDistS")
    return ntos(ref.dist(query, true));
  else if (test == "getRelPosFirst")
    return ntos(ref.getPosRel(query, ref.getFirst()));
  else if (test == "getRelPosRatioFirst")
    return ntos(ref.getPosRelRatio(query, ref.getFirst()));
  else if (test == "getRelPosFirstS")
    return ntos(ref.getPosRel(query, ref.getFirst(), true));
  else if (test == "getRelPosRatioFirstS")
    return ntos(ref.getPosRelRatio(query, ref.getFirst(), true));
  else if (test == "getRelPosCenter")
    return ntos(ref.getPosRel(query, ref.getCenter()));
  else if (test == "getRelPosRatioCenter")
    return ntos(ref.getPosRelRatio(query, ref.getCenter()));
  else if (test == "getRelPosCenterS")
    return ntos(ref.getPosRel(query, ref.getCenter(), true));
  else if (test == "getRelPosRatioCenterS")
    return ntos(ref.getPosRelRatio(query, ref.getCenter(), true));
  else if (test == "getRelPosLast")
    return ntos(ref.getPosRel(query, ref.getLast()));
  else if (test == "getRelPosRatioLast")
    return ntos(ref.getPosRelRatio(query, ref.getLast()));
  else if (test == "getRelPosLastS")
    return ntos(ref.getPosRel(query, ref.getLast(), true));
  else if (test == "getRelPosRatioLastS")
    return ntos(ref.getPosRelRatio(query, ref.getLast(), true));
  else if (test == "getRelPosFirstS")
    return ntos(ref.getPosRel(query, ref.getFirst(), true));
  else if (test == "getRelPosRatioFirstS")
    return ntos(ref.getPosRelRatio(query, ref.getFirst(), true));
  else if (test == "getRelPosCenter")
    return ntos(ref.getPosRel(query, ref.getCenter()));
  else if (test == "getRelPosRatioCenter")
    return ntos(ref.getPosRelRatio(query, ref.getCenter()));
  else if (test == "getRelPosCenterS")
    return ntos(ref.getPosRel(query, ref.getCenter(), true));
  else if (test == "getRelPosRatioCenterS")
    return ntos(ref.getPosRelRatio(query, ref.getCenter(), true));
  else if (test == "getRelPosLast")
    return ntos(ref.getPosRel(query, ref.getLast()));
  else if (test == "getRelPosRatioLast")
    return ntos(ref.getPosRelRatio(query, ref.getLast()));
  else if (test == "getRelPosLastS")
    return ntos(ref.getPosRel(query, ref.getLast(), true));
  else if (test == "getRelPosRatioLastS")
    return ntos(ref.getPosRelRatio(query, ref.getLast(), true));
  else if (test == "getShared")
    return ntos(ref.getShared(query));
  else if (test == "getDiff")
    return ntos(ref.getDiff(query));
  else if (test == "getUnion")
    return ntos(ref.getUnion(query));
  else if (test == "getGap")
    return ntos(ref.getGap(query));
  else if (test == "getSharedLength")
    return ntos(ref.getSharedLength(query));
  else if (test == "getCoveredRate")
    return ntos(ref.getCoveredRate(query));
  else if (test == "getCapturedRate")
    return ntos(ref.getCapturedRate(query));
  else if (test == "toStr")
    return ref.str();
  else {
    throw std::runtime_error(test + " is not supported");
  }
}

pair_int test_queries(const vector<pair_str> &queries, size_t &counter,
                      bool verbose) {

  int total{0}, failed{0};
  Region ref, query;
  string id;

  for (auto & [ func, expected ] : queries) {
    if (func == "ID")
      id = expected;
    else if (func == "Error" || func == "Chrom" || func == "First" ||
             func == "Last" || func == "Strand")
      continue;
    else if (func == "Reference")
      ref = Region(expected);
    else if (func == "Query")
      query = Region(expected);
    else if (func == "Expected")
      ref = Region(expected);
    else {
      ++total;
      ++counter;
      auto result(get_result(ref, query, func));
      auto passed = bool(expected == result);
      failed += !passed;
      if (!passed || verbose)
        cout << "Test: " << std::setw(4) << counter
             << "| Pair: " << std::setw(4) << id << "| Ref: " << std::setw(10)
             << ref.str() << "| Query:" << std::setw(10) << query.str()
             << "| Func: " << std::setw(22) << func
             << "| Result: " << std::setw(15) << result
             << "| Expected: " << std::setw(15) << expected
             << (!passed ? "| [FAILED]" : "| [PASSED]") << endl;
    }
  }

  return {total, failed};
}

pair_int EvalRegion::check_functions(bool verbose) {
  cout << "~~~ Checking functions with two Regions" << endl;
  int total{0};
  int failed{0};
  const auto tests = vector<pair<string, string>>{
      {"Materials/special.tsv", "Special"},
      {"Materials/basic-normal.tsv", "Normal Basic"},
      {"Materials/basic-mixed.tsv", "Mixed Basic"},
      {"Materials/basic-pure.tsv", "Pure Basic"},
      {"Materials/strand-normal.tsv", "Normal Strand"},
      {"Materials/strand-mixed.tsv", "Mixed Strand"},
      {"Materials/strand-pure.tsv", "Pure Strand"},
      //        {"extra.tsv", "Extra"},
  };

  for (const auto & [ file_name, caption ] : tests) {
    cout << "~~~~~~ Verifying " << caption << " Cases" << endl;
    int local_total{0}, local_failed{0};
    string line;
    size_t line_num{0};
    size_t counter{0};
    ifstream input;
    vector<string> header{};
    open_file(file_name, line, input, header);

    while (getline(input, line)) {
      ++line_num;
      auto splitted = str_split(line);
      auto queries = pairify<string, string>(header.begin(), header.end(),
                                             splitted.begin(), splitted.end());
      General::update_stats(test_queries(queries, counter, verbose),
                            local_total, local_failed);
    }

    input.close();

    total += local_total;
    failed += local_failed;

    cout << "~~~~~~ " << gen_summary(local_total, local_failed, "Verification")
         << "\n";
  }

  cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;

  return {total, failed};
}

int eval_wrong_constructor(const vector<pair_str> &tasks, size_t &counter,
                           bool verbose = false) {
  string message, id{tasks.at(0).second}, error_name{tasks.at(1).second},
      chrom{tasks.at(2).second}, first{tasks.at(3).second},
      last{tasks.at(4).second}, strand{tasks.at(5).second};

  message += "#### Test " + id + ":\n    Query: \"" + chrom + "\", " + first +
             ", " + last + ", \"" + strand + "\"\n" +
             "   Expecting: " + error_name + "\n";

  try {
    Region result;
    if (first == "NA")
      result = Region(chrom);
    else if (last != "NA")
      result = Region(chrom, stoi(first), stoi(last),
                      (strand == "NA" ? "" : strand));
    else
      result = Region(chrom, stoi(first), (strand == "NA" ? "" : strand));

    cout << message << "\t>>>> Did not throw\n#### Failed!\n" << endl;
    return 1;
  } catch (const RegionError &ex) {
    message +=
        "    Error message:\n" + string(45, '-') + ex.what() + string(45, '-');
    if (error_name != ex.getName()) {
      cout << message << "\n>>>> Wrong message thrown\n#### Failed!\n" << endl;
      return 1;
    } else {
      message += "\n#### Passed!\n";
    }
  }

  if (verbose)
    cout << message << endl;

  ++counter;

  return 0;
}

pair_int eval_proper_constructor(const vector<pair_str> &tasks, size_t &counter,
                                 bool verbose = false) {
  int total{0}, failed{0};

  string message, id{tasks.at(0).second}, chrom{tasks.at(2).second},
      first{tasks.at(3).second}, last{tasks.at(4).second},
      strand{tasks.at(5).second}, expected{tasks.at(6).second};

  message += "#### Test " + id + ":\n    Query: \"" + chrom + "\", " + first +
             ", " + last + ", \"" + strand + "\"\n" +
             "    Expecting: " + expected + "\n";

  try {
    Region result;
    if (first == "NA")
      result = Region(chrom);
    else if (last != "NA")
      result = Region(chrom, stoi(first), stoi(last),
                      (strand == "NA" ? "" : strand));
    else
      result = Region(chrom, stoi(first), (strand == "NA" ? "" : strand));

    message += "\t  Result: " + result.str() + "\n";
    if (result.str() == expected)
      message += "\tMatching: True\n#### Passed!\n";
    else {
      cout << message << "\tMatching: False\n#### Failed!\n" << endl;
      return {tasks.size() - 6, tasks.size() - 6};
    }
    ++total;
    update_stats(test_queries(tasks, counter, verbose), total, failed);
  } catch (const RegionError &ex) {
    cout << ex.what() << endl;
    cout << message << "\n#### Failed!\n";
    return {tasks.size() - 6, tasks.size() - 6};
  }
  if (verbose)
    cout << message << endl;
  return {total, failed};
}

pair_int EvalRegion::check_constructors(bool verbose) {
  cout << "~~~ Checking constructors and getters" << endl;

  int total{0}, failed{0};

  auto update{[&total, &failed](pair<int, int> result) {
    total += result.first;
    failed += result.second;
  }};

  string line;
  ifstream input;
  vector<string> header{};
  open_file("Materials/constructors.tsv", line, input, header);
  size_t counter{0};

  while (getline(input, line)) {
    auto splitted = str_split(line);
    if (splitted.at(1) == "NA")
      update(eval_proper_constructor(
          pairify<string, string>(header.begin(), header.end(),
                                  splitted.begin(), splitted.end()),
          counter, verbose));
    else {
      ++total;
      failed += eval_wrong_constructor(
          pairify<string, string>(header.begin(), header.end(),
                                  splitted.begin(), splitted.end()),
          counter, verbose);
    }
  }

  input.close();

  cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;

  return {total, failed};
}

pair_int eval_state(bool verbose) {
  int total{0}, failed{0};
  string message;

  if (verbose)
    cout << message << endl;

  return {total, failed};
}

pair_int eval_resize(bool verbose) {
  int total{0}, failed{0};
  string message;

  message += "\n>>> Checking resize capabilities:\n";

  const vector<tuple<pair<string, string>, vector<int>>> query_resize{
      {{":0", ":0"}, {1, -1, 0}},

      {{":5", ":5"}, {0, 0, 0}},           {{":5", ":1-10"}, {5, 5, 0}},
      {{":5", ":5-10"}, {0, 5, 0}},        {{":5", ":1-5"}, {5, 0, 0}},
      {{":5", ":0"}, {-5, -5, 0}},         {{":5", ":0"}, {0, -5, 0}},
      {{":5", ":0"}, {-5, 0, 0}},

      {{":5/-", ":5/-"}, {0, 0, 1}},       {{":5/-", ":1-10/-"}, {5, 5, 1}},
      {{":5/-", ":1-5/-"}, {0, 5, 1}},     {{":5/-", ":5-10/-"}, {5, 0, 1}},
      {{":5/-", ":0/-"}, {-5, -5, 1}},     {{":5/-", ":0/-"}, {0, -5, 1}},
      {{":5/-", ":0/-"}, {-5, 0, 1}},

      {{":1-5", ":5-10"}, {-4, 5, 0}},     {{":5-10", ":1-5"}, {5, -5, 0}},
      {{":1-10", ":5"}, {-4, -5, 0}},

      {{":5-10/-", ":1-5/-"}, {-5, 4, 1}}, {{":5-10/-", ":1-5/-"}, {-5, 5, 1}},
      {{":1-10/-", ":5/-"}, {-5, -4, 1}},
  };

  for (auto & [ query, extent ] : query_resize) {
    ++total;
    stringstream outcome;
    auto subject = Region(query.first);
    auto result = Region(query.second);

    outcome << "Test " << total << ") {" << subject << "}.resize("
            << extent.at(0) << ", " << extent.at(1) << ", " << extent.at(2)
            << ")";

    subject.resize(extent.at(0), extent.at(1), extent.at(2));
    auto passed = (subject == result);

    outcome << " -> " << subject;

    if (passed) {
      outcome << " [VALID] State: ";
      auto sub_state = subject.getLength() < 2 ? subject.getLength() : 2;
      auto res_state = result.getLength() < 2 ? result.getLength() : 2;

      passed = (sub_state == res_state);

      if (!passed) {
        failed++;
        outcome << sub_state << " != " << res_state << " [MISMATCH]\n";
      } else
        outcome << sub_state << " [MATCH]\n";
    } else {
      failed++;
      outcome << " != " << result << " [INVALID]\n";
    }

    if (verbose or !passed)
      message += outcome.str();
  }

  if (verbose or failed)
    cout << message << ">>> " << string(64, '-') << "\n" << endl;

  return {total, failed};
}

pair_int EvalRegion::check_operations(bool verbose) {
  cout << "### Checking various Region's functions" << endl;

  int total{0}, failed{0};

  update_stats(eval_resize(verbose), total, failed);

  cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;

  return {total, failed};
}

pair_int EvalRegion::benchmark_region(size_t base, bool verbose) {

  vector<pair<int, int>> ranges{};
  auto ref = Region("A", 50, 100);
  std::stringstream result;
  std::chrono::time_point<std::chrono::steady_clock> start, end;

  if (verbose) {
    cout << "#### Benchmarking\nGenerating ranges..." << endl;
  }

  for (size_t i = 1; i < base + 1; ++i) {
    for (size_t j = i; j < base + 1; ++j) {
      ranges.push_back({i, j});
    }
  }

  if (verbose) {
    cout << "Done\n" << endl;
    cout << "Iterating threw ..." << endl;
  }

  start = std::chrono::steady_clock::now();

  for (auto[first, last] : ranges) {
    auto reg = Region("A", first, last);
    result << reg.str();
    result << reg.getLength();
    result << std::hash<Region>{}(reg);
    result << reg.getChrom();
    result << reg.getFirst();
    result << reg.getLast();

    result << reg.isPos();

    result << reg.shares(ref);
    result << reg.next(ref);
    result << reg.covers(ref);
    result << reg.inside(ref);

    result << reg.dist(ref, false).value();
    result << reg.dist(ref, true).value();

    result << reg.getShared(ref).value();
    result << reg.getSharedLength(ref).value();
    result << reg.getGap(ref).value();
    result << reg.getDiff(ref).value().first;
    result << reg.getCoveredRate(ref).value();
    result << reg.getCapturedRate(ref).value();
    result << "\n";
  }

  end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  if (verbose)
    cout << "Generated string " << result.str().size() << " chars long!"
         << endl;

  cout << "### Completed " << 0.5 * base * (base + 1) << " in "
       << elapsed_seconds.count() << "s\n"
       << endl;

  return {1, 0};
}

pair_int EvalRegion::check_basic(bool verbose) {
  cout << "~~~ Checking basic functionality" << endl;

  int total = 0, failed = 0;

  stringstream message;

  bool passed{false};

  int width = 40;

  try {
    ++total;
    message << std::setw(width) << "Region() == \":0\"";
    passed = Region().str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(:0) == \":0\"";
    passed = Region(":0").str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A:1-2/+\") == \"A:1-2/+\"";
    passed = Region("A:1-2/+").str() == "A:1-2/+";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"\", 0, 0, 0) == \":0\"";
    passed = Region("", 0, 0, 0).str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A\", 1, 2, '+') == \"A:1-2/+\"";
    passed = Region("A", 1, 2, '+').str() == "A:1-2/+";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"\", 0, 0, \"\") == \":0\"";
    passed = Region("", 0, 0, "").str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A\", 1, 2, \"+\") == \"A:1-2/+\"";
    passed = Region("A", 1, 2, "+").str() == "A:1-2/+";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"\", 0, 0) == \":0\"";
    passed = Region("", 0, 0).str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A\", 1, 2) == \"A:1-2\"";
    passed = Region("A", 1, 2).str() == "A:1-2";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(0, 0) == \":0\"";
    passed = Region(0, 0).str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(1, 2) == \":1-2\"";
    passed = Region(1, 2).str() == ":1-2";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(0) == \":0\"";
    passed = Region("").str() == ":0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A\") == \"A:0\"";
    passed = Region("A").str() == "A:0";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"\", \"+\") == \":0/+\"";
    passed = Region("", "+").str() == ":0/+";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

    ++total;
    message << std::setw(width) << "Region(\"A\", \"+\") == \"A:0/+\"";
    passed = Region("A", "+").str() == "A:0/+";
    if (passed)
      message << " [VALID]\n";
    else
      message << " [INVALID] - Wrong format: " << Region().str() << "\n";
    failed += !passed;

  } catch (const RegionError &ex) {
    message << " [INVALID]" << ex.what() << "\n";
    failed = total;
  }

  if (failed or verbose)
    cout << message.str();

  cout << "~~~ " << gen_summary(total, failed, "Check") << "\n" << endl;

  return {total, failed};
}

pair_int EvalRegion::eval_region(bool verbose) {
  cout << gen_framed("Evaluating Region Class") << "\n\n";

  int total = 0, failed = 0;

  update_stats(check_basic(verbose), total, failed);
  update_stats(check_constructors(verbose), total, failed);
  update_stats(check_functions(verbose), total, failed);
  //    update_stats(check_functions(functions, true), total, failed);

  cout << gen_framed("Evaluation Completed") << "\n\n";

  return {total, failed};
}
