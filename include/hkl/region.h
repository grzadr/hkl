#pragma once

#include "comtol.h"

using std::abs;
using std::map;
using std::max;
using std::min;
using std::optional;
using std::ostream;
using std::pair;
using std::string;
using std::to_string;

using opt_int = optional<int>;
using opt_double = optional<double>;

using namespace ComTol;
using namespace ComTol::StrTol;

namespace HKL {

enum class RegionErrorType {
  None = 0,
  ChrFormat,
  PosMissing,
  PosFormat,
  PosRange,
  StrandMissing,
  StrandFormat,
};

using RET = RegionErrorType;

class RegionError : public std::exception {
private:
  string query{};
  string message{};
  RET type{RET::None};

  void genMessage() {
    this->message = "\n######### REGION ERROR #########\n"
                    "    Name: " +
                    this->getName() +
                    "\n"
                    "   Query: " +
                    this->query + "\n";
  }

public:
  RegionError() = default;
  RegionError(RET type, string query) : type{type} {
    this->query = "'" + query + "'";
    if (type == RET::PosMissing && count_all(query, ':') > 1) {
      this->type = RET::ChrFormat;
      this->genMessage();
      this->message += " Details: ':' signs not allowed in names\n";
    } else
      this->genMessage();
  }

  RegionError(RET type, string chrom, int first, int last, string strand)
      : type{type} {
    this->query = "'" + chrom + "', " + to_string(first) + ", " +
                  to_string(last) + ", '" + strand + "'";
    this->genMessage();
  }

  RegionError(RET type, int first, int last) : type{type} {
    this->query = std::to_string(first) + ", " + std::to_string(last);
    this->genMessage();
  }

  RegionError(RET type, char query) : RegionError(type, string(1, query)) {}

  RegionError(RET type, string chrom, int first, int last, char strand)
      : RegionError(type, chrom, first, last, string(1, strand)) {}

  const RET &getType() const { return this->type; }
  string getName() const {
    switch (this->type) {
    case RET::None:
      return "None";
    case RET::ChrFormat:
      return "ChrFormat";
    case RET::PosMissing:
      return "PosMissing";
    case RET::PosFormat:
      return "PosFormat";
    case RET::PosRange:
      return "PosRange";
    case RET::StrandMissing:
      return "StrandMissing";
    case RET::StrandFormat:
      return "StrandFormat";
    default:
      throw std::runtime_error("Unknown Error\n");
    }
  }
  virtual inline const char *what() const noexcept override {
    return this->message.c_str();
  }
};

class Region {
private:
  string chrom{""};
  int first{0};
  int last{0};
  size_t length{0};
  char strand{0};

  void update() {
    if (!this->first)
      this->length = 0;
    else
      this->length = static_cast<size_t>(this->last - this->first + 1);
  }

  template <typename T>
  T calcPosRel(T pos, bool orient = false) const noexcept {
    if (orient && this->strand == '-')
      return this->last - pos;
    else
      return pos - this->first;
  }

public:
  using opt_region = optional<Region>;
  using vec_region = vector<Region>;

  Region() = default;

  Region(string query) {
    if (!query.length())
      *this = Region();
    else {
      auto strand_mark = query.rfind("/");

      if (strand_mark == query.size() - 1)
        throw RegionError{RET::StrandMissing, query};

      const auto chrom_mark = query.rfind(":", strand_mark - 1);

      if (chrom_mark == query.size() - 1 or
          (chrom_mark != string::npos and chrom_mark + 1 == strand_mark))
        throw RegionError{RET::PosMissing, query};

      this->setChrom(query.substr(0, min(chrom_mark, strand_mark)));

      strand_mark =
          (strand_mark == string::npos ? query.length() : strand_mark);

      if (chrom_mark != string::npos) {
        try {
          this->setRange(
              query.substr(chrom_mark + 1, strand_mark - chrom_mark - 1));
        } catch (const RegionError &ex) {
          throw RegionError{ex.getType(), query};
        }
      }

      if (strand_mark != query.length()) {
        try {
          this->setStrand(query.substr(strand_mark + 1));
        } catch (const RegionError &ex) {
          throw RegionError{ex.getType(), query};
        }
      }
    }
  }

  Region(string chrom, int first, int last, string strand = "") {
    try {
      this->setChrom(chrom);
      this->setRange(first, last);
      this->setStrand(strand);
    } catch (const RegionError &ex) {
      throw RegionError(ex.getType(), chrom, first, last, strand);
    }
  }

