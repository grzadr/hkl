#include <hkl/gfflatter.hpp>
#include <memory>

using std::string;
using std::vector;

using namespace HKL;

using std::cerr;

int main(int argc, char* argv[]) {
  const auto args = GFF::parse_arguments(argc, argv);

  std::unique_ptr<GFF::GFFReader> reader{};

  if (const auto file_name = args.getFileName())
    std::make_unique<GFF::GFFReader>(*file_name);
  else
    std::make_unique<GFF::GFFReader>(std::cin);

  std::cerr << "Reading";

  while (const auto line = reader->getItem()) {
    cerr << (*line).index() << "\n";
    std::visit([](auto&& ele) { cerr << ele << "\n"; }, *line);
  }

  return 0;
}

GFF::Parameters GFF::parse_arguments(int argc, char* argv[]) {
  auto params = GFF::Parameters(Args::Arguments(argc, argv));

  return params;
}
