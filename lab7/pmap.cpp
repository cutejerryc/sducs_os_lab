#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <iomanip> 
using namespace std;

//输出help内容
void get_help() {
    cout << "Usage: ./pmap_sim [options] <pid>" << endl;
    cout << "\nOptions:" << endl;
    cout << "  -h, --help                 Show this help message and exit" << endl;
    cout << "  -x                         Display extended memory map (basic details)" << endl;
    cout << "  -X                         Display detailed memory map (all fields)" << endl;
    cout << "  -d, --device               Show the device format in the output" << endl;
    cout << "  -q, --quiet                Do not display header and footer" << endl;
    cout << "\nExample:" << endl;
    cout << "  ./pmap_sim -x -d <pid>     Display extended memory map with device info" << endl;
    cout << "  ./pmap_sim -X -q <pid>     Display detailed memory map without headers" << endl;
}

//输出内存的内容
void get_info(const string& pid, const unordered_set<string>& options) {
    string path = "/proc/" + pid + "/maps";
    ifstream fmap(path);

    if (!fmap.is_open()) {
        cerr << "Failed to open " << path << endl;
        return;
    }

    string line;
    //搜索相关参数
    bool show_device = options.count("-d") || options.count("--device"); 
    bool quiet = options.count("-q") || options.count("--quiet");     

    //输出header和footer
    if (!quiet) {
        if (options.count("-x") || options.count("-X")) {
            cout << "Address               Perms Offset   Device    Inode    Path" << endl;
            cout << "--------------------------------------------------------" << endl;
        } 
        else {
            cout << "Address           Path" << endl;
            cout << "---------------------------------" << endl;
        }
    }

    //获取进程信息
    while (getline(fmap, line)) {
        istringstream iss(line);
        string address, perm, offset, device, inode, path;

        iss >> address >> perm >> offset >> device >> inode;
        getline(iss, path);

        if (options.count("-x") || options.count("-X")) {
            //实现-x参数
            cout << address << " " << perm << " " << offset << " " 
                      << device << " " << inode << " " << path << endl;
        } 
        else {
            //默认情况下的输出，这里需要考虑是否有-d参数
            if (show_device) {
                cout << address << " " << device << " " << path << endl;
            } else {
                cout << address << " " << path << endl;
            }
        }
    }

    //除了-q参数外执行的输出
    if (!quiet) {
        if (options.count("-x")) {
            cout << "--------------------------------------------------------" << endl;
        } else if (options.count("-X")) {
            cout << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        }
    }

    fmap.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./pmap_sim [options] <pid>" << endl;
        return 1;
    }
    //hash存储参数
    unordered_set<string> options;
    string pid;

    //解析参数
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            //打印帮助内容
            get_help();
            return 0;
        } else if (arg[0] == '-') {
            //添加对应的参数
            options.insert(arg);
        } else {
            //末尾为PID
            pid = arg;
        }
    }
    //缺少PID的输入的情况
    if (pid.empty()) {
        cerr << "PID empty!" << endl;
        return 1;
    }

    get_info(pid, options);

    return 0;
}