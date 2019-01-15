#pragma once

#include <algorithm>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <agizmo/files.hpp>
#include <agizmo/printable.hpp>
#include <agizmo/strings.hpp>

namespace HKL::VCF {

using std::optional;
using std::string;
using std::vector;

using opt_str = optional<string>;
using opt_double = optional<double>;

using runerror = std::runtime_error;

using namespace AGizmo;

class VCFGenotype {
 public:
  VCFGenotype() = default;
};

class VCFRecord {
 private:
  string chrom;
  int pos = 0;
  vector<string> id;
  string ref;
  vector<string> alt;
  opt_double qual;
  vector<string> filter;
  vector<string> format;
  vector<VCFGenotype> genotypes;

  Printable::PrintableStrMap info{};

 public:
  VCFRecord() = default;
  VCFRecord(const string &line) {
    //      std::cerr << line << "\n";
    if (line.empty()) throw runerror{"Empty line"};
  }
};

class VCFHeader {
 private:
  vector<string> samples;
  size_t header_size{0};

 public:
  VCFHeader() = default;
  VCFHeader(const string &line) {
    auto fields = StringDecompose::str_split(line, '\t', true);
    header_size = fields.size();
    if (header_size > 9) {
      if (fields[8] != "FORMAT") throw runerror{"Header is malformed"};
      samples = vec_str(fields.begin() + 9, fields.end());
    } else if (fields.size() < 8 || fields.size() == 9)
      throw runerror{"Wrong number of columns"};
  }

  bool hasSamples() const { return !samples.empty(); }
  vector<string> getSamples() const { return samples; }
  size_t getSize() const { return header_size; }
};

class VCFComment {
 private:
  string field;
  string value;
  bool proper{false};

 public:
  VCFComment() = default;
  VCFComment(const string &line) {
    const auto &[field, value] =
        StringDecompose::str_split_in_half(line.substr(2), '=');
    if (field.empty() || value.empty())
      throw runerror{"Error in formatting of line:\n" + line};

    this->field = field;
    this->value = value;
    this->proper =
        (StringSearch::str_starts_with(value, "<ID=") && value.back() == '>');

    if (isProper()) this->value = this->value.substr(1, this->value.size() - 2);
  }

  string getField() const noexcept { return field; }
  string getValue() const noexcept { return value; }

  bool isProper() const noexcept { return proper; }

  string str() const { return field + "=" + value; }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const VCFComment &item) {
    return stream << item.str();
  }
};

using var_vcf = std::variant<VCFRecord, VCFComment, VCFHeader>;

class VCFReader {
 private:
  Files::FileReader reader;

  vector<string> samples_reference{};
  vector<int> samples_picked{};

  var_vcf process(const string &line) {
    if (line.find_last_of('#') == 0) {
      auto header = VCFHeader{line};
      if (header.hasSamples()) {
        samples_reference = header.getSamples();
        samples_picked =
            vector<int>{static_cast<int>(samples_reference.size())};
        std::iota(samples_picked.begin(), samples_picked.end(), 0);
      }
      return var_vcf{header};
    } else if (line.find_first_not_of('#') == 2)
      return VCFComment(line);
    else if (line.front() != '#')
      return VCFRecord{line};
    else
      throw runerror{"Oddly formatted line " +
                     std::to_string(reader.getLineNum())};
  }

 public:
  VCFReader() = delete;
  VCFReader(const string &file_name) : reader{file_name} {}
  VCFReader(std::istream &stream) : reader{stream} {}

  optional<var_vcf> getItem(const string &skip = {}) { return (*this)(skip); }

  optional<var_vcf> operator()(const string &skip = {}) {
    if (const auto line = reader(skip)) {
      if ((*line).empty())
        throw runerror{"Empty line " + std::to_string(reader.getLineNum())};
      else
        return process(*line);
    } else
      return nullopt;
  }

  void provideSamples(const string &samples) {
    provideSamples(StringDecompose::str_split(samples, ',', true));
  }

  void provideSamples(const vector<string> &samples) {
    samples_picked = {};

    if (samples.empty()) return;
    samples_picked.reserve(samples.size());
    auto samples_end = samples_reference.end();
    auto samples_begin = samples_reference.begin();

    for (const auto &ele : samples) {
      auto result = std::find(samples_begin, samples_end, ele);
      if (result == samples_end)
        throw runerror{"Sample " + ele + "not found in header"};
      else
        samples_picked.emplace_back(std::distance(samples_begin, result));
    }
  }

  bool hasSamples() const { return samples_picked.size(); }
};

}  // namespace HKL::VCF
