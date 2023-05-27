#include <windows.h>
#include <tchar.h>
#include <iostream>
using namespace std;

STARTUPINFO si;
PROCESS_INFORMATION pi;

void CheckZombie() {
    // check zombie
    auto result = WaitForSingleObject(pi.hProcess, 0);

    if (result != WAIT_OBJECT_0) {
        cout << "Process ended\n";
    }
    else {
        cout << "Zombie\n";
    }
    // check zombie
}

void RunChildProcess(bool saveHandle) {
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Start the child process.
    if (!CreateProcess(L"D:\\Microsoft VS Code\\Code.exe",    // No module name (use command line)
        NULL,      // Command line
        NULL,     // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi)           // Pointer to PROCESS_INFORMATION structure
        ) {
        std::cout << "Unable to generate process\n";
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    if (saveHandle) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

}

int main() {
    cout << "Start checking zombie\n";
    int savehandle;
    while (true) {
        cout << "Would you like to save handle? 1 - Yes, 0 - No, 2 - stop programm\n";

        cin >> savehandle;
        if (savehandle == 2) {
            return 0;
            // есть ли он в диспетчере задач
            
        }
        RunChildProcess(savehandle);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CheckZombie();
    }
    
}
