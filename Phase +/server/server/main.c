/* COPYRIGHT 2019 - 2020 BY MAHDI SALMANI*/

/*include lib we need*/

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
//#include "cJSON.h"
#include "cJSON.c"
#include <string.h>
#include <unistd.h>
#include <time.h>

#pragma comment(lib , "Ws2_32.lib")

/*Macros*/

#define PORT 12345
#define LOCALHOST "127.0.0.1"

//Prototypes

SOCKET ConnectionMaker();
void ConnectionCloser(SOCKET );

void Request(char* , SOCKET);

void RegisterOperand(char* , SOCKET);
void LoginOperand(char* , SOCKET);

void CreateChannel(char* , SOCKET);
void JoinChannel(char* , SOCKET);

void LogoutOperand(char* , SOCKET);
void LeaveOperand(char* , SOCKET);

void SendOperand(char* , SOCKET);
void RefreshOperand(char*, SOCKET);
void MemberOperand(char*, SOCKET);

void InvalidRequest(SOCKET);

void Send_Message(cJSON* , SOCKET);

int TokenGenerator();

void FindOperand(char* , SOCKET);

void SearchOperand(char* , SOCKET);

int isinstring(char* , char*);
//
int TOKENS[1000000]; // saving all Tokens
int ALLTOKENS; // number of all assigned tokens

// struct containing OnlineUsers attributes
struct Users
{
    char Auth[7];
    char Username[50];
    char Channel[50];
    int Last_Seen;
} OnlineUsers[1000000]; // 999999 is Maximum Token We assign

//

int main()
{
    // Socket
    ALLTOKENS = 0;
    SOCKET S ;
    puts("MY SERVER\n");

    char BUFFER[2000];

    // Loop for server recv

    while (1)
    {
        memset(BUFFER , 0 , 2000);
        S = ConnectionMaker();

        listen(S , 65535); // Listen to client

        SOCKET NewS = accept(S , NULL , NULL);

        //Recv from Client

        recv(NewS , BUFFER , 2000 , 0);
        printf("%s\n\n" , BUFFER);
        shutdown(NewS , 0);

        //Request(Evaluate the request)

        Request(BUFFER , NewS);

        //Close Connection;

        ConnectionCloser(NewS);
        ConnectionCloser(S);

    }

}

//

SOCKET ConnectionMaker() // Making Socket Function

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

    if (bind(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)

    {
        printf("Client: connect() - Failed to connect.\n");
        WSACleanup();
    }

    // Return socket

    return m_socket;
}

//

void ConnectionCloser(SOCKET s) // Closing Socket Function

{
    closesocket(s);
}

//

void Request(char* BUFFER , SOCKET S)

{
    char Operand[1028] , Option[2000]; // String for format reading

    sscanf(BUFFER , "%s %[^\n]" , Operand , Option);

    if (strcmp(Operand , "register") == 0)

        RegisterOperand(Option , S);

    else if (strcmp(Operand , "login") == 0)

        LoginOperand(Option , S);

    else if(strcmp(Operand , "find") == 0)

        FindOperand(Option , S);

    else if(strcmp(Operand , "search") == 0)

        SearchOperand(Option , S);

    else if (strcmp(Operand , "refresh") == 0)

        RefreshOperand(Option , S);

    else if (strcmp(Operand , "logout") == 0)

        LogoutOperand(Option , S);

    else if (strcmp(Operand , "send") == 0)

        SendOperand(Option , S);

    else if (strcmp(Operand , "leave") == 0)

        LeaveOperand(Option , S);

    else if (strcmp(Operand , "create") == 0)
    {
        sscanf(Option , "%s %[^\n]" , Operand , Option);

        if (strcmp(Operand , "channel") == 0)
            CreateChannel(Option , S);

        else
            InvalidRequest(S);
    }

    else if (strcmp(Operand , "join") == 0)
    {
        sscanf(Option , "%s %[^\n]" , Operand , Option);

        if (strcmp(Operand , "channel") == 0)
            JoinChannel(Option , S);

        else
            InvalidRequest(S);
    }

    else if (strcmp(Operand , "channel") == 0)
    {
        sscanf(Option , "%s %[^\n]" , Operand , Option);

        if (strcmp(Operand , "members") == 0)
            MemberOperand(Option , S);

        else
            InvalidRequest(S);
    }

    else
        InvalidRequest(S);
}

