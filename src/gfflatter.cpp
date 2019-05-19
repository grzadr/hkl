#include <hkl/gfflatter.hpp>

#include <algorithm>
#include <memory>
#include <unordered_set>

using std::string;
using std::vector;

using namespace HKL;

using std::cerr;

int main(int argc, char *argv[]) {
  GFF::Parameters args{};

  if (args.parse(argc, argv))
    return 1;

  std::unique_ptr<GFF::GFFReader> reader{nullptr};

  if (!args.hasInput())
    reader = std::make_unique<GFF::GFFReader>(std::cin);
  else if (args.size() == 1)
    reader = std::make_unique<GFF::GFFReader>(args.getInput().front());
  else
    return 1;

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

bool GFF::Parameters::parse(int argc, char *argv[]) {
  Args::Arguments args{"GFFlatter"};

  args.addMulti("input", "Input file in GFF format", 'i');
  args.addArgument("input", "Format of output", 'f', "tsv");
  args.addSwitch("comments", "Print comments", 'c');
  args.addArgument(
      "keys",
      "Get only these keys as columns. Values should be delimetered with ','.",
      'k');
  args.enableAppend("keys", ',');
  args.addArgument("missing", "Value for .", 'm', "true");
  args.addArgument("empty", "Value to use when columns is empty.", 'e', ".");

  if (args.parse(argc, argv))
    return 1;

  this->input = args.getIterable("input");

  if (const auto format = *args.getValue("format"); format == "tsv")
    this->format = Formats::TSV;
  else
    throw runtime_error{"Unrecognized format '" + format + "'"};

  this->comments = args.isSet("comments");
  this->output = args.getValue("output");
  this->keys = args.getIterable("keys");
  this->empty = *args.getValue("empty");
  this->missing = *args.getValue("missing");

  return 0;
}

void GFF::gffile_to_tsv(std::unique_ptr<GFF::GFFReader> &reader,
                        std::unique_ptr<std::ostream> &writer,
                        const string &missing, const string &empty,
                        bool comments) {
  string header = "seqid\tsource\ttype\tstart\tend\tscore\tstrand\tphase";
  std::unordered_set<string> keys_to_print;
  vector<GFF::GFFRecord> records;
  int counter = 0;

  while (const auto line = reader->getItem()) {
    if (++counter % 1000000 == 0)
      std::clog << counter << "\n";
    if ((*line).index() == 1) {
      auto record = std::get<GFF::GFFRecord>(*line);
      std::copy(record.keys_begin(), record.keys_end(),
                std::inserter(keys_to_print, keys_to_print.begin()));
      records.push_back(record);
    } else if (comments)
      std::visit([&writer](auto &&ele) { *writer << ele << "\n"; }, *line);
  }

  *writer << header << "\t"
          << StringCompose::str_join(keys_to_print.begin(), keys_to_print.end(),
                                     "\t")
          << "\n";
  counter = 0;
  for (const auto &record : records) {
    if (++counter % 1000000 == 0)
      std::clog << counter << "\n";
    *writer << record.strFields(missing, "\t")
            << record.strAttributes(keys_to_print.begin(), keys_to_print.end(),
                                    missing, "\t", empty)
            << "\n";
  }
}
