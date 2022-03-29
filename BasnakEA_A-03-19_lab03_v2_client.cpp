#include <iostream>
#include <stdio.h>
#include <windows.h>

using namespace std;
int main() {

    printf("Enter pipe name (without `\\\\.\\pipe\\'): ");
    string name;
    cin >> name;
    getchar();

    auto path = "\\\\.\\pipe\\" + name;

    // подключение к именнованному каналу со стороны клиента
    auto client_pipe = CreateFileA(path.c_str(),
                                     GENERIC_READ | GENERIC_WRITE,  // режим доступа (чтение и запись)
                                     0,                             // разделение доступа
                                     NULL,                          // атрибуты безопасности
                                     OPEN_EXISTING,                 // подключение к существующему каналу
                                     FILE_ATTRIBUTE_NORMAL,         // отсутствие особых атрибутов у канала
                                     NULL);

    if (client_pipe == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile Error: %d\n", GetLastError());
        return -5;
    }

    printf("To know available instructions type 'help'.\n");

    while (true)
    {
        printf(">> ");

        // отправка запроса серверу
        string client_request = "";
        getline(cin, client_request);

        if (!WriteFile(client_pipe, client_request.c_str(), client_request.size(), NULL, NULL))
        {
            auto writeError = GetLastError();
            if (writeError != ERROR_IO_PENDING)
            {
                printf("WriteFile Error: %d", writeError);
                CloseHandle(client_pipe);
                return -2;
            }
        }

        // получение ответа от сервера
        if (client_request != "quit")
        {
            string client_response(1024, '\0');
            if (!ReadFile(client_pipe, &client_response[0], client_response.size(), NULL, NULL))
            {
                auto readError = GetLastError();
                if (readError != ERROR_IO_PENDING)
                {
                    printf("ReadFile Error: %d\n", readError);
                    CloseHandle(client_pipe);
                    return -3;
                }
            }

            printf("%s\n", client_response.c_str());
        }
        else
            break;

    }

    CloseHandle(client_pipe);

    printf("Work finished!\n");

    return 0;
}
