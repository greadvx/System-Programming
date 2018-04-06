//server
#include<windows.h>
#include<iostream>
#include<conio.h>

using namespace std;

void main(VOID)
{
	LPCWSTR pipeName = L"\\\\.\\pipe\\MyPipe";
	LPCWSTR semaphoreName = L"MyOwnSemaphore";
	HANDLE myPipe;
	DWORD cbWritten;
	char string[256];
	HANDLE mySemaphore;
	mySemaphore = CreateSemaphore(NULL, 0, 1, semaphoreName);//семафор для одного потока. Мы уже захватили его
	if (mySemaphore == NULL)
	{
		cout << "Semaphore creation has been failed. Error number: " << GetLastError() << endl;
		return;
	}
	myPipe = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, NULL);
	if (myPipe == INVALID_HANDLE_VALUE)
	{
		cout << "Error has been created, code: " << GetLastError() << endl;
		CloseHandle(mySemaphore);
		return;
	}
	cout << "Pipe has been created. Waiting for connection... ";
	BOOL fConnect = ConnectNamedPipe(myPipe, NULL);//синхронный
	if (!fConnect)
	{
		cout << endl << "Some problem";
		CloseHandle(mySemaphore); 
		CloseHandle(myPipe); 
		return;
	}
	cout << "Connected" << endl<<"Input your messages.\nInput 'exit' to stop."<<endl;
	while (true)
	{
		cin.getline(string, 256);
		if (cin.fail())
		{
			cin.clear();
		}
		WriteFile(myPipe, string, strlen(string) + 1, &cbWritten, NULL);
		
		cout << "data has been sent" << endl;
		ReleaseSemaphore(mySemaphore, 1, NULL);
		WaitForSingleObject(mySemaphore, INFINITE);
		//Sleep(1000);
		if (!strcmp(string, "exit\0")) break;
	}
	cout << "\t\tFinish\n";
	DisconnectNamedPipe(myPipe);
	CloseHandle(myPipe);
	CloseHandle(mySemaphore);

	return;
}