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
  int range_start{0};
  int range_end{0};
  size_t length{0};
  opt_double score{};
  opt_char strand{};
  opt_int phase{};
  Printable::PrintableStrMap attr{};

 public:
  static constexpr char gff3_escape[]{"\t\n\r%;=&,"};

  static string gff3_str_escape(const string &input) {
    sstream message;
    size_t last{0}, pos{input.find_first_of(gff3_escape)};

    if (pos == string::npos) return input;

    do {
      message << input.substr(last, pos - last);
      if (pos == string::npos) break;
      message << "%" << std::hex << std::uppercase << int(input.at(pos));
      last = pos + 1;
      pos = input.find_first_of(gff3_escape, last);
    } while (true);

    return message.str();
  }

  static string gff3_str_clean(string input) {
    size_t pos = input.find('%');
    while (pos != string::npos) {
      auto ele = char(stoi(input.substr(pos + 1, 2), nullptr, 16));
      input.replace(pos, 3, 1, ele);
      pos = input.find('%', pos + 1);
    }
    return input;
  }

  GFFRecord() = default;
  GFFRecord(const string &line) {
    const auto splitted = StringDecompose::str_split(line, "\t");

    assert(splitted.size() == 9);

    auto item = splitted.begin();

    if (const auto &seqid = *item; seqid != ".")
      this->seqid = gff3_str_clean(seqid);

    if (const auto &source = *(++item); source != ".")
      this->source = gff3_str_clean(source);

    if (const auto &type = *(++item); type != ".")
      this->type = gff3_str_clean(type);

    if (const auto &start = StringFormat::str_to_int(*(++item)))
      this->range_start = *start;
    else
      throw runerror{"Start field, 4th column, is malformed - " + *item};

    if (const auto &end = StringFormat::str_to_int(*(++item)))
      this->range_end = *end;
    else
      throw runerror{"Start field, 5th column, is malformed - " + *item};

    length = static_cast<size_t>(range_end - range_start + 1);

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
    for (const auto &key : attr.get_keys()) {
      if (auto &value = attr[key]; value.has_value())
        if ((*value).find(',') == string::npos)
          attr[key] = gff3_str_clean(*value);
    }
  }

  auto &getKeys() { return attr.get_keys(); }

  auto size() const { return attr.size(); }
  auto isEmpty() const { return attr.isEmpty(); }

  opt_str getSeqID() const { return seqid; }
  opt_str getType() const { return type; }
  opt_str getSource() const { return source; }
  int getStart() const { return range_start; }
  int getEnd() const { return range_end; }
  size_t getLength() const { return length; }
  opt_double getScore() const { return score; }
  opt_char getStrand() const { return strand; }
  opt_int getPhase() const { return phase; }

  bool isRecord() const noexcept { return true; }
  bool isComment() const noexcept { return false; }

  auto begin() const noexcept { return attr.begin(); }
  auto end() const noexcept { return attr.end(); }

  auto keys_begin() noexcept { return attr.keys_begin(); }
  auto keys_end() noexcept { return attr.keys_end(); }
  auto keys_cbegin() const noexcept { return attr.keys_cbegin(); }
  auto keys_cend() const noexcept { return attr.keys_cend(); }

  auto &operator[](const string &key) { return attr[key]; }
  auto at(string key) const { return attr.at(key); }
  optional<opt_str> get(const string &key) const noexcept {
    return attr.get(key);
  }
  string get(const string &key, const string &value,
             const string &empty = "") const {
    return attr.get(key, value, empty);
  }

  bool has(const string &key) const { return attr.has(key); }

  string str() const {
    sstream output;
    output << gff3_str_escape(seqid.value_or(".")) << '\t'
           << gff3_str_escape(source.value_or(".")) << '\t'
           << gff3_str_escape(type.value_or(".")) << '\t' << range_start << "\t"
           << range_end << "\t";
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

    output << "\t"
           << attr.join_fields(attr.keys_cbegin(), attr.keys_cend(), ';', '=',
                               [](const string &ele) {
                                 if (ele.find(',') == string::npos)
                                   return gff3_str_escape(ele);
                                 else
                                   return ele;
                               });

    return output.str();
  }

  string strFields(const string &missing = ".",
                   const string &sep = "\t") const {
    sstream output;
    output << seqid.value_or(missing) << sep << source.value_or(missing) << sep
           << type.value_or(missing) << sep << range_start << "\t" << range_end
           << "\t";
    if (const auto &score = this->score) {
      if (*score != 1.0)
        output << StringFormat::str_double(*score, 3, 0);
      else
        output << 1;
    } else
      output << missing;

    if (strand.has_value())
      output << sep << *strand << sep;
    else
      output << sep << missing << sep;

    if (const auto &phase = this->phase)
      output << *phase;
    else
      output << missing;

    return output.str();
  }

  template <class It>
  string strAttributes(It begin, It end, const string &missing = ".",
                       const string &sep = "\t",
                       const string &empty = "true") const {
    sstream output;

    for (auto key = begin; key != end; ++key) {
      output << sep << get(*key, missing, empty);
    }

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
  bool isRecord() const noexcept { return false; }
  bool isComment() const noexcept { return true; }

  string str() const {
    if (isEmpty())
      return "###";
    else if (isMeta())
      return "#" + field + " " + value;
    else if (isRegion())
      return "##" + field + " " + value;
    else
      return "##" + field + " " + value;
  }

  friend std::ostream &operator<<(ostream &stream, const GFFComment &item) {
    return stream << item.str();
  }

  optional<Region> getRegion() const {
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

using gff_variant = variant<GFFComment, GFFRecord>;

class GFFReader {
 private:
  Files::FileReader reader;

  gff_variant process(const string &line) const {
    if (line.find_first_of('#') == 0)
      return GFFComment{line};
    else
      return GFFRecord{line};
  }

 public:
  GFFReader() = delete;
  GFFReader(const string &file_name) : reader{file_name} {}
  GFFReader(std::istream &stream) : reader{stream} {}

  optional<gff_variant> getItem(const string &skip = {}) {
    return (*this)(skip);
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
