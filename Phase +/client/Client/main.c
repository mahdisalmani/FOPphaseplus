/* COPYRIGHT 2019 - 2020 BY MAHDI SALMANI*/

/*include lib we need*/

#include <stdio.h>
#include <winsock2.h>
//#include "cJSON.h"
#include "cJSON.c"
#include <string.h>


#pragma comment(lib , "Ws2_32.lib")

/*Macros*/

#define PORT 12345
#define LOCALHOST "127.0.0.1"
//

/* Prototypes Define*/

SOCKET ConnectionMaker();
void ConnectionCloser(SOCKET);

void AccountMenu();
void RegisterOperand();
void LoginOperand();

void MainMenu(cJSON*);
void CreatechannelOperand(cJSON*);
void JoinchannelOperand(cJSON*);
void LogoutOperand(cJSON*);

void ChannelMenu(cJSON*);
void SendOperand(cJSON*);
void RefreshOperand(cJSON*);
void LeaveOperand(cJSON*);
void MembersOperand(cJSON*);
void FindOperand(cJSON*);
void SearchOperand(cJSON*);

//

int main()

{
    puts("WARNING : Logout Before Exit Because This Program Doesn't Save Your Token In A File!");

    AccountMenu();
}

//ENDOFMAIN

SOCKET ConnectionMaker()

{
    WSADATA wsaData;

    WSAStartup(MAKEWORD(2,2), &wsaData);

    // Create a socket.

    SOCKET m_socket;

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_socket == INVALID_SOCKET)

    {
        printf("Client: socket() - Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
    }

    // Connect to a server.

    SOCKADDR_IN clientService;

    clientService.sin_family = AF_INET;

    clientService.sin_addr.s_addr = inet_addr(LOCALHOST);

    clientService.sin_port = htons(PORT);

    if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)

    {
        printf("Client: connect() - Failed to connect.\n");
        WSACleanup();
    }

    // Return socket

    return m_socket;
}

//

void ConnectionCloser(SOCKET s)

{
    closesocket(s);
}

//

void AccountMenu()

{
    // View

    puts("\nAccount Menu");
    puts("1 : Register");
    puts("2 : Login");

    // Operand

    char operand[10] = {0} ;

    puts("Enter num of operate: ");
    scanf("%s" , &operand);

    if (operand[0] == '1' && operand[1] == '\0')

        RegisterOperand();

    else if (operand[0] == '2' && operand[1] == '\0')

        LoginOperand();

    else
    {
        puts("Invalid Operate");

        AccountMenu();
    }
}

//

void RegisterOperand()

{
    //view

    char username[50] = {0};

    puts("Enter Chosen Username (Without Space and MAX 49 Characters): ");
    scanf("%s" , username );

    char password[50] = {0};

    puts("Enter Chosen Password (Without Space and MAX 49 Characters) : ");
    scanf("%s" , password );

    //Send to Server

    char BUFFER[1028] = {0};

    strcat(BUFFER , "register ");
    strcat(BUFFER , username);
    strcat(BUFFER , ", ");
    strcat(BUFFER , password);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    char ErOSuc[100] = {0};

    for(int i = 0 ; i < strlen(cJSON_Print(type)) ; i++)
        ErOSuc[i] = *(cJSON_Print(type) + i);

    if (ErOSuc == "\"Error\"")

        printf("%s %s \n" , cJSON_Print(type) , cJSON_Print(content));

    else

        printf("%s \n" , cJSON_Print(type));


    //FINISH

    ConnectionCloser(s);

    AccountMenu();

}

//

void LoginOperand()

{
    //view

    char username[50] = {0};

    puts("Enter Username : ");
    scanf("%s" , username );

    char password[50] = {0};

    puts("Enter Password : ");
    scanf("%s" , password );

    //Send to Server

    char BUFFER[1028] = {0};

    strcat(BUFFER , "login ");
    strcat(BUFFER , username);
    strcat(BUFFER , ", ");
    strcat(BUFFER , password);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    char ErOSuc[100] = {0};

    for(int i = 0 ; i < strlen(cJSON_Print(type)); i++)
        ErOSuc[i] = *(cJSON_Print(type) + i);

    if ( strcmp("\"Error\"" , ErOSuc) == 0)
    {
        printf("%s %s \n" , cJSON_Print(type) , cJSON_Print(content));

        AccountMenu();

    }

    else

        MainMenu(content);

    //FINISH

}

