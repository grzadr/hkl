#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <agizmo/evaluation.hpp>

#include <hkl/gff.hpp>

namespace TestHKL::TestGFF {

using std::string;

using namespace AGizmo;
using namespace Evaluation;
using namespace HKL::GFF;

using std::visit;

struct OutputGFF {
  size_t records{0};

  string str() const {
    sstream result;
    result << records;
    return result.str();
  }

  bool operator==(const OutputGFF &other) const {
    return this->records == other.records;
  }
};

class InputGFF {
private:
  string file_name;

public:
  InputGFF() = delete;
  InputGFF(string file_name) : file_name{file_name} {}

  string str() const { return "(" + file_name + ")"; }

  OutputGFF execute() const {
    GFFReader reader{file_name};
    while (auto line = reader("#")) {
      //      cerr << "READ: ";
      //      visit([](auto const &e) { cerr << e; }, *line);
      cerr << "\n";
      //      cerr << *line << "\n";
    }
    return {};
  }
};

class GFFTest : public BaseTest<InputGFF, OutputGFF> {
public:
  GFFTest() = default;
  GFFTest(InputGFF input, OutputGFF expected);

  string args() const { return this->input.str(); }

  string str() const noexcept {
    return "Outcome: " + outcome.str() + "\nExpected: " + expected.str();
  }

  bool validate() {
    this->outcome = input.execute();
    return this->setStatus(outcome == expected);
  }
};

Stats check_gffreader(bool verbose);

} // namespace TestHKL::TestGFF