//

void RegisterOperand(char* BUFFER , SOCKET S)

{
    cJSON *Message , *Data;

    char Username[50] , Password[50];

    sscanf(BUFFER , "%[^,], %[^\n]" , Username , Password); //Format reading

    Message = cJSON_CreateObject();

    Data = cJSON_CreateObject();

    //Opening Proper File

    char address[100] = {0};

    strcat(address , "Data/Users/");
    strcat(address , Username);
    strcat(address , ".json");

    FILE *fileptr = fopen(address , "r+");

    //if the file doesn't exist the user doesn't exist too

    if (fileptr != NULL)

    {   // Create Proper Message
        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString("The username has already existed"));

    }

    else

    {
        fileptr = fopen(address , "w"); // if the file doesn't exist we will make it

        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

        cJSON_AddItemToObject(Data, "username", cJSON_CreateString(Username));
        cJSON_AddItemToObject(Data, "password", cJSON_CreateString(Password));

        fputs(cJSON_Print(Data), fileptr) ;
    }


    fclose(fileptr);

    // End of file reading

    // send Message

    Send_Message(Message , S);
}

//

void LoginOperand(char *BUFFER , SOCKET S)

{
    cJSON *Message , *DataJ;

    char Username[50] , Password[50];

    sscanf(BUFFER , "%[^,], %[^\n]" , Username , Password); //Format reading

    Message = cJSON_CreateObject();

    DataJ = cJSON_CreateObject();

    // Same as previous function

    char address[100] = {0};

    strcat(address , "Data/Users/");
    strcat(address , Username);
    strcat(address , ".json");

    FILE *fileptr = fopen(address , "rb");

    if (fileptr == NULL)

    {
        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString("The username doesn't exist"));
    }

    else

    {
        // Read file data in cjson format

        fseek (fileptr, 0, SEEK_END);
        int length = ftell (fileptr);
        fseek (fileptr, 0, SEEK_SET);
        char *Data = malloc (length);
        fread (Data, 1 , length, fileptr);

        DataJ = cJSON_Parse(Data);
        char *Pass;

        Pass = cJSON_Print(cJSON_GetObjectItem(DataJ , "password"));

        // End of reading file in cjson format

        // Checking the password is correct or not

        *(Pass + strlen(Pass) - 1) = 0;

        if (strcmp(Pass + 1 , Password) == 0) // if pass is correct
        {
            cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful")); // Proper Message

            // Making 6-digit Token

            int Token = TokenGenerator();
            char str[7];
            sprintf(str, "%06d", Token);

            // End of Generating

            cJSON_AddItemToObject(Message, "content", cJSON_CreateString(str)); // Proper Message

            //Fill Online User attributes

            sprintf(OnlineUsers[Token].Auth , "%s" , str);
            sprintf(OnlineUsers[Token].Channel , "%s" , "");
            sprintf(OnlineUsers[Token].Username , "%s" , Username);
            OnlineUsers[Token].Last_Seen = 0 ;

            // End of Filling
        }

        else
        {
            // Proper Message
            cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
            cJSON_AddItemToObject(Message, "content", cJSON_CreateString("Password is invalid"));
        }

    }


    fclose(fileptr);

    // End of File reading

    // Same as previous function

    Send_Message(Message , S);
}

//

void CreateChannel(char *BUFFER , SOCKET S)

