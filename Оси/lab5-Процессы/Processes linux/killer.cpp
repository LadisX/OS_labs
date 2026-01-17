#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <signal.h>
#include <sys/wait.h>
#include <fstream>

using namespace std;

class ProcessKiller {
private:
    static string getProcessNameFromPID(pid_t pid) {
        string procPath = "/proc/" + to_string(pid) + "/comm";
        ifstream procFile(procPath);
        if (procFile.is_open()) {
            string name;
            getline(procFile, name);
            procFile.close();
            return name;
        }
        return "";
    }

    static vector<pid_t> getAllProcessesByName(const string& processName) {
        vector<pid_t> pids;
        DIR* procDir = opendir("/proc");
        
        if (!procDir) {
            return pids;
        }

        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                char* endptr;
                pid_t pid = strtol(entry->d_name, &endptr, 10);
                if (*endptr == '\0') {
                    string name = getProcessNameFromPID(pid);
                    if (!name.empty() && name == processName) {
                        pids.push_back(pid);
                    }
                }
            }
        }

        closedir(procDir);
        return pids;
    }

public:
    static bool killProcessById(pid_t processId) {
        if (processId <= 1) {
            cout << "Invalid process ID: " << processId << endl;
            return false;
        }
        
        // Проверяем, существует ли процесс
        if (kill(processId, 0) == -1) {
            if (errno == ESRCH) {
                cout << "Process with ID " << processId << " does not exist" << endl;
            } else if (errno == EPERM) {
                cout << "Permission denied to kill process with ID " << processId << endl;
            }
            return false;
        }
        
        string procName = getProcessNameFromPID(processId);
        if (!procName.empty()) {
            cout << "Terminating process: " << procName << " (ID: " << processId << ")" << endl;
        } else {
            cout << "Terminating process with ID: " << processId << endl;
        }
        
        if (kill(processId, SIGTERM) == 0) {
            for (int i = 0; i < 10; i++) {
                if (kill(processId, 0) == -1) {
                    cout << "Process terminated successfully" << endl;
                    return true;
                }
                usleep(100000); // 100ms
            }

            cout << "Process did not terminate with SIGTERM, sending SIGKILL..." << endl;
            if (kill(processId, SIGKILL) == 0) {
                cout << "Process killed with SIGKILL" << endl;
                return true;
            }
        }
        
        cout << "Failed to terminate process with ID: " << processId 
             << " (Error: " << strerror(errno) << ")" << endl;
        return false;
    }

    static bool killProcessesByName(const string& processName) {
        vector<pid_t> pids = getAllProcessesByName(processName);
        
        if (pids.empty()) {
            cout << "No processes found with name: '" << processName << "'" << endl;
            return false;
        }
        
        cout << "Found " << pids.size() << " process(es) with name: '" << processName << "'" << endl;
        
        bool allKilled = true;
        for (pid_t pid : pids) {
            if (!killProcessById(pid)) {
                allKilled = false;
            }
        }
        
        return allKilled;
    }

    static vector<string> getProcessesFromEnv() {
        vector<string> processes;
        char* envVar = getenv("PROC_TO_KILL");
        
        if (envVar != nullptr) {
            string envValue(envVar);
            istringstream ss(envValue);
            string token;
            
            while (getline(ss, token, ',')) {
                // Убираем пробелы в начале и конце
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                
                if (!token.empty()) {
                    processes.push_back(token);
                }
            }
        }
        
        return processes;
    }

    static void killProcessesFromEnv() {
        auto processes = getProcessesFromEnv();
        if (processes.empty()) {
            cout << "Environment variable PROC_TO_KILL not found or empty" << endl;
            return;
        }

        cout << "Terminating processes from PROC_TO_KILL environment variable:" << endl;
        for (const auto& processName : processes) {
            cout << "  - " << processName << endl;
            killProcessesByName(processName);
        }
    }

    static void printUsage() {
        cout << "Usage:" << endl;
        cout << "  ./killer                    - terminate processes from PROC_TO_KILL" << endl;
        cout << "  ./killer --id <process_id>  - terminate process by ID" << endl;
        cout << "  ./killer --name <name>      - terminate processes by name" << endl;
        cout << "  ./killer --help             - show this help message" << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        ProcessKiller::killProcessesFromEnv();
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if ((arg == "--id" || arg == "-i") && i + 1 < argc) {
            try {
                pid_t processId = stoul(argv[++i]);
                if (!ProcessKiller::killProcessById(processId)) {
                    return 1;
                }
            } catch (const exception& e) {
                cout << "Invalid process ID format: " << argv[i] << endl;
                return 1;
            }
        } 
        else if ((arg == "--name" || arg == "-n") && i + 1 < argc) {
            string processName = argv[++i];
            if (!ProcessKiller::killProcessesByName(processName)) {
                return 1;
            }
        } 
        else if (arg == "--help" || arg == "-h") {
            ProcessKiller::printUsage();
            return 0;
        } 
        else if (arg == "--list" || arg == "-l") {
            cout << "Listing processes is not implemented in this version." << endl;
        }
        else {
            cout << "Unknown parameter: " << arg << endl;
            ProcessKiller::printUsage();
            return 1;
        }
    }
    
    return 0;
}