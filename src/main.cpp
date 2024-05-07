#include <string>
#include <vector>
#include <iostream>

#include "lexer/include.hpp"

// gets all of the arguments
std::vector<std::string> get_arguments(int argc, char** argv){
    std::vector<std::string> arguments;
    for(int i = 0; i < argc; i++){
       arguments.push_back((std::string)argv[i]);
    }
    return arguments;
}

// running point
int main(int argc, char* argv[]){
    std::vector<std::string> arguments = get_arguments(argc, argv);
    Lexer* lexer = new Lexer(arguments); 
    lexer->parse();
    return 0;
}
