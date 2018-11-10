#pragma once

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>

#include <agizmo/files.hpp>
#include <agizmo/printable.hpp>
#include <agizmo/strings.hpp>

#include <hkl/region.hpp>

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

namespace HKL::GFF {

using namespace AGizmo;

using std::cerr;
using std::nullopt;
using std::optional;
using std::ostream;
using runerror = std::runtime_error;
using std::stod;
using std::string;
using std::to_string;
using std::variant;

using opt_str = optional<string>;
using opt_double = optional<double>;
using opt_char = optional<char>;
using opt_int = optional<int>;

class GFFRecord {
 private:
  //  string temp{};

  opt_str seqid{};
  opt_str source{};
  opt_str type{};
  int start{0};
  int end{0};
  size_t length{0};
  opt_double score{};
  opt_char strand{};
  opt_int phase{};
  Printable::PrintableStrMap attr{};

 public:
  GFFRecord() = default;
  GFFRecord(const string &line) {
    //    temp = line;
    const auto splitted = StringDecompose::str_split(line, "\t");

    assert(splitted.size() == 9);

    auto item = splitted.begin();

    if (const auto &seqid = *item; seqid != ".") this->seqid = seqid;

    if (const auto &source = *(++item); source != ".") this->source = source;

    if (const auto &type = *(++item); type != ".") this->type = type;

    if (const auto &start = StringFormat::str_to_int(*(++item)))
      this->start = *start;
    else
      throw runerror{"Start field, 4th column, is malformed - " + *item};

    if (const auto &end = StringFormat::str_to_int(*(++item)))
      this->end = *end;
    else
      throw runerror{"Start field, 5th column, is malformed - " + *item};

    length = static_cast<size_t>(end - start + 1);

    if (const auto &score = *(++item); score != ".") this->score = stod(score);

    if (const auto &strand = *(++item); strand != ".") {
      if (strand == "-" || strand == "+")
        this->strand = strand.at(0);
      else
        throw runerror{"Strand field, 6th column, is malformed - " + *item};
    }

    if (const auto &phase = *(++item); phase != ".") {
      if (const auto int_phase = StringFormat::str_to_int(phase))
        this->phase = *int_phase;
      else
        throw runerror{"Phase field, 8th column, is malformed - " + *item};
    }

    attr = Printable::PrintableStrMap(splitted.at(8), ';', '=');
  }

  string str() const {
    sstream output;
    output << seqid.value_or(".") << '\t' << source.value_or(".") << '\t'
           << type.value_or(".") << '\t' << start << "\t" << end << "\t";
    if (const auto &score = this->score) {
      if (*score != 1.0)
        output << StringFormat::str_double(*score, 3, 0);
      else
        output << 1;
    } else
      output << ".";

    output << "\t" << strand.value_or('.') << "\t";

    if (const auto &phase = this->phase)
      output << *phase;
    else
      output << ".";

    output << '\t' << attr;

    return output.str();
  }

  friend std::ostream &operator<<(ostream &stream, const GFFRecord &item) {
    return stream << item.str();
  }
};

class GFFComment {
 private:
  string field{}, value{};
  bool empty{true};

 public:
  GFFComment() = default;
  GFFComment(const string &line) {
    empty = line.find_last_of('#') == line.size() - 1;
    if (!isEmpty()) {
      const auto field_first = line.find_first_not_of('#');
      const auto field_last = line.find_first_of(' ');

      field = line.substr(field_first, field_last - field_first);
      const auto value_first = line.find_first_not_of(' ', field_last);

      value = line.substr(value_first);
    }
  }

  bool isEmpty() const { return empty; }
  bool isMeta() const { return (field[0] == '!'); }
  bool isRegion() const { return field == "sequence-region"; }

  string str() const {
    if (isEmpty())
      return "###";
    else if (isMeta())
      return "#" + field + " " + value;
    else if (isRegion())
      return "##" + field + "   " + value;
    else
      return "##" + field + " " + value;
  }

  friend std::ostream &operator<<(ostream &stream, const GFFComment &item) {
    return stream << item.str();
  }

  optional<Region> getRegion() {
    if (isRegion() && StringSearch::count_all(value, ' ') == 2) {
      const auto chrom_sep = value.find_first_of(' ');
      const auto first_sep = value.find_first_of(' ', chrom_sep + 1);

      return Region(value.substr(0, chrom_sep),
                    value.substr(chrom_sep + 1, first_sep - chrom_sep - 1),
                    value.substr(first_sep + 1));
    } else {
      return nullopt;
    }
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

}  // namespace HKL::GFF
