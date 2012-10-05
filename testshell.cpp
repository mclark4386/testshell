#include <iostream>
#include <functional>
#include <future>
#include <map>
#include <string>
#include <chrono>
#include <vector>

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

int main(int argc, char** argv){
  if(argc > 1){
    string param(argv[1]);
    if(caseInsensitiveStringCompare(param,string("help"))){
      cout<<"Usage: "<<argv[0]<<endl;
      cout<<"  This is a test shell that uses c++11 features including lambdas and async futures."<<endl;
      return 0;
    }
  }

  map<string, function<void (void)>> commands;

  commands["test"] = [](){cout<<"\ntest success!"<<endl;};
  commands["longtest"] = [](){
    auto start = chrono::system_clock::now();
    for(double i=0;i<5000;){i = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-start).count();}
    cout<<"\nlong test success!"<<endl;
  };

  string cmd = "";

  vector<future<void>> tasks;

  while(1){
    cout<<"TestShell>";
    cin>>cmd;

    //break if user uses exit command
    if(caseInsensitiveStringCompare(cmd,"exit"))
      break;

    //clean tasks queue of finished tasks
    for(auto i = tasks.begin();i<tasks.end();i++){
      if(isFutureReady(*i))
	tasks.erase(i);
    }

    if(commands.find(strToLower(cmd)) == commands.end()){
      cout<<"No such command:"<<cmd<<endl;
    }else{
      tasks.push_back(async(commands[strToLower(cmd)]));
    }
  }

  return 0;
}
