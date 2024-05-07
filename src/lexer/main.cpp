#include "include.hpp"

// lexer intilizer
Lexer::Lexer(std::vector<std::string> arguments){
    this->arguments = arguments;
    cwd = get_cwd();
    if(is_file(BUILTIN)){ include_builtin(); }
}

// include the builtin functions
void Lexer::include_builtin(){
    std::string content = read_content(BUILTIN);
    std::string argument = "";
    int i = 0;
    int size = 0;
    while(i < content.size()){
        if((content[i] == ' ' || content[i] == '\n') && argument != ""){
            arguments.insert(arguments.begin() + index + size++, argument);
            argument = "";
        }
        else if(!(content[i] == ' ' || content[i] == '\n')){ argument += content[i];}
        i++;
    }
}

// parse the arguments
void Lexer::parse(){
    while(index < arguments.size()){
        std::cout << arguments[index] << std::endl; 
        if(commands.find(arguments[index]) != commands.end()){
            Command command = commands[arguments[index]];
            // command doesnt have enough arguments
            if(!enough_arguments(command.usage_size)){
                error("Not enought arguments for " + arguments[index]);
            }

            if(command.builtin){ command.function(); }
            else{  }
        }
        index++;
    } 
}

// overwrite the path
bool Lexer::overwrite(std::string path){
    std::string input;
    std::cout << "Are you sure, you want to overwrite the path\"" << path << "\" Y/n? ";
    std::cin >> input;
    if(input == "Y" || input == "y"){
        if(is_file(path)){ std::filesystem::remove(path); }
        else if(is_directory(path)){ std::filesystem::remove_all(path); }
        return true;
    }
    return false;
}

// read the file
std::string Lexer::read_content(std::string filename){
    std::ifstream file(filename);
    std::string content;

    if(file.is_open()){
        std::string line;
        while(std::getline(file, line)){ content += line + "\n"; }
        file.close();
    }
    return content;
}
