#pragma once

#include <optional>

#include <agizmo/evaluation.hpp>

#include <hkl/region.hpp>
#include <hkl/regionseq.hpp>

namespace TestHKL::TestRegionSeq {
using std::optional;

using namespace AGizmo;
using namespace Evaluation;

using HKL::Region;
using HKL::RegionSeq;

class InputRegionSeq {
private:
  string name, seq;
  Region reg;

public:
  InputRegionSeq() = default;
  InputRegionSeq(string name, string seq, string reg)
      : name{name}, seq{seq}, reg{Region(reg)} {}

  RegionSeq construct() const { return RegionSeq(name, seq, reg); }

  string str() const { return ""; }
};

class RegionSeqConstructors : public BaseTest<InputRegionSeq, string> {
public:
  RegionSeqConstructors() = default;
  RegionSeqConstructors(InputRegionSeq input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    const auto temp = input.construct();
    this->outcome = temp.str();
    return this->setStatus(outcome == expected);
  }
};

Stats check_basic(bool verbose);
} // namespace TestHKL::TestRegionSeq
