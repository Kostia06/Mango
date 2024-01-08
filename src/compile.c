#include "include.h"

int build(){
    if(builder->compiler == NULL){ 
        printf("%sERROR:%s No compiler set\n%s",RED,WHITE,RESET); 
        return 0; 
    }
    char* files = into_string(builder->files, builder->files_size);
    char* flags = into_string(builder->flags, builder->flags_size);
    char* command = malloc(
        sizeof(char) * 
        (
            strlen(builder->compiler) + 
            strlen(files) + 
            strlen(flags) + 
            strlen(builder->output) + 
            10
        )
    );
    sprintf(command, "%s %s %s -o %s", builder->compiler,files, flags, builder->output);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sCompiled successfully into \"%s\"%s\n",GREEN, builder->output, RESET); }
    else{ printf("%sFailed to compile%s\n",RED,RESET); return 0; }
    return 1;
}

int run(){
    char* run_flags = into_string(builder->run_flags, builder->run_flags_size);
    char* command = malloc(sizeof(char) * (strlen(builder->output) + strlen(run_flags) + 25));
    // create the command
    #ifdef _WIN32
        sprintf(command, "%s %s", builder->output, run_flags);
    #else
        if(builder->output[0] == '/') sprintf(command, "%s %s", builder->output, run_flags);
        else{sprintf(command, "./%s %s", builder->output, run_flags);}
    #endif
    // run the command
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sRan successfully%s\n",GREEN,RESET); }
    else{ printf("%sFailed to run%s\n",RED,RESET); return 0; }
    if(builder->del){
        char* command = malloc(sizeof(char) * strlen(builder->output) + 10);
        sprintf(command, "rm %s", builder->output);
        int result = system(command);
        if(result != -1 && result == 0){ printf("%sDeleted executable \"%s\"%s\n",GREEN,builder->output,RESET); }
        else{ printf("%sFailed to delete executable%s\n", RED,RESET); return 0; }
    } 
    return 1;
}

void save(){
    char* builder_path = combine_dir(builder->dd, BUILDER_NAME);
    FILE* f = fopen(builder_path, "w");
    fprintf(f, "%s", builder->cmd);
    fclose(f);
    printf("%sINFO: %sSaved build command to \"%s\"%s\n",YELLOW,WHITE, builder_path, RESET);
}


void run_default(char* builder_path, char* path){
    if(!is_dir(path)){
        printf("%sERROR:%s Directory \"%s\" not found%s\n",RED,WHITE, path, RESET);
        exit(1);
    }
    // loop through all of the files
    FILE* f = NULL;
    if(search_path(path, BUILDER_NAME)){ f = fopen(combine_dir(path,BUILDER_NAME), "r"); }
    else{ printf("%sERROR:%s No \"%s\" file found in %s%s\n",RED,WHITE, BUILDER_NAME,path, RESET); exit(1); }
    // read the entire file put into a single string without the new line
    char* line = malloc(sizeof(char));
    char c;
    size_t line_size = 0;
    while((c = fgetc(f)) != EOF){
        if(c == '\n'){ continue; }
        line = realloc(line, sizeof(char) * (line_size + 2));
        line[line_size++] = c;
    }
    line[line_size] = '\0';
    fclose(f);
    // get the compiler
    char* command = malloc(sizeof(char) * (strlen(builder_path) + strlen(line) + 2));
    sprintf(command, "%s %s", builder_path, line);
    system(command);
}
