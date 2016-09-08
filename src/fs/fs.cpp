#include "score/fs/fs.h"
#include <fstream>
#include <streambuf>
#include "score/macros.h"

using namespace std;

namespace score { namespace fs {

bool readFileSync(const string &filePath, string &result) {
  ifstream ifs {filePath.c_str()};
  ifs.seekg(0, ifs.end);
  result.reserve(ifs.tellg());
  ifs.seekg(0, ifs.beg);
  result.assign(istreambuf_iterator<char>(ifs),
    istreambuf_iterator<char>()
  );
  return true;
}

string readFileSync(const string &filePath) {
  string result;
  SCHECK(readFileSync(filePath, result));
  return result;
}

void writeFileSync(const string &fpath, const string &body) {
  ofstream ofs {fpath};
  ofs << body;
  ofs.close();
}

}} // score::fs

