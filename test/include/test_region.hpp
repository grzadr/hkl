#pragma once

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
using std::string;
using std::to_string;

using HKL::Region;
using HKL::RegionError;

class InputRegionArgs {
private:
  int first{-1}, last{-1};
  string chrom{}, strand{};

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
    if (!query.empty() && query != "NA") {
      this->chrom = query;
      this->mode = ConstructorMode::SingleString;
    } else
      this->mode = ConstructorMode::Default;
  }

  InputRegionArgs(string chrom, string first, string last, string strand) {
    if (!chrom.empty() && chrom != "NA")
      this->chrom = chrom;

    if (!strand.empty() && strand != "NA")
      this->strand = strand;

    if (auto converted_first = StringFormat::str_to_int(first)) {
      this->first = *converted_first;
      if (first.find('-') == 0)
        this->first *= -1;
      if (auto converted_last = StringFormat::str_to_int(last)) {
        this->last = *converted_last;
        if (last.find('-') == 0)
          this->last *= -1;
      }
    }

    if (chrom.empty()) {
      if (this->first < 0)
        if (this->strand.empty())
          this->mode = ConstructorMode::Default;
        else
          this->mode = ConstructorMode::ChromWithStrand;
      else if (this->last < 0)
        this->mode = ConstructorMode::Position;
      else
        this->mode = ConstructorMode::Range;
    } else {
      if (this->first < 0) {
        if (this->strand.empty())
          this->mode = ConstructorMode::SingleString;
        else
          this->mode = ConstructorMode::ChromWithStrand;
      } else if (this->last < 0)
        this->mode = ConstructorMode::ChromWithPosition;
      else
        this->mode = ConstructorMode::ChromWithRange;
    }

    cout << this->str() << endl;
  }

  string str() const {
    sstream message;
    switch (mode) {
    case ConstructorMode::Default:
      break;
    case ConstructorMode::SingleString:
      message << "'" << chrom << "'";
      break;
    case ConstructorMode::ChromWithStrand:
      message << "'" << chrom << "', '" << strand << "'";
      break;
    case ConstructorMode::Position:
      message << first << ", '" << strand << "'";
      break;
    case ConstructorMode::Range:
      message << first << ", " << last << ", '" << strand << "'";
      break;
    case ConstructorMode::ChromWithPosition:
      message << "'" << chrom << "', " << first << ", '" << strand << "'";
      break;
    case ConstructorMode::ChromWithRange:
      message << "'" << chrom << "', " << first << ", " << last << ", '"
              << strand << "'";
      break;
    }

    return message.str();
  }

  Region construct() const {
    switch (mode) {
    case ConstructorMode::Default:
      return Region();
    case ConstructorMode::SingleString:
      return Region(this->chrom);
    case ConstructorMode::ChromWithStrand:
      return Region(this->chrom, this->strand);
    case ConstructorMode::Position:
      return Region(this->first, this->strand);
    case ConstructorMode::Range:
      return Region(this->first, this->last, this->strand);
    case ConstructorMode::ChromWithPosition:
      return Region(this->chrom, this->first, this->strand);
    case ConstructorMode::ChromWithRange:
      return Region(this->chrom, this->first, this->last, this->strand);
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
    if (!function.empty())
      result << "." << function << "()";
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
} // namespace TestHKL::TestRegion
