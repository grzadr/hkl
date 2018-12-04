#pragma once

#include <agizmo/args.hpp>
#include <hkl/gff.hpp>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace AGizmo;

namespace HKL {

namespace GFF {

using std::cerr;
using std::optional;
using std::string;
using std::vector;

enum class Formats { TSV = 0, JSON = 1 };

class Parameters {
 private:
  optional<string> input;
  optional<string> output;
  vector<string> keys;
  Formats format{Formats::TSV};
  string missing{"."};
  string empty{"true"};
  bool comments{false};

 public:
  Parameters() = default;
  void parse(int argc, char* argv[]);

  bool hasInput() const { return input.has_value(); }
  auto getInput() const { return input; }
  auto getFormat() const { return format; }
  auto hasComments() const { return comments; }
  bool hasOutput() const { return output.has_value(); }
  auto getOutput() const { return output; }
  auto getMissing() const { return missing; }
  auto getEmpty() const { return empty; }
};

void gffile_to_tsv(std::unique_ptr<GFFReader>& reader,
                   std::unique_ptr<std::ostream>& writer, const string& missing,
                   const std::string& empty, bool comments);

}  // namespace GFF

}  // namespace HKL
