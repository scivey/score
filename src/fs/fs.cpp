#include "score/fs/fs.h"
#include <fstream>
#include <streambuf>

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

}} // score::fs

