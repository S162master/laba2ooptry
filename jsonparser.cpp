#include "jsonparser.h"
#include <sstream>
#include <iostream>
#include "jsonError.h"

using namespace std;

bool jsonParser::checkCorrect(vector <string> &dataVector, string &error){ // проверка корректности
    bool result = true; // костыль
    string full; // с этим мы будем работать

    for(string &i : dataVector) // добавление переноса строки после каждого слова
        full += i + '\n';

    stringstream json_stream(full); // представляем слово, как поток ввода. Как будто руками вводим
    unique_ptr<Element> res; //умный итератор. Советую почитать в интернете
    try {
        res = scanEl(json_stream);//с чего начинается парсинг (может в себе вызвать себя)
    }  catch (jsonError &ex) {
        size_t n_s = json_stream.tellg();//сколько символов считали и вывод ошибок, если таковые есть
        n_s--;
        size_t i = 0;
        for(size_t s = 0; s <= n_s; i++){
           s += dataVector[i].size() + 1;
           cout << i << ' ' << s << ' ' << dataVector[i] << endl;
        }
        cout << n_s << endl;
        ostringstream os;
        os << "line - " << i << endl;
        error = os.str() + ' ' + ex.toString();
        result = false;
    }
    if(result == true){
        char ch;
        while(json_stream.get(ch)){
            if(ch != ' ' && ch != '\n' && ch != '\t'){ // если символ не стандартный, то ошибка
                result = false;
                size_t n_s = json_stream.tellg();
                n_s--;
                size_t i = 0;
                for(size_t s = 0; s <= n_s; i++){
                   s += dataVector[i].size() + 1;
                   cout << i << ' ' << s << ' ' << dataVector[i] << endl;
                }
                cout << n_s << endl;
                ostringstream os;
                os << "line - " << i << endl;
                os <<  "incorrect symbol after correct JSON:( - " << ch << endl;
                error = os.str();
                break;
            }
        }
    }
    return result;
}

unique_ptr<string> jsonParser::scanStr(stringstream& input){ // проверка строки целиком
    unique_ptr<string> result = make_unique<string>();
    string name = "scanStr: ";
    char c;
    while(input.get(c)){
        if(c != ' ' && c != '\n' && c != '\t'){ // по выводу в консоль, видно, что именно тут происходит
            break;
        }
    }
    if(c != '\"'){
        ostringstream os;
        os << name << "str does not start with \" - " << c << endl;
        throw jsonError(os.str());
    }
    while(input.get(c)){
        if(c == '\"'){
            return move(result);
        }
        if(c == '\\'){
            input.get(c);
        }
        (*result) += c;
    }
    ostringstream os;
    os << name << "str does not end" << endl;
    throw jsonError(os.str());
}

