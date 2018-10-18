#pragma once

#include "agizmo/evaluation.hpp"
#include "test_region.hpp"
#include "test_regionseq.hpp"

using namespace AGizmo::Evaluation;

namespace TestHKL {
int perform_tests(bool verbose);
}

//#include "lib/comtol.h"
//#include "lib/dogitoys.h"
//#include "lib/region.h"
//#include "lib/regionseq.h"
//#include "lib/regionset.h"
//#include "lib/sequence.h"

//#include <chrono>
//#include <exception>
//#include <iomanip>
//#include <iostream>
//#include <sstream>
//#include <string>
//#include <tuple>
//#include <vector>

// using namespace std::string_literals;
// using namespace HKL;
// using namespace ComTol;
// using namespace ComTol::StrTol;

// using std::cout;
// using std::endl;
// using std::pair;
// using std::size_t;
// using std::string;
// using std::to_string;
// using std::tuple;
// using std::vector;

// using sstream = std::stringstream;
// using pair_int = pair<int, int>;
// using pair_str = pair<string, string>;

// namespace General {

// struct Stats {
//  int total{0}, failed{0};

//  Stats &operator++() {
//    ++total;
//    return *this;
//  }

//  void operator()(const pair_int &stats) {
//    total += stats.first;
//    failed += stats.second;
//  }

//  void operator()(int t, int f) {
//    total += t;
//    failed += f;
//  }

//  void operator()(const Stats &input) {
//    total += input.total;
//    failed += input.failed;
//  }

//  void add_fail(int f = 1) { failed += f; }

//  pair_int get() const { return {total, failed}; }

//  double get_ratio() const { return failed / total; }

//  friend std::ostream &operator<<(ostream &stream, const Stats &stats) {
//    return stream << stats.total;
//  }
//};

// inline bool update_stats(const pair<int, int> &result, int &total,
//                         int &failed) {
//  total += result.first;
//  failed += result.second;
//  return failed;
//}

// const string passed_str("[ PASSED ]");
// const string failed_str("[~FAILED~]");

// string gen_framed(const string &message, size_t width = 80, char frame =
// '#'); string gen_pretty(const string &message, size_t width = 80); string
// gen_summary(int total, int failed, string type = "Evaluation"); inline string
// gen_summary(const Stats &stats, string type = "Evaluation") {
//  sstream message;

//  message << type;

//  if (stats.failed)
//    message << " failed in " << stats.failed << "/" << stats.total << " ("
//            << std::setprecision(5) << 100 * stats.get_ratio() << "%) cases!";
//  else
//    message << " succeeded in all " << stats.total << " cases!";

//  return message.str();
//}
// int perform_tests(bool verbose = false);
//} // namespace General

// namespace EvalComTol {

// template <typename It>
// inline pair_int test_XOR(It begin, It end, sstream &message) {
//  int total = 0, failed = 0;

//  for (; begin != end; ++begin) {
//    const auto [key, value] = *begin;
//    const auto result = ComTol::XOR(key.first, key.second);
//    message << ++total << ") XOR(" << to_string(key.first) << ","
//            << to_string(key.second) << ") -> " << to_string(result);

//    if (result != value) {
//      message << " != " << to_string(value) << "[FAILED]\n";
//      ++failed;
//    } else {
//      message << " == " << to_string(value) << "[PASSED]\n";
//    }
//  }

//  message << "\n";

//  return {total, failed};
//}

// pair_int eval_comtol(bool verbose = false);
//} // namespace EvalComTol

// namespace EvalRegion {

// template <typename T> string ntos(const optional<T> &result) {
//  if constexpr (std::is_same_v<Region, T>)
//    if (result.has_value())
//      return result.value().str();
//    else
//      return "None";
//  else if constexpr (std::is_same_v<pair<Region, Region>, T>)
//    if (result.has_value())
//      return (result.value().first.str() + ";" + result.value().second.str());
//    else
//      return "None";
//  else {
//    if (result.has_value())
//      return to_string(result.value());
//    else
//      return "None";
//  }
//}

//// BASIC EVALUATION
// pair_int check_basic(bool verbose = false);
//// CONSTRUCTOR AND EXCEPTION EVALUATION
// pair_int check_constructors(bool verbose = false);
//// FUNCTIONS EVALUATION
// pair_int check_functions(bool verbose = false);
//// BASIC OPERATIONS
// pair_int check_operations(bool verbose = false);
//// BENCHMARK
// pair_int benchmark_region(size_t base = 1000, bool verbose = true);
//// Wrapper function
// pair_int eval_region(bool verbose = false);
//} // namespace EvalRegion

// namespace EvalRegionSet {
// pair_int basic_check(bool verbose = false);
// pair_int count_check();
//} // namespace EvalRegionSet

// namespace EvalRegionSeq {
// using General::Stats;
// Stats check_basic(bool verbose = false);
// Stats check_get_seq(bool verbose = false);
// Stats check_read_fasta(bool verbose = false);

// Stats eval_regionseq(bool verbose = false);
//} // namespace EvalRegionSeq

// namespace EvalSeq {
// pair_int basic_check();
// void test();
//} // namespace EvalSeq

// namespace EvalDOGIToys {
// using General::Stats;
// Stats check_gff3_str_escape(bool verbose);
// Stats check_gff3_str_clean(bool verbose);
// Stats check_DOGI_basic(QString db_name, bool verbose);
// Stats check_DOGI_populate(QString db_name, QString config_file, bool
// verbose); Stats check_DOGI_hpa_expression(const QString &db_name,
//                                const QString expression, bool verbose);
// Stats check_Dogi_mapper_uniprot(const QString &db_name, const QString
// file_name,
//                                bool verbose);
// Stats check_DOGI_go_terms(const QString &db_name, const QString file_name,
//                          bool verbose);
// Stats check_DOGI_go_annotations(const QString &db_name, const QString
// file_name,
//                                bool verbose);

// Stats check_DOGI_gene_ontology(const QString &db_name, const QString
// go_terms,
//                               const QString &annotations, bool verbose);
// Stats check_DOGI_mapper(const QString &db_name, const QString &database_from,
//                        const QString &database_to, const QString &file_name,
//                        bool verbose);
// Stats check_DOGI_genome(const QString &db_name, const QString &id_database,
//                        const QString &masking, const QString &file_name,
//                        bool verbose);
// Stats check_DOGI_GVF(const QString &db_name, const QString &id_database,
//                     const QString &file_name, bool verbose);
// Stats check_DOGI_Regulatory(const QString &db_name, const QString
// &id_database,
//                            const QString &file_name, bool verbose);

// Stats eval_dogitoys(bool verbose = false);
//} // namespace EvalDOGIToys
