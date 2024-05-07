#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>

#define RED "\033[1;31m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

#define BUILTIN ".mango_builtin"

typedef struct {
    size_t usage_size;
    std::string usage;
    std::string description;
    bool builtin;
    std::function<void()> function;
    std::string bash;
} Command;

class Lexer{
    private:
        std::vector<std::string> arguments;

        std::string cwd = ""

        size_t index = 1;

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
        bool is_file(std::string path){ return is_path(path) && std::filesystem::is_regular_file(path); }
        // check if the directory path exists
        bool is_directory(std::string path){ return is_path(path) && std::filesystem::is_directory(path); }
        // check if the path exists
        bool is_path(std::string path){ return std::filesystem::exists(path); }
        // get current working directory
        std::string get_cwd(){ return (std::__fs::filesystem::current_path()).string(); }
        // print all usefull commands
        void help(){
            for(const auto& pair : commands) {
                std::cout << "\t" << pair.first << " " << pair.second.usage << " -> " << pair.second.description << std::endl;
            }
        }

        void save_command();
        void get_all_files();

        std::map<std::string, Command> commands{
            {"@help", { 0,  "",                 "Gives a list of all useful commands",    true, [this](){ help(); }}},
            {"@sm",   { 1,  "<name>",           "Saves the current command as <name>",    true, [this](){ save_command(); }}},
            {"@f",    { 2, "<dir> <file type>", "Gets all files from <dir>",              true, [this]{}{ get_all_files(); }]}}
        };

    public:
        Lexer(std::vector<std::string> argumensts);
        void parse();
};



