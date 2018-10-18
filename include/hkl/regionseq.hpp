#pragma once

#include <fstream>
#include <iostream>
#include <numeric>

#include "hkl/region.hpp"

#include "agizmo/files.hpp"

namespace HKL {

using std::move;

using std::accumulate;

using runerror = std::runtime_error;
using ooferror = std::out_of_range;
using std::getline;
using std::ifstream;

using namespace AGizmo;

class RegionSeq {
private:
  string name{""};
  string seq{""};
  Region loc = Region();

public:
  RegionSeq() = default;

  RegionSeq(string name, string seq, Region loc = Region()) {
    this->setName(name);
    this->setSeq(seq, loc);
  }

  RegionSeq(string name) : RegionSeq(name, "") {}

  void setName(string name) { this->name = name; }
  void setSeq(string seq, Region loc = Region()) {
    if (!seq.length()) {
      this->seq = "";
      this->loc = Region();
    } else {
      this->seq = move(seq);
      this->setLoc(loc);
    }
  }

  void resetLoc() {
    this->loc = Region("", 1, static_cast<int>(this->seq.size()));
  }

  void setLoc(Region loc) {
    if (!this->seq.size() and !loc.isEmpty())
      throw runerror("Setting location for empty sequence "
                     "is prohibited!");

    if (loc.isEmpty())
      this->resetLoc();
    else {
      if (this->seq.size() != loc.getLength())
        throw runerror{"Sequence and provied Region differ in length."};
      else
        this->loc = move(loc);
    }
  }
  void setChrom(string chrom) { this->loc.setChrom(chrom); }
  void setRange(int first, int last) { this->loc.setRange(first, last); }
  void setRange(pair<int, int> range) {
    this->setRange(range.first, range.second);
  }
  void setStrand(char strand) { this->loc.setStrand(strand); }
  void setStrand(string strand = "") { this->loc.setStrand(strand); }

  string str() const noexcept {
    sstream result;
    result << "IDX=" << this->name << ";";
    result << "LEN=" << this->getLength() << ";";
    result << "LOC=" << this->loc << ";";
    result << "SEQ=" << this->seq;

    return result.str();
  }

  friend std::ostream &operator<<(ostream &stream, const RegionSeq &seq) {
    return stream << seq.str();
  }

  friend bool operator==(const RegionSeq &left, const RegionSeq &right) {
    return (left.name == right.name && left.seq == right.seq &&
            left.loc == right.loc);
  }

  friend bool operator==(const RegionSeq &left, const string &right) {
    return (left.seq == right.c_str());
  }

  friend bool operator==(const RegionSeq &left, const char *right) {
    return (left.seq == right);
  }

  template <class T>
  friend bool operator!=(const RegionSeq &left, const T &right) {
    return !(left.seq == right);
  }

  template <class T>
  friend bool operator==(const T &left, const RegionSeq &right) {
    return (left == right.seq);
  }

  template <class T>
  friend bool operator!=(const T &left, const RegionSeq &right) {
    return !(left == right.seq);
  }

  friend bool operator<(const RegionSeq &left, const RegionSeq &right) {
    if (left.loc < right.loc)
      return true;
    else if (left.name < right.name)
      return true;
    else if (left.seq < right.seq)
      return true;
    else
      return false;
  }

  string getName() const { return this->name; }
  const string &getSeq() const { return this->seq; }
  const Region &getLoc() const { return this->loc; }
  string getChrom() const { return this->loc.getChrom(); }
  int getFirst() const { return this->loc.getFirst(); }
  int getLast() const { return this->loc.getLast(); }

  size_t getLength() const noexcept { return this->loc.getLength(); }
  size_t size() const noexcept { return this->getLength(); }

  bool isEmpty() const { return this->loc.isEmpty(); }

  const char &operator[](size_t pos) const { return this->seq[pos]; }
  const char &at(size_t pos) const {
    if (pos < this->size())
      return this->seq[pos];
    else
      throw ooferror{"Position is out of the scope of this sequence"};
  }

  const char &at(const Region &pos) const {
    if (!pos.isPos())
      throw runerror{"Region does not consist of single position"};

    if (!pos.inside(this->loc))
      throw ooferror{"Position is not located in this sequence"};

    return this->seq.at(static_cast<size_t>(*pos.getPosRel(this->loc)));
  }

