#include "include.h"


void help(char* file){
    printf("%sMango Help\n", WHITE);
    printf("Usage: mango [options]\n");
    printf("Options:\n");
    size_t i = 0;
    Flag flag = flags[i];
    
    size_t count = strlen("<directory>")  + 1;
    size_t spaces = 60 - count < 0 ? 0 : 60 - count;
    printf("\t%s <directory>", file);
    for(size_t j = 0; j < spaces; j++){ printf(" "); }
    printf("Runs .mango file in the directory\n");
    while(flag.name != NULL){
        size_t count = strlen(flag.flag) + strlen(flag.value)  + 2;
        size_t spaces = 60 - count < 0 ? 0 : 60 - count;
        printf("\t%s %s %s",file, flag.flag, flag.value);
        for(size_t j = 0; j < spaces; j++){ printf(" "); }
        printf("%s\n", flag.name);
        flag = flags[++i];
    }
    printf("%s", RESET);
}


int check_arg(char* file, char* flag, char* arg, int argc, int i, int n){
    if(strcmp(arg, flag)){ return 0;}
    if(i + n >= argc){
        printf("%sNot enough arguments for \"%s\"\n%s",RED, flag, RESET);
        help(file);
        exit(1);
    }
    return 1;
}


void add_file(char* file){
    builder.files = realloc(builder.files, sizeof(char*) * (builder.files_size + 1));
    builder.files[builder.files_size++] = file;
}


void add_flag(char* flag){
    builder.flags = realloc(builder.flags, sizeof(char*) * (builder.flags_size + 1));
    builder.flags[builder.flags_size++] = flag;
    add_cmd(flag);
}


void add_cmd(char* cmd){
    builder.cmd = realloc(builder.cmd, sizeof(char) * (strlen(builder.cmd) + strlen(cmd) + 1));
    strcat(builder.cmd, cmd);
    strcat(builder.cmd, " ");
}


void   add_run_flag(char* flag){
    builder.run_flags = realloc(builder.run_flags, sizeof(char*) * (builder.run_flags_size + 1));
    builder.run_flags[builder.run_flags_size++] = flag;
    add_cmd(flag);
}

char* into_string(char** array, size_t size){
    char* str = malloc(sizeof(char));
    for(int i = 0; i < size; i++){
        str = realloc(str, sizeof(char) * (strlen(str) + strlen(array[i]) + 1));
        strcat(str, array[i]);
        strcat(str, " ");
    }
    return str;
}

int check_in(char* str, char** arr, size_t size){
    for(int i = 0; i < size; i++){ if(!strcmp(str, arr[i])){ return i; } }
    return -1;
}


int check_dir(const char* path){
    DIR *dir = opendir(path);
    if(dir == NULL) return 0;
    closedir(dir);
    return 1;
}

char* get_current_dir(){
    char* cwd = malloc(sizeof(char) * 1024);
    getcwd(cwd, 1024);
    return cwd;
}

char* combine_dir(const char* dir1, const char* dir2){
    char* dir = malloc(sizeof(char) * (strlen(dir1) + strlen(dir2) + 2));
    strcat(dir, dir1);
    if(dir1[strlen(dir1) - 1] != '/'){ strcat(dir, "/"); }
    strcat(dir, dir2);
    return dir;
}

char** get_args(char** argv, int argc, int i, size_t* new_args_size){
    char** args = malloc(sizeof(char*));
    size_t args_size = 0;
    while(i < argc && !check_flag(argv[i])){
        args = realloc(args, sizeof(char*) * (args_size + 1));
        args[args_size++] = argv[i++];
    }
    *new_args_size = args_size;
    return args;
}


int check_end(const char* str, char** ends, size_t size){
    for(int i = 0; i < size; i++){
        size_t str_len = strlen(str);
        size_t end_len = strlen(ends[i]);
        if(!strcmp(str + str_len - end_len, ends[i])){ return 1; }
    }
    return 0;
}


int check_file(const char* path){
    FILE* file = fopen(path, "r");
    if(file == NULL) return 0;
    fclose(file);
    return 1;
}


int check_flag(char* flag){
    size_t i = 0;
    Flag f = flags[i];
    while(f.name != NULL){
        if(!strcmp(flag, f.flag)){ return 1; }
        f = flags[++i];
    }
    return 0;
}


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
                // create file memory
                char* file = malloc(sizeof(char) * strlen(dir_files[i]) + strlen(new_path) + 1);
                strcat(file, dir_files[i]);
                // add file to files
                files = realloc(files, sizeof(char*) * (files_size + 1));
                files[files_size++] = file;
            }
        }
    }
    closedir(dir);
    // add end of files
    files = realloc(files, sizeof(char*) * (files_size + 1));
    files[files_size] = NULL;
    return files;
}


long   get_time(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

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

char *str_remove(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len > 0) {
        char *p = str;
        while ((p = strstr(p, sub)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return str;
}
