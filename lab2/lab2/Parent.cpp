#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>

using namespace std;

int main() {
	STARTUPINFO sti = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD excode;

	wstring ApplicationName(L"D:\labs\sp\lab2\x64\Debug\Child.exe");
	LPWSTR lpwAppName = &ApplicationName[0];

	wstring CommandLine(L"Child.exe");
	LPWSTR lpwCmdLine = &CommandLine[0];

	cout << "PARENT PROCESS START\n";
	if (!CreateProcess(NULL, // имя исполн модуля
		lpwCmdLine, //командная строка
		NULL, 
		NULL,
		TRUE, //флаг наследования дескриптора текущего процесса
		NULL, //флаги способов создания процесса
		NULL, //указатель на блок среды
		NULL, //текущий каталог
		&sti,
		&pi
	)) {
		cout << "Unable to generate process\n";
		return -1;
	}

	_getch();

	GetExitCodeProcess(pi.hProcess, &excode);
	cout << excode << endl;

	if (excode != STILL_ACTIVE) {
		cout << "Done\n";
	}
	else {
		cout << "Process stil running\n";
	}

	cout << "PARENT PROCESS END";
	_getch();
	return 0;
}