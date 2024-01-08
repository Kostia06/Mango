#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/time.h>

#define     BUILDER_NAME   ".mango"
#define     VERSION        "1.0.0"

#define     RESET   "\033[0m"
#define     WHITE   "\033[1m\033[37m"
#define     PINK    "\033[1m\033[35m"
#define     RED     "\033[1m\033[31m"
#define     GREEN   "\033[1m\033[32m"
#define     YELLOW  "\033[1m\033[33m"
#define     BLUE    "\033[1m\033[34m"
#define     CYAN    "\033[1m\033[36m"

#define ARG(arg, n) check_arg(exe, arg, argv[i], argc, i, n)
#define CALL(func) if(!func){ goto end_error; }
#define CMD_FLAG(arg) \
        if((check_flag(arg) || !strcmp(arg,"\n")) && !(!strcmp(arg, "-sr")) && !(!strcmp(arg, "-s"))){ \
            builder->cmd = realloc(builder->cmd, sizeof(char) * (strlen(builder->cmd) + strlen(arg) + 2)); \
            strcat(builder->cmd, arg); \
            strcat(builder->cmd, " "); \
        }

#define CMD_ARG(arg) \
        builder->cmd = realloc(builder->cmd, sizeof(char) * (strlen(builder->cmd) + strlen(arg) + 3)); \
        strcat(builder->cmd, "\""); \
        strcat(builder->cmd, arg); \
        strcat(builder->cmd, "\" "); \



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
    char* flag, *value, *name;
} Flag;

typedef struct{
    char* compiler, *output;
    char* dd, *cmd;
    char** files, **actual_files, **flags, **run_flags;
    size_t files_size, flags_size, run_flags_size;
    int run, save, time_build, time_run, del;
} Builder;

Builder* builder;
static int error = 0;

static Flag flags[30] = { 
    {"-h",    "",                                                 "help"},
    {"-v",    "",                                                 "version"},
    {"-o",    "<name>",                                           "output file name"},
    {"-c",    "<name>",                                           "change compiler"},
    {"-d",    "<dir>",                                            "change the default directory"},
    {"-s",    "",                                                 "save all commands to a .mango file"},
    {"-sr",   "<dir>",                                            "adds all of the flags to the builder from .mango"},
    {"-rs",   "",                                                 "remove save file"},
    {"-t",    "",                                                 "time the build and autorun"},
    {"-tr",  "",                                                  "time the run of the executable"},
    {"-tb",   "",                                                 "time the build"},
    {"-r",   "",                                                  "will run the executable after building it"},
    {"-af",   "<file>",                                           "add a file"},
    {"-aff",  "<flag>",                                           "add a flag"},
    {"-pf",   "",                                                 "print all of the files"},
    {"-pff",  "",                                                 "print all of the flags"},
    {"-de",   "" ,                                                "delete executable after autorun"},
    {"-f",    "<dir> <file extention 1> <file extention 2> ...",  "add files(s)"},
    {"-ff",   "<flag1> <flag2> ...",                              "add flag(s)"},
    {"-rf",   "<file1> <file2> ...",                              "remove file(s)"},
    {"-rff",  "<flag1> <flag2> ...",                              "remove flag(s)"},
    {"-arff", "<flag1> <flag2> ...",                              "add flag(s) to the autorun"},
    {"-ln",   "<from dir> <to dir> <name>",                       "link a folder to a directory"},
    {"-cp",   "<from dir> <to dir> <name>",                       "copy a folder to a directory"},
    {"-git",  "<dir> <SSH Git Link>",                             "set up git in the current directory"},
    {"-push", "<commit name>",                                    "push to git"},
    {"-pull", "",                                                 "pull from git"},
    {NULL,    NULL,                                               NULL}
};

// compile
int build();
int run();
void save();
void run_default(char* builder_path, char* path);
// commands
void help(char* file);
int change_default_dir(char** argv, int argc, size_t* i);
void time_everything();
void time_run();
int add_file(char** argv, int argc, size_t* i);
void add_flag(char** argv, int argc, size_t* i);
void print_files();
void print_flags();
int add_files(char** argv, int argc, size_t* i);
void add_flags(char** argv, int argc, size_t* i);
void remove_file(char** argv, int argc, size_t* i);
void remove_flag(char** argv, int argc, size_t* i);
void add_run_flag(char** argv, int argc, size_t* i);
int ln(char** argv, int argc, size_t* i);
int copy(char** argv, int argc, size_t* i);
int git_init(char** argv, int argc, size_t* i);
int git_push(char** argv, int argc, size_t* i);
int git_pull(char** argv, int argc, size_t* i);
char** add_save(char** argv, int* argc, size_t* i);
int remove_save_file(char** argv, int argc, size_t* i);
// utils
int check_arg(char* file, char* flag, char* arg, int argc, int i, int n);
char* get_current_dir();
int is_file(char* path);
int is_dir(char* path);
int is_path(char* path);
char** get_args(char** argv, int argc, size_t* i, size_t* new_args_size);
int check_flag(char* flag);
char** get_files_exts(const char* path, char** exts, size_t exts_size);
int check_in(char* str, char** arr, size_t size);
void remove_item(char** arr, size_t* size, size_t index);
char* combine_dir(char* dir_1, char* dir_2);
char* get_dir(char* path);
char* get_file(char* path);
int search_path(char* dir, char* path);
long  get_time();
char* time_to_string(long time);
char* into_string(char** array, size_t size);
char** check_valid_flag(char* arg, char** argv, int* argc, size_t i);
char* get_direct_dir(char* path);
