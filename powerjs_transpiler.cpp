/*
  powerjs is an attempt to make javascript more readable and easier to maintain.
*/
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;
string version = "1.0.0";


bool endsWith(const string &mainStr, const string &toMatch)
{
    if(mainStr.size() >= toMatch.size() &&
            mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
            return true;
        else
            return false;
}

void introMessage () {
  cout << "powerjs transpiler c++ v1.0" << endl;
  cout << endl;
  cout << "options:" << endl;
  cout << " -translate,-t        translate powerjs files into one js file" << endl;
  cout << "      -name,-n        set name of powerjs file after done (def: index.powerjs)" << endl;
  cout << " -version             get translator version" << endl;
}

string replaceAll(string input, string toReplace, string replaceWith) {
  string outputText = input;
  while(outputText.find(toReplace) != string::npos) {
    size_t pos = input.find(toReplace);
    outputText = outputText.replace(pos, toReplace.length(), replaceWith);
  }
  return outputText;
}

string replaceFunctions(string strData) {
  strData = replaceAll(strData, "printl", "console.log");
  for(int first = 0, second = 11; second <= strData.length(); first++, second++) {
    if(strData.substr(first,second) == "promise() {" || strData.substr(first,second - 1) == "promise(){") {
      int functionNameEnd = first;
      int functionNameBegin;
      for(int functionNameInt = first - 2; functionNameInt >= 0; functionNameInt--) {
        if(functionNameInt == 0) {
          functionNameBegin = functionNameInt;
        } else if(strData[functionNameInt] == ' ') {
          functionNameBegin = functionNameInt;
          break;
        }
      }
      string functionName = strData.substr(functionNameBegin,functionNameEnd);
      string workAsync = functionName.substr(0, functionName.length() - 1) + "().then((promise)=> {";
      strData.replace(first - functionName.length(), second, workAsync);
    }
  }
  return strData;
}

string translateLibrary (string libData) {
  libData = replaceFunctions(libData);
  return libData;
}

int translateFile (string fileName, string compileName="index.js") {
  ifstream js(fileName);
  if(!js) {
    cout << "ERROR: " << fileName << " was not found" << endl;
    return -1;
  }
  vector<string>strVector;
  string str((istreambuf_iterator<char>(js)),
                istreambuf_iterator<char>());
  istringstream strStream(str);

  string fullString = "";

  string curlLine;
  while (getline(strStream,curlLine)) {
    if(!curlLine.empty())
      strVector.push_back(curlLine);
  }
  for(int i = 0; i < strVector.size(); i++) {
    string lineString = string(strVector[i]);
    if(lineString.substr(0,6) == "import") {
      if(lineString.size() == 6) {
        cout << "ERROR: an import was left without a library to account for" << endl;
        return -1;
      }
      string importName = lineString.substr(7) + ".powerjs";
      string originalName = lineString.substr(7);
      ifstream powerjsImport;
      powerjsImport.open(importName);
      if(!powerjsImport) {
        cout << fileName << " has an invalid import." << endl;
        cout << "check the file if there is no imports in current directory." << endl;
        cout << "ERROR: the import " << originalName << " isnt found!" << endl;
        return -1;
      } else {
        string importStr((istreambuf_iterator<char>(powerjsImport)),
                              istreambuf_iterator<char>());
        importStr = translateLibrary(importStr);
        lineString.replace(lineString.begin(),lineString.end(), importStr);
      }
    }
    lineString = replaceFunctions(lineString);
    fullString += lineString + "\n";
  }
  while(fullString.find("then((promise)=> {") != string::npos) {
    size_t pos = fullString.find("then((promise)=> {");
    int bracketCount = 1;
    for(int character = int(pos) + 19; character <= fullString.length(); character++) {
      if(bracketCount == 0) {
        fullString.replace(character - 1, 2, "})\n");
        break;
      }
      if(fullString[character] == '{') {
        bracketCount++;
      }
      if(fullString[character] == '}') {
        bracketCount--;
      }
    }
     fullString.replace(pos, string("then((promise) => {").length(), "then((promise) => {\n");
  }
  ofstream output(compileName);
  output << fullString;
  output.close();
  cout << "DONE\nTranslated all imports and " << fileName << " to: " << compileName << endl;
  return 0;
}

int main(int argc, char* argv[]) {
  if(argc == 1 ) {
    introMessage();
  }
  bool translatingFile = false;
  bool hasOptioned = false;
  string fileName = "";
  // cout << "index file: ";
  // cin >> fileName;
  if(argc > 1) {
    if(argv[1] == string("-translate") || argv[1] == string("-t")) {
      if(argc > 2) {
        if(endsWith(argv[2], string(".powerjs"))) {
          fileName = argv[2];
          string doneFile = "index.js";

          if(argc == 5 && argv[3] == string("-name")) {
            if(endsWith(argv[4], ".js")) {
              doneFile = argv[4];
            } else {
              doneFile = string(argv[4]) + ".js";
            }
          } else if(argc == 5 && argv[3] == string("-n")) {
            if(endsWith(argv[4], ".js")) {
              doneFile = argv[4];
            } else {
              doneFile = string(argv[4]) + ".js";
            }
          }

          int runStatus = translateFile(fileName, doneFile);
          if(runStatus < 0) {
            return 1;
          }
        } else {
          cout << "File must end with .powerjs extension!" << endl;
        }
        
        translatingFile = true;
      } else {
        cout << "ERROR: no powerjs file inputted" << endl;
      }
      hasOptioned = true;
    }
    if(argv[1] == string("-version")) {
      cout << "current version: " << version << endl;
      hasOptioned = true;
    }
  }
  if(!translatingFile && !hasOptioned && argc > 1) {
    introMessage();
  }
}
