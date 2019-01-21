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

using vec_str = vector<string>;

using runerror = std::runtime_error;

using namespace AGizmo;

using map_str = Printable::PrintableStrMap;

class VCFGenotype {
 private:
  map_str data{};

 public:
  VCFGenotype() = delete;
  VCFGenotype(const vec_str &format, const string &query) {
    data = map_str{format, StringDecompose::str_split(query, ':', true)};
  }
};

class VCFRecord {
 private:
  string chrom;
  int pos_start = 0;
  int pos_end = 0;
  int pos_length = 0;
  vector<string> id;
  string ref;
  vector<string> alt;
  opt_double qual{};
  optional<vector<string>> filter;
  vector<string> format;
  vector<VCFGenotype> genotypes;

  map_str info{};

  int header_size = 0;

 public:
  VCFRecord() = delete;
  VCFRecord(const string &line) {
    std::cerr << line << "\n";
    if (line.empty()) throw runerror{"Empty line"};
    auto fields = StringDecompose::str_split(line, '\t', true);
    header_size = static_cast<int>(fields.size());

    if (header_size < 8)
      throw runerror{"Number of columns is lower that VCF standard specifies " +
                     std::to_string(header_size) + "< 8!"};

    auto iter = fields.begin();

    chrom = *iter++;
    if (chrom.empty() || chrom == ".")
      throw runerror{"CHROM column cannot be empty"};

    pos_start = std::stoi(*iter++);
    if (auto temp_id = *iter++; !temp_id.empty() && temp_id != ".")
      id = StringDecompose::str_split(temp_id, ',', true);

    ref = *iter++;
    if (ref.empty()) throw runerror{"Empty REF column"};

    pos_length = static_cast<int>(ref.size());
    pos_end = pos_start + pos_length - 1;
    alt = StringDecompose::str_split(*iter++, ',', true);
    if (auto temp_qual = *iter++; temp_qual == ".")
      qual = nullopt;
    else
      qual = std::stod(temp_qual);
    if (auto temp_filter = *iter++; temp_filter == ".")
      filter = nullopt;
    else if (temp_filter == "PASS")
      filter = vec_str{};
    else
      filter = StringDecompose::str_split(temp_filter, ',', true);
    info = map_str{*iter++, ';', '=', '"'};

    if (iter < fields.end()) {
      format = StringDecompose::str_split(*iter++, ':', true);

      for (; iter < fields.end(); ++iter) genotypes.emplace_back(format, *iter);
    }
  }

  auto getChrom() const { return chrom; }
  auto getStart() const { return pos_start; }
  auto getEnd() const { return pos_end; }
  auto getLength() const { return pos_length; }
  auto getRef() const { return ref; }
  auto getAlt() const { return alt; }
  auto getQual() const { return qual; }
  optional<int> getPass() const {
    if (auto pass = this->filter)
      return static_cast<int>(filter->size());
    else
      return nullopt;
  }
  auto getAlleles() const { return 1 + static_cast<int>(alt.size()); }
  auto getIDs() const { return id; }
  auto getFilters() const { return filter; }
  auto getInfo() const { return info; }
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
  int header_size = 0;

  vector<string> samples_reference{};
  vector<int> samples_picked{};

  var_vcf process(const string &line) {
    if (line.find_last_of('#') == 0) {
      auto header = VCFHeader{line};
      this->header_size = header.getSize();
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

  auto getHeaderSize() const { return header_size; }

  void provideSamples(const string &samples) {
    provideSamples(StringDecompose::str_split(samples, ',', true));
  }

  void provideSamples(const vector<string> &samples) {
    samples_picked = {};

    if (samples.empty()) return;

    if (!this->hasSamples())
      throw runerror{"VCF does not contain any samples!"};

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

  auto getSamplesReference() const { return samples_reference; }
  auto getSamplesPicked() const { return samples_picked; }
};

}  // namespace HKL::VCF
