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
    // iterate through the directory
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
    while(i < index){ 
        if(arguments[i].find(" ") != std::string::npos){ code += "\"" + arguments[i++] + "\" "; }
        else{ code += arguments[i++] + " "; }
    }
    if(system(code.c_str())){ error("Bash Code failed to run \""+code +"\""); }
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

// saves the bash code as a command
void Lexer::save_command(){
    std::string name = arguments[++index];
    // number of commands
    size_t number_of_parameters = 0;
    std::string number_of_parameters_str = arguments[++index];
    try{number_of_parameters = std::stoi(number_of_parameters_str); }
    catch(...){ error("Failed to convert the number of commands to a number"); }
    if(number_of_parameters < 0){ error("Number of commands should be greater than or equal to 0"); }
    // get the amount of commands
    if(!enough_arguments(number_of_parameters + 1)){ error("Not enough arguments for the number of commands and description for @"+name); }
    std::string parameters = "";
    for(size_t i = 0; i < number_of_parameters; i++){ parameters += "<" + arguments[++index] + "> "; }
    // get the description
    std::string description = arguments[++index];
    // get the bash code
    std::vector<std::string> bash_code;
    int i = save_command_index;
    for(; save_arguments[i] != "@sc"; i++){ 
        if(save_arguments[i][0] == '@'){
            std::string possible_parameter = save_arguments[i].substr(1);
            size_t parameter_index = 0;
            try{ parameter_index = std::stoi(possible_parameter); }
            catch(...){ error("Failed to convert the parameter to a number in @"+name); }
            if(parameter_index >= number_of_parameters){ error("Parameter index is greater than the number of parameters in @"+name); }
        }
        bash_code.push_back(save_arguments[i]); 
    }
    // remove the arguments
    arguments.erase(arguments.begin() + index - 3 - number_of_parameters - bash_code.size(), arguments.begin() + index+1);
    save_command_index = i+2;
    index -= 3 + number_of_parameters + bash_code.size(); 
    commands["@" + name] = (Command){ number_of_parameters, parameters, description, [this, bash_code, number_of_parameters](){ add_commands(bash_code, number_of_parameters); }};
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
        if(content[i] == '\"' && argument == ""){
            while(content[++i] != '\"'){ argument += content[i]; }
            arguments.insert(arguments.begin() + index + size++, "\""+argument+"\"");
        }
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
        if(content[i] == '\"' && argument == ""){
            while(content[++i] != '\"'){ argument += content[i]; }
            arguments.insert(arguments.begin() + index + size++, "\""+argument+"\"");
            argument = "";
            i++;
        }
        if((content[i] == ' ' || content[i] == '\n') && argument != ""){
            arguments.insert(arguments.begin() + index + size++, argument);
            argument = "";
        }
        else if(!(content[i] == ' ' || content[i] == '\n')){ argument += content[i];}
        i++;
    }

}
