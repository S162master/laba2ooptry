#include "fasade.h"
#include <fstream>
#include "jsonparser.h"

using namespace std;

bool fasade::interfaceBundle(enum compilation comp, string &str,  std::vector<std::string> &dataVector){
   bool result;
   switch (comp){
       case OPEN_FILE: result = openFile(str, dataVector); break;
       case CHECK: result = checkCorrect(dataVector, str); break;
   }
   return result;
}

bool fasade::openFile(string nameFile, vector <string> &dataVector){ // единая точка входа, такое уже делали
    fstream dataFile(nameFile);
    bool result = false;
    if(dataFile.is_open()){
        result = true;
        for(string line; getline(dataFile, line); dataVector.push_back(line));
        dataFile.close();
    }
    return result;
}

bool fasade::checkCorrect(vector <string> &dataVector, string &error){
    return jsonParser::checkCorrect(dataVector, error);
}