{
    cJSON *Message , *Data , *Array;

    char Channel[50] , Token[7];

    sscanf(BUFFER , "%[^,], %[^\n]" , Channel , Token); // Format Reading

    Message = cJSON_CreateObject();

    Data = cJSON_CreateObject();

    Array = cJSON_CreateArray();

    // File Opening

    char address[100] = {0};

    strcat(address , "Data/Channels/");
    strcat(address , Channel);
    strcat(address , ".json");

    FILE *fileptr = fopen(address , "r+");

    if (fileptr != NULL)

    { // if file existed
        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString("The channel has already existed"));

    }

    else

    {
        fileptr = fopen(address , "w");

        //proper Message

        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

        // New Message in channel

        cJSON_AddItemToObject(Data, "sender", cJSON_CreateString("server"));
        cJSON_AddItemToObject(Data, "content", cJSON_CreateString("channel created"));
        cJSON_AddItemToArray(Array , Data);

        int Token_num = strtol(Token , NULL , 10); // elicit the token

        //filling user attribute

        sprintf(OnlineUsers[Token_num].Channel , "%s" , Channel);
        OnlineUsers[Token_num].Last_Seen = 0;

        //

        //writing new information in Proper file

        fputs(cJSON_Print(Array), fileptr) ;
    }


    fclose(fileptr);

    //End of file reading

    Send_Message(Message , S);
}

//

void LogoutOperand(char *Token , SOCKET S)

{

    int Token_num = strtol(Token , NULL , 10); // elicit the Token

    //Remove Users Data

    if (strcmp(OnlineUsers[Token_num].Channel , "") != 0) // if The user was in a channel it should be deleted from channel

        LeaveOperand(Token , S);

    sprintf(OnlineUsers[Token_num].Auth , "%s" , "");
    sprintf(OnlineUsers[Token_num].Username , "%s" , "");
    sprintf(OnlineUsers[Token_num].Channel , "%s" , "");
    OnlineUsers[Token_num].Last_Seen = 0 ;

    // End of Removing

    // Sending proper response

    cJSON *Message;
    Message = cJSON_CreateObject();

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

    Send_Message(Message , S);

    // End of sending

}

//

void LeaveOperand(char *Token , SOCKET S)

{
    cJSON *Message , *DataJ , *NewMessage;

    Message = cJSON_CreateObject();
    DataJ = cJSON_CreateArray();
    NewMessage = cJSON_CreateObject();

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

    int Token_num = strtol(Token , NULL , 10); // elicit the Token

    //Reading file for send leave channel message

    char address[100] = {0};
    strcat(address , "Data/Channels/");
    strcat(address , OnlineUsers[Token_num].Channel);
    strcat(address , ".json");
    FILE *fileptr = fopen(address , "rb");


    fseek (fileptr, 0, SEEK_END); // reading file
    int length = ftell (fileptr);
    fseek (fileptr, 0, SEEK_SET);
    char *Data = malloc (length);
    fread (Data, 1 , length, fileptr);

    DataJ = cJSON_Parse(Data);

    cJSON_AddItemToObject(NewMessage , "sender" , cJSON_CreateString(OnlineUsers[Token_num].Username));
    cJSON_AddItemToObject(NewMessage , "content" , cJSON_CreateString("Leaved Channel"));
    cJSON_AddItemToArray(DataJ , NewMessage); // add new message

    fclose(fileptr);
    fileptr = fopen(address , "w");
    fputs(cJSON_Print(DataJ), fileptr); // rewrite new data

    // End of sending

    // Deleting Channel name in user's data

    sprintf(OnlineUsers[Token_num].Channel , "%s" , "");
    OnlineUsers[Token_num].Last_Seen = 0 ;

    fclose(fileptr);

    // Response to Client

    Send_Message(Message , S);

}

//

void SendOperand(char *BUFFER , SOCKET S)

{
    // Same as previous Function

    char content[1028] , Token[7];

    sscanf(BUFFER , "%[^,], %[^\n]" , content , Token);

    cJSON *Message , *DataJ , *NewMessage;

    Message = cJSON_CreateObject();
    DataJ = cJSON_CreateArray();
    NewMessage = cJSON_CreateObject();

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

    int Token_num = strtol(Token , NULL , 10);

    char address[100] = {0};
    strcat(address , "Data/Channels/");
    strcat(address , OnlineUsers[Token_num].Channel);
    strcat(address , ".json");
    FILE *fileptr = fopen(address , "rb");


    fseek (fileptr, 0, SEEK_END);
    int length = ftell (fileptr);
    fseek (fileptr, 0, SEEK_SET);
    char *Data = malloc (length);
    fread (Data, 1 , length, fileptr);

    DataJ = cJSON_Parse(Data);


    cJSON_AddItemToObject(NewMessage , "sender" , cJSON_CreateString(OnlineUsers[Token_num].Username));
    cJSON_AddItemToObject(NewMessage , "content" , cJSON_CreateString(content));
    cJSON_AddItemToArray(DataJ , NewMessage);

    fclose(fileptr);
    fileptr = fopen(address , "w");
    fputs(cJSON_Print(DataJ), fileptr);

    fclose(fileptr);

    Send_Message(Message , S);
    // The difference is just in Deleting channel Data

}

