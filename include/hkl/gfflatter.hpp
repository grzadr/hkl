#pragma once

#include <agizmo/args.hpp>
#include <hkl/gff.hpp>

#include <iostream>
#include <optional>
#include <string>

using namespace AGizmo;

namespace HKL {

namespace GFF {

using std::cerr;
using std::optional;
using std::string;

enum class Formats { TSV = 0, JSON = 1 };

class Parameters {
 private:
  optional<string> file_name;
  Formats format{Formats::TSV};
  bool comments{false};

 public:
  Parameters() = delete;
  Parameters(const Args::Arguments& args) {
    for (const auto& flag : args) {
      if (const auto name = flag.getName(); name == "input" || name == "i") {
        if (flag.isEmpty())
          throw runtime_error{"File not specifed, but flag set!"};
        else
          file_name = *flag.getValue();
      } else if (name == "format" || name == "f") {
        if (const auto value = flag.getValue()) {
          if (value == "tsv" || value == "TSV")
            format = Formats::TSV;
          else if (value == "json" || value == "JSON")
            format = Formats::JSON;
          else
            throw runtime_error{"Unrecognized format '" + *value + "'"};
        } else
          throw runtime_error{"Output format not specified!"};
      } else if (name == "comments" || name == "c") {
        if (flag.hasValue())
          throw runtime_error{"Values are not permitted with --comments flag"};
        comments = true;
      }
    }
  }

  bool hasFile() const { return file_name.has_value(); }
  auto getFileName() const { return file_name; }
  auto getFormat() const { return format; }
  auto hasComments() const { return comments; }
};

Parameters parse_arguments(int argc, char* argv[]);

}  // namespace GFF

}  // namespace HKL
