#include "include.h"



int main(int argc, char** old_argv){
    // not making argv constant
    char** argv = malloc(sizeof(char*) * argc);
    for(int i = 0; i < argc; i++){ argv[i] = old_argv[i];}
    // init builder
    builder = malloc(sizeof(Builder));
    builder->compiler =       COMPILER;
    builder->output =         "a.out";
    builder->dd =             get_current_dir();
    builder->cmd =            malloc(sizeof(char));
    // current files
    builder->files =          malloc(sizeof(char*));
    builder->actual_files =   malloc(sizeof(char*));
    builder->files_size =     0;
    // current flags
    builder->flags =          malloc(sizeof(char*));
    builder->flags_size =     0;
    // flags that are given to the executable
    builder->run  =       0;
    builder->run_flags =      malloc(sizeof(char*));
    builder->run_flags_size = 0;
    // save the command after everything
    builder->save = 0;
    // delete the executable after running
    builder->del = 0;
    // if there is no flags
    if(argc == 1){ help(argv[0]); return 0; }
    // if there is one
    if(argc == 2 && !check_flag(argv[1])){ run_default(argv[0], argv[1]); return 0; }
    char* exe = argv[0];
    size_t i = 1;
    while(i < argc){
        argv = check_valid_flag(argv[i], argv, &argc, i);
        CMD_FLAG(argv[i]);
        // check flags 
        if(ARG("-h", 0)){ help(exe); }
        else if(ARG("-v", 0)){ printf("%sVERSION:%s %s%s\n",YELLOW,WHITE,VERSION,RESET); }
        else if(ARG("-o", 1)){ builder->output = argv[++i]; CMD_ARG(combine_dir(builder->dd,argv[i])); }
        else if(ARG("-c", 1)){ builder->compiler = argv[++i]; CMD_ARG(argv[i]); }
        else if(ARG("-d", 1)){ CALL(change_default_dir(argv, argc, &i)) } 
        else if(ARG("-s", 0)){ builder->save = 1; }
        else if(ARG("-t", 0)){ time_everything(); }
        else if(ARG("-tr", 0)){ time_run();}
        else if(ARG("-tb", 0)){ builder->time_build = 1; }
        else if(ARG("-rr", 0)){ builder->run = 0; }   
        else if(ARG("-r", 0)){ builder->run = 1; }
        else if(ARG("-sr",1)){ 
            char** new_argv = add_save(argv, &argc, &i);
            if(!new_argv){ goto end_error; }
            argv = new_argv;
            i--;
        }
        else if(ARG("-rs", 0)){ CALL(remove_save_file(argv, argc, &i)) }
        else if(ARG("-af", 1)){ CALL(add_file(argv, argc, &i)) }
        else if(ARG("-aff", 1)){ add_flag(argv, argc, &i); }
        else if(ARG("-pf", 0)){ print_files(); }
        else if(ARG("-pff", 0)){ print_flags(); }
        else if(ARG("-de", 0)){builder->del = 1; }
        else if(ARG("-f", 2)){ CALL(add_files(argv, argc, &i)) }
        else if(ARG("-ff", 1)){ add_flags(argv, argc, &i); }
        else if(ARG("-rf", 1)){ remove_file(argv, argc, &i); }
        else if(ARG("-rff", 1)){ remove_flag(argv, argc, &i); }
        else if(ARG("-arff", 1)){ add_run_flag(argv, argc, &i); }
        else if(ARG("-ln", 3)){ CALL(ln(argv, argc, &i)) }
        else if(ARG("-cp", 3)){ CALL(ln(argv, argc, &i)) }
        else if(ARG("-git", 2)){ CALL(git_init(argv, argc, &i)) }
        else if(ARG("-push", 1)){ CALL(git_push(argv, argc, &i)) }
        else if(ARG("-pull", 0)){ CALL(git_pull(argv, argc, &i)) }
        else{
            printf("%sERROR: %sInvalid flag \"%s\"\n%s",RED,WHITE, argv[i], RESET);
        }
        CMD_FLAG("\n")
        i++; continue;
        end_error:
            i++; error = 1;
    }
    int result = !error;
    // build
    if(builder->files_size == 0){ printf("%sERROR: %sNo files to build%s\n",RED,WHITE,RESET); result = 0; }
    long time_start_build = get_time();
    if(builder->files_size > 0 && result){ result = build(); }
    long time_end_build = get_time();
    // run
    long time_start_run = get_time();
    if(builder->run && result){ result = run(); }
    long time_end_run = get_time();
    // save
    if(builder->save && result){ save(); }
    if(result){
        if(builder->time_build){ printf("%sBuild time:%s %s\n",YELLOW,WHITE,time_to_string(time_end_build - time_start_build)); }
        if(builder->run && builder->time_run){ printf("%sRun time:%s %s\n",YELLOW,WHITE,time_to_string(time_end_run - time_start_run)); }
    }
}

