#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " FILE [FILE...]" << endl;
    return -1;
  }
  for (size_t i = 1; i < argc; ++i) {
    fstream fIn(argv[i], ios::in | ios::binary);
    if (!fIn.good()) {
      cerr << "ERROR: Cannot open '" << argv[i] << "'!" << endl;
      continue;
    }
    // make name
    string name = argv[i];
    name = name.substr(0, name.find('.'));
    /// @todo more sophisticated name mangling?
    // print preface
    cout << "struct { const unsigned char *data; int size; } " << name << " = {" << endl
      << "  \"";
    // print data
    const char hex[] = "0123456789abcdef";
    unsigned char byte;
    enum { BytesPerLine = 16 };
    size_t n = 0;
    for (unsigned char byte; fIn.get((char&)byte); ++n) {
      if (n && !(n % BytesPerLine)) cout << "\"\n  \"";
      cout << "\\x" << hex[byte / 16] << hex[byte % 16];
    }
    // print size
    cout << "\",\n"
      "  " << n << "\n"
      "};" << endl;
  }
  return 0;
}