#include <iostream>
#include <functional>
#include <future>
#include <map>
#include <string>
#include <chrono>
#include <vector>
#include <sstream>
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

int main(int argc, char** argv){
  if(argc > 1){
    string param(argv[1]);
    if(caseInsensitiveStringCompare(param,string("help"))){
      cout<<"Usage: "<<argv[0]<<endl;
      cout<<"  This is a test shell that uses c++11 features including lambdas and async futures."<<endl;
      return 0;
    }
  }

  map<string, function<void (vector<string>)>> commands;

  //example commands
  commands["test"] = [](vector<string> args){
    stringstream ss("\ntest success!\nargs:\n");
    for(string arg:args){ss<<"   "<<arg<<endl;}
    cout<<ss.str()<<flush;
  };
  commands["longtest"] = [](vector<string> args){
    auto start = chrono::system_clock::now();
    for(double i=0;i<5000;){i = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-start).count();}
    cout<<"\nlong test success!"<<endl;
  };

  commands["setuproot"] = [&commands](vector<string> args){
    commands["root"] = [](vector<string> args){cout<<"you are now root"<<endl;};
  };

  commands["deroot"] = [&commands](vector<string> args){
    if(commands.find("root") != commands.end()){
      commands.erase(commands.find("root"));
    }
  };


  vector<future<void>> tasks;

  commands["test"](WSTokenizeString("test this thing"));

  while(1){
    string cmd = "";
    cout<<"TestShell>";
    cin>>cmd;

    //break if user uses exit command
    if(caseInsensitiveStringCompare(WSTokenizeString(cmd)[0],"exit"))
      break;

    //clean tasks queue of finished tasks
    for(auto i = tasks.begin();i<tasks.end();i++){
      if(isFutureReady(*i))
	tasks.erase(i);
    }

    if(commands.find(strToLower(WSTokenizeString(cmd)[0])) == commands.end()){
      cout<<"No such command: "<<WSTokenizeString(cmd)[0]<<endl;
    }else{
      tasks.push_back(async(commands[strToLower(WSTokenizeString(cmd)[0])],WSTokenizeString(cmd)));
    }
  }

  return 0;
}
