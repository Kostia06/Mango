#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>

#define RED "\033[1;31m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

#define BUILTIN ".mango_builtin"

typedef struct {
    size_t usage_size;
    std::string usage;
    std::string description;
    std::function<void()> function;
    std::string bash;
} Command;

class Lexer{
    private:
        std::vector<std::string> save_arguments;
        std::vector<std::string> arguments;
        
        std::string cwd = "";

        size_t index = 0;
        size_t save_index = 0;
        size_t save_command_index = 0;

        void include_builtin();
        bool overwrite(std::string path);
        std::string read_content(std::string filename);
        // check if there is enought arguments
        bool enough_arguments(size_t amount){ return index + amount < arguments.size(); }
        // display error
        void error(std::string message){
            std::cout << RED << "error: " << WHITE << message << RESET << std::endl; 
            exit(0);
        }
        // check if the file path exists
        bool is_file(std::string path){ return is_path(path) && std::filesystem::is_regular_file(cwd+"/"+path); }
        // check if the directory path exists
        bool is_directory(std::string path){ return is_path(path) && std::filesystem::is_directory(cwd+"/"+path); }
        // check if the path exists
        bool is_path(std::string path){ return std::filesystem::exists(cwd+"/"+path); }
        // get current working directory
        std::string get_cwd(){ return (std::__fs::filesystem::current_path()).string(); }

        std::string clean_path(std::string& path); 
        void add_commands(std::vector<std::string> bash_code, size_t number_of_parameters);


        void help();
        void get_all_files();
        void change_cwd();
        void run_bash();
        void save();
        void save_exe();
        void save_command();
        void load();
        void load(std::string directory);

        std::map<std::string, Command> commands{
            {"@help", { 0,  "",                 "Gives a list of all useful commands",  [this](){ help(); }}},
            {"@f",    { 2, "<dir> <file type>", "Gets all files from <dir>",            [this](){ get_all_files(); }}},
            {"@ccwd", { 1, "<dir>",             "Change the current working directory", [this](){ change_cwd();   }}},
            {"@r",    { 0, "",                  "Runs the given bash code",             [this](){ run_bash(); }}},
            {"@l",    { 1, "<file>",            "Loads the given file",                 [this](){ load(); }}},
            {"@s",    { 1, "<as>",              "Saves the given bash code as a file from a current directory",     [this](){ save(); }}},
            {"@se",   { 1, "<as>",              "Saves the given bash code as a file from a executable directory",  [this](){ save_exe(); }}},
            {"@sc",   { 3, "<as> <number of commands> ... <description>","Saves the given bash code as a command",  [this](){ save_command(); }}},
        };

    public:
        Lexer(std::vector<std::string> argumensts);
        void parse();
};



