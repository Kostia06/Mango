#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>

#define     BUILDER_NAME   ".mango"

#define     RESET   "\033[0m"
#define     WHITE   "\033[1m\033[37m"
#define     MAGENTA "\033[1m\033[35m"
#define     RED     "\033[1m\033[31m"
#define     GREEN   "\033[1m\033[32m"
#define     YELLOW  "\033[1m\033[33m"
#define     BLUE    "\033[1m\033[34m"
#define     CYAN    "\033[1m\033[36m"

#ifdef __clang__
    #define COMPILER "clang"
#elif __GNUC__
    #define COMPILER "gcc"
#elif _MSC_VER
    #define COMPILER "msvc"
#else
    #define COMPILER NULL
#endif

#ifdef __windows__
    #define OS "windows"
#elif __linux__
    #define OS "linux"
#elif __unix__
    #define OS "unix"
#elif __APPLE__ || __MACH__
    #define OS "macos"
#else
    #define OS NULL
#endif

    
typedef struct{
    char* compiler, *output, **flags, **files, **run_flags, *default_directory, *cmd;
    size_t flags_size, files_size, run_flags_size;
    int autorun, time_build, time_run, delete_executable, save;
} Builder;

typedef struct{
    char* flag, *value, *name;
} Flag;

Builder builder;
static int error = 0;

static Flag flags[25] = { 
    {"-h",    "",                                                 "help"},
    {"-o",    "<name>",                                           "output file name"},
    {"-c",    "<name>",                                           "change compiler"},
    {"-af",   "<file>",                                           "add a file"},
    {"-aff",  "<flag>",                                           "add a flag"},
    {"-f",    "<dir> <file extention 1> <file extention 2> ...",  "add files(s)"},
    {"-ff",   "<flag1> <flag2> ...",                              "add flag(s)"},
    {"-rf",   "<file1> <file2> ...",                              "remove file(s)"},
    {"-rff",  "<flag1> <flag2> ...",                              "remove flag(s)"},
    {"-pf",   "",                                                 "print all of the files"},
    {"-pff",  "",                                                 "print all of the flags"},
    {"-ar",   "",                                                 "autorun on"},
    {"-de",   "" ,                                                "delete executable after autorun"},
    {"-arff", "<flag1> <flag2> ...",                              "add flag(s) to the autorun"},
    {"-tar",  "",                                                 "time the autorun"},
    {"-tb",   "",                                                 "time the build"},
    {"-t",    "",                                                 "time the build and autorun"},
    {"-cdd",  "<dir>",                                            "change the default directory"},
    {"-s",    "",                                                 "save all commands to a .mango file"},
    {"-ln",   "<from dir> <to dir> <name>",                       "link a folder to a directory"},
    {"-cp",   "<from dir> <to dir> <name>",                       "copy a folder to a directory"},
    {"-git",  "<dir> <SSH Git Link>",                             "set up git in the current directory"},
    {"-push", "<commit name>",                                    "push to git"},
    {"-pull", "",                                                 "pull from git"},
    {NULL,    NULL,                                               NULL}
};


// prints all of the flags and their descriptions
void    help(char* file);
// check if the argument is valid
int     check_arg(char* file, char* flag, char* arg, int argc, int i, int n);
// add a file to the builder
void    add_file(char* file);
// add a flag to the builder
void    add_flag(char* flag);
// add to cmd
void    add_cmd(char* cmd);
// add a flag to the run flags
void    add_run_flag(char* flag);
// add everything into one string
char*   into_string(char** array, size_t size);
// check if string is in string array
int     check_in(char* str, char** arr, size_t size);
// check if directory exists
int     check_dir(const char* path);
// get the current directory
char*   get_current_dir();
// combine two direcotries
char* combine_dir(const char* dir1, const char* dir2);
// get all of the argumnets until a flag
char**  get_args(char** argv, int argc, int i, size_t* new_args_size);
// check if string is valid based of extentions
int     check_end(const char* str, char** ends, size_t size);
// check if file exists
int     check_file(const char* path);
// check if string is a flag
int     check_flag(char* flag);
// get files based of extentions
char**  get_files_exts(const char* path, char** exts, size_t exts_size);
// get current time in miliseconds
long    get_time();
// turn time into a string
char*   time_to_string(long time);
// remove part of a string based of a other string
char*   str_remove(char *str, const char *sub);
// search for a file in a current directory
int     search_file(const char* path, const char* file);
// get correct path
char*   get_dir(char* path);
