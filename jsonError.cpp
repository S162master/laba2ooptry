#include "jsonError.h"

//вывод ошибок, которые были найдены в процессе обработки

jsonError::jsonError()
{
    message = "error.";
}

jsonError::jsonError(std::string str){
    message = str;
}

std::ostream& operator <<(std::ostream &os, jsonError &exc){
    os << "exception: " << exc.message << std::endl;
    return os;
}

std::string jsonError::toString(){
    return message;
}
