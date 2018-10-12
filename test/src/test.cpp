#include "test.h"

string General::gen_framed(const string &message, size_t width, char frame) {

  string result{string(width, frame) + "\n"};
  string frame_string = string(3, frame);

  result += str_frame(message, width, frame_string, frame_string);

  return result + "\n" + string(width, frame);
}

string General::gen_pretty(const string &message, size_t width) {
  if (width % 2)
    ++width;

  string frame = string(width / 2, '<') + string(width / 2, '>');
  string result{frame};
  result += "\n" + str_frame(message, width, "<<<", ">>>");

  return result + "\n" + frame;
}

string General::gen_summary(int total, int failed, std::string type) {
  sstream message;

  message << type;

  if (failed)
    message << " failed in " << failed << "/" << total << " ("
            << std::setprecision(3) << 100.0 * failed / total << "%) cases!";
  else
    message << " succeeded in all " << total << " cases!";

  return message.str();
}

int General::perform_tests(bool verbose) {
  Stats stats;

  cout << gen_pretty("Starting HKL Evaluation") << "\n\n";

  stats(EvalComTol::eval_comtol(verbose));
  stats(EvalRegion::eval_region(verbose));
  stats(EvalRegionSeq::eval_regionseq(true));
  stats(EvalDOGIToys::eval_dogitoys(verbose));

  cout << gen_pretty(gen_summary(stats)) << endl;
  cout << "\n" << endl;

  return static_cast<int>(stats.failed);
}
