#include "include.h"

// all of the flags
Variable* get_var(char* name){
    for(size_t i = 0; i < builder->vars_size; i++){
        if(!strcmp(builder->vars[i]->name, name)){ return builder->vars[i]; }
    }
    return NULL;
}
// create new variable
Variable* new_var(char* name){
    Variable* var = malloc(sizeof(Variable));
    var->name = name;
    var->value = malloc(sizeof(char));
    builder->vars = realloc(builder->vars, sizeof(Variable*) * (builder->vars_size + 1));
    builder->vars[builder->vars_size++] = var;
    return var;
}
// import all of the flags from var
static char** import_flags(Variable* var, char** argv, int* argc, size_t* i){
    char** args = malloc(sizeof(char*));
    size_t args_size = 0;
    size_t size = strlen(var->value);
    size_t j = 0;
    while(j < size){
        char* word = malloc(sizeof(char));
        size_t word_size = 0;
        if(var->value[j] == ' '){ j++; continue; }
        else if(var->value[j] == '\"'){
            j++;
            while(j < size && var->value[j] != '\"'){
                word = realloc(word, sizeof(char) * (word_size + 2));
                word[word_size++] = var->value[j++];
            }
            j++;
        }
        else{
            while(j < size && var->value[j] != ' ' && var->value[j] != '\"'){
                word = realloc(word, sizeof(char) * (word_size + 2));
                word[word_size++] = var->value[j++];
                if(j+1 < size && var->value[j] == '\\' && var->value[j+1] == '\"'){
                    word = realloc(word, sizeof(char) * (word_size + 2));
                    word[word_size++] = var->value[j++];
                }
            } 
        }
        word = realloc(word, sizeof(char) * (word_size + 2));
        word[word_size] = '\0';
        args = realloc(args, sizeof(char*) * (args_size + 1));
        args[args_size++] = word;
    }
    // from point i move the args_size amount of item to the right in argv
    argv = realloc(argv, sizeof(char*) * (*argc + args_size - 1));
    for(size_t j = *argc; j > *i; j--){
        argv[j + args_size - 1] = argv[j];
    }
    for(size_t j = 0; j < args_size; j++){
        argv[*i + j] = args[j];
    }
    *argc += args_size - 1;
    *i += 1;
    return argv;
}
// create a new variable
Variable* create_new_variabe(char** argv, int* argc, size_t* i){
    Variable* var = new_var(argv[*i]);
    int found = 0;
    while(*i < *argc){
        var->value = realloc(var->value, sizeof(char) * (strlen(var->value) + strlen(argv[*i]) + 2));
        strcat(var->value, argv[*i]);
        strcat(var->value, " ");
        SAVE(argv[*i]);
        if(!strcmp(argv[*i], "@end")){ found = 1; break; }
        (*i)++;
    }
    if(!found){
        printf("%sERROR:%s Expected \"@end\" in \"%s\"%s\n",RED,WHITE,var->name,RESET);
        return NULL;
    }
    return var;
}
// handle if there is a unknown flag
char** handle_var(char** argv, int* argc, size_t* i, int* result){
    Variable* var = get_var(argv[++(*i)]); 
    if(!var){ 
        var = create_new_variabe(argv, argc, i); 
        *result = var ? 1 : 0;
        if(!var){ return NULL; }
    }
    else{ return import_flags(var, argv, argc, i);}
    return NULL;
}
// print all of the flags and how to use them
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
// change the default directory
int change_default_dir(char** argv, int argc, size_t* i){
    char* dir1 = argv[++(*i)];
    char* dir = get_dir(dir1);
    if(!dir){
        printf("%sERROR:%s \"%s\" is not a directory%s\n",RED,WHITE, dir1, RESET);
        return 0;
    }
    builder->dd = get_direct_dir(dir);
    SAVE(dir);
    return 1;
}
// time everything
void time_everything(){
    if(!builder->run){
        printf("%sWARNING:%s Autorun is not enabled, meaning the run time of the executable won't show\n%s",PINK,WHITE,RESET);
    }
    builder->time_build = 1;
    builder->time_run = 1;
}
// time run
void time_run(){
    if(!builder->run){
        printf("%sWARNING:%s Autorun is not enabled, meaning the run time of the executable won't show\n%s",PINK,WHITE,RESET);
    }
    builder->time_run = 1;
}
// add file to the builder
int add_file(char** argv, int argc, size_t* i){
    size_t size = 0;
    (*i)++;
    char** files = get_args(argv, argc, i, &size);
    int result =1;
    for(size_t j = 0; j < size; j++){
        if(!is_file(files[j])){
            printf("%sERROR:%s \"%s\" is not a file%s\n",RED,WHITE, files[j], RESET);
            result = 0;
        }
        else{
            SAVE(get_direct_dir(files[j]));
            CMD(get_direct_dir(files[j])); 
        }
    }
    return result;
}
// add all files based of extension
int add_files(char** argv, int argc, size_t* i){
    char* dir1 = argv[++(*i)];
    char* dir = get_dir(dir1);
    if(!is_dir(dir)){
        printf("%sERROR:%s \"%s\" is not a directory%s\n",RED,WHITE, dir1, RESET);
        return 0;
    }
    size_t ext_size = 0;
    (*i)++;
    char** extensions = get_args(argv, argc, i, &ext_size);
    char** files = get_files_exts(dir, extensions, ext_size);
    for(size_t j = 0; files[j] != NULL; j++){ 
        char* file = files[j];
        CMD(file);
    }
    SAVE(get_direct_dir(dir1));
    for(size_t j = 0; j < ext_size; j++){ SAVE(extensions[j]); }
    return 1;
}
// add flags to the executable on run
int add_run_flag(char** argv, int argc, size_t* i){
    if(!builder->run){
        printf("%sWARNING:%s Autorun is not enabled, meaning the run flags won't be added%s\n",PINK,WHITE,RESET);
    }
    (*i)++;
    Variable* var = get_var("@ff");
    if(!var){ var = new_var("@ff"); }
    char** flags = malloc(sizeof(char*));
    size_t flags_size = 0, found = 0;
    while(*i < argc){
        char* flag = malloc(sizeof(char) * (strlen(argv[*i]) + 1));
        strcpy(flag, argv[(*i)++]);
        if(!strcmp(flag, "@end")){ found = 1;break; }
        flags = realloc(flags, sizeof(char*) * (flags_size + 1));
        flags[flags_size] = flag;
        SAVE(flags[flags_size]);
        flags_size++;
    }
    if(!found){
        printf("%sERROR:%s Expected \"@end\" in \"@ff\"%s\n",RED,WHITE,RESET);
        return 0;
    }
    SAVE("@end");
    for(size_t j = 0; j < flags_size; j++){    
        var->value = realloc(var->value, sizeof(char) * (strlen(var->value) + strlen(flags[j]) + 2));
        strcat(var->value, flags[j]);
        strcat(var->value, " ");
    }
    *i -= 1;
    return 1;
}
// link folder to the default directory
int ln(char** argv, int argc, size_t* i){
    char* from = get_dir(argv[++(*i)]);
    char* to = get_dir(argv[++(*i)]);
    char* name = argv[++(*i)];
    int result = 1;
    if(!from){
        printf("%sERROR:%s From Folder \"%s\" not found%s\n",RED,WHITE, argv[*i-2], RESET);
        result =  0;
    }
    if(!to){
        printf("%sERROR:%s To Path \"%s\" not found%s\n",RED,WHITE, argv[*i-1], RESET);
        result =  0;
    }
    if(!result){ return 0; }
    to = combine_dir(to, name);
    symlink(from, to);
    SAVE(from);
    SAVE(to);
    SAVE(name);
    printf("%sLinked:%s %s%s\n",YELLOW,WHITE,from,RESET);
    return 1;
}
// copy
int copy(char** argv, int argc, size_t* i){
    char* from_dir = get_dir(argv[++(*i)]);
    char* from_file = get_file(argv[*i]);
    char* to = get_dir(argv[++(*i)]);
    char* name = argv[++(*i)];
    int result = 1;
    char* from = from_dir ? from_dir : from_file;
    if(!from){
        printf("%sERROR:%s From Folder \"%s\" not found%s\n",RED,WHITE, argv[*i-2], RESET);
        result =  0;
    }
    if(!to){
        printf("%sERROR:%s To Path \"%s\" not found%s\n",RED,WHITE, argv[*i-1], RESET);
        result =  0;
    }
    if(!result){ return 0; }
    to = combine_dir(to, name);
    to = get_direct_dir(to);
    from = get_direct_dir(from);
    char* command = malloc(sizeof(char) * (strlen(from) + strlen(to) + 10));
    if(from_dir){ sprintf(command, "cp -r %s %s", from, to); }
    else{ sprintf(command, "cp %s %s", from, to); }
    system(command);
    SAVE(from);
    SAVE(to);
    SAVE(name);
    printf("%sINFO: %sCopied from %s to %s%s\n",YELLOW,WHITE,from, to,RESET);
    return 1;
}

