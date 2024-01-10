#include "include.h"



int main(int argc, char** old_argv){
    // not making argv constant
    char** argv = malloc(sizeof(char*) * argc);
    for(int i = 0; i < argc; i++){ argv[i] = old_argv[i];}
    // init builder
    builder = malloc(sizeof(Builder));
    builder->compiler =       NULL;
    builder->run =            NULL;
    builder->dd =             get_current_dir();
    builder->cmd =            malloc(sizeof(char));
    builder->cmd_save =       malloc(sizeof(char));
    builder->vars=            malloc(sizeof(Variable*));
    builder->vars_size =      0;
    builder->save =           0;
    builder->show_cmd =       0;
    builder->build =          0;
    builder->del =            0;
    // if there is no flags
    if(argc == 1 && is_file(combine_dir(builder->dd, BUILDER_NAME))){ 
        if(run_default(argv[0], builder->dd)){ return 0; }
    }
    if(argc == 1){ help(argv[0]); return 0; }
    // if there is one
    if(argc == 2 && !check_flag(argv[1])){ run_default(argv[0], argv[1]); return 0; }
    char* exe = argv[0];
    size_t i = 1;
    SAVE("\n");
    while(i < argc){
        if(!(strlen(argv[i]) < 2 && argv[i][0] == '@' && argv[i][1] == '@')){
            argv = check_valid_flag(argv[i], argv, &argc, i);
        }
        SAVE(argv[i]);
        // check flags 
        if(ARG("@h", 0)){ help(exe); }
        else if(ARG("@v", 0)){ printf("%sVERSION:%s %s%s\n",YELLOW,WHITE,VERSION,RESET); }
        else if(ARG("@c", 1)){ 
            builder->compiler = argv[++i]; 
            char* cmd = malloc(sizeof(char) * (strlen(builder->compiler) + strlen(builder->cmd) +2));
            strcpy(cmd, builder->compiler);
            strcat(cmd, " ");
            strcat(cmd, builder->cmd);
            builder->cmd = cmd;
            SAVE(argv[i]);
        }
        else if(ARG("@d", 1)){ CALL(change_default_dir(argv, argc, &i)) } 
        else if(ARG("@s", 0)){ builder->save = 1; }
        else if(ARG("@t", 0)){ time_everything(); }
        else if(ARG("@tr", 0)){ time_run();}
        else if(ARG("@tb", 0)){ builder->time_build = 1; }
        else if(ARG("@dd", 0)){ if(!builder->run){ printf("%sERROR:%s No executable to execute is set\n%s",RED,WHITE,RESET);} builder->del = 1; }
        else if(ARG("@r", 1)){ 
            builder->run = get_direct_dir( argv[++i]); 
            SAVE(get_direct_dir(argv[i])); 
        }
        else if(ARG("@b", 0)){ builder->build = 1;}
        else if(ARG("@sr",1)){ 
            char** new_argv = add_save(argv, &argc, &i);
            if(!new_argv){ goto end_error; }
            argv = new_argv;
            i--;
        }
        else if(ARG("@rs", 0)){ CALL(remove_save_file(argv, argc, &i)) }
        else if(ARG("@af", 1)){ CALL(add_file(argv, argc, &i)) }
        else if(ARG("@f", 2)){ CALL(add_files(argv, argc, &i)) }
        else if(ARG("@ff", 1)){ CALL(add_run_flag(argv, argc, &i)) }
        else if(ARG("@scmd", 0)){ builder->show_cmd = 1;}
        else if(ARG("@ln", 3)){ CALL(ln(argv, argc, &i)) }
        else if(ARG("@cp", 3)){ CALL(copy(argv, argc, &i)) }
        else if(ARG("@git", 2)){ CALL(git_init(argv, argc, &i)) }
        else if(ARG("@push", 1)){ CALL(git_push(argv, argc, &i)) }
        else if(ARG("@pull", 0)){ CALL(git_pull(argv, argc, &i)) }
        else if(argv[i][0] == '@'){ 
            char** new_argv = handle_var(argv,&argc, &i); 
            if(!new_argv){ goto end_error; }
            argv = new_argv;
            i--;
        }
        else{ CMD(argv[i]); }
        SAVE("\n");
        i++; continue;
        end_error:
            i++; error = 1;
    }
    int result = !error;
    // build    
    long time_start_build = get_time();
    if(result && builder->build ){ result = build(); }
    long time_end_build = get_time();
    // run
    long time_start_run = get_time();
    if(builder->run && result){ result = run(); }
    long time_end_run = get_time();
    // save
    if(builder->save){ save(); }    
    if(result){
        if(builder->time_build){ printf("%sBuild time:%s %s\n",YELLOW,WHITE,time_to_string(time_end_build - time_start_build)); }
        if(builder->run && builder->time_run){ printf("%sRun time:%s %s\n",YELLOW,WHITE,time_to_string(time_end_run - time_start_run)); }
    }   
    return 0;
}

