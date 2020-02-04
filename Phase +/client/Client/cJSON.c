// My cJSON COPYRIGHT 2019-20 by Mahdi Salmani

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Struct

typedef struct json
{
    struct json *Next , *Prev , *Child;

    char *Name;

    int Type;

    char *Value;
} cJSON;

//Macros for cJSON type
#define ArrayJSON 1
#define ObjectJSON 2
#define StringJSON 3
//

//Prototypes

cJSON* cJSON_CreateString(char *);
cJSON* cJSON_CreateArray();
cJSON* cJSON_CreateObject();

void cJSON_AddItemToArray(cJSON * , cJSON*);
void cJSON_AddItemToObject(cJSON* , char* , cJSON*);

int cJSON_GetArraySize(cJSON *);
cJSON* cJSON_GetArrayItem(cJSON * , int);
cJSON* cJSON_GetObjectItem(cJSON*, char*);

void cJSON_MyPrint(cJSON* , char** , int);
cJSON* cJSON_MyParse(char* , int , int);

char* cJSON_Print(cJSON*);
cJSON* cJSON_Parse(char*);

// include Prototypes
//#include "cJSON.h"
//

cJSON* cJSON_CreateString(char *string)

{
    cJSON *J = (cJSON*)malloc(sizeof(cJSON));

    memset(J , 0 , sizeof(cJSON));

    J->Type = StringJSON;

    J->Value = string;

    return J;
}

cJSON* cJSON_CreateArray()

{
    cJSON *J = (cJSON*)malloc(sizeof(cJSON));

    memset(J , 0 , sizeof(cJSON));

    J->Type = ArrayJSON;

    return J;
}

cJSON* cJSON_CreateObject()

{
    cJSON *J = (cJSON*)malloc(sizeof(cJSON)); // important

    memset(J , 0 , sizeof(cJSON));

    J->Type = ObjectJSON;

    return J;
}

// End of create function

void cJSON_AddItemToArray(cJSON *Arr , cJSON *Node)

{
    cJSON *J = (cJSON*)malloc(sizeof(cJSON));

    memset(J , 0 , sizeof(cJSON));

    if (Arr->Child == 0)
    {
        Arr->Child = Node;
        return;
    }

    J = Arr->Child;

    while (J->Next != 0)

        J = J->Next;

    J->Next = Node;

    Node->Prev = J;
}

void cJSON_AddItemToObject(cJSON *Obj , char *name , cJSON *Node)

{
    cJSON *J = (cJSON*)malloc(sizeof(cJSON));

    memset(J , 0 , sizeof(cJSON));

    Node->Name = name;

    if (Obj->Child == 0)
    {
        Obj->Child = Node;
        return;
    }

    J = Obj->Child;

    while (J->Next != 0)

        J = J->Next;

    J->Next = Node;

    Node->Prev = J;
}

// Get functions

int cJSON_GetArraySize(cJSON *Arr)

{
    if (Arr->Child == 0) return 0;

    int length = 0 ;

    cJSON *J = (cJSON*)malloc(sizeof(cJSON));

    memset(J , 0 , sizeof(cJSON));

    J = Arr->Child;

    while (J != 0)
    {
        length++;

        J = J->Next;
    }

    return length;
}

cJSON* cJSON_GetArrayItem(cJSON *Arr , int index)

{
    cJSON *J = (cJSON*)calloc(1 , sizeof(cJSON));
    memset(J , 0 , sizeof(cJSON));
    memcpy(J , Arr->Child , sizeof(cJSON));

    while (index > 0 && J != 0)

    {
        index--;
        J = J->Next;
    }

    cJSON *Out = J;

    return Out;
}

cJSON* cJSON_GetObjectItem(cJSON* Obj, char *name)

{
    cJSON *J = (cJSON*)calloc(1 , sizeof(cJSON));
    memset(J , 0 , sizeof(cJSON));
    memcpy(J , Obj->Child , sizeof(cJSON));

    while (strcmp(J->Name , name) != 0 && J != 0)

    {
        J = J->Next;
    }

    cJSON *Out = J;

    return Out;
}

void cJSON_MyPrint(cJSON *Js , char **out , int depth)

