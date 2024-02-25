#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <ctype.h>

#define     LOCAL           ".mango"
#define     VERSION         "0.0.2"

#define     RESET   "\033[0m"
#define     WHITE   "\033[1m\033[37m"
#define     PINK    "\033[1m\033[35m"
#define     RED     "\033[1m\033[31m"
#define     GREEN   "\033[1m\033[32m"
#define     YELLOW  "\033[1m\033[33m"
#define     BLUE    "\033[1m\033[34m"
#define     CYAN    "\033[1m\033[36m"


static std::string default_code = 
"@git @args 1 git init && git checkout -b main && git remote add origin @0 && touch .gitignore @r @ "
"@push @args 1 git add . && git commit -m @0 && git push -u origin main @r @ "
"@pull @args 1 git pull  @r @ "
"@back git reset --hard HEAD@{1} @r git pull @r @ "
"@cp @args 2 cp @0 @1 @r @ "
"@fetch @args 2 git fetch @0 @1 @r @ "
;

class Command{
    private:
    public:
        size_t ignore = 0;
        std::vector<std::string> flags;
        void add(std::string str);
};
  

class Object{
    private:
    public:
        std::vector<std::string> flags;
        std::string name = "";
        size_t args_size = 0;
};


class Builder{
    private:
        std::vector<Object> objects;
        std::vector<std::string> current_flags;
        Command cmd, save_cmd;
        std::string dd;
        bool time = false;
        bool info = true;
        bool silent = false;
    public:
        Builder(int argc, char* argv[]);
        void run(std::vector<std::string> args);

        void help();
        void get_files(std::vector<std::string>& args, size_t& index);
        void run_code(std::vector<std::string>& args, size_t& index);
        void change_dd(std::vector<std::string>& args, size_t& index);
        void save(std::vector<std::string>& args, size_t& index);
        void run_file(std::vector<std::string>& args, size_t& index);
        void new_object(std::vector<std::string>& args, size_t& index);
};
