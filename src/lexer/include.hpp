#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdlib.h>
#include <cstdlib>

using namespace std;
using namespace std::filesystem;

#define RED "\033[1;31m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

#define BUILTIN ".mango_builtin"
#define DEFAULT ".mango"

#define __edir(x) cwd + "/" + x

typedef struct {
    size_t usage_size;
    string usage;
    string description;
    function<void()> function;
    string bash;
} Command;

class Lexer{
    private:
        vector<string> save_arguments;
        vector<string> arguments;
        
        string cwd = "";

        size_t index = 0;
        size_t save_index = 0;
        size_t save_command_index = 0;

        void include_builtin();
        bool overwrite(string path);
        string read_content(string filename);
        // check if there is enought arguments
        bool enough_arguments(size_t amount){ return index + amount < arguments.size(); }
        // display error
        void error(string message){
            cout << RED << "error: " << WHITE << message << RESET << endl; 
            exit(0);
        }
        // check if the file path exists
        bool is_file(string path){ return is_path(path) && filesystem::is_regular_file(path); }
        // check if the directory path exists
        bool is_directory(string path){ return is_path(path) && filesystem::is_directory(path); }
        // check if the path exists
        bool is_path(string path){ return exists(path); }
        // get current working directory
        string get_cwd(){ return (__fs::filesystem::current_path()).string(); }

        string clean_path(string& path); 
        void add_commands(vector<string> bash_code, size_t number_of_parameters);

        vector<string> get_bash_directories();

        void help();
        void get_all_files();
        void change_cwd();
        void run_bash();
        void save();
        void save_exe();
        void save_command();
        void load();
        void load(string directory);

        map<string, Command> commands{
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
        Lexer(vector<string> argumensts);
        void parse();
};



