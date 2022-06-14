#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <memory>
#include <vector>
#include <string>
#include <QString>

class jsonParser
{
private:
    struct Element{
    public:
        enum Type{
            STRING,
            NUMBER,
            OBJECT,
            ARRAY,
            EL_TRUE,
            EL_FALSE,
            EL_NULL,
        };
    private:
        Type type;
        //void *value;
        std::unique_ptr<void, void(*)(void*)> value;
    public:
        Element(Type type, std::unique_ptr<void, void(*)(void*)> value) : type(type), value(move(value)) {this->type = type;}
    };

    struct Object_field{
    private:
        std::string key;
        std::unique_ptr<Element> element;
    public:
        Object_field(std::string key, std::unique_ptr<Element> element) : key(key), element(move(element))  {}
    };

public:
    static bool checkCorrect(std::vector <std::string> &dataVector, std::string &error);
private:
    //статик по методам - метод может быть вызван от реализации класса и от копии (jsonParser:: нейм функшен)
    //статик методы внутри себя могут вызывать только статик методы
    static std::unique_ptr<std::string> scanNumber(std::stringstream& input);
    static std::unique_ptr<std::vector<std::unique_ptr<Object_field>>> scanObj(std::stringstream& input);
    static std::unique_ptr<Element> scanEl(std::stringstream &input);
    static std::unique_ptr<std::string> scanStr(std::stringstream& input);

};

#endif // JSONPARSER_H
