#ifndef FASADE_H
#define FASADE_H
#include <iostream>
#include <string>
#include <vector>
#include "jsonparser.h"

class fasade
{
public:
    enum compilation{
       OPEN_FILE,
       CHECK,
    };

    static bool interfaceBundle(enum compilation comp, std::string &str,  std::vector<std::string> &dataVector);
private:
    jsonParser* validator;
    static bool openFile(std::string nameFile, std::vector <std::string> &dataVector);
    static bool checkCorrect(std::vector <std::string> &dataVector, std::string &error);
};

#endif // FASADE_H