  Region(string chrom, int first, int last, char strand)
  //        :Region(chrom, first, last, string(1, strand))
  {
    try {
      this->setChrom(chrom);
      this->setRange(first, last);
      this->setStrand(strand);
    } catch (const RegionError &ex) {
      throw RegionError(ex.getType(), chrom, first, last, strand);
    }
  }

  Region(string chrom, int pos, string strand = "")
      : Region(chrom, pos, pos, strand) {}

  Region(int first, int last, string strand = "")
      : Region("", first, last, strand) {}
  Region(int pos, string strand = "") : Region("", pos, pos, strand) {}

  Region(string chrom, string strand) : Region(chrom, 0, strand) {}

  friend std::ostream &operator<<(ostream &stream, const Region &region) {
    return stream << region.str();
  }

  friend bool operator==(const Region &left, const Region &right) {
    return (left.chrom == right.chrom && left.first == right.first &&
            left.last == right.last && left.strand == right.strand);
  }

  friend bool operator<(const Region &left, const Region &right) {
    if (left.chrom < right.chrom)
      return true;
    else if (left.chrom > right.chrom)
      return false;
    else if (left.first < right.first)
      return true;
    else if (left.first > right.first)
      return false;
    else if (left.last < right.last)
      return true;
    else if (left.last > right.last)
      return false;
    else
      return (left.strand < right.strand);
  }

  friend bool operator!=(const Region &left, const Region &right) {
    return !(left == right);
  }

  friend bool operator<=(const Region &left, const Region &right) {
    return left < right || left == right;
  }

  friend bool operator>(const Region &left, const Region &right) {
    return !(left < right) && left != right;
  }

  friend bool operator>=(const Region &left, const Region &right) {
    return left > right && left == right;
  }

  void setChrom(string query) {
    if (query.find(":") != string::npos)
      throw RegionError{RET::ChrFormat, query};

    this->chrom = query;
  }

  void setRange(int first, int last = 0) {
    if (first < 0 || last < 0)
      throw RegionError{RET::PosFormat, first, last};

    if (last) {
      if (!first || last < first)
        throw RegionError{RET::PosFormat, first, last};
      this->first = first;
      this->last = last;
    } else
      this->last = this->first = first;

    this->update();
  }
  void setRange(pair<int, int> range) {
    this->setRange(range.first, range.second);
  }
  void setRange(string coord) {
    if (!coord.size())
      this->setRange(0);

    switch (count_all(coord, '-')) {
    case 0:
      if (auto first = strtoi(coord))
        this->setRange(*first);
      else
        throw RegionError(RET::PosFormat, coord);
      break;
    case 1: {
      auto mark_pos = coord.find("-");

      if (!mark_pos || mark_pos == coord.length() - 1)
        throw RegionError(RET::PosMissing, coord);

      if (auto first = strtoi(coord.substr(0, mark_pos));
          auto last = strtoi(coord.substr(mark_pos + 1)))
        this->setRange(*first, *last);
      else
        throw RegionError(RET::PosFormat, coord);
      break;
    }
    default:
      throw RegionError{RET::PosFormat, coord};
    }
  }

  void setPos(int pos) { this->setRange(pos, pos); }

  void setFirst(int first) { this->setRange(first, this->last); }
  void setLast(int last) { this->setRange(this->first, last); }

  void setStrand(char strand) {
    switch (strand) {
    case 0:
    case '0':
      this->strand = 0;
      break;
    case '+':
    case '1':
    case 'F':
    case 'f':
    case 'P':
    case 'p':
      this->strand = '+';
      break;
    case '-':
    case 'R':
    case 'r':
    case 'N':
    case 'n':
      this->strand = '-';
      break;
    default:
      throw RegionError{RET::StrandFormat, strand};
    }
  }
  void setStrand(string strand = "") {
    if (strand.length()) {
      try {
        this->setStrand(strand[0]);
      } catch (const RegionError &) {
        throw RegionError(RET::StrandFormat, strand);
      }
    } else
      this->strand = 0;
  }

  void resize(int upstream, int downstream, bool orient = true) {
    if (this->isEmpty())
      return;

    int temp_first = this->first;
    int temp_last = this->last;

    if (orient && this->strand == '-') {
      temp_first -= downstream;
      temp_last += upstream;
    } else {
      temp_first -= upstream;
      temp_last += downstream;
    }

    if (temp_first < 1)
      temp_first = 1;
    if (temp_last < 1)
      temp_last = 0;

    if (temp_first > temp_last)
      this->setRange(0, 0);
    else
      this->setRange(temp_first, temp_last);
  }
  void resize(pair<int, int> extent, bool orient = true) {
    this->resize(extent.first, extent.second, orient);
  }

