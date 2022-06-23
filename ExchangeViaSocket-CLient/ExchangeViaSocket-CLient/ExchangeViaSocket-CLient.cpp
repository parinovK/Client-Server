// ExchangeViaSocket-CLient.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <experimental/filesystem>
#include <string>
#include <fcntl.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void send_file(SOCKET* sock, uint16_t Port_UDP, const string& file_name);

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
    string IP = argv[1],Path = argv[4];
    uint16_t Port_to_Server = atoi(argv[2]);
    uint16_t Port_UDP = atoi(argv[3]);
    int ms = atoi(argv[5]);

    addr.sin_addr.s_addr = inet_addr(IP.c_str());
    addr.sin_port = htons(Port_to_Server);
    addr.sin_family = AF_INET;

    SOCKET Connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr))!=0)
    {
        cout << "Error: failed connect to server\n";
        return -1;
    }
    else
    {
        cout << "Connected to server!\n";
        send_file(&Connection, Port_UDP, Path);
    }
    system("pause");
    return 0;
}

void send_file(SOCKET* sock, uint16_t Port_UDP, const string& file_name)
{
   
    send(*sock, to_string(Port_UDP).c_str(), 7, 0);

    fstream file;
    file.open(file_name, ios_base::in | ios_base::binary);

    if (file.is_open()) {

        int file_size = experimental::filesystem::file_size(file_name) + 1;

        char* bytes = new char[file_size];

        file.read(bytes, file_size);

        cout << "size: " << file_size << endl;
        cout << "name: " << file_name << endl;
        cout << "data: " << bytes << endl;

        send(*sock, to_string(file_size).c_str(), 16, 0);
        send(*sock, file_name.c_str(), 32, 0);
        send(*sock, bytes, file_size, 0);
       
        delete[] bytes;
    }
    else
        cout << "Error file open\n";
    file.close();
}