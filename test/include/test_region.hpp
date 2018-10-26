#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "agizmo/evaluation.hpp"
#include "agizmo/files.hpp"
#include "agizmo/printable.hpp"
#include "agizmo/strings.hpp"

#include "hkl/region.hpp"

namespace TestHKL::TestRegion {
using namespace AGizmo;
using namespace Evaluation;

using std::ifstream;
using sstream = std::stringstream;
using std::quoted;
using std::setprecision;
using std::string;
using std::to_string;

using HKL::Region;
using HKL::RegionError;

using Printable::PrintableOptional;

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
    if (query != "None") {
      this->chrom = query;
      this->mode = ConstructorMode::SingleString;
    } else
      this->mode = ConstructorMode::Default;
  }

  InputRegionArgs(string chrom, string first, string last, string strand) {
    if (chrom != "None")
      this->chrom = chrom;
    if (strand != "None")
      this->strand = strand;
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
      message << *first << ", " << *last << ", " << quoted(strand.value_or(""));
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

  friend ostream &operator<<(ostream &stream, const InputRegionArgs &item) {
    return stream << item.str();
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
    return Region();
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

class InputPairRegionEval {
private:
  int id;
  InputRegionArgs args_ref, args_query;
  string function;

public:
  InputPairRegionEval() = default;
  InputPairRegionEval(int id, InputRegionArgs ref, InputRegionArgs query,
                      string function)
      : id{id}, args_ref{ref}, args_query{query}, function{function} {}

  pair<Region, Region> construct() const {
    return {args_ref.construct(), args_query.construct()};
  }

  string execute() const {
    sstream result;
    const auto [ref, query] = this->construct();
    if (function == "Ope=")
      result << (ref == query);
    else if (function == "Ope<")
      result << (ref < query);
    else if (function == "Ope>")
      result << (ref > query);
    else if (function == "sameChrom")
      result << (ref.sameChrom(query));
    else if (function == "sameRange")
      result << (ref.sameRange(query));
    else if (function == "sameStrand")
      result << (ref.sameStrand(query));
    else if (function == "sharesChrom")
      result << (ref.sharesChrom(query));
    else if (function == "sharesRange")
      result << (ref.sharesRange(query));
    else if (function == "sharesStrand")
      result << (ref.sharesStrand(query));
    else if (function == "shares")
      result << (ref.shares(query));
    else if (function == "around")
      result << (ref.around(query));
    else if (function == "next")
      result << (ref.next(query));
    else if (function == "inside")
      result << (ref.inside(query));
    else if (function == "covers")
      result << (ref.covers(query));
    else if (function == "upstream")
      result << (ref.upstream(query));
    else if (function == "downstream")
      result << (ref.downstream(query));
    else if (function == "getDist")
      result << PrintableOptional(ref.dist(query));
    else if (function == "getDistS")
      result << PrintableOptional(ref.dist(query, true));
    else if (function == "getRelPosFirst")
      result << PrintableOptional(ref.getRelPos(query));
    else if (function == "getRelPosFirstRatio")
      result << PrintableOptional(ref.getRelPosRatio(query));
    else if (function == "getRelPosFirstS")
      result << PrintableOptional(ref.getRelPos(query, true));
    else if (function == "getRelPosFirstRatioS")
      result << PrintableOptional(ref.getRelPosRatio(query, true));
    else if (function == "getRelPosCenter")
      result << PrintableOptional(ref.getRelPos(query.getCenter()));
    else if (function == "getRelPosCenterRatio")
      result << PrintableOptional(ref.getRelPosRatio(query));
    else if (function == "getRelPosCenterS")
      result << PrintableOptional(ref.getRelPos(query.getCenter(), true));
    else if (function == "getRelPosCenterRatioS")
      result << PrintableOptional(ref.getRelPosRatio(query.getCenter(), true));
    else if (function == "getRelPosLast")
      result << PrintableOptional(ref.getRelPosLast(query));
    else if (function == "getRelPosLastRatio")
      result << PrintableOptional(ref.getRelPosRatio(query));
    else if (function == "getRelPosLastS")
      result << PrintableOptional(ref.getRelPosLast(query, true));
    else if (function == "getRelPosLastRatioS")
      result << PrintableOptional(ref.getRelPosRatio(query, true));
    else if (function == "getShared")
      result << PrintableOptional(ref.getShared(query));
    else if (function == "getDiff")
      result << PrintableOptional(ref.getDiff(query));
    else if (function == "getUnion")
      result << PrintableOptional(ref.getUnion(query));
    else if (function == "getGap")
      result << PrintableOptional(ref.getGap(query));
    else if (function == "getSharedLength")
      result << PrintableOptional(ref.getSharedLength(query));
    else if (function == "getCoveredRate")
      result << PrintableOptional(ref.getCoveredRate(query));
    else if (function == "getCapturedRate")
      result << PrintableOptional(ref.getCapturedRate(query));
    else if (function == "toStr")
      return ref.str();
    else
      throw runtime_error{"Function '" + function + "' is not supported!"};
    return result.str();
  }

  string str() const {
    sstream result;
    result << " " << id << " (" << args_ref.str() << ")"
           << "." << function << "(" << args_query.str() << ")";
    return result.str();
  }
};

class RegionPaired : public BaseTest<InputPairRegionEval, string> {
public:
  RegionPaired() = default;
  RegionPaired(InputPairRegionEval input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    this->outcome = input.execute();
    return this->setStatus(outcome == expected);
  }
};

class InputRegionResize {
private:
  InputRegionArgs args;
  int upstream, downstream;
  bool orient;

public:
  InputRegionResize() = default;
  InputRegionResize(InputRegionArgs args, int upstream, int downstream,
                    bool orient)
      : args{args}, upstream{upstream}, downstream{downstream}, orient{orient} {
  }

  string str() const {
    sstream result;
    result << "(" << args << ").resize(" << upstream << ", " << downstream
           << ", " << orient << ")";
    return result.str();
  }

  Region construct() const { return args.construct(); }

  string execute() const {
    auto temp = this->construct();
    temp.resize(upstream, downstream, orient);
    return temp.str();
  }
};

class RegionResize : public BaseTest<InputRegionResize, string> {
public:
  RegionResize() = default;
  RegionResize(InputRegionResize input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    this->outcome = input.execute();
    return this->setStatus(outcome == expected);
  }
};

class InputRegionSlice {
private:
  InputRegionArgs ref;
  int pos;
  size_t length;

public:
  InputRegionSlice(string ref, int pos, size_t length)
      : ref{ref}, pos{pos}, length{length} {}

  string str() const {
    sstream result;
    result << ref << ".getSlice(" << pos << ", " << length << ")";
    return result.str();
  }

  string execute() const {
    if (const auto result = ref.construct().getSlice(pos, length))
      return (*result).str();
    else
      return "None";
  }
};

class RegionSlice : public BaseTest<InputRegionSlice, string> {
public:
  RegionSlice() = default;
  RegionSlice(InputRegionSlice input, string expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome + "\nExpected: " + expected;
  }

  bool validate() {
    this->outcome = input.execute();
    return this->setStatus(outcome == expected);
  }
};

Stats check_region_constructors(bool verbose);
Stats check_region_formation(bool verbose);
Stats check_region_failure(bool verbose);
Stats check_region_paired(bool verbose);
Stats check_region_resize(bool verbose);
Stats check_region_slice(bool verbose);
} // namespace TestHKL::TestRegion
