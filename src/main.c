#include "include.h"


static void build(){
    if(builder.compiler == NULL){ printf("%sERROR:%s No compiler set\n%s",RED,WHITE,RESET); exit(1); }
    char* files = into_string(builder.files, builder.files_size);
    char* flags = into_string(builder.flags, builder.flags_size);
    char* command = malloc(sizeof(char) * (strlen(builder.compiler) + strlen(files) + strlen(flags) + strlen(builder.output) + 10));
    sprintf(command, "%s %s %s -o %s", builder.compiler,files, flags, builder.output);
    printf("%sCompiling:%s %s%s\n",YELLOW,WHITE,command,RESET);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sCompiled successfully into \"%s\"%s\n",GREEN, builder.output, RESET); }
    else{ printf("%sFailed to compile%s\n",RED,RESET); }
    if(builder.save){
        char* builder_path = combine_dir(builder.default_directory, BUILDER_NAME);
        FILE* f = fopen(builder_path, "w");
        fprintf(f, "%s", builder.cmd);
        fclose(f);
        printf("%sSaved build command to \"%s\"%s\n",GREEN, builder_path, RESET);
    }       
}

static void run(){
    char* run_flags = into_string(builder.run_flags, builder.run_flags_size);
    char* command = malloc(sizeof(char) * (strlen(builder.output) + strlen(run_flags) + 25));
    // create the command
    #ifdef _WIN32
        sprintf(command, "%s %s", builder.output, run_flags);
    #else
        sprintf(command, "./%s %s", builder.output, run_flags);
    #endif
    // run the command
    printf("%sRunning:%s %s%s\n",YELLOW,WHITE,command,RESET);
    int result = system(command);
    if(result != -1 && result == 0){ printf("%sRan successfully%s\n",GREEN,RESET); }
    else{ printf("%sFailed to run%s\n",RED,RESET); }
    if(builder.delete_executable){
        char* command = malloc(sizeof(char) * strlen(builder.output) + 10);
        sprintf(command, "rm %s", builder.output);
        int result = system(command);
        if(result != -1 && result == 0){ printf("%sDeleted executable \"%s\"%s\n",GREEN,builder.output,RESET); }
        else{ printf("%sFailed to delete executable%s\n",RED,RESET); }
    } 
}


static void run_default(char* builder_path, char* path){
    if(!check_dir(path)){
        printf("%sERROR:%s Directory \"%s\" not found%s\n",RED,WHITE, path, RESET);
        exit(1);
    }
    // loop through all of the files
    FILE* f = NULL;
    DIR* dir = opendir(path);
    struct dirent* entry;
    printf("%sSearching for %s in %s%s\n",YELLOW, BUILDER_NAME,path, RESET);
    while((entry = readdir(dir)) != NULL){
        if(entry->d_type == DT_REG){
            if(strcmp(entry->d_name, BUILDER_NAME) == 0){ 
                printf("%sFound %s%s\n",GREEN,entry->d_name,RESET);
                f = fopen(combine_dir(path,entry->d_name), "r"); 
                break; 
            }
        }
    }
    if(f == NULL){ printf("%sERROR:%s No \"%s\" file found in %s%s\n",RED,WHITE, BUILDER_NAME,path, RESET); exit(1); }
    // get the first line
    char* line = malloc(sizeof(char) * 1024);
    size_t line_size = 1024;
    getline(&line, &line_size, f);
    // get the compiler
    char* command = malloc(sizeof(char) * (strlen(builder_path) + strlen(line) + 2));
    sprintf(command, "%s %s", builder_path, line);
    printf("%sRunning:%s %s%s\n",YELLOW,WHITE,command,RESET);
    system(command);
}


