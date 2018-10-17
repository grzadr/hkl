#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "agizmo/evaluation.hpp"
#include "agizmo/files.hpp"
#include "agizmo/strings.hpp"
#include "hkl/region.hpp"

namespace TestHKL::TestRegion {
using namespace AGizmo;
using namespace Evaluation;

using std::ifstream;
using sstream = std::stringstream;
using std::quoted;
using std::string;
using std::to_string;

using HKL::Region;
using HKL::RegionError;

class InputRegionArgs {
 private:
  opt_int first, last;
  opt_str chrom, strand;
  //  int first{-1}, last{-1};
  //  string chrom{}, strand{};

  enum class ConstructorMode {
    Default = 0,
    SingleString,
    Position,
    Range,
    ChromWithStrand,
    ChromWithPosition,
    ChromWithRange,
  };

  ConstructorMode mode{};

 public:
  InputRegionArgs() = default;
  InputRegionArgs(string query) {
    if (query != "NA") {
      this->chrom = query;
      this->mode = ConstructorMode::SingleString;
    } else
      this->mode = ConstructorMode::Default;
  }

  InputRegionArgs(string chrom, string first, string last, string strand) {
    if (chrom != "NA") this->chrom = chrom;
    if (strand != "NA") this->strand = strand;
    this->first = StringFormat::str_to_int(first, true);
    this->last = StringFormat::str_to_int(last, true);

    if (this->chrom) {
      if (this->first) {
        if (this->last)
          this->mode = ConstructorMode::ChromWithRange;
        else
          this->mode = ConstructorMode::ChromWithPosition;
      } else {
        if (this->strand)
          this->mode = ConstructorMode::ChromWithStrand;
        else
          this->mode = ConstructorMode::SingleString;
      }
    } else {
      if (this->first) {
        if (this->last)
          this->mode = ConstructorMode::Range;
        else
          this->mode = ConstructorMode::Position;
      } else {
        if (this->strand)
          this->mode = ConstructorMode::ChromWithStrand;
        else
          this->mode = ConstructorMode::Default;
      }
    }
  }

  string str() const {
    sstream message;
    switch (mode) {
      case ConstructorMode::Default:
        break;
      case ConstructorMode::SingleString:
        message << quoted(chrom.value_or(""));
        break;
      case ConstructorMode::ChromWithStrand:
        message << quoted(chrom.value_or("")) << ", "
                << quoted(strand.value_or(""));
        break;
      case ConstructorMode::Position:
        message << *first << ", " << quoted(strand.value_or(""));
        break;
      case ConstructorMode::Range:
        message << *first << ", " << *last << ", "
                << quoted(strand.value_or(""));
        break;
      case ConstructorMode::ChromWithPosition:
        message << quoted(chrom.value_or("")) << ", " << *first << ", "
                << quoted(strand.value_or(""));
        break;
      case ConstructorMode::ChromWithRange:
        message << quoted(chrom.value_or("")) << ", " << *first << ", " << *last
                << ", " << quoted(strand.value_or(""));
        break;
    }

    return message.str();
  }

  Region construct() const {
    switch (mode) {
      case ConstructorMode::Default:
        return Region();
      case ConstructorMode::SingleString:
        return Region(*chrom);
      case ConstructorMode::ChromWithStrand:
        return Region(chrom.value_or(""), strand.value_or(""));
      case ConstructorMode::Position:
        return Region(*first, strand.value_or(""));
      case ConstructorMode::Range:
        return Region(*first, *last, strand.value_or(""));
      case ConstructorMode::ChromWithPosition:
        return Region(chrom.value_or(""), *first, strand.value_or(""));
      case ConstructorMode::ChromWithRange:
        return Region(chrom.value_or(""), *first, *last, strand.value_or(""));
    }
  }
};

class RegionConstructors : public BaseTest<InputRegionArgs, string> {
 public:
  RegionConstructors();
  RegionConstructors(InputRegionArgs input, string expected);

  string args() const { return "(" + this->input.str() + ")"; }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    this->outcome = input.construct().str();
    return this->setStatus(outcome == expected);
  }
};

class InputSingleRegionEval {
 private:
  int id;
  InputRegionArgs args;
  string function;

 public:
  InputSingleRegionEval() = default;
  InputSingleRegionEval(int id, InputRegionArgs args, string function)
      : id{id}, args{args}, function{function} {}

  Region construct() const { return args.construct(); }

  string execute() const {
    const auto region = args.construct();
    if (function.empty())
      return region.str();
    else if (function == "getChrom")
      return region.getChrom();
    else if (function == "getFirst")
      return to_string(region.getFirst());
    else if (function == "getCenter")
      return to_string(region.getCenter());
    else if (function == "getLast")
      return to_string(region.getLast());
    else if (function == "getStrand")
      return string(1, region.getStrand() ? region.getStrand() : '0');
    else if (function == "getRange") {
      const auto [first, second] = region.getRange();
      return to_string(first) + ";" + to_string(second);
    } else if (function == "getLength")
      return to_string(region.getLength());
    else if (function == "isEmpty")
      return to_string(region.isEmpty());
    else if (function == "isPos")
      return to_string(region.isPos());
    else if (function == "isRange")
      return to_string(region.isRange());
    else
      throw runtime_error{"Function '" + function + "' is not supported!"};
  }

  string str() const {
    sstream result;
    result << " " << id << " (" << args.str() << ")";
    if (!function.empty()) result << "." << function << "()";
    return result.str();
  }
};

class RegionFormation : public BaseTest<InputSingleRegionEval, string> {
 public:
  RegionFormation() = default;
  RegionFormation(InputSingleRegionEval input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    this->outcome = input.execute();
    return this->setStatus(outcome == expected);
  }
};

class RegionFailure : public BaseTest<InputRegionArgs, string> {
 public:
  RegionFailure();
  RegionFailure(InputRegionArgs input, string expected);

  string args() const { return "(" + this->input.str() + ")"; }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    try {
      this->outcome = input.construct().str();
    } catch (const RegionError &ex) {
      this->outcome = ex.getName();
    }
    return this->setStatus(outcome == expected);
  }
};

Stats check_region_constructors(bool verbose);
Stats check_region_formation(bool verbose);
Stats check_region_failure(bool verbose);
Stats check_region_special(bool verbose);
}  // namespace TestHKL::TestRegion
