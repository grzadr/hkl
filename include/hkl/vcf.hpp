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

class VCFAllele {
private:
  int position;
  opt_int allele{};
  opt_str seq{};
  opt_int depth{};

public:
  VCFAllele() = delete;
  VCFAllele(int position, const string &allele, const vec_str &allele_seq)
      : position{position} {

    if (allele != ".") {
      this->allele = std::stoi(allele);
      this->seq = allele_seq[static_cast<size_t>(*this->allele)];
    }
  }

  auto getPosition() const { return position; }
  auto getAllele() const { return allele; }
  auto getSeq() const { return seq; }
  auto hasAllele() const { return allele.has_value(); }
};

class VCFPhasedGenotype {
private:
  opt_str gt{};
  opt_str id{};

public:
  VCFPhasedGenotype() = default;
  VCFPhasedGenotype(const string &pgt, const string &pid) {
    this->gt = pgt.find('.') != string::npos ? nullopt : opt_str{pgt};
    this->id = pid == "." ? nullopt : opt_str{pid};
  }

  bool hasValue() const { return gt.has_value() && id.has_value(); }
  auto getGT() const { return gt; }
  auto getID() const { return id; }
};

class VCFGenotype {
private:
  map_str data{};
  vector<VCFAllele> alleles{};
  opt_str gt{};
  opt_int dp{};
  bool is_phased;
  VCFPhasedGenotype phased{};

public:
  VCFGenotype() = delete;
  VCFGenotype(const vec_str &format, const string &query,
              const vec_str &alleles_seq) {
    auto query_values = StringDecompose::str_split(query, ':', true);

    if (format.size() != query_values.size()) {
      if (format.back() == "PL" && format.size() - query_values.size() == 1)
        query_values.emplace_back(".");
    }

    data =
        map_str{vec_str(format.begin(),
                        format.begin() + static_cast<int>(query_values.size())),
                query_values};

    if (auto temp_gt = data.get("GT");
        temp_gt.has_value() && (*temp_gt)->find('.') == string::npos) {
      this->gt = *temp_gt;

      is_phased = (*temp_gt)->find('/') == string::npos;

      const auto vector_gt = StringDecompose::str_split(
          *(*temp_gt), (is_phased ? '|' : '/'), true);
      int pos = 0;

      for (const auto &gt : vector_gt)
        alleles.emplace_back(pos++, gt, alleles_seq);
    }

    if (auto temp_dp = data.get("DP");
        temp_dp.has_value() && (*temp_dp)->find('.') == string::npos)
      this->dp = std::stoi(*(*temp_dp));

    if (auto temp_gt = data.get("PGT"))
      phased = VCFPhasedGenotype{*(*temp_gt), *(*data.get("PID"))};
  }

  auto begin() const noexcept { return data.begin(); }
  auto end() const noexcept { return data.end(); }

  auto getAlleles() const { return alleles; }

  auto get(const string &key, const string &value,
           const string &empty = "") const {
    return data.get(key, value, empty);
  }
  auto get(const string &key) const { return data.get(key); }
  bool hasGT() const { return gt.has_value(); }
  auto getGT() const { return gt; }
  bool hasDP() const { return dp.has_value(); }
  auto getDP() const { return dp; }
  auto isPhased() const { return is_phased; }
  auto hasPhases() const { return phased.hasValue(); }
  auto getPhasedGT() const { return phased.getGT(); }
  auto getPhasedID() const { return phased.getID(); }
};

class VCFRecord {
private:
  string chrom;
  int pos_start = 0;
  int pos_end = 0;
  int pos_length = 0;
  vec_str id;
  //  string ref;
  //  vec_str alt;
  vec_str alleles;
  opt_double qual{};
  optional<vector<string>> filter;
  vector<string> format;
  vector<VCFGenotype> genotypes;

  map_str info{};

  int header_size = 0;

public:
  VCFRecord() = delete;
  VCFRecord(const string &line) {
    //    std::cerr << line << "\n";
    if (line.empty())
      throw runerror{"Empty line"};
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

    auto ref = *iter++;
    if (ref.empty())
      throw runerror{"Empty REF column"};

    pos_length = static_cast<int>(ref.size());
    pos_end = pos_start + pos_length - 1;
    alleles = StringDecompose::str_split(ref + "," + *iter++, ',', true);

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

      for (; iter < fields.end(); ++iter)
        genotypes.emplace_back(format, *iter, alleles);
    }
  }

  auto getChrom() const { return chrom; }
  auto getStart() const { return pos_start; }
  auto getEnd() const { return pos_end; }
  auto getLength() const { return pos_length; }
  auto getRef() const { return alleles[0]; }
  auto getAlt() const { return vec_str(alleles.begin() + 1, alleles.end()); }
  auto getQual() const { return qual; }
  optional<int> getPass() const {
    if (auto pass = this->filter)
      return static_cast<int>(filter->size());
    else
      return nullopt;
  }
  auto getAllelesCount() const { return static_cast<int>(alleles.size()); }
  auto countIDs() const { return static_cast<int>(id.size()); }
  auto getIDs() const { return id; }
  auto getFilters() const { return filter; }
  auto getInfo() const { return info; }
  auto getGenotypes() const { return genotypes; }
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
      if (fields[8] != "FORMAT")
        throw runerror{"Header is malformed"};
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

    if (isProper())
      this->value = this->value.substr(1, this->value.size() - 2);
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
      this->header_size = static_cast<int>(header.getSize());
      if (header.hasSamples()) {
        samples_reference = header.getSamples();
        samples_picked = vector<int>(samples_reference.size(), 0);
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

    if (samples.empty())
      return;

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

} // namespace HKL::VCF