int git_init(char** argv, int argc, size_t* i){
    char* dir = get_dir(argv[++(*i)]);
    char* link = argv[++(*i)];
    int found = search_path(dir, ".git");
    if(found){
        printf("%sWARNING:%s Already a git repository%s\n",PINK,WHITE,RESET);
    }
    else{
        char* command = malloc(sizeof(char) * (strlen(dir) + 10));
        sprintf(command, "git init %s", dir);
        int result = system(command);
        if(!(result != -1 && result == 0)){ 
            result = system("git --version");
            if(result != -1 && result == 0){ printf("%sGit is not installed%s\n",RED,RESET); }
            else{ printf("%sFailed to create git repository%s\n",RED,RESET); }
            return 0;
        }
        // add branch
        command = malloc(sizeof(char) * (strlen(dir) + 10));
        sprintf(command, "cd %s && git checkout -b main", dir);
        result = system(command);
        if(result != -1 && result == 0){ printf("%sBranch created%s\n",GREEN,RESET); }
        else{ printf("%sFailed to create branch%s\n",RED,RESET); return 0; }
        // add origin
        command = malloc(sizeof(char) * (strlen(dir) + strlen(link) + 10));
        sprintf(command, "cd %s && git remote add origin %s", dir, link);
        result = system(command);
        if(result != -1 && result == 0){ printf("%sOrigin added%s\n",GREEN,RESET); }
        else{ printf("%sFailed to add origin%s\n",RED,RESET); return 0; }
        // add .gitignore
        command = malloc(sizeof(char) * (strlen(dir) + 10));
        sprintf(command, "cd %s && touch .gitignore", dir);
        result = system(command);
        if(result != -1 && result == 0){ printf("%s.gitignore added%s\n",GREEN,RESET); }
        else{ printf("%sFailed to add .gitignore%s\n",RED,RESET); return 0; }
        SAVE(dir);
        SAVE(link);
    }
    return 1;
}
int git_push(char** argv, int argc, size_t* i){
    char* commit = argv[++(*i)];
    char* command = malloc(sizeof(char) * (strlen(builder->dd) + strlen(commit) + 20));
    sprintf(command, "cd %s && git add . && git commit -m \"%s\" && git push -u origin main", builder->dd, commit);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sPushed to git repository%s\n",GREEN,RESET); }
    else{ printf("%sFailed to push to git repository%s\n",RED,RESET); return 0; }
    SAVE(commit);
    return 1;
}

