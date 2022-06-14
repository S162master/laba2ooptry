#ifndef JSONERROR_H
#define JSONERROR_H

#include <iostream>

class jsonError
{
    std::string message;
public:
    jsonError();
    jsonError(std::string str);

    std::string toString();
    friend std::ostream& operator <<(std::ostream &os, jsonError &exc);
};
#endif // EXCEPTION_H
