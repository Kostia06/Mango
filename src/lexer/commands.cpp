#include "include.hpp"

// print all of the commands
void Lexer::help(){
    for(const auto& pair : commands) {
        std::cout << "\t" << pair.first << " " << pair.second.usage << " -> " << pair.second.description << std::endl;
    }
}
// get all of the files
void Lexer::get_all_files(){
    std::string directory = arguments[++index];
    if(!is_directory(directory)){ error("@f failed to find the <dir>"); }
    directory = cwd+"/"+directory;
    clean_path(directory);

    std::string type = arguments[++index];
    arguments.erase(arguments.begin() + index - 2, arguments.begin() + index+1);

    std::filesystem::path path(directory);
    int i = 0;
    index -= 2;

    for(const auto& entry : std::filesystem::recursive_directory_iterator(path)){
        if(std::filesystem::is_regular_file(entry)) {
            if (type.size() > entry.path().string().size()){ continue; };
            if(std::equal(type.rbegin(), type.rend(), entry.path().string().rbegin())){
                arguments.insert(arguments.begin() + index + i++, entry.path().string());
            }
        }
    }
}

// change the current working directory
void Lexer::change_cwd(){
    std::string directory = arguments[++index];
    arguments.erase(arguments.begin() + index - 1, arguments.begin() + index+1);
    index -= 1;
    if(!is_directory(directory)){ error("@ccwd failed to find <dir>"); }
    cwd = cwd+"/"+directory;
    clean_path(cwd);
}

// run the bash code
void Lexer::run_bash(){
    std::string code = "";
    int i = 0;
    while(i < index){ code += arguments[i++] + " "; }
    if(system(code.c_str())){ error("Bash Code failed to run"); }
    arguments.erase(arguments.begin() + index , arguments.begin() + index+1);
    index--;
}

// saves the bash code from a current path
void Lexer::save(){
    std::string directory = arguments[++index];
    arguments.erase(arguments.begin() + index - 1, arguments.begin() + index+1);
    // write the code to the file 
    std::ofstream file(cwd + "/" + directory);
    if(!file.is_open()){ error("Failed to open file"); }
    int i = save_index;
    for(; save_arguments[i] != "@s"; i++){ file << save_arguments[i] << " "; }
    file.close();
    save_index = i+2;
    index--;
} 

// saves the bash code from a executable path
void Lexer::save_exe(){
    std::string directory = arguments[++index];
    arguments.erase(arguments.begin() + index - 1, arguments.begin() + index+1);
    // write the code to the file 
    std::ofstream file(directory);
    if(!file.is_open()){ error("Failed to open file"); }
    int i = save_index;
    for(; save_arguments[i] != "@se"; i++){ file << save_arguments[i] << " "; }
    file.close();
    save_index = i+2;
    index--;
}

// loads the commands
void Lexer::load(){
    std::string directory = arguments[++index];
    arguments.erase(arguments.begin() + index - 1, arguments.begin() + index+1);
    index--;
    if(!is_file(directory)){ directory = cwd + "/" + directory; }
    if(!is_file(directory)){ error("Failed to find the file"); }
    std::string content = read_content(directory);
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
    index--;
}

// loads the commands
void Lexer::load(std::string directory){
    if(!is_file(directory)){ directory = cwd + "/" + directory; }
    if(!is_file(directory)){ error("Failed to find the file"); }
    std::string content = read_content(directory);
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
