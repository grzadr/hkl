#include <hkl/gfflatter.hpp>

#include <algorithm>
#include <memory>
#include <unordered_set>

using std::string;
using std::vector;

using namespace HKL;

using std::cerr;

int main(int argc, char* argv[]) {
  GFF::Parameters args{};

  try {
    args.parse(argc, argv);
  } catch (const runtime_error& ex) {
    std::cerr << ex.what() << "\n";
    return 1;
  }

  std::unique_ptr<GFF::GFFReader> reader{nullptr};

  if (const auto file_name = args.getInput())
    reader = std::make_unique<GFF::GFFReader>(*file_name);
  else
    reader = std::make_unique<GFF::GFFReader>(std::cin);

  std::unique_ptr<ostream> writer{nullptr};

  if (const auto file_name = args.getOutput())
    writer = std::make_unique<std::ofstream>(*file_name);
  else
    writer = std::make_unique<std::ostream>(std::cout.rdbuf());

  switch (args.getFormat()) {
    case GFF::Formats::TSV:
      GFF::gffile_to_tsv(reader, writer, args.getMissing(), args.getEmpty(),
                         args.hasComments());
      break;
    default:
      throw runtime_error{"Unsupported format"};
  }

  return 0;
}

void GFF::Parameters::parse(int argc, char* argv[]) {
  for (const auto& flag : Args::Arguments(argc, argv)) {
    if (const auto name = flag.getName(); name == "input" || name == "i") {
      if (flag.isEmpty())
        throw runtime_error{
            "Input file not specifed, but --input/-i flag set!"};
      else
        input = *flag.getValue();
    } else if (name == "format" || name == "f") {
      if (const auto value = flag.getValue()) {
        if (value == "tsv" || value == "TSV")
          format = Formats::TSV;
        else if (value == "json" || value == "JSON")
          format = Formats::JSON;
        else
          throw runtime_error{"Unrecognized format '" + *value + "'"};
      } else
        throw runtime_error{
            "Output format not specified, but --format/-f flag set!"};
    } else if (name == "comments" || name == "c") {
      if (flag.hasValue())
        throw runtime_error{"Values are not permitted with --comments flag"};
      comments = true;
    } else if (name == "output" || name == "o") {
      if (flag.isEmpty())
        throw runtime_error{
            "Output file not specifed, but --output/-o flag set!"};
      else
        output = *flag.getValue();
    } else if (name == "keys" || name == "k") {
      if (flag.isEmpty())
        throw runtime_error{"Keys not specifed, but --keys/-k flag set!"};
      else
        keys = StringDecompose::str_split(*flag.getValue(), '\t', true);
    } else if (name == "missing" || name == "m") {
      if (flag.isEmpty())
        throw runtime_error{
            "Missing value not specifed, but --missing/-m flag set!"};
      else
        missing = *flag.getValue();
    } else if (name == "empty" || name == "e") {
      if (flag.isEmpty())
        throw runtime_error{
            "Empty value not specifed, but --empty/-e flag set!"};
      else
        empty = *flag.getValue();
    } else
      throw runtime_error{"Unknown flag '" + name + "'"};
  }
}

void GFF::gffile_to_tsv(std::unique_ptr<GFF::GFFReader>& reader,
                        std::unique_ptr<std::ostream>& writer,
                        const string& missing, const string& empty,
                        bool comments) {
  string header = "seqid\tsource\ttype\tstart\tend\tscore\tstrand\tphase";
  std::unordered_set<string> keys_to_print;
  vector<GFF::GFFRecord> records;
  int counter = 0;

  while (const auto line = reader->getItem()) {
    if (++counter % 1000000 == 0) std::clog << counter << "\n";
    if ((*line).index() == 1) {
      auto record = std::get<GFF::GFFRecord>(*line);
      std::copy(record.keys_begin(), record.keys_end(),
                std::inserter(keys_to_print, keys_to_print.begin()));
      records.push_back(record);
    } else if (comments)
      std::visit([&writer](auto&& ele) { *writer << ele << "\n"; }, *line);
  }

  *writer << header << "\t"
          << StringCompose::str_join(keys_to_print.begin(), keys_to_print.end(),
                                     "\t")
          << "\n";
  counter = 0;
  for (const auto& record : records) {
    if (++counter % 1000000 == 0) std::clog << counter << "\n";
    *writer << record.strFields(missing, "\t")
            << record.strAttributes(keys_to_print.begin(), keys_to_print.end(),
                                    missing, "\t", empty)
            << "\n";
  }
}
