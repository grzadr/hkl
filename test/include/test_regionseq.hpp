#pragma once

#include <optional>
#include <stdexcept>
#include <variant>

#include <agizmo/evaluation.hpp>

#include <hkl/region.hpp>
#include <hkl/regionseq.hpp>

namespace TestHKL::TestRegionSeq {
using std::get;
using std::holds_alternative;
using std::optional;
using std::variant;
using ooferror = std::out_of_range;

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

  string str() const {
    sstream result;
    result << "(" << name << ", " << seq << ", " << reg << ")";
    return result.str();
  }
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

class InputRegionSeqGetSeq {
 private:
  InputRegionSeq args;
  variant<int, string, pair<int, size_t>> loc;

 public:
  InputRegionSeqGetSeq() = default;
  InputRegionSeqGetSeq(InputRegionSeq args,
                       variant<int, string, pair<int, size_t>> loc)
      : args{args}, loc{loc} {}

  //  InputRegionSeqGetSeq()

  string str() const {
    sstream result;

    if (holds_alternative<int>(loc))
      result << ".at(" << get<int>(loc);
    else if (holds_alternative<string>(loc))
      result << ".getSeq(" << get<string>(loc);
    else {
      const auto [first, second] = get<2>(loc);
      result << ".getSeq(" << first << ", " << second;
    }

    return result.str() + ")";
  }

  string execute() const {
    const auto seq = args.construct();
    if (holds_alternative<int>(loc))
      return string(1, seq.at(get<int>(loc)));
    else if (holds_alternative<string>(loc))
      return seq.getSeq(Region(get<string>(loc)));
    else {
      const auto [first, second] = get<2>(loc);
      return seq.getSeq(first, second);
    }
  }
};

class RegionSeqGetSeq : public BaseTest<InputRegionSeqGetSeq, string> {
 public:
  RegionSeqGetSeq() = default;
  RegionSeqGetSeq(InputRegionSeqGetSeq input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    try {
      this->outcome = input.execute();
    } catch (const ooferror&) {
      this->outcome = "ooferror";
    }

    return this->setStatus(outcome == expected);
  }
};

Stats check_basic(bool verbose);
Stats check_get_seq(bool verbose);
}  // namespace TestHKL::TestRegionSeq