unique_ptr<jsonParser::Element> jsonParser::scanEl(stringstream &input){ // проверка элементов по отдельности
    //cout << "scanEl: " << endl;
    string name = "scanEl: ";
    unique_ptr<Element> result;
    char c;
    while(input.get(c)){
        if(c != ' ' && c != '\n' && c != '\t'){
            input.unget();
            break;
        }
    }
    if(c >= '0' && c <= '9'){//убедиться что всё число считает
        unique_ptr<string> number = scanNumber(input);
        unique_ptr<double> value = make_unique<double>();
        stringstream os(*(number));
        os >> *value;
        unique_ptr<void, void(*)(void*)> new_val(value.release(), [](void* ptr){delete (double*)ptr;});
        result = make_unique<Element>(Element::Type::NUMBER, move(new_val));
    } else {
        switch(c){
        case '\"':{
            unique_ptr<string> value;
            value = scanStr(input);
            unique_ptr<void, void(*)(void*)> new_val(value.release(), [](void* ptr){delete (string*)ptr;}); // выбор дальнейщего типа обработки по спецсимволу
            result = make_unique<Element>(Element::Type::STRING, move(new_val));
            break;
        }
        case '{':{
            unique_ptr<vector<unique_ptr<Object_field>>> value;
            value = scanObj(input);
            unique_ptr<void, void(*)(void*)> new_val(value.release(), [](void* ptr){delete (vector<unique_ptr<Object_field>>*)ptr;});
            result = make_unique<Element>(Element::Type::OBJECT, move(new_val));
            break;
        }
        case '[':{
            unique_ptr<vector<unique_ptr<Element>>> value = make_unique<vector<unique_ptr<Element>>>();
            char ch;
            input.get(ch);
            input.get(ch);
            if(ch == ' ' || ch == '\n' || ch == '\t'){
                while(input.get(ch)){
                    if(ch != ' ' && ch != '\n' && ch != '\t'){
                        break;
                    }
                }
            }
            if(ch != ']'){
                cout << "ne" << endl;
                input.unget();
                unique_ptr<Element> buffer;
                buffer = scanEl(input);
                value->push_back(move(buffer));
                while(input.get(ch)){
                    if(ch == ' ' || ch == '\n' || ch == '\t'){
                        while(input.get(ch)){
                            if(ch != ' ' && ch != '\n' && ch != '\t'){
                                break;
                            }
                        }
                    }
                    cout << '=' << ch << endl;
                    if(ch == ']'){
                        break;
                    }else if(ch != ',' && ch != '['){
                        ostringstream os;
                        os << name << "impostor symbol is among the element - " << ch << endl;
                        throw jsonError(os.str());
                    }
                    unique_ptr<Element> buffer;
                    buffer = scanEl(input);
                    value->push_back(move(buffer));
                }
                if(ch != ']'){
                    ostringstream os;
                    os << name << "array does not end" << endl;
                    throw jsonError(os.str());
                }
            }
            unique_ptr<void, void(*)(void*)> new_val(value.release(), [](void* ptr){delete (vector<unique_ptr<Element>>*)ptr;});
            result = make_unique<Element>(Element::Type::ARRAY, move(new_val));
            break;
        }
        case 't':{
            string value;
            input >> value;
            if(value == "true," || value == "true}"){
                input.unget();
            } else if(value != "true"){
                ostringstream os;
                os << name << "incorrect word after the t - " << value << endl;
                throw jsonError(os.str());
            }
            unique_ptr<void, void(*)(void*)> new_val(nullptr, [](void* ptr){});
            result = make_unique<Element>(Element::Type::EL_TRUE, move(new_val));
            break;
        }
        case 'f':{
            string value;
            input >> value;
            if(value == "false," || value == "false}"){
                input.unget();
            } else if(value != "false"){
                ostringstream os;
                os << name << "incorrect word after the f - " << value << endl;
                throw jsonError(os.str());
            }
            unique_ptr<void, void(*)(void*)> new_val(nullptr, [](void* ptr){});
            result = make_unique<Element>(Element::Type::EL_FALSE, move(new_val));
            break;
        }
        case 'n':{
            string value;
            input >> value;
            if(value == "null," || value == "null}"){
                input.unget();
            } else if(value != "null"){
                ostringstream os;
                os << name << "incorrect word after the n - " << value << endl;
                throw jsonError(os.str());
            }
            unique_ptr<void, void(*)(void*)> new_val(nullptr, [](void* ptr){});
            result = make_unique<Element>(Element::Type::EL_NULL, move(new_val));
            break;
        }
        default:{
            ostringstream os;
            os << name << "incorrect symbol - " << c << endl;
            throw jsonError(os.str());;
        }
        }
    }
    return move(result);//мув отдает право на владение указателем (умным) наверх (чтобы объект автоматически не очищался при выходе из функции
}

unique_ptr<vector<unique_ptr<jsonParser::Object_field>>> jsonParser::scanObj(stringstream& input){
    unique_ptr<vector<unique_ptr<Object_field>>> result = make_unique<vector<unique_ptr<Object_field>>>(); // в прошлой функции выбирали, какой объект обрабатывать. Здесь они именно обрабатываются
    char c;
    string name = "scanObj: ";
    while(input.get(c)){
        if(c == ' ' || c == '\n' || c == '\t'){
            while(input.get(c)){
                if(c != ' ' && c != '\n' && c != '\t'){
                    break;
                }
            }
        }
        if(c == '}'){
            return move(result);
        }else if(c != ',' && c != '{'){
            ostringstream os;
            os << name << "incorrect char after the key value pairs - " << c << endl;
            throw jsonError(os.str());
        }
        unique_ptr<string> key;
        key = scanStr(input);
        char ch;
        while(input.get(ch)){
            if(ch != ' ' && ch != '\n' && ch != '\t'){
                break;
            }
        }
        if(ch != ':'){
            ostringstream os;
            os << name << "incorrect char after the key - " << ch << endl;
            throw jsonError(os.str());
        }
        unique_ptr<Element> value;
        value = scanEl(input);
        unique_ptr<Object_field> obj_f = make_unique<Object_field>(*key, move(value));
        result->push_back(move(obj_f));
    }
    ostringstream os;
    os << name << "obj does not end" << endl;
    throw jsonError(os.str());
}

unique_ptr<string> jsonParser::scanNumber(stringstream& input){ // считывание числа
    unique_ptr<string> result = make_unique<string>();
    string name = "scanNumber: ";
    char c;
    while(input.get(c)){
        if(c != ' ' && c != '\n' && c != '\t'){
            break;
        }
    }
    while(input.get(c)){
        if(!((c >= '0' && c <= '9') || c == '.')){
            input.unget();
            (*result) += c;
            break;
        }
    }
    if((*result).size() > 0 && (*result)[0] == '0' && (*result).size() > 1 && (*result)[1] == '0'){
        ostringstream os;
        os << name << "array by 0:)." << endl;
        throw jsonError(os.str());
    }
    if((*result).size() == 0){
        ostringstream os;
        os << name << "number does not end" << endl;
        throw jsonError(os.str());
    }
    return move(result);
}
