#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <string>
#include <experimental/filesystem>
#include <windows.h>
#include <shlobj.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

const int Number_Clients = 100;

SOCKET clients[Number_Clients];
int Counter = 0;

void recv_file(SOCKET* sock, string Path, int iterator);

int main(int argc, char* argv[])
{
    WSADATA data;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &data) != 0) {
        std::cout << "Error loading Lib" << endl;
        return -1;
    };

    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    string IP = argv[1], Path = argv[3];
    uint16_t Port = atoi(argv[2]);

    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(Port);
    addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;
    for (int i = 0; i < Number_Clients; i++)
    {
        newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
        if (newConnection == 0)
            std::cout << "Error. Connection failed\n";
        else {
            cout << "\n-------------------";
            cout << "\nClient connected!\n";
            recv_file(&newConnection, Path, i);
            cout << "Client disconnected!\n";
            cout << "-------------------\n";

            clients[i] = newConnection;
        }
    }
    system("pause");
    return 0;
}

void recv_file(SOCKET* sock, string Path, int iterator)
{
    cout << "===================";
    cout << "\ndata transmission\n";
    cout << "===================\n";
    char Port_UDP[8];
    recv(*sock, Port_UDP, 8, 0);
    cout << "UDP-Port: " << Port_UDP << endl;

    char file_size_str[16];
    char file_name[32];
    
    recv(*sock, file_size_str, 16, 0);
    int file_size = atoi(file_size_str);
    char* bytes = new char[file_size];

    recv(*sock, file_name, 32, 0);

    fstream file;
    if (Path == "temp" || Path == "TEMP")
        Path = getenv("temp");
    if (Path == "appdata" || Path == "APPDATA")
        Path = getenv("appdata");
    if (iterator == 0)
        cout << "\nThe file will be uploaded to this directory:\n" << Path << "\n\n";
    file.open(Path + '\\' + file_name, ios_base::out | ios_base::binary);

    cout << "Size: " << file_size << endl;
    cout << "Name: " << file_name << endl;

    if (file.is_open()) {
        recv(*sock, bytes, file_size, 0);

        file.write(bytes, file_size);
        cout << "Ok\n";
    }
    else cout << "Error file open\n";

    delete[] bytes;
    file.close();
}