  string getSeqSlice(size_t first, size_t length) const {
    if (first >= this->getLength())
      throw ooferror{"Range (" + to_string(first) + ", " + to_string(length) +
                     ") out of scope of this Seq!"};
    else if (length - first + 1 >= this->getLength())
      throw ooferror("Requested slice is longer than the sequence itself!");
    else
      return this->seq.substr(first, length);
  }

  optional<string> getSeqSlice(const Region &loc) const noexcept {
    if (const auto &shared = this->loc.getShared(loc)) {
      return this->getSeqSlice(
          static_cast<size_t>((*shared).getPosRel(this->loc).value()),
          (*shared).getLength());
    } else
      return nullopt;
  }

  optional<RegionSeq> getSlice(const Region &loc) const noexcept {
    if (const auto &shared = this->loc.getShared(loc)) {
      return RegionSeq(
          this->name,
          this->getSeqSlice(
              static_cast<size_t>((*shared).getPosRel(this->loc).value()),
              (*shared).getLength()),
          *shared);
    } else
      return nullopt;
  }

  string toFASTA(size_t line = 60, size_t chunk = 0, bool loc = false) const {
    string result = ">" + this->name;
    if (loc)
      result += this->loc.str();

    if (!this->isEmpty()) {
      result.reserve(result.size() +
                     (chunk + line + 1) * (this->size() % line));
      for (auto ele : StringDecompose::str_segment(this->seq, line))
        result += "\n" + StringDecompose::str_segment(ele, chunk, " ");
    }

    return result + "\n";
  }

  auto begin() { return this->seq.begin(); }
  auto end() { return this->seq.end(); }
  auto cbegin() const { return this->seq.cbegin(); }
  auto cend() const { return this->seq.cend(); }

  int countGC() const {
    return accumulate(this->cbegin(), this->cend(), 0, [](int a, char c) {
      return a + (c == 'C' or c == 'c' || c == 'G' || c == 'g');
    });
  }

  double calcGCRatio() const {
    return countGC() / static_cast<double>(getLength());
  }
};

class FASTAReader {
private:
  static void prepareSeq(string &old_name, string &name, string &seq,
                         bool upper) {
    swap(name, old_name);
    seq.erase(std::remove(seq.begin(), seq.end(), ' '), seq.end());
    if (upper)
      transform(seq.begin(), seq.end(), seq.begin(), toupper);
  }

  static string cutFASTAMarker(const string &name) {
    return name.substr(1, name.size() - 1);
  }

  string file_name, old_name;
  ifstream input;

public:
  static bool readFASTASeq(ifstream &input, string &old_name, string &name,
                           string &seq, bool upper = false) {
    string line{};
    name = {};
    seq = {};

    while (getline(input, line)) {
      if (!line.size())
        continue;

      if (line.at(0) == '>') {
        name = cutFASTAMarker(line);
        if (old_name.size()) {
          prepareSeq(old_name, name, seq, upper);
          return true;
        } else {
          old_name = name;
        }
      } else
        seq += line;
    }

    prepareSeq(old_name, name, seq, upper);
    return name.size() ? true : false;
  }

  static optional<RegionSeq> readFASTASeq(ifstream &input, string &old_name,
                                          bool upper = false) {
    string name, seq;

    if (readFASTASeq(input, old_name, name, seq, upper))
      return RegionSeq(name, seq);
    else
      return nullopt;
  }

  static vector<RegionSeq> readFASTAFile(const string &file_name,
                                         bool upper = false) {
    ifstream input;
    Files::open_file(file_name, input);

    return readFASTAFile(input, upper);
  }

  static vector<RegionSeq> readFASTAFile(ifstream &input, bool upper = false) {
    vector<RegionSeq> result{};
    string old_name, name, seq;
    while (readFASTASeq(input, old_name, name, seq, upper))
      result.emplace_back(name, seq);

    input.close();

    return result;
  }

  FASTAReader(string file_name) {
    this->file_name = move(file_name);
    Files::open_file(this->file_name, input);
  }

  ~FASTAReader() { input.close(); }

  vector<RegionSeq> readFASTAFile(bool upper = false) {
    return readFASTAFile(input, upper);
  }

  bool readFASTASeq(string &name, string &seq, bool upper = false) {
    return readFASTASeq(this->input, this->old_name, name, seq, upper);
  }

  optional<RegionSeq> readFASTASeq(bool upper = false) {
    string name, seq;

    if (readFASTASeq(this->input, this->old_name, name, seq, upper))
      return RegionSeq(name, seq);
    else
      return nullopt;
  }
};

} // namespace HKL
