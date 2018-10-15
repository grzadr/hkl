#pragma once

#include "agizmo/evaluation.hpp"
#include "agizmo/strings.hpp"
#include "hkl/region.hpp"
#include <sstream>
#include <string>

namespace TestHKL::TestRegion {
using namespace AGizmo;
using namespace Evaluation;
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
      if (auto converted_last = StringFormat::str_to_int(last))
        this->last = *converted_last;
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
        if (strand.empty())
          this->mode = ConstructorMode::SingleString;
        else
          this->mode = ConstructorMode::ChromWithStrand;
      } else if (this->last < 0)
        this->mode = ConstructorMode::ChromWithPosition;
      else
        this->mode = ConstructorMode::ChromWithRange;
    }
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

  string construct() const {
    try {
      switch (mode) {
      case ConstructorMode::Default:
        return Region().str();
      case ConstructorMode::SingleString:
        return Region(this->chrom).str();
      case ConstructorMode::ChromWithStrand:
        return Region(this->chrom, this->strand).str();
      case ConstructorMode::Position:
        return Region(this->first, this->strand).str();
      case ConstructorMode::Range:
        return Region(this->first, this->last, this->strand).str();
      case ConstructorMode::ChromWithPosition:
        return Region(this->chrom, this->first, this->strand).str();
      case ConstructorMode::ChromWithRange:
        return Region(this->chrom, this->first, this->last, this->strand).str();
      }
    } catch (const RegionError &ex) {
      return ex.what();
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
    this->outcome = input.construct();
    return this->setStatus(outcome == expected);
  }
};

Stats check_region_constructors(bool verbose);
} // namespace TestHKL::TestRegion
