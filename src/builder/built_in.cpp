#include "include.hpp"

#define ARG_COUNT(num) if(index + num > args.size()){ std::cout << RED << "Error: " << WHITE << "Not enough arguments for \""<< args[index] << "\"" << RESET << std::endl; return; } 


static long long get_time(){
    std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
    std::chrono::milliseconds time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
    return time_ms.count();
}

static std::string ms_str(long long ms){
    int days = 0, hours = 0,  minutes = 0, seconds = 0;
    while(ms - 1000 > 0   ){ ms -= 1000;    seconds++;  }
    while(seconds - 60 > 0){ seconds -= 60; minutes++;  }
    while(minutes - 60 > 0){ minutes -= 60; hours++;    }
    while(hours - 24 > 0  ){ hours -= 24;   days++;     }
    std::string str = "";
    if(days > 0){ str += std::to_string(days) + " days "; }
    if(hours > 0){ std::to_string(hours) + " hours "; } 
    if(minutes > 0){ str += std::to_string(minutes) + " minutes "; }
    if(seconds > 0){ str += std::to_string(seconds) + " seconds ";}
    if(ms > 0){ str += std::to_string(ms) + " ms"; }
    return str;
}

static bool is_dir(std::string dir){
    try{ return std::__fs::filesystem::is_directory(dir); } 
    catch (... ) { return false; }
}

static bool is_file(std::string file){
    try{ return std::__fs::filesystem::is_regular_file(file); } 
    catch (... ) { return false; }
}

static bool is_num(std::string str){
    for(char c: str){ if(!isdigit(c)){ return false; } }
    return true;
}

static std::string get_direct_path(std::string path){
    size_t i = 0;
    std::string new_path = "";
    while(i < path.size()){
        if(i != 0 && i+2 < path.size() && path[i] == '.' && path[i+1] == '.' && path[i+2] == '/'){ 
            // remove the last directory
            if(new_path.size() > 0 && new_path[new_path.size() - 1] == '/'){ 
                new_path = new_path.substr(0, new_path.size() - 1);
            } 
            while(new_path.size() > 0 && new_path[new_path.size() - 1] != '/'){ 
                new_path = new_path.substr(0, new_path.size() - 1);
            }
            i += 3;
        }
        else{
            new_path += path[i++];
        }
    }
    return new_path;
}

void Builder::help(){
    std::cout << WHITE;
    std::cout << "Usage: " << std::endl;
    std::cout << "  build [options] [objects] [built-in functions]" << std::endl;
    std::cout << "Options: " << std::endl;
    std::cout << "\t@h @help                      Prints all of the flags" << std::endl;
    std::cout << "\t@v @version                   Prints the version of the build" << std::endl;
    std::cout << "\t@f @files [directory] [ext]   Gets all of the files in the directory with the extention" << std::endl;
    std::cout << "\t@r @run                       Runs the code" << std::endl;
    std::cout << "\t@t @time                      Prints the time it took to run the code" << std::endl;
    std::cout << "\t@s @save [file]               Saves the flags into a file" << std::endl;
    std::cout << "\t@i @info                      Prints info about the code" << std::endl;
    std::cout << "\t@si @silent                   Silents the output of the code" << std::endl;
    std::cout << "\t@d @directory [directory]     Changes the directory" << std::endl;
    std::cout << "\t@l @load [file]               Loads the flags from a file" << std::endl;
    std::cout << "\t@git [URL]                    Initilizes git" << std::endl;
    std::cout << "\t@push [message]               Pushes the code to github" << std::endl;
    std::cout << "\t@pull [whete]                 Pulls the code from github" << std::endl;
    std::cout << "\t@fetch [alias] [branch]       Fetches the code from github" << std::endl;
    std::cout << "\t@cp [folder] [folder]         Copies the folder to the other folder" << std::endl;
    std::cout << "\t@[name] [@args] [size] @      Creates a new object" << std::endl;


    std::cout << RESET;
}


void Builder::get_files(std::vector<std::string>& args, size_t& index){
    ARG_COUNT(2); index++;
    // get the directory
    std::string dir;
    std::string original_dir = args[index++];
    if(is_dir(dd + original_dir)){ dir = dd + original_dir; }
    else if(is_dir(original_dir)){ dir = original_dir; }
    else{ std::cout << RED << "Error: " << WHITE << "Directory \"" << original_dir << "\" does not exist" << RESET << std::endl; return; }
    // get the extention
    std::string ext = args[index++];
    // get the files
    cmd.add("@f");
    cmd.add(original_dir);
    cmd.add(ext);
    // add the files to the args
    size_t size = 0;
    for(const auto & entry : std::__fs::filesystem::recursive_directory_iterator(dir)){
        if(std::__fs::filesystem::is_regular_file(entry.path())){
            if(entry.path().extension() == ext){
                std::string path = entry.path().string();
                path = get_direct_path(path);
                args.insert(args.begin() + index, path);
                current_flags.push_back(path);
                size++;
            }
        }
    }
    index += size;
}