  bool isEmpty() const { return !this->length; }
  bool isPos() const { return this->length == 1; }
  bool isRange() const { return this->length > 1; }
  bool isPure() const { return !this->chrom.length() && !this->isEmpty(); }

  string getChrom() const { return this->chrom; }
  string getChrom(const Region &other) const {
    return this->sameChrom(other) ? this->chrom : "";
  }

  int getFirst() const { return this->first; }
  int getLast() const { return this->last; }

  double getCenter() const noexcept { return (this->first + this->last) / 2.0; }
  pair<int, int> getRange() const { return {this->first, this->last}; }

  char getStrand() const { return this->strand; }
  char getStrand(const Region &other) const {
    return this->sameStrand(other) ? this->strand : 0;
  }

  size_t size() const { return this->length; }
  size_t getLength() const { return this->length; }

  int at(int pos, bool orient = false) const {
    pos = orient && this->strand == '-' ? this->first - pos : this->first + pos;
    if (pos < 1)
      throw std::out_of_range("");
    else
      return pos;
  }

  string str() const {
    return this->chrom + ":" + to_string(this->first) +
           (this->isRange() ? "-" + to_string(this->last) : "") +
           (!this->strand ? "" : ("/" + string(1, this->strand)));
  }

  bool sameChrom(const Region &other) const {
    return this->chrom == other.chrom;
  }
  bool sharesChrom(const Region &other) const {
    return (!this->isEmpty() && !other.isEmpty() &&
            (this->isPure() || other.isPure() || this->sameChrom(other)));
  }

  bool sameRange(const Region &other) const {
    return this->first == other.first && this->last == other.last;
  }
  bool sharesRange(const Region &other) const {
    return !(this->first > other.last || this->last < other.first);
  }

  bool sameStrand(const Region &other) const {
    return this->strand == other.strand;
  }
  bool sharesStrand(const Region &other) const {
    return this->sameStrand(other) || XOR(this->strand, other.strand);
  }

  opt_int dist(const Region &other, bool orient = false) const {
    if (!this->sharesChrom(other))
      return nullopt;
    if (this->sharesRange(other))
      return 0;

    int dist = this->first > other.last ? other.last - this->first
                                        : other.first - this->last;

    return (orient && this->strand == '-') ? -dist : dist;
  }

  bool shares(const Region &other) const {
    return this->sharesChrom(other) && this->sharesRange(other);
  }
  bool around(const Region &other) const {
    return this->sharesChrom(other) && !this->sharesRange(other);
  }
  bool next(const Region &other) const {
    if (auto dist = this->dist(other))
      return abs(*dist) == 1;
    else
      return false;
  }

  bool inside(const Region &other) const {
    return (this->sharesChrom(other) &&
            !(this->first < other.first || this->last > other.last));
  }
  bool covers(const Region &other) const { return other.inside(*this); }

  bool upstream(const Region &other, bool orient = true) const {
    if (const auto &dist = other.dist(*this, orient))
      return dist < 0;
    else
      return false;
  }
  bool downstream(const Region &other, bool orient = true) const {
    if (const auto &dist = other.dist(*this, orient))
      return dist > 0;
    else
      return false;
  }

  template <typename T = int>
  optional<T> getPosRel(const Region &other, T pos = T{0},
                        bool orient = false) const noexcept {
    if (!this->sharesChrom(other) or pos < 0)
      return nullopt;
    else
      return other.calcPosRel((pos ? pos : this->first), orient);
  }

  template <typename T>
  optional<T> getPosRel(T pos, bool orient = false) const noexcept {
    if (pos <= 0)
      return nullopt;
    else
      return this->calcPosRel(pos, orient);
  }

  template <typename T = int>
  opt_double getPosRelRatio(const Region &other, T pos = T{0},
                            bool orient = false) const noexcept {
    if (auto pos_rel = this->getPosRel(other, pos, orient)) {
      if (*pos_rel < 0 or size_t(*pos_rel) >= other.length)
        return nullopt;
      else if (other.isPos())
        return *pos_rel;
      else
        return *pos_rel / (other.length - 1.0);
    } else
      return nullopt;
  }

