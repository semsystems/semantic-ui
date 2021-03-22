#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "FileUtils.hpp"

using namespace std;

string FileUtils::getFileContent(string path)
{
  string json;
  ifstream in(path, std::ifstream::binary);

  ostringstream tmp;
  tmp << in.rdbuf();
  string s = tmp.str();
  s.erase(remove(s.begin(), s.end(), ' '), s.end());
  s.erase(remove(s.begin(), s.end(), '\n'), s.end());

  return s;
}
