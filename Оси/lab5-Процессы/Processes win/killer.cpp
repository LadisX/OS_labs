#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <algorithm>
#include <sstream>
#include <tlhelp32.h>

using namespace std;

class ProcessKiller {
private:
    static string wstringToString(const wstring& wstr) {
        string result;
        result.resize(wstr.size());
        transform(wstr.begin(), wstr.end(), result.begin(), [](wchar_t c) {
            return (char)c;
        });
        return result;
    }

public:
    static bool killProcessById(DWORD processId) {
        HANDLE proc = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
        if (proc == NULL) {
            cout << "Failed to open process with ID: " << processId << " (Error: " << GetLastError() << ")" << endl;
            return false;
        }

        if (TerminateProcess(proc, 0)) {
            cout << "Process with ID " << processId << " terminated successfully" << endl;
            CloseHandle(proc);
            return true;
        } else {
            cout << "Failed to terminate process with ID: " << processId << " (Error: " << GetLastError() << ")" << endl;
            CloseHandle(proc);
            return false;
        }
    }

    static bool killProcessesByName(const string& processName) {
        bool found = false;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            cout << "Failed to create process snapshot" << endl;
            return false;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshot, &pe)) {
            do {
                wstring wideName(pe.szExeFile);
                string currentProcessName = wstringToString(wideName);
                
                if (_stricmp(currentProcessName.c_str(), processName.c_str()) == 0) {
                    cout << "Found process: " << currentProcessName << " (ID: " << pe.th32ProcessID << ")" << endl;
                    
                    if (killProcessById(pe.th32ProcessID)) {
                        found = true;
                    }
                }
            } while (Process32NextW(snapshot, &pe));
        }

        CloseHandle(snapshot);
        
        if (!found) {
            cout << "No processes found with name: '" << processName << "'" << endl;
        }
        
        return found;
    }

    static vector<string> getProcessesFromEnv() {
        vector<string> processes;
        char* envVar = getenv("PROC_TO_KILL");
        if (envVar != nullptr) {
            string envValue(envVar);
            stringstream ss(envValue);
            string processName;
            
            while (getline(ss, processName, ',')) {
                if (!processName.empty()) {
                    if (processName.front() == '"') {
                        processName.erase(0, 1);
                    }
                    if (processName.back() == '"') {
                        processName.pop_back();
                    }
                    processName.erase(0, processName.find_first_not_of(" "));
                    processName.erase(processName.find_last_not_of(" ") + 1);
                    if (!processName.empty()) {
                        processes.push_back(processName);
                    }
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
            cout << " - " << processName << endl;
            killProcessesByName(processName);
        }
    }

    static void printUsage() {
        cout << "Usage:" << endl;
        cout << "  killer.exe                    - terminate processes from PROC_TO_KILL" << endl;
        cout << "  killer.exe --id <process_id>  - terminate process by ID" << endl;
        cout << "  killer.exe --name <name>      - terminate processes by name" << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        ProcessKiller::killProcessesFromEnv();
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "--id" && i + 1 < argc) {
            try {
                DWORD processId = stoul(argv[++i]);
                ProcessKiller::killProcessById(processId);
            } catch (const exception& e) {
                cout << "Invalid process ID format: " << argv[i] << endl;
            }
        } else if (arg == "--name" && i + 1 < argc) {
            ProcessKiller::killProcessesByName(argv[++i]);
        } else {
            cout << "Unknown parameter: " << arg << endl;
            ProcessKiller::printUsage();
            return 1;
        }
    }
    
    return 0;
}