  template <typename T>
  opt_double getPosRelRatio(T pos, bool orient = false) const noexcept {
    if (auto pos_rel = this->getPosRel(pos, orient)) {
      if (*pos_rel < 0 or size_t(*pos_rel) >= this->length)
        return nullopt;
      else
        return *pos_rel / (this->length - 1.0);
    } else
      return nullopt;
  }

  opt_region getShared(const Region &other) const {
    if (!this->sharesChrom(other))
      return nullopt;
    else if (!this->sharesRange(other)) {
      if (this->isPure() || other.isPure())
        return nullopt;
      else
        return Region(this->chrom, string(1, this->getStrand(other)));
    } else
      return Region(this->getChrom(other), max(this->first, other.first),
                    min(this->last, other.last), this->getStrand(other));
  }

  optional<pair<Region, Region>> getDiff(const Region &other) const {
    if (!this->shares(other))
      return nullopt;
    else {
      return pair<Region, Region>{
          (this->first == other.first
               ? Region(this->getChrom(other),
                        string(1, this->getStrand(other)))
               : Region(this->getChrom(other), min(this->first, other.first),
                        max(this->first, other.first) - 1,
                        this->getStrand(other))),
          (this->last == other.last
               ? Region(this->getChrom(other),
                        string(1, this->getStrand(other)))
               : Region(this->getChrom(other), min(this->last, other.last) + 1,
                        max(this->last, other.last), this->getStrand(other)))};
    }
  }

  opt_region getUnion(const Region &other) const {
    if (auto dist = this->dist(other); dist.has_value() && abs(*dist) <= 1)
      return Region(this->getChrom(other), min(this->first, other.first),
                    max(this->last, other.last), this->getStrand(other));
    else
      return nullopt;
  }
  opt_region getGap(const Region &other) const {
    if (auto dist = this->dist(other)) {
      switch (abs(*dist)) {
      case 0:
        return nullopt;
      case 1:
        return Region(this->getChrom(other), string(1, this->getStrand(other)));
      default:
        if (this->first < other.first)
          return Region(this->getChrom(other), this->last + 1, other.first - 1,
                        this->getStrand(other));
        else
          return Region(this->getChrom(other), other.last + 1, this->first - 1,
                        this->getStrand(other));
      }
    } else
      return nullopt;
  }

  vec_region getSlices(size_t size) const {
    if (this->isEmpty())
      return vec_region{};
    if (this->length <= size)
      return vec_region{*this};

    vec_region result{};

    int slice = static_cast<int>(size);

    for (int start = this->first, end = start + slice - 1; start <= this->last;
         start += slice, end += slice)
      result.push_back(
          Region{this->chrom, start, (end > this->last ? this->last : end)});

    return result;
  }

  //    vec_region getSlices(uint size, uint shift, bool beyond = false) const{
  //        if (this->isEmpty()) return vec_region{};
  //        if (!shift) shift = size;
  //        if (this->isPos() || this->length <= size)
  //            return vec_region{*this};

  //        vec_region result{};

  //        int start = this->first;
  //        int end = this->last;

  //        if (beyond){
  //            start = start <= size ? 1 : start - size + 1;
  //            end += size - 1;
  //        }

  //        uint temp_end = start + size - 1;

  //        do{
  //            result.push_back(Region{this->chrom, (int)start,
  //                                    (int)(temp_end > end ? end :
  //                                    temp_end)});
  //            start += shift;
  //            temp_end = start + size - 1;
  //        } while(temp_end <= end);

  //        return result;
  //    }

  opt_int getSharedLength(const Region &other) const {
    if (!this->sharesChrom(other))
      return nullopt;
    else if (!this->sharesRange(other))
      return 0;
    else
      return (min(this->last, other.last) - max(this->first, other.first) + 1);
  }

  opt_double getCoveredRate(const Region &other) const {
    if (auto shared = this->getSharedLength(other))
      return static_cast<double>(*shared) / this->length;
    else
      return nullopt;
  }
  opt_double getCapturedRate(const Region &other) const {
    if (auto shared = this->getSharedLength(other))
      return static_cast<double>(*shared) / other.length;
    else
      return nullopt;
  }
};

} // namespace HKL

namespace std {
template <> struct hash<HKL::Region> {
  std::size_t operator()(const HKL::Region &r) const {
    return std::hash<std::string>{}(r.str());
  }
};
} // namespace std