int git_pull(char** argv, int argc, size_t* i){
    char* command = malloc(sizeof(char) * (strlen(builder->dd) + 20));
    sprintf(command, "cd %s && git pull origin main", builder->dd);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sPulled from git repository%s\n",GREEN,RESET); }
    else{ printf("%sFailed to pull from git repository%s\n",RED,RESET); return 0;}
    return 1;
}
// add commands from save file
char** add_save(char** argv, int* argc, size_t* i){
    char* dir = combine_dir(builder->dd, BUILDER_NAME);
    FILE* f = fopen(dir, "r");
    if(!f){
        printf("%sERROR:%s \"%s/%s\" is not a file%s\n",RED,WHITE, dir,BUILDER_NAME, RESET);
        return NULL;
    }
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
    char** args = malloc(sizeof(char*));
    size_t args_size = 0;
    size_t size = strlen(line);
    size_t j = 0;
    while(j < size){
        char* word = malloc(sizeof(char));
        size_t word_size = 0;
        if(line[j] == ' '){ j++; continue; }
        else if(line[j] == '\"'){
            j++;
            while(j < size && line[j] != '\"'){
                word = realloc(word, sizeof(char) * (word_size + 2));
                word[word_size++] = line[j++];
            }
            j++;
        }
        else{
            while(j < size && line[j] != ' ' && line[j] != '\"'){
                word = realloc(word, sizeof(char) * (word_size + 2));
                word[word_size++] = line[j++];
            } 
        }
        word[word_size] = '\0';
        args = realloc(args, sizeof(char*) * (args_size + 1));
        args[args_size++] = word;
    }
    // from point i move the args_size amount of item to the right in argv
    argv = realloc(argv, sizeof(char*) * (*argc + args_size - 1));
    for(size_t j = *argc; j > *i; j--){
        argv[j + args_size - 1] = argv[j];
    }
    for(size_t j = 0; j < args_size; j++){
        argv[*i + j] = args[j];
    }
    *argc += args_size - 1;
    return argv;
}
// remove save file
int remove_save_file(char** argv, int argc, size_t* i){
    char* path = get_file(combine_dir(builder->dd, BUILDER_NAME));
    if(!path){
        printf("%sERROR:%s \"%s\" is not a file%s\n",RED,WHITE, BUILDER_NAME, RESET);
        return 0;
    }
    char* command = malloc(sizeof(char) * (strlen(path) + 10));
    sprintf(command, "rm %s", path);
    int result = system(command);
    return 1;
}
