#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

bool CreateProcessWithRedirect(const std::string& command, HANDLE hStdin, HANDLE hStdout, PROCESS_INFORMATION& pi) {
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdin;
    si.hStdOutput = hStdout;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    char* cmd = new char[command.length() + 1];
    strcpy(cmd, command.c_str());

    BOOL result = CreateProcessA(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    delete[] cmd;
    return result;
}

std::string ReadFromPipe(HANDLE hPipe) {
    std::string result;
    char buffer[1024];
    DWORD bytesRead;
    while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }
    return result;
}

int main() {
    HANDLE hPipeMainToM_Read, hPipeMainToM_Write;
    HANDLE hPipeMToA_Read, hPipeMToA_Write;
    HANDLE hPipeAToP_Read, hPipeAToP_Write;
    HANDLE hPipePToS_Read, hPipePToS_Write;
    HANDLE hPipeSToMain_Read, hPipeSToMain_Write;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    PROCESS_INFORMATION piM, piA, piP, piS;

    CreatePipe(&hPipeMainToM_Read, &hPipeMainToM_Write, &sa, 0);
    CreatePipe(&hPipeMToA_Read, &hPipeMToA_Write, &sa, 0);

    SetHandleInformation(hPipeMainToM_Write, HANDLE_FLAG_INHERIT, 0);

    std::cout << "Launching M..." << std::endl;
    if (!CreateProcessWithRedirect("M.exe", hPipeMainToM_Read, hPipeMToA_Write, piM)) return 1;

    CloseHandle(hPipeMainToM_Read); ю
    CloseHandle(hPipeMToA_Write);

    CreatePipe(&hPipeAToP_Read, &hPipeAToP_Write, &sa, 0);
    
    std::cout << "Launching A..." << std::endl;
    if (!CreateProcessWithRedirect("A.exe", hPipeMToA_Read, hPipeAToP_Write, piA)) return 1;

    CloseHandle(hPipeMToA_Read);
    CloseHandle(hPipeAToP_Write);


    CreatePipe(&hPipePToS_Read, &hPipePToS_Write, &sa, 0);

    std::cout << "Launching P..." << std::endl;
    if (!CreateProcessWithRedirect("P.exe", hPipeAToP_Read, hPipePToS_Write, piP)) return 1;

    CloseHandle(hPipeAToP_Read);
    CloseHandle(hPipePToS_Write);

    CreatePipe(&hPipeSToMain_Read, &hPipeSToMain_Write, &sa, 0);

 
    SetHandleInformation(hPipeSToMain_Read, HANDLE_FLAG_INHERIT, 0);

    std::cout << "Launching S..." << std::endl;
    if (!CreateProcessWithRedirect("S.exe", hPipePToS_Read, hPipeSToMain_Write, piS)) return 1;

    CloseHandle(hPipePToS_Read);
    CloseHandle(hPipeSToMain_Write); 


    // ввод
    std::cout << "Enter numbers separated by spaces: ";
    std::string input;
    std::getline(std::cin, input);

    DWORD bytesWritten;
    std::string dataToWrite = input + "\n";
    WriteFile(hPipeMainToM_Write, dataToWrite.c_str(), dataToWrite.length(), &bytesWritten, NULL);
    
    CloseHandle(hPipeMainToM_Write);

    std::cout << "Processing..." << std::endl;

    WaitForSingleObject(piM.hProcess, INFINITE);
    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piP.hProcess, INFINITE);
    WaitForSingleObject(piS.hProcess, INFINITE);

    std::string result = ReadFromPipe(hPipeSToMain_Read);
    std::cout << "Result: " << result << std::endl;

    CloseHandle(hPipeSToMain_Read);

    CloseHandle(piM.hProcess); CloseHandle(piM.hThread);
    CloseHandle(piA.hProcess); CloseHandle(piA.hThread);
    CloseHandle(piP.hProcess); CloseHandle(piP.hThread);
    CloseHandle(piS.hProcess); CloseHandle(piS.hThread);

    return 0;
}