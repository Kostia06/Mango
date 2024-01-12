#include "include.h"

int build(){
    if(builder->show_cmd){ printf("%sINFO: %sRunning \"%s\"%s\n",YELLOW,WHITE, builder->cmd, RESET); }
    char* command = malloc(sizeof(char) * (strlen(builder->dd) + strlen(builder->cmd) + 10));
    sprintf(command, "cd %s && %s", builder->dd, builder->cmd);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sCompiled successfully%s\n",GREEN, RESET); }
    else{ printf("%sFailed to compile%s\n",RED,RESET); return 0; }
    return 1;
}

int run(){
    if(!builder->run || !is_file(combine_dir(builder->dd,builder->run))){ 
        printf("%sERROR:%s No executable to execute is set\n%s",RED,WHITE,RESET); 
        return 0; 
    }
    builder->run = get_direct_dir(combine_dir(builder->dd,builder->run));
    Variable* var = get_var("@ff");
    char* run_flags = "";
    if(var){ run_flags = var->value; }
    char* command = malloc(sizeof(char) * (strlen(builder->run) + strlen(run_flags) + 2));
    // create the command
    #ifdef _WIN32
        sprintf(command, "%s %s", builder->run, run_flags);
    #else
        if(builder->run[0] == '/'){sprintf(command, "%s %s", builder->run, run_flags);}
        else{sprintf(command, "./%s %s", builder->run, run_flags);}
    #endif
    if(builder->show_cmd){ printf("%sINFO: %sRunning \"%s\"%s\n",YELLOW,WHITE, command, RESET); } 
    // run the command
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sRan successfully%s\n",GREEN,RESET); }
    else{ printf("%sFailed to run%s\n",RED,RESET); return 0; }
    if(builder->del){ remove(builder->run); }
    if(builder->del && builder->show_cmd){ printf("%sINFO: %sDeleted \"%s\"%s\n",YELLOW,WHITE, builder->run, RESET); }
    return 1;
}

void save(){
    char* builder_path = combine_dir(builder->dd, BUILDER_NAME);
    FILE* f = fopen(builder_path, "w");
    fprintf(f, "%s", builder->cmd_save);
    fclose(f);
    printf("%sINFO: %sSaved build command to \"%s\"%s\n",YELLOW,WHITE, builder_path, RESET);
}


int run_default(char* builder_path, char* path){
    if(!is_dir(path)){
        printf("%sERROR:%s Directory \"%s\" not found%s\n",RED,WHITE, path, RESET);
        return 0;
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
    return 1;
}