int main(int argc, char *argv[]){
    builder = (Builder){
        .compiler = COMPILER,
        .cmd = malloc(sizeof(char*)),
        .output = "a.out",
        .flags = malloc(sizeof(char*)),
        .flags_size = 0,
        .files = malloc(sizeof(char*)),
        .files_size = 0,
        .autorun = 0,
        .run_flags = malloc(sizeof(char*)),
        .run_flags_size = 0,
        .time_run = 0,
        .time_build = 0,
        .delete_executable = 0,
        .default_directory = get_current_dir(),
        .save = 0,
    };
    // loop through all of the arguments
    if(argc == 2){ run_default(argv[0], argv[1]); return 0; }
    size_t i = 1;
    while(i < argc){
        add_cmd(argv[i]);   
        if(check_arg(argv[0],"-h",    argv[i], argc, i, 0)){ help(argv[0]); }
        else if(check_arg(argv[0],"-o",    argv[i], argc, i, 1)){ builder.output = argv[++i]; }
        else if(check_arg(argv[0],"-c",    argv[i], argc, i, 1)){ builder.compiler = argv[++i]; }
        else if(check_arg(argv[0],"-af",   argv[i], argc, i, 1)){ check_file(argv[++i]); add_file(argv[i]); }
        else if(check_arg(argv[0],"-aff",  argv[i], argc, i, 1)){ add_flag(argv[++i]); }
        else if(check_arg(argv[0],"-tb", argv[i], argc, i, 0)){ builder.time_build = 1; }
        else if(check_arg(argv[0],"-ar",   argv[i], argc, i, 0)){ builder.autorun = 1; }
        else if(check_arg(argv[0],"-rf",   argv[i], argc, i, 1)){ 
            size_t files_size = 0;
            char** files = get_args(argv, argc, ++i, &files_size);
            i += files_size - 1;
            for(int j = 0; j < files_size; j++){
                int found = 0;
                int index = check_in(files[j], builder.files, builder.files_size);
                if(check_file(files[j]) && index != -1){
                    found = 1;
                    builder.files[index] = builder.files[--builder.files_size];
                    builder.files = realloc(builder.files, sizeof(char*) * builder.files_size);
                    memmove(builder.files + index, builder.files + index + 1, (builder.files_size - index) * sizeof(char*));
                    break;
                }
                if(!found){ printf("%sWARNING:%s File \"%s\" not found to remove\n%s",MAGENTA,WHITE, files[j], RESET); }
            }
        }
        else if(check_arg(argv[0],"-rff",   argv[i], argc, i, 1)){ 
            size_t flags_size = 0;
            char** flags = get_args(argv, argc, ++i, &flags_size);
            i += flags_size - 1;
            for(int j = 0; j < flags_size; j++){
                int found = 0;
                int index = check_in(flags[j], builder.flags, builder.flags_size);
                if(check_file(flags[j]) && index != -1){ 
                    found = 1;
                    builder.flags[index] = builder.flags[--builder.flags_size];
                    builder.flags = realloc(builder.flags, sizeof(char*) * builder.flags_size);
                    memmove(builder.flags + index, builder.flags + index + 1, (builder.flags_size - index) * sizeof(char*));
                    break;
                }
                if(!found){ printf("%sWARNING:%s Flags \"%s\" not found to remove\n%s",MAGENTA, WHITE, flags[j], RESET); }
            }
        }
        else if(check_arg(argv[0],"-pf",   argv[i], argc, i, 0)){ 
            printf("%sFiles:%s\n",YELLOW,WHITE);
            for(int j = 0; j < builder.files_size; j++){ printf("\t%s\n", builder.files[j]); }
            printf("%s",RESET);
        }
        else if(check_arg(argv[0],"-pff", argv[i], argc, i, 0)){ 
            printf("%sFlags:%s\n",YELLOW,WHITE);
            for(int j = 0; j < builder.flags_size; j++){ printf("\t%s\n", builder.flags[j]); }
            printf("%s",RESET);
        }
        else if(check_arg(argv[0],"-f",    argv[i], argc, i, 1)){
            char* dir = combine_dir(builder.default_directory,argv[++i]);
            if(!check_dir(dir)){
                printf("%sERROR:%s Directory \"%s\" not found%s\n",RED,WHITE, dir, RESET);
                error = 1;
                i++; continue;
            }
            add_cmd(argv[i]);
            size_t exts_size = 0;
            char** exts = get_args(argv, argc, ++i, &exts_size);
            i += exts_size - 1;
            if(exts_size == 0){
                printf("%sERROR:%s No extention given%s\n",RED,WHITE, RESET);
                error = 1;
                i++; continue;
            }
            for(int j = 0; j < exts_size; j++){
                if(exts[j][0] != '.'){
                    printf("%sERROR:%s Extention \"%s\" must start with a \".\"%s\n",RED,WHITE, exts[j], RESET);
                    error = 1;
                    i++; continue;
                }
                add_cmd(exts[j]);
            }
            // get all of the files
            char** files = get_files_exts(dir, exts, exts_size);
            for(int j = 0; files[j] != NULL; j++){
                if(check_in(files[j], builder.files, builder.files_size) == -1){ add_file(files[j]); } 
            }
        }
        else if(check_arg(argv[0],"-ff",   argv[i], argc, i, 1)){ 
            while(i + 1 < argc && !check_flag(argv[i + 1])){
                if(check_in(argv[i + 1], builder.flags, builder.flags_size) == -1){ add_flag(argv[++i]); }
                else{ i++; }
            }
        }
        else if(check_arg(argv[0],"-arff", argv[i], argc, i, 1)){
            if(!builder.autorun){
                printf("%sWARNING: %sAutorun is off\n%s, so the \"-arff\" won't effect the build",MAGENTA,WHITE, RESET);
            }
            while(i + 1 < argc && !check_flag(argv[i + 1])){
                if(check_in(argv[i + 1], builder.run_flags, builder.run_flags_size) == -1){ add_run_flag(argv[++i]); }
                else{ i++; }
            }
        }
        else if(check_arg(argv[0],"-tar", argv[i], argc, i, 0)){
            if(!builder.autorun){
                printf("%sWARNING: %sAutorun is off\n%s, so the \"-tar\" won't effect the build",MAGENTA,WHITE, RESET);
            }
            builder.time_run = 1;
        }
        else if(check_arg(argv[0],"-t",argv[i], argc, i, 0)){
            if(!builder.autorun){
                printf("%sWARNING: %sAutorun is off\n%s, so the \"-t\" won't effect the build",MAGENTA,WHITE, RESET);
            }
            builder.time_build = 1; builder.time_run = 1;
        }
        else if(check_arg(argv[0],"-cdd", argv[i], argc, i, 1)){
            char* new_dir = NULL;
            char* dir_1 = argv[++i];
            char* dir_2 = combine_dir(builder.default_directory, dir_1);
            if(check_dir(dir_2)){new_dir = dir_2;}
            else if(check_dir(dir_1) || !strcmp(dir_1, "")){new_dir = dir_1;}
            else{
                printf("%sERROR:%s Directory \"%s\" nor \"%s\" not found%s\n",RED,WHITE, dir_1,dir_2, RESET);
                error = 1;
                i++; continue;
            }
            add_cmd(new_dir);
            builder.default_directory = new_dir;
        }
        else if(check_arg(argv[0],"-de", argv[i], argc, i , 0)){
            if(!builder.autorun){
                printf("%sWARNING: %sAutorun is off\n%s, so the \"-t\" won't effect the build",MAGENTA,WHITE, RESET);
            }
            builder.delete_executable = 1;
        }
        else if(check_arg(argv[0],"-s", argv[i], argc, i, 0)){ builder.save = 1; }
        else{
            if(argv[i][0] == '-'){ 
                printf("%sERROR:%s Invalid flag \"%s\"\n%s",RED,WHITE, argv[i], WHITE); 
                error = 1;
            }
            else{ 
                printf("%sERROR:%s Invalid argument \"%s\"\n%s",RED,WHITE, argv[i], RESET);
                error = 1;
            }
        }
        i++;
    }
    // compile
    long build_start = get_time();
    if(!error && builder.files_size > 0){ build(); }
    long build_end = get_time();
    // autorun
    long run_start = get_time();
    if(!error && builder.autorun){ run(); }
    long run_end = get_time();
    // get times
    if(builder.time_build){ printf("%sBuild time:%s %s\n",YELLOW,WHITE, time_to_string(build_end - build_start)); }
    if(builder.time_run){ printf("%sRun time:%s %s\n",YELLOW,WHITE, time_to_string(run_end - run_start)); }
    

    return 0;
}