void Builder::run_code(std::vector<std::string>& args, size_t& index){
    cmd.add("@r");
    size_t from = index++;
    std::string code = "cd " + dd + " && ";
    std::string local = "";
    // add the flags into a single string
    for(std::string flags: current_flags){ local += flags + " "; }
    code += local;
    // reset the flags
    current_flags.clear();
    // silents the output
    if(silent){ code += " > /dev/null 2>&1"; }
    if(info){ std::cout << YELLOW << "Running:  " << WHITE << local << RESET << std::endl; }
    // run the code
    int result = system(code.c_str());
    if(info){
        if(result != 0 || result == -1){ 
            std::cout << RED << "Error: " << WHITE << "@r failed running \"" << local << "\"" << RESET << std::endl; 
        }
        else{ std::cout << GREEN << "Ran succesfully" << RESET << std::endl; }
    }
    // erase from to index
    args.erase(args.begin() + from, args.begin() + index);
    index = from;
}

void Builder::change_dd(std::vector<std::string>& args, size_t& index){
    ARG_COUNT(1); index++;
    if(is_dir(dd + args[index])){ dd += args[index++]; }
    else if(is_dir(args[index])){ dd = args[index++]; }
    else{ std::cout << RED << "Error: " << WHITE << "Directory \"" << args[index] << "\" does not exist" << RESET << std::endl; return; } 
    if(dd[dd.size()-1] != '/'){ dd += '/'; }
}


void Builder::save(std::vector<std::string>& args, size_t& index){
    index++;
    std::string file;
    if(index < args.size() && is_file(dd + args[index])){ file = dd + args[index]; }
    else{ file = dd + LOCAL; }
    std::ofstream out(file);
    for(std::string flag: cmd.flags){ out << flag << std::endl; }
    out.close();
    if(info){ std::cout << GREEN << "Saved succesfully into \"" << file << "\"" << RESET << std::endl; }

}

void Builder::run_file(std::vector<std::string>& args, size_t& index){
    index++;
    std::string file;
    if(index < args.size() && is_file(dd + args[index])){ file = dd + args[index]; }
    else{ file = dd + LOCAL; }
    std::ifstream in(file);
    std::string line;
    // get the the file into a single string
    size_t size = 0;
    while(std::getline(in, line)){ 
        args.insert(args.begin() + index + size++, line);
    }
    
    in.close(); 
    if(info){ std::cout << GREEN << "Loaded succesfully from \"" << file << "\"" << RESET << std::endl; }

}

void Builder::new_object(std::vector<std::string>& args, size_t& index){
    std::string name = args[index++];
    Object object;
    for(Object obj: objects){ if(obj.name == name){ object = obj; break; } }
    cmd.add(name);
    // create a new object
    if(object.name == ""){
        object.name = name;
        // name is a number
        if(is_num(name)){ std::cout << RED << "Error: " << WHITE << "Invalid object name \"" << name << "\"" << RESET << std::endl; return; }
        // object has parameters
        if(args[index] == "@args"){
            std::string num_str = args[++index];
            index++;
            if(!is_num(num_str)){ std::cout << RED << "Error: " << WHITE << "Invalid number \"" << num_str << "\"" << RESET << std::endl; return; }
            int num = std::stoi(num_str);
            if(index + num > args.size()){ std::cout << RED << "Error: " << WHITE << "Not enough arguments for \""<< object.name << "\"" << RESET << std::endl; return; }
            object.args_size = num;
            cmd.add("@args");
            cmd.add(num_str);
        } 
        // loop until the end of the object
        while(index < args.size() && args[index] != "@"){
            cmd.add(args[index]);
            object.flags.push_back(args[index++]);
        }
        // if invalid end of the object
        if(args[index] != "@"){ std::cout << RED << "Error: " << WHITE << "Object \"" << name << "\" doesn't have an end \"@\"" << RESET << std::endl; return; }
        objects.push_back(object);
        cmd.add("@");
        index++;
        return;
    }
    // load an object
    std::vector<std::string> object_args;
    if(index + object.args_size > args.size()){ std::cout << RED << "Error: " << WHITE << "Not enough arguments for \""<< object.name << "\"" << RESET << std::endl; return; }
   
    for(size_t i = 0; i < object.args_size; i++){  
        cmd.add(args[index]);
        object_args.push_back(args[index++]); 
    }

    size_t size = 0;
    for(std::string flag: object.flags){ 
        if(flag[0] == '@'){
            std::string num_str = flag.substr(1, flag.size()-1);
            if(is_num(num_str)){
                size_t num = std::stoi(num_str);  
                if(num >= object_args.size()){ std::cout << RED << "Error: " << WHITE << "Parameter  \""<< flag << "\" doesn't exist" << RESET << std::endl; return; }
                flag = object_args[num];
            }
        } 
        cmd.ignore++;
        if(flag.find(' ') != std::string::npos){ flag = "\"" + flag + "\""; }
        args.insert(args.begin() + index + size++, flag); 
    }
}
