/*------------------------------

  TestShell
  Author: Matthew Clark

  Feel free to use this for whatever. I'm not responsible for damage. 
  If you want to give me credit for something cool feel free.^_^

  ------------------------------*/

#include <iostream>
#include <functional>
#include <future>
#include <map>
#include <string>
#include <chrono>
#include <vector>
#include <mutex>
#include <utility>

#include "utils.hpp"

using namespace std;

mutex coutMutex;

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

    lock_guard<mutex> cm(coutMutex);
    cout<<ss.str()<<flush;
  };
  commands["longtest"] = [](vector<string> args){
    auto start = chrono::system_clock::now();
    for(double i=0;i<5000;){i = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now()-start).count();}

    lock_guard<mutex> cm(coutMutex);
    cout<<"\nlong test success!"<<endl;
  };

  commands["setuproot"] = [&commands](vector<string> args){
    commands["root"] = [](vector<string> args){lock_guard<mutex> cm(coutMutex);cout<<"you are now root"<<endl;};
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

    coutMutex.lock();
    cout<<"TestShell>";
    coutMutex.unlock();
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
      lock_guard<mutex> cm(coutMutex);
      cout<<"No such command: "<<WSTokenizeString(cmd)[0]<<endl;
    }else{
      tasks.push_back(async(commands[strToLower(WSTokenizeString(cmd)[0])],WSTokenizeString(cmd)));
    }
  }

  return 0;
}
