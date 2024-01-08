#include "include.h"

// check if the flag matches the string
int check_arg(char* file, char* flag, char* arg, int argc, int i, int n){
    if(strcmp(arg, flag)){ return 0;}
    if(i + n >= argc){
        printf("%sNot enough arguments for \"%s\"\n%s",RED, flag, RESET);
        return 0;
    }
    return 1;
}
// get current directory
char* get_current_dir(){
    char* cwd = malloc(sizeof(char) * 1024);
    getcwd(cwd, 1024);
    return cwd;
}
// check if the file exists
int is_file(char* path){
    FILE* file = fopen(path, "r");
    int result = file != NULL;
    fclose(file);
    return result;
}
// check if the directory exists
int is_dir(char* path){
    DIR *dir = opendir(path);
    if(dir == NULL) return 0;
    closedir(dir);
    return 1;
}
// check if the path exists
int is_path(char* path){ return is_file(path) || is_dir(path); }
// get all arguments after the flag
char** get_args(char** argv, int argc, size_t* i, size_t* new_args_size){
    char** args = malloc(sizeof(char*));
    size_t args_size = 0;
    while(*i < argc && !check_flag(argv[*i])){
        args = realloc(args, sizeof(char*) * (args_size + 1));
        args[args_size++] = argv[(*i)++];
    }
    if(*i < argc){ (*i)--; }
    *new_args_size = args_size;
    return args;
}
// check if item is a flag
int check_flag(char* flag){
    size_t j = 0;
    Flag f = flags[j];
    while(f.name != NULL){
        if(!strcmp(f.flag, flag)){ return 1; }
        f = flags[j++];
    }
    f = flags[0]; j = 0;
    while(f.name != NULL){
        size_t flag_len = strlen(f.flag);
        size_t arg_len = strlen(flag);
        if(!strcmp(f.flag, flag)){ return 1; }
        int k = 0, found = arg_len >= flag_len;
        while(k < flag_len && found){  
            if(flag[k] != f.flag[k]){ found = 0; break; }
            k++;
        }
        if(found){ return 1; }
        f = flags[++j];
    }
    return 0;
}
// check if the end of the string matches with any of the ends
static int check_end(const char* str, char** ends, size_t size){
    for(int i = 0; i < size; i++){
        size_t str_len = strlen(str);
        size_t end_len = strlen(ends[i]);
        if(!strcmp(str + str_len - end_len, ends[i])){ return 1; }
    }
    return 0;
}
// get all files based of the extension
char** get_files_exts(const char* path, char** exts, size_t exts_size){
    DIR *dir = opendir(path);
    // create files
    char** files = malloc(sizeof(char*));
    size_t files_size = 0;
    // get files
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_REG && check_end(entry->d_name, exts, exts_size)){
            // create file memory
            char* file = malloc(sizeof(char) * strlen(entry->d_name) + strlen(path) + 2);
            strcat(file, path);
            if(path[strlen(path) - 1] != '/'){ strcat(file, "/"); }
            strcat(file, entry->d_name);
            // add file to files
            files = realloc(files, sizeof(char*) * (files_size + 1));
            files[files_size++] = file;
        } 
        else if(entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            // create a new path
            char new_path[2048];
            if(path[strlen(path) - 1] == '/'){ snprintf(new_path, sizeof(new_path), "%s%s", path, entry->d_name); }
            else{ snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name); }
            // get files from new path
            char** dir_files = get_files_exts(strdup(new_path), exts, exts_size);
            // add files to files
            for(int i = 0; dir_files[i] != NULL; i++){
                // add file to files
                files = realloc(files, sizeof(char*) * (files_size + 1));
                files[files_size++] = dir_files[i];
            }
        }
    }
    closedir(dir);
    // add end of files
    files = realloc(files, sizeof(char*) * (files_size + 1));
    files[files_size] = NULL;
    return files;
}
// check if the string is in the array
int check_in(char* str, char** arr, size_t size){
    for(int i = 0; i < size; i++){
        if(!strcmp(str, arr[i])){ return i; } 
    }
    return -1;
}
// remove item from the array
void remove_item(char** arr, size_t* size, size_t index){
    for(size_t i = index; i < *size - 1; i++){ arr[i] = arr[i + 1]; }
    arr = realloc(arr, sizeof(char*) * (*size - 1));
    (*size)--;
    // move all of the items down
    memmove(arr + index, arr + index + 1, sizeof(char*) * (*size - index));
}
// combine two directories
char* combine_dir(char* dir_1, char* dir_2){
    char* dir = malloc(sizeof(char) * (strlen(dir_1) + strlen(dir_2) + 2));
    strcat(dir, dir_1);
    if(dir_1[strlen(dir_1) - 1] != '/'){ strcat(dir, "/"); }
    strcat(dir, dir_2);
    return dir;
}
// check two directories
char* get_dir(char* path){
    char* dir_1 = path;
    char* dir_2 = combine_dir(builder->dd, path);
    if(is_dir(dir_2)){ return dir_2; }
    if(is_dir(dir_1)){ return dir_1; }
    return NULL;
}
// get two file paths
char* get_file(char* path){
    char* dir_1 = path;
    char* dir_2 = combine_dir(builder->dd, path);
    if(is_file(dir_2)){ return dir_2; }
    if(is_file(dir_1)){ return dir_1; }
    return NULL;
}
// search for a file or directory
int search_path(char* dir,char* path){
    DIR *d = opendir(dir);
    if(d == NULL){ return 0; }
    struct dirent *entry;
    while((entry = readdir(d)) != NULL){
        if(!strcmp(entry->d_name, path)){ return 1; }
    }
    return 0;
}
// get time in milliseconds
long  get_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
// turns ms to a string
char* time_to_string(long time){
    int days    = 0;
    int hours   = 0;
    int minutes = 0;
    int seconds = 0;
    int ms      = time;
    while(ms - 1000 > 0   ){ ms -= 1000;    seconds++;  }
    while(seconds - 60 > 0){ seconds -= 60; minutes++;  }
    while(minutes - 60 > 0){ minutes -= 60; hours++;    }
    while(hours - 24 > 0  ){ hours -= 24;   days++;     }
    char* str = malloc(sizeof(char) * 100);
    if(days > 0){ sprintf(str, "%d days %d hours %d minutes %d seconds %d ms", days, hours, minutes, seconds, ms); }
    else if(hours > 0){ sprintf(str, "%d hours %d minutes %d seconds %d ms", hours, minutes, seconds, ms); }
    else if(minutes > 0){ sprintf(str, "%d minutes %d seconds %d ms", minutes, seconds, ms); }
    else if(seconds > 0){ sprintf(str, "%d seconds %d ms", seconds, ms); }
    else if(ms > 0){ sprintf(str, "%d ms", ms); }
    else{ sprintf(str, "0 ms"); }
    return str;
}
// combine array of strings into a string
char* into_string(char** array, size_t size){
    char* str = malloc(sizeof(char));
    for(int i = 0; i < size; i++){
        str = realloc(str, sizeof(char) * (strlen(str) + strlen(array[i]) + 1));
        strcat(str, array[i]);
        strcat(str, " ");
    }
    return str;
}
// check if the string is a valid flag
char** check_valid_flag(char* arg, char** argv, int* argc, size_t i){
    size_t j = 0;
    Flag f = flags[j];
    while(f.name != NULL){
        if(!strcmp(f.flag, arg)){ return argv; }
        f = flags[j++];
    }
    f = flags[0]; j = 0;
    while(f.name != NULL){
        size_t flag_len = strlen(f.flag);
        size_t arg_len = strlen(arg);
        int k = 0, found = arg_len >= flag_len;
        while(k < flag_len && found){  
            if(arg[k] != f.flag[k]){ found = 0; break; }
            k++;
        }
        if(found){
            // remove the begging of the arg by the flag size
            char* arg = argv[i] + flag_len;
            // add the flag to the argv
            argv = realloc(argv, sizeof(char*) * (*argc + 1));
            // from position i-1, move all of the items up by 2 
            argv[i] = malloc(sizeof(char) * strlen(f.flag) + 1);
            strcpy(argv[i], f.flag);
            memmove(argv + i + 1, argv + i, sizeof(char*) * (*argc - i));
            argv[i + 1] = arg;
            (*argc)++;
            return argv; 
        }
        f = flags[++j];
    }
    return argv;
}
// remove all ../ from the path      /Users/Kostia/Desktop/Programming/C/Builder/../../pop -> /Users/Kostia/Desktop/pop
char* get_direct_dir(char* path){
    size_t len = strlen(path);
    size_t  i = 0;
    char* new_path = malloc(sizeof(char));
    size_t new_path_size = 0;
    while(i < len){
        if(i != 0 && i+2 < len && path[i] == '.' && path[i+1] == '.' && path[i+2] == '/'){ 
            // remove the last directory
            if(new_path_size > 0 && new_path[new_path_size - 1] == '/'){ 
                new_path = realloc(new_path, sizeof(char) * (new_path_size - 1));
                new_path[new_path_size--] = '\0';
            } 
            while(new_path_size > 0 && new_path[new_path_size - 1] != '/'){ 
                new_path = realloc(new_path, sizeof(char) * (new_path_size - 1));
                new_path[new_path_size--] = '\0';
            }
            i += 3;
        }
        else{
            new_path = realloc(new_path, sizeof(char) * (new_path_size + 2));
            new_path[new_path_size++] = path[i++];
        }
    }

    return new_path;
} 
