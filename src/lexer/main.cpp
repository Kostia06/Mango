#include "include.hpp"

// lexer intilizer
Lexer::Lexer(std::vector<std::string> arguments){
    this->arguments = arguments;
    for(std::string argument : arguments){ save_arguments.push_back(argument); }
    cwd = get_cwd();
    if(is_file(BUILTIN)){ load(BUILTIN); }
}

// parse the arguments
void Lexer::parse(){
    while(index < arguments.size()){
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

// get simplified path Users/kostia/home/..
std::string Lexer::clean_path(std::string& path){
    // loop through the path string
    size_t i = 0;
    size_t last_path_change = 0; 
    while(i < path.size()){
        // trying to find ../ pattern
        if(i+2 < path.size() && path[i] == '/' && path[i+1] == '.' && path[i+2] == '.'){
            path.erase(path.begin()+last_path_change,path.begin()+ i+3);
            i = 0;
        } 
        if(path[i] == '/'){ last_path_change = i; }
        i++;
    }
    return path;
}
