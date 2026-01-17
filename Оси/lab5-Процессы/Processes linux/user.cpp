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
#include <sys/wait.h>
#include <fstream>

using namespace std;

class ProcessManager {
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

public:
    static bool isProcessRunning(const string& processName) {
        DIR* procDir = opendir("/proc");
        if (!procDir) {
            return false;
        }

        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                char* endptr;
                pid_t pid = strtol(entry->d_name, &endptr, 10);
                if (*endptr == '\0') {  // Это PID
                    string name = getProcessNameFromPID(pid);
                    if (!name.empty() && name == processName) {
                        closedir(procDir);
                        return true;
                    }
                }
            }
        }

        closedir(procDir);
        return false;
    }

    static pid_t findProcessId(const string& processName) {
        DIR* procDir = opendir("/proc");
        if (!procDir) {
            return 0;
        }

        struct dirent* entry;
        while ((entry = readdir(procDir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                char* endptr;
                pid_t pid = strtol(entry->d_name, &endptr, 10);
                if (*endptr == '\0') {  // Это PID
                    string name = getProcessNameFromPID(pid);
                    if (!name.empty() && name == processName) {
                        closedir(procDir);
                        return pid;
                    }
                }
            }
        }

        closedir(procDir);
        return 0;
    }

   static bool runKiller(const string& parameters) {
        pid_t childPid = fork();
        
        if (childPid == -1) {
            cerr << "Fork failed: " << strerror(errno) << endl;
            return false;
        }
        
        if (childPid == 0) {  // Child process
            vector<char*> argv;
            argv.push_back(const_cast<char*>("killer"));

            vector<string> args;
            stringstream ss(parameters);
            string token;
            
            while (ss >> token) {
                args.push_back(token);
            }
            
            for (auto& arg : args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);

            execvp("./killer", argv.data());

            cerr << "Failed to execute killer: " << strerror(errno) << endl;
            exit(EXIT_FAILURE);
        } 
        else {  // Parent process
            int status;
            waitpid(childPid, &status, 0);
            
            if (WIFEXITED(status)) {
                return (WEXITSTATUS(status) == 0);
            }
            return false;
        }
    }
};

void testKillByName(const string& name) {
    if (ProcessManager::isProcessRunning(name)) {
        cout << name + " is running" << endl;
        
        cout << "Running: killer --name " + name << endl;
        ProcessManager::runKiller("--name " + name);
        sleep(1);

        if (!ProcessManager::isProcessRunning(name)) {
            cout << "SUCCESS:" + name + " was terminated" << endl;
        } else {
            cout << "FAIL:" + name + " is still running" << endl;
        }
    } else {
        cout << name + " was not running" << endl;
    }
}

void testKillById(const string& name) {
    if (ProcessManager::isProcessRunning(name)) {
        pid_t id = ProcessManager::findProcessId(name);
        cout << name + " is running (PID: " << id << ")" << endl;
        if (id == 0) {
            cout << "ERROR: Could not find PID for process: " << name << endl;
            return;
        }
        string command = "--id " + to_string(id);
        cout << "Running: killer " << command << endl;

        string checkCmd = "ps -p " + to_string(id) + " > /dev/null 2>&1";
        if (system(checkCmd.c_str()) != 0) {
            cout << "WARNING: Process with PID " << id << " does not exist!" << endl;
            return;
        }
        if (ProcessManager::runKiller(command)) {
            sleep(1);
            
            if (!ProcessManager::isProcessRunning(name)) {
                cout << "SUCCESS: " << name << " was terminated" << endl;
            } else {
                cout << "FAIL: " << name << " is still running" << endl;
            }
        } else {
            cout << "FAIL: Failed to run killer" << endl;
        }
    } else {
        cout << name + " was not running" << endl;
    }
}

void testKillByEnv(const vector<string> &v) {
    sleep(2);

    for(size_t i = 0; i < v.size(); i++){
        bool r = ProcessManager::isProcessRunning(v[i]);
        cout << "Before killer: " + v[i] << (r ? " running" : " not running") << endl;
    }

    cout << "Running: killer" << endl;
    ProcessManager::runKiller("");
    sleep(1);
    
    for(size_t i = 0; i < v.size(); i++){
        bool r = ProcessManager::isProcessRunning(v[i]);
        cout << "After killer: " + v[i] << (r ? " running" : " not running") << endl;
    }
}

int main() {
    vector<string> testProcesses = {"sleep", "ping"};
    
    string envValue;
    for (size_t i = 0; i < testProcesses.size(); i++) {
        envValue += testProcesses[i];
        if (i < testProcesses.size() - 1) {
            envValue += ",";
        }
    }

    setenv("PROC_TO_KILL", envValue.c_str(), 1);
    cout << "PROC_TO_KILL = " << envValue << endl;

    cout << "\nKill by name test:" << endl;
    testKillByName("tail");

    cout << "\n Kill by id test:" << endl;

    testKillById("yes");

    cout << "\nKill by environment variable test" << endl;
    testKillByEnv(testProcesses);
    
    return 0;
}