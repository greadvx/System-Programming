#include<iostream>
#include<Windows.h>
#include<conio.h>

using namespace std;

void main(VOID)
{
	LPCWSTR pipeName = L"\\\\.\\pipe\\MyPipe";
	LPCWSTR semaphoreName = L"MyOwnSemaphore";

	HANDLE myPipe;
	DWORD cbRead;
	char string[256];

	HANDLE mySemaphore;
	mySemaphore=OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, semaphoreName);
	if (mySemaphore == NULL)
	{
		cout << "Semaphore can not be opened. Error number: " << GetLastError() << endl;
		system("pause");
		return;
	}

	myPipe = CreateFile(pipeName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (myPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Can not get access to pipe. Error number: " << GetLastError() << endl;
		CloseHandle(mySemaphore);
		system("pause");
		return;
	}
	cout << "Client connected." << endl;
	//Дать сигнал серверу
	while (true)
	{
		WaitForSingleObject(mySemaphore, INFINITE);
		ReadFile(myPipe, string, 256, &cbRead, NULL);
		cout << "Data recived: " << string << endl;
		ReleaseSemaphore(mySemaphore, 1, NULL);
		//Sleep(1000);
		if (!strcmp(string, "exit\0")) break;
	}
	DisconnectNamedPipe(myPipe);
	CloseHandle(mySemaphore);
	CloseHandle(myPipe);

	system("pause");
	return;
}