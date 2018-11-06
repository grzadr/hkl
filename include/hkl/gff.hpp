#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <variant>

#include <agizmo/files.hpp>
#include <agizmo/strings.hpp>
#include <map>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace HKL::GFF {

using namespace AGizmo;

using std::cerr;
using std::nullopt;
using std::optional;
using std::ostream;
using std::string;
using std::variant;

using opt_str = optional<string>;
using opt_double = optional<double>;
using opt_char = optional<char>;
using opt_int = optional<int>;
using map_attr = std::unordered_map<string, opt_str>;

class GFFRecord {
private:
  string temp{};

  opt_str seqid{};
  opt_str source{};
  opt_str type{};
  int start{0};
  int end{0};
  size_t length{0};
  opt_double score{};
  opt_char strand{};
  opt_int phase{};
  map_attr attr{};

public:
  GFFRecord() = default;
  GFFRecord(const string &line) : temp{line} {
    cerr << "LINE: " << line << "\n";
    const auto splitted = StringDecompose::str_split(line, "\t");

    assert(splitted.size() == 9);

    for (const auto ele : splitted) {
      cerr << ele << "\n";
    }
  }

  string str() const { return temp; }

  friend std::ostream &operator<<(ostream &stream, const GFFRecord &item) {
    return stream << "GFFRecord: " << item.str();
  }
};

class GFFComment {
private:
  string temp{};

public:
  GFFComment() = default;
  GFFComment(const string &line) : temp{line} {}

  string str() const { return temp; }

  friend std::ostream &operator<<(ostream &stream, const GFFComment &item) {
    return stream << "GFFComment: " << item.str();
  }
};

using gff_variant = variant<GFFRecord, GFFComment>;

class GFFReader {
private:
  Files::FileReader reader;

public:
  GFFReader() = delete;
  GFFReader(const string &file_name) : reader{file_name} {}

  gff_variant process(const string &line) const {
    if (line.find_first_of('#') == 0)
      return GFFComment{line};
    else
      return GFFRecord{line};
  }
  optional<gff_variant> operator()(const string &skip = {}) {
    if (const auto line = reader(skip)) {
      if ((*line).empty())
        return (*this)(skip);
      else
        return process(*line);
    } else
      return nullopt;
  }
};

} // namespace HKL::GFF
