#include<Windows.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<iostream>
#include<stack>
#include<locale.h>
using namespace std;

CRITICAL_SECTION critSec;
HANDLE event;

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	char *buf = static_cast<char*>(lpParameter);
	while (1)
	{		
		EnterCriticalSection(&critSec);
		if (WaitForSingleObject(event, 0) == WAIT_OBJECT_0)
		{
			LeaveCriticalSection(&critSec);
		}
		else
		{
			//Sleep(1000);
			for (int i = 0; i < strlen(buf); i++)
			{
				cout << buf[i];
				Sleep(100);
			}
			cout << endl;
			LeaveCriticalSection(&critSec);
		}
		Sleep(100);
	}
}


void main(VOID)
{
	try{
		setlocale(LC_ALL, "Russian");
		Sleep(1000);
		printf("Hello, mr. User. I'm a parent :)\n");
		Sleep(2000);
		time_t realT;

		stack<HANDLE> threadStack;

		time(&realT);
		cout << "start: " << ctime(&realT) << endl;
		char c;
		char strMas[4][50] = { { "Проблема с доступом в ДжойКазино?" }, { "Где взять ссылку на Гит?" }, { "Какая лаба по СПОВМ?" }, { "Дайте пряник и валерьянку" } };
		short i = 0;
		InitializeCriticalSection(&critSec);
		cout << "Введите действие:\n+ -- добавить поток\n- --удалить последний добавленный поток\nq -- завершить работу\nс -- отчистить экран" << endl;//g -- запустить последний созданный поток\n
		event=CreateEvent(NULL, TRUE, FALSE, NULL);
		if (event == NULL)
		{
			cout << "Событие не создано" << endl;
			return;
		}
		while (1)
		{
			switch (_getch())
			{
			case '+':
			{
						LPVOID string = static_cast<LPVOID>(strMas[i]);
						HANDLE buf = CreateThread(NULL, 0, ThreadProc, string,  NULL/*CREATE_SUSPENDED*/, NULL);
						if (buf == NULL)
						{
							cout << "Create proc failed\n";
							CloseHandle(event);
							system("pause");
							return;
						}
						else
						{
							threadStack.push(buf);
							cout << "Создан" << endl;
						}
						i++;
						if (i > 3) i = 0;
						break;
			}
			case '-':
			{
						if (threadStack.empty())
						{
							cout << "Нет потоков к удалению" << endl;
						}
						else
						{
							//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
							SetEvent(event);
							EnterCriticalSection(&critSec);
							HANDLE buf = threadStack.top();
							if (!TerminateThread(buf, NO_ERROR))
							{
								cout << "Код ошибки: " << GetLastError() << endl << endl;
							}
							//SetThreadPriority(GetCurrentThread(), 0);
							LeaveCriticalSection(&critSec);
							ResetEvent(event);
							CloseHandle(buf);
							threadStack.pop();
							cout << "Я удалил" << endl;
						}
						break;
			}
			case 'q':
			{
						while (!threadStack.empty())
						{
							HANDLE buf = threadStack.top();
							if (!TerminateThread(buf, NO_ERROR))
							{
								cout << "Код ошибки: " << GetLastError() << endl << endl;
							}
							CloseHandle(buf);
							threadStack.pop();
						}

						DeleteCriticalSection(&critSec);
						time(&realT);
						cout << "finish: " << ctime(&realT) << endl;
						CloseHandle(event);
						//system("pause");
						return;
			}
			//case 'g':
			//{
			//		/*	if (!threadStack.empty())
			//				ResumeThread(threadStack.top());
			//			else
			//				cout << "Нет потоков" << endl;*/
			//			//PulseEvent(event);
			//			break;
			//}
			case 'c':
			{
						system("cls");
						break;
			}
			default:
			{
					   cout << "Данная функция отсутствует";
			}
			}


			cout << endl << endl;
			fflush(stdin);
			cin.sync();
		}
		getch();
	}
	catch (...)
	{
		cout << GetLastError() << endl;
		exit(0);
	}
}