{
    cJSON *J = Js;

    if (J == 0) // Base
        return;

    // NewSize Allocate
    int len1 = 0  , len2 = 0;
    if (J->Value != 0) len1 = strlen(J->Value);
    if (J->Name != 0) len2 = strlen(J->Name);
    int newsize = strlen(*out) + len1 + len2 + 10;
    //

    if (J->Type == ArrayJSON)
    {
        *out = realloc(*out, newsize);

        if (J->Name != 0  && strlen(J->Name)!= 0 && depth != 0)
        {
            strcat(*out , J->Name);
            strcat(*out , ":");
        }

        strcat(*out , "[");
        cJSON_MyPrint(J->Child, out , 1);
        strcat(*out , "]");

        if (J->Next == 0 || depth == 0)
            return;

        strcat(*out , ",");
        cJSON_MyPrint(J->Next, out , 1);
    }

    else if (J->Type == ObjectJSON)
    {
        *out = realloc(*out, newsize);

        if ( J->Name != 0 && strlen(J->Name)!= 0 && depth != 0)
        {
            strcat(*out , J->Name);
            strcat(*out , ":");
        }

        strcat(*out , "{");
        cJSON_MyPrint(J->Child, out , 1);
        strcat(*out , "}");

        if (J->Next == 0 || depth == 0)
            return;

        strcat(*out , ",");
        cJSON_MyPrint(J->Next, out , 1);
    }

    else if (J->Type == StringJSON)
    {
        *out = realloc(*out, newsize);
        if (J->Name != 0 && strlen(J->Name)!=0  && depth != 0)
        {
            strcat(*out , J->Name);
            strcat(*out , ":");
        }

        strcat(*out , "\"");
        strcat(*out , J->Value);
        cJSON_MyPrint(J->Child, out , 1);
        strcat(*out , "\"");

        if (J->Next == 0 || depth == 0)
            return;

        strcat(*out , ",");
        cJSON_MyPrint(J->Next, out , 1);
    }
}
//

int ischar(char c)
{
    if (c != '{' && c != '}' && c != '[' && c != ']' && c != ':' && c != '"')
        return 1;
    return 0;
}

cJSON* cJSON_MyParse(char* string , int first , int last)

{
    if (first >= last) return 0;

    cJSON *J;
    char *name = (char*)calloc(1 , sizeof(char));

    int index = first;

    while (ischar(string[index]) == 1)
        {
            name = (char*)realloc(name , index - first + 2);
            memset(name , 0 , index - first + 2);

            memcpy(name , string + first , index - first + 1); // complicated!
            index++;
        }

    if (string[index] == ':') index++;

    if (string[index] == '[')
    {
        J = cJSON_CreateArray();

        J->Name = name;

        int equity = 1;

        index++;

        int newindex = index;

        while (equity > 0)
        {
            if (string[newindex] == '[') equity++;
            if (string[newindex] == ']') equity--;
            newindex++;
        }
        J->Child = cJSON_MyParse(string , index , newindex - 2);

        if (string[newindex] == ',')

            J->Next = cJSON_MyParse(string , newindex + 1, last);

        return J;

    }

    else if (string[index] == '{')
    {
        J = cJSON_CreateObject();

        J->Name = name;

        int equity = 1;

        index++;

        int newindex = index;

        while (equity > 0)
        {
            if (string[newindex] == '{') equity++;
            if (string[newindex] == '}') equity--;
            newindex++;
        }

        J->Child = cJSON_MyParse(string , index , newindex - 2);

        if (string[newindex] == ',')

            J->Next = cJSON_MyParse(string , newindex + 1, last);

        return J;

    }


    else if (string[index] == '"')
    {
        J = cJSON_CreateString("");

        J->Name = name;

        int newindex = index + 1;

        J->Value = (char*)calloc(1 , sizeof(char));

        while (string[newindex] != '"')
            {
                J->Value = (char*)realloc(J->Value , newindex - index + 1);
                memset(J->Value , 0 , newindex - index + 1);

                memcpy(J->Value , string + index + 1 , newindex - index);
                newindex++;
            }

        if (string[newindex + 1] == ',')

            J->Next = cJSON_MyParse(string , newindex + 2 , last);

        return J;


    }

    free(name);
}

char* cJSON_Print(cJSON *J)

{
    char *out = (char*)calloc(1 , sizeof(char));

    cJSON_MyPrint(J , &out , 0);

    return out;
}

//

cJSON* cJSON_Parse(char *string)

{
    return cJSON_MyParse(string , 0 , strlen(string) - 1);
}

