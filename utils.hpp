#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <future>
#include <algorithm>
#include <iterator>

using namespace std;


string strToLower(const string& a){
  string str(a);
  transform(str.begin(), str.end(), str.begin(), ::tolower);
  return str;
}

bool caseInsensitiveStringCompare(const string& a, const string& b){
  return (strToLower(a) == strToLower(b));
}

template<typename T>
bool isFutureReady(future<T>& f){
  return f.wait_for(chrono::seconds(0)) == future_status::ready;
}

vector<string> WSTokenizeString(string str){
  vector<string> out;
  istringstream iss(str);
  copy(istream_iterator<string>(iss), istream_iterator<string>(),back_inserter<vector<string>>(out));
  return out;
}
