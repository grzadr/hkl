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
using pGFFReader = std::unique_ptr<GFF::GFFReader>;

enum class Formats { TSV = 0, JSON = 1 };

class Parameters {
private:
  vector<string> input;
  optional<string> output;
  vector<string> keys;
  Formats format{Formats::TSV};
  string missing{"."};
  string empty{"true"};
  bool comments{false};

public:
  Parameters() = default;
  bool parse(int argc, char *argv[]);

  bool hasInput() const { return !input.empty(); }
  auto getInput() const { return input; }
  auto size() const { return input.size(); }
  auto getFormat() const { return format; }
  auto hasComments() const { return comments; }
  bool hasOutput() const { return output.has_value(); }
  auto getOutput() const { return output; }
  auto getMissing() const { return missing; }
  auto getEmpty() const { return empty; }

  auto begin() { return input.begin(); }
  auto end() { return input.end(); }
  auto cbegin() const { return input.cbegin(); }
  auto cend() const { return input.cend(); }
};

void gffile_to_tsv(vector<pGFFReader> &readers,
                   std::unique_ptr<std::ostream> &writer, const string &missing,
                   const std::string &empty, bool comments);

} // namespace GFF

} // namespace HKL