//

void JoinChannel(char *BUFFER , SOCKET S)

{
    cJSON *Message;

    char Channel[50] , Token[7];

    sscanf(BUFFER , "%[^,], %[^\n]" , Channel , Token);

    Message = cJSON_CreateObject();

    // Opening the file

    char address[100] = {0};

    strcat(address , "Data/Channels/");
    strcat(address , Channel);
    strcat(address , ".json");

    FILE *fileptr = fopen(address , "r");

    //

    if (fileptr == NULL) // if channel doesn't exist

    {
        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString("The doesn't exist"));
    }

    else // if channel existed
    {
        cJSON *DataJ , *NewMessage;

        DataJ = cJSON_CreateArray();
        NewMessage = cJSON_CreateObject();

        cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString(""));

        int Token_num = strtol(Token , NULL , 10);

        sprintf(OnlineUsers[Token_num].Channel , "%s" , Channel);


        char address[100] = {0};
        strcat(address , "Data/Channels/");
        strcat(address , OnlineUsers[Token_num].Channel);
        strcat(address , ".json");
        FILE *fileptr = fopen(address , "rb");

        // reading file in json format
        fseek (fileptr, 0, SEEK_END);
        int length = ftell (fileptr);
        fseek (fileptr, 0, SEEK_SET);
        char *Data = malloc (length);
        fread (Data, 1 , length, fileptr);

        DataJ = cJSON_Parse(Data);


        cJSON_AddItemToObject(NewMessage , "sender" , cJSON_CreateString(OnlineUsers[Token_num].Username));
        cJSON_AddItemToObject(NewMessage , "content" , cJSON_CreateString("joined the channel"));
        cJSON_AddItemToArray(DataJ , NewMessage); // new data for rewrite

        fclose(fileptr);

        //close reading mode

        // opening in write mode
        fileptr = fopen(address , "w");
        fputs(cJSON_Print(DataJ), fileptr); // rewrite the file

        fclose(fileptr);
    }

    // Response to client

    Send_Message(Message , S);

}

//

void RefreshOperand(char* Token , SOCKET S)

{
    cJSON *Message , *DataJ;

    Message = cJSON_CreateObject();
    DataJ = cJSON_CreateArray();

    int Token_num = strtol(Token , NULL , 10);

    // Same as previous function

    char address[100] = {0};
    strcat(address , "Data/Channels/");
    strcat(address , OnlineUsers[Token_num].Channel);
    strcat(address , ".json");
    FILE *fileptr = fopen(address , "rb");

    // reading file in binary mode for eliciting in json format

    fseek (fileptr, 0, SEEK_END);
    int length = ftell (fileptr);
    fseek (fileptr, 0, SEEK_SET);
    char *Data = malloc (length);
    fread (Data, 1 , length, fileptr);

    DataJ = cJSON_Parse(Data);

    cJSON *Seen = cJSON_CreateArray();

    // in this situation cjson performs a weird action and it copies the objects that are in array and index is upper than token last seen

    cJSON_AddItemToArray(Seen , cJSON_GetArrayItem(DataJ , OnlineUsers[Token_num].Last_Seen));

    OnlineUsers[Token_num].Last_Seen = cJSON_GetArraySize(DataJ); // refresh the last seen point

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(Message, "content", Seen);

    fclose(fileptr);

    // Same as previous function

    Send_Message(Message , S);
}

//

void MemberOperand(char* Token , SOCKET S)

