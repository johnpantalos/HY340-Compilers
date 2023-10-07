#include <iostream>
#include <list>
#include <stack>

typedef std::pair<unsigned, unsigned> linePair;

struct alpha_token_t
{
    unsigned    numLine;
    unsigned    numToken;
    std::string     content;
    std::string     type;
    alpha_token_t() {}
    alpha_token_t(unsigned numLine, unsigned numToken, std::string content, std::string type) :
                    numLine(numLine), numToken(numToken), content(content), type(type) {}
    void print_token(){
        std::cout
                << this->numLine
                << ":\t#"
                << this->numToken
                << "\t\""
                << this->content
                <<  "\"\t"
                << this->type
                << std::endl;
    }
};

static inline void replace_backslash(std::string& myString) {
    for(unsigned i = 0; myString[i]; ++i){
        if(myString[i] == '\\'){
            switch (myString[i+1])
            {
            case 'n':
                myString.replace(i, 2, "\n");
                break;
            case 't':
                myString.replace(i, 2, "\t");
                break;
            case '\\':
                myString.replace(i, 2, "\\");
                break;
            case '\"':
                myString.replace(i, 2, "\"");
                break;
            default:
                break;
            }
        }      
    }
}