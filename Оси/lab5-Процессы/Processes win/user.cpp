        #include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <tlhelp32.h>
#include <sstream>
#include <algorithm>

using namespace std;

class ProcessManager {
public:
    static bool isProcessRunning(const string& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshot, &pe)) {
            do {
                wstring wideName(pe.szExeFile);
                string currentProcessName;
                currentProcessName.resize(wideName.size());
                transform(wideName.begin(), wideName.end(), currentProcessName.begin(), [](wchar_t c) {
                    return (char)c;
                });
                
                if (_stricmp(currentProcessName.c_str(), processName.c_str()) == 0) {
                    CloseHandle(snapshot);
                    return true;
                }
            } while (Process32NextW(snapshot, &pe));
        }

        CloseHandle(snapshot);
        return false;
    }


    static DWORD findProcessId(const string& processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshot, &pe)) {
            do {
                wstring wideName(pe.szExeFile);
                string currentProcessName;
                currentProcessName.resize(wideName.size());
                transform(wideName.begin(), wideName.end(), currentProcessName.begin(), [](wchar_t c) {
                    return (char)c;
                });
                
                if (_stricmp(currentProcessName.c_str(), processName.c_str()) == 0) {
                    CloseHandle(snapshot);
                    return pe.th32ProcessID;
                }
            } while (Process32NextW(snapshot, &pe));
        }

        CloseHandle(snapshot);
        return 0;
    }

     static bool runKiller(const string& parameters) {
        STARTUPINFOW si;
        PROCESS_INFORMATION pi;
        
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        
        string command = "killer.exe " + parameters;
    
        wstring wcommand(command.length(), L' ');
        for (size_t i = 0; i < command.length(); ++i) {
            wcommand[i] = static_cast<wchar_t>(command[i]);
        }
        
        wchar_t cmd[256];
        wcscpy(cmd, wcommand.c_str());
        
        if (!CreateProcessW(nullptr, cmd, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            cout << "Failed to run killer.exe (Error: " << GetLastError() << ")" << endl;
            return false;
        }
        
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return (exitCode == 0);
    }

    
};


void testKillByName(const string& name) {
    if (ProcessManager::isProcessRunning(name)) {
        cout << name + " is running" << endl;
        
        cout << "Running: killer.exe --name " + name << endl;
        ProcessManager::runKiller("--name " + name);
        Sleep(1000);

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
        DWORD id = ProcessManager::findProcessId(name);
        cout << name + " is running (PID: " << id << ")" << endl;
        
        string command = "--id " + to_string(id);
        cout << "Running: " << command << endl;
        ProcessManager::runKiller(command.c_str());
        Sleep(1000);

        if (!ProcessManager::isProcessRunning(name)) {
            cout << "SUCCESS:" +name + "was terminated" << endl;
        } else {
            cout << "FAIL" << endl;
        }
    } else {
        cout << name + "was not running" << endl;
    }
}

void testKillByEnv(const vector<string> &v) {
    Sleep(2000);

    for(int i = 0; i < v.size(); i++){
        bool r = ProcessManager::isProcessRunning(v[i]);
        cout << "Before killer: " + v[i] << (r ? " running" : " not running") << endl;
    }

    cout << "Running: killer.exe" << endl;
    ProcessManager::runKiller("");
    Sleep(1000);
    
    for(int i = 0; i < v.size(); i++){
        bool r = ProcessManager::isProcessRunning(v[i]);
        cout << "After killer: " + v[i] << (r ? " running" : " not running") << endl;
    }
}

int main() {
    vector<string> testProcesses = {"Discord.exe", "mspaint.exe"};
    string envValue;
    for (size_t i = 0; i < testProcesses.size(); i++) {
        envValue += testProcesses[i];
        if (i < testProcesses.size() - 1) {
            envValue += ",";
        }
    }

    SetEnvironmentVariableA("PROC_TO_KILL", envValue.c_str());
    cout << "PROC_TO_KILL = " << envValue << endl;

    cout << "\nKill by name " << endl;
    testKillByName("Telegram.exe");

    cout << "\nKill by id " << endl;
    testKillById("cmd.exe");

    cout << "\nKill by environment variable" << endl;
    testKillByEnv(testProcesses);
    return 0;
}