{
    int Token_num = strtol(Token , NULL , 10);

    cJSON *Message , *Data;

    Message = cJSON_CreateObject();
    Data = cJSON_CreateArray();

    for (int i = 0 ; i < 1000000 ; i++) // Checking current channel name in other online users

    {
        if (strcmp(OnlineUsers[i].Channel , OnlineUsers[Token_num].Channel) == 0) // if the channels are same
        {
            cJSON_AddItemToArray(Data , cJSON_CreateString(OnlineUsers[i].Username)); // adding usernames
        }
    }

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(Message, "content", Data);

    Send_Message(Message , S);
}

//

void InvalidRequest(SOCKET S) // if the request from client was invalid

{
    cJSON *Message;

    Message = cJSON_CreateObject();

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("Error"));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString("Invalid Request"));

    Send_Message(Message , S);
}

void Send_Message(cJSON* Message , SOCKET S)

{
    char *out;

    out = cJSON_Print(Message);

    send(S , out , strlen(out) , 0);

    shutdown(S , 2);
}

int TokenGenerator()

{
    int Token , i;
    srand(time(NULL));

    while (1)
    {
        Token = rand() % 1000000;
        for(i = 0 ; i < ALLTOKENS ; i++) // Token should be unique
            if (Token == TOKENS[i])
                break;
        if (i == ALLTOKENS)
        {
            ALLTOKENS++;
            return Token;
        }
    }
}

void FindOperand(char* BUFFER , SOCKET S)

{
    char Token[7] = {0} , Name[50] = {0} ;

    sscanf(BUFFER , "%[^,], %[^\n]" , Name , Token);

    int Token_num = strtol(Token , NULL , 10);

    cJSON *Message;

    Message = cJSON_CreateObject();

    char Data[4] = "No" ;

    for (int i = 0 ; i < 1000000 ; i++) // Checking current channel name in other online users

    {
        if (strcmp(OnlineUsers[i].Channel , OnlineUsers[Token_num].Channel) == 0) // if the channels are same
        {
            if (strcmp(OnlineUsers[i].Username , Name) == 0) memcpy(Data , "Yes" , 3); // adding usernames
        }
    }

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString(Data));

    Send_Message(Message , S);
}

//

void SearchOperand(char* BUFFER, SOCKET S)

{
    char Token[7] = {0} , Word[1028] = {0} ;

    sscanf(BUFFER , "%[^,], %[^\n]" , Word , Token);

    int Token_num = strtol(Token , NULL , 10);

    // Same as previous function

    char address[100] = {0};
    strcat(address , "Data/Channels/");
    strcat(address , OnlineUsers[Token_num].Channel);
    strcat(address , ".json");
    FILE *fileptr = fopen(address , "rb");

    // reading file in binary mode for eliciting in json format

    fseek (fileptr, 0, SEEK_END);
    int length = ftell (fileptr);
    fseek (fileptr, 0, SEEK_SET);
    char *Data = malloc (length);
    fread (Data, 1 , length, fileptr);

    cJSON *Message , *DataJ = cJSON_Parse(Data);

    cJSON *Seen = cJSON_CreateArray();

    Message = cJSON_CreateObject();

    //

    for(int i = 0 ; i < cJSON_GetArraySize(DataJ) ; i++)

    {
        char *out = cJSON_Print(cJSON_GetObjectItem(cJSON_GetArrayItem(DataJ , i) , "content"));

        if (isinstring(out , Word) == 1) cJSON_AddItemToArray(Seen , cJSON_Parse(cJSON_Print(cJSON_GetArrayItem(DataJ , i))));
    }

    //

    cJSON_AddItemToObject(Message, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(Message, "content", Seen);

    fclose(fileptr);

    // Same as previous function

    Send_Message(Message , S);

}

//

int isinstring(char* string , char* word)

{
    int i = 1;
    char *each = (char*)malloc(1028) , *temp = (char*)malloc(1028);

    string[strlen(string) - 1] = 0;

    while (i < strlen(string) - 1)

    {
        memset(each , 0 , 1028);

        sscanf(string + i , "%s %[^\n]" , each , temp);

        if (strcmp(each , word) == 0)  { free(each) ; free(temp); return 1;}

        i += (strlen(each) + 1);
    }

    free(each);
    free(temp);

    return 0;
}

//