//

void MainMenu(cJSON* AuthToken)

{
// View

    puts("\nMain Menu");
    puts("1 : Create Channel");
    puts("2 : Join Channel");
    puts("3 : Logout");

    // Operand

    char operand[10] = {0} ;

    puts("Enter num of operate: ");
    scanf("%s" , &operand);

    if (operand[0] == '1' && operand[1] == '\0')

        CreatechannelOperand(AuthToken);

    else if (operand[0] == '2' && operand[1] == '\0')

        JoinchannelOperand(AuthToken);

    else if (operand[0] == '3' && operand[1] == '\0')

        LogoutOperand(AuthToken);

    else
    {
        puts("Invalid Operate");

        MainMenu(AuthToken);
    }
}

//

void CreatechannelOperand(cJSON* AuthToken)

{
    //view

    char channel_name[50] = {0};

    puts("Enter Channel name (without space and MAX 49 characters) : ");
    scanf("%s" , channel_name );

    //Send to Server

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "create channel ");
    strcat(BUFFER , channel_name);
    strcat(BUFFER , ", ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    char ErOSuc[100] = {0};

    for(int i = 0 ; i < strlen(cJSON_Print(type)) ; i++)
        ErOSuc[i] = *(cJSON_Print(type) + i);

    if (strcmp("\"Error\"" , ErOSuc) == 0)
    {
        printf("%s %s \n" , cJSON_Print(type) , cJSON_Print(content));

        MainMenu(AuthToken);

    }

    else

        ChannelMenu(AuthToken);

}

//

void JoinchannelOperand(cJSON* AuthToken)

{
//view

    char channel_name[50] = {0};

    puts("Enter Channel name : ");
    scanf("%s" , channel_name );

    //Send to Server

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "join channel ");
    strcat(BUFFER , channel_name);
    strcat(BUFFER , ", ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    char ErOSuc[100] = {0};

    for(int i = 0 ; i < strlen(cJSON_Print(type)) ; i++)
        ErOSuc[i] = *(cJSON_Print(type) + i);

    if (strcmp("\"Error\"" , ErOSuc) == 0)
    {
        printf("%s %s \n" , cJSON_Print(type) , cJSON_Print(content));

        MainMenu(AuthToken);

    }

    else

        ChannelMenu(AuthToken);
}

//

void LogoutOperand(cJSON* AuthToken)

{

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "logout ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    printf("%s\n" , cJSON_Print(type));

    AccountMenu();
}

//

void ChannelMenu(cJSON* AuthToken)

{
    // View

    puts("\nChannel Menu");
    puts("1 : Send Message");
    puts("2 : Refresh");
    puts("3 : Channel Members");
    puts("4 : Leave Channel");
    puts("5 : Logout");
    puts("6 : Find Members");
    puts("7 : Search Message");
    // Operand

    char operand[10] = {0} ;

    puts("Enter num of operate: ");
    scanf("%s" , &operand);

    if (operand[0] == '1' && operand[1] == '\0')

        SendOperand(AuthToken);

    else if (operand[0] == '2' && operand[1] == '\0')

        RefreshOperand(AuthToken);

    else if (operand[0] == '3' && operand[1] == '\0')

        MembersOperand(AuthToken);

    else if (operand[0] == '4' && operand[1] == '\0')

        LeaveOperand(AuthToken);

    else if (operand[0] == '5' && operand[1] == '\0')

        LogoutOperand(AuthToken);

    else if (operand[0] == '6' && operand[1] == '\0')

        FindOperand(AuthToken);

    else if (operand[0] == '7' && operand[1] == '\0')

        SearchOperand(AuthToken);

    else
    {
        puts("Invalid Operate");

        ChannelMenu(AuthToken);
    }
}

