#include "include.hpp"

#define ARG(name1, name2) (args[index] == name1 || args[index] == name2)

static std::string get_cwd(){    
    try{ return (std::__fs::filesystem::current_path()).string(); } 
    catch (const std::exception& e) {
        std::cerr << "Error getting current working directory: " << e.what() << std::endl;
        return "";
    }
}

void Command::add(std::string str){
    // find space in str
    size_t space = str.find(' ');
    if(space != std::string::npos){ str = "\"" + str + "\""; }
    if(ignore > 0){ ignore--; return; }
    flags.push_back(str);
}


Builder::Builder(int argc, char* argv[]){
    std::vector<std::string> args;
    dd = get_cwd() + "/";
    for(int i = 1; i < argc; i++){ args.push_back(argv[i]); }

    // split defualt_code by space
    std::string str = default_code;    
    std::vector<std::string> default_args;
    std::string temp = "";
    for(char c: str){
        if(c == ' ' || c == '\n'){ default_args.push_back(temp); temp = ""; }
        else{ temp += c; }
    }
    run(default_args);
    if(args.size() == 0){ help(); }
    run(args);
}

void Builder::run(std::vector<std::string> args){
    size_t index = 0;
    cmd = Command();
    while(index < args.size()){
        if(ARG("@h","@help")){ help(); index++; }
        else if(ARG("@v","@version")){ std::cout << YELLOW << "Version: " << WHITE << VERSION << RESET << std::endl; index++;}
        else if(ARG("@f", "@files")){ get_files(args, index);}
        else if(ARG("@r","@run")){ run_code(args, index);}
        else if(ARG("@t","@time")){ time = !time; index++; }
        else if(ARG("@s","@save")){ save(args,index); }
        else if(ARG("@i","@info")){ info = !info; index++; }
        else if(ARG("@si","@silent")){ silent = !silent; index++; }
        else if(ARG("@d","@directory")){ change_dd(args, index); }
        else if(ARG("@l","@load")){ run_file(args, index); }
        else if(args[index][0] == '@'){ new_object(args, index); }
        else{
            cmd.add(args[index]);
            current_flags.push_back(args[index]);
            index++;
        }
    }
}