//

void SendOperand(cJSON* AuthToken)

{
    //view

    char temp;
    char Message[1028] = {0};

    puts("Enter Your Message (MAX 1027 character) : ");
    scanf("%c" , &temp); //I Searched this in internet
    scanf("%[^\n]" , Message );

    //Send to Server

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "send ");
    strcat(BUFFER , Message);
    strcat(BUFFER , ", ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    ChannelMenu(AuthToken);
}

//

void RefreshOperand(cJSON* AuthToken)

{
    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "refresh ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    //char ANSWER[1000000] = {0};
    char *ANSWER = malloc(1000000);
    shutdown(s , 1);

    recv(s , ANSWER , 1000000 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    free(ANSWER); // THIS IS ESSENTIAL

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    for (int i = 0 ; i < cJSON_GetArraySize(content) ; i++)
    {

        cJSON* Sub = cJSON_GetArrayItem(content , i);

        cJSON* Sender = cJSON_GetObjectItem(Sub , "sender");

        cJSON* Message = cJSON_GetObjectItem(Sub , "content");

        //Print

        printf("%s  send   %s  \n\n" , cJSON_Print(Sender) , cJSON_Print(Message));
    }

    ChannelMenu(AuthToken);
}

//

void MembersOperand(cJSON* AuthToken)

{
    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "channel members ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[10000] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 10000 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    for (int i = 0 ; i < cJSON_GetArraySize(content) ; i++)
    {

        cJSON* Sub = cJSON_GetArrayItem(content , i);

        //Print

        printf("%s \n\n" , cJSON_Print(Sub));
    }

    ChannelMenu(AuthToken);
}

//

void LeaveOperand(cJSON* AuthToken)
{
    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "leave ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    printf("%s\n" , cJSON_Print(type));

    MainMenu(AuthToken);
}

//

void FindOperand(cJSON* AuthToken)

{
//view

    char name[50] = {0};

    puts("Enter Username you want find : ");
    scanf("%s" , name );

    //Send to Server

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "find ");
    strcat(BUFFER , name);
    strcat(BUFFER , ", ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    char ANSWER[1028] = {0};

    shutdown(s , 1);

    recv(s , ANSWER , 1028 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    printf("%s\n" , cJSON_Print(content));

    ChannelMenu(AuthToken);
}

//

void SearchOperand(cJSON *AuthToken)

{
    char word[1028] = {0};

    puts("Enter Word you want find : ");
    scanf("%s" , word );

    char BUFFER[1028] = {0};

    char *Auth = cJSON_Print(AuthToken);
    *(Auth + strlen(Auth) - 1) = NULL;

    strcat(BUFFER , "search ");
    strcat(BUFFER , word);
    strcat(BUFFER , ", ");
    strcat(BUFFER , Auth + 1);

    //

    SOCKET s;
    s = ConnectionMaker();

    send(s , BUFFER , strlen(BUFFER) , 0 );

    //Recv from Server

    //char ANSWER[1000000] = {0};
    char *ANSWER = malloc(1000000);
    shutdown(s , 1);

    recv(s , ANSWER , 1000000 , 0 );

    shutdown(s , 2);

    ConnectionCloser(s);

    //Convert to cJSON

    cJSON *ans ;

    ans = cJSON_Parse(ANSWER);

    free(ANSWER); // THIS IS ESSENTIAL

    cJSON *type = cJSON_GetObjectItem(ans , "type");

    cJSON *content = cJSON_GetObjectItem(ans , "content");

    // Calculate

    for (int i = 0 ; i < cJSON_GetArraySize(content) ; i++)
    {

        cJSON* Sub = cJSON_GetArrayItem(content , i);

        cJSON* Sender = cJSON_GetObjectItem(Sub , "sender");

        cJSON* Message = cJSON_GetObjectItem(Sub , "content");

        //Print

        printf("%s  send   %s  \n\n" , cJSON_Print(Sender) , cJSON_Print(Message));
    }

    ChannelMenu(AuthToken);
}
// FINISH
