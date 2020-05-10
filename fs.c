/*
ls : to list all the Files
clear : to clear the console
open : to open the File
close : To close the File
closeall : To close all the opened files
read : To read the contents of the File
write : To write contents into file
stat : To Display information file using name
fstat : To Display information of file using Discriptor
truncate : To remove all data from file
rm : To delete the file
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define MAXINODE 10
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2
struct superblock
{
    int TotalInode;
    int FreeInode;
};
typedef struct superblock SUPERBLOCK;
typedef struct superblock *PSUPERBLOCK;
struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    int LinkCount;
    int RefferenceCount;
    int Permission;
    char *Buffer;
    struct inode *next;
};
typedef struct inode INODE;
typedef struct inode *PINODE;
typedef struct inode **PPINODE;
struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
};
typedef struct filetable FILETABLE;
typedef struct filetable *pfiletable;
struct ufdt
{
    pfiletable ptrfiletable;
};
typedef struct ufdt UFDT;
UFDT UFDTArr[MAXINODE];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;
void InitialiseSuperBlock();
void CreateDILB();
PINODE GetInode(char *name);
int GetFDFromName(char *name);
// if ( count == 1) fuctions
void ls_File();
void CloseAllFiles();
void DisplayHelp();
// if( count == 2) functions
int stat_file(char *);
int fstat_file(int fd);
int CloseFileByName(char *);
int rm_File(char *name);
void man(char *name);
int WriteFile(int fd, char *arr, int isize);
int truncate_File(char *name);
// if ( count == 3)
int CreateFile(char *name, int Permission);
int OpenFile(char *name, int mode);
int ReadFile(int fd, char *arr, int size);
// count == 4
int LseekFile(int fd, int size, int from);
/// Functionality for Password
int Password(char *arr);
// main ()
int main(int argc, char *argv[])
{
    int count = 0, fd = 0, ret = 0;
    char command[4][80];
    char str[80];
    char arr[MAXFILESIZE];
    char *ptr = NULL;
    char pass[50] = {"0000"};
    //printf("\nImside main\n");
    ret = Password(pass);
    if (ret == 1)
    {
        InitialiseSuperBlock();
        CreateDILB();
        while (1)
        {
            fflush(stdin);
            printf("\nFile System :> ");
            strcpy_s(str, 80, "");
            fgets(str, 80, stdin);
            count = sscanf(str, "%s %s %s", command[0], command[1], command[2], command[3]);
            //printf("Count = %d \n",count);
            if (count == 1)
            {
                if (_stricmp(command[0], "ls") == 0)
                {
                    ls_File();
                    continue;
                }
                else if (_stricmp(command[0], "closeall") == 0)
                {
                    CloseAllFiles();
                    printf("\nAll Files closed Succesfully\n");
                    continue;
                }
                else if (_stricmp(command[0], "clear") == 0)
                {
                    system("cls");
                    continue;
                }
                else if (_stricmp(command[0], "help") == 0)
                {
                    DisplayHelp();
                    continue;
                }
                else if (_stricmp(command[0], "exit") == 0)
                {
                    printf("\nTerminating the File System \n ");
                    break;
                }
                else
                {
                    printf("\nERROR : Command not Found !!!\n");
                    continue;
                }
            }
            else if (count == 2)
            {
                if (_stricmp(command[0], "stat") == 0)
                {
                    ret = stat_file(command[1]);
                    if (ret == -1)
                    {
                        printf("\nERROR : Incorrect Parameter\n");
                    }
                    if (ret == -2)
                    {
                        printf("\nERROR : There is no such file\n");
                    }
                }
                else if (_stricmp(command[0], "fstat") == 0)
                {
                    ret =
                        fstat_file((atoi(command[1])));
                    if (ret == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                    }
                    if (ret == -2)
                    {
                        printf("ERROR : There is no such file !!!\n");
                    }
                    continue;
                }
                else if (_stricmp(command[0], "close") == 0)
                {
                    ret = CloseFileByName(command[1]);
                    if (ret == -1)
                    {
                        printf("ERROR : There is no such File !!!\n");
                    }
                    else if (ret == 0)
                    {
                        printf("file Succesfully closed \n");
                    }
                    continue;
                }
                else if (_stricmp(command[0], "rm") == 0)
                {
                    ret = rm_File(command[1]);
                    if (ret == -1)
                    {
                        printf("ERROR : There is no such File !!!\n");
                    }
                    continue;
                }
                else if (_stricmp(command[0], "man") == 0)
                {
                    man(command[1]);
                }
                else if (_stricmp(command[0], "write") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if (fd == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                    }
                    printf("Enter the data : \n");
                    scanf("%[^\n]s", arr);
                    ret = strlen(arr);
                    if (ret == 0)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                        continue;
                    }
                    ret = WriteFile(fd, arr, ret);
                    if (ret == -1)
                    {
                        printf("ERROR : Permission Denied !!!\n");
                    }
                    if (ret == -2)
                    {
                        printf("ERROR : There is no suffiecient memoru=y to write\n");
                    }
                    if (ret == -3)
                    {
                        printf("ERROR : It is not REGULAR file !!!\n");
                    }
                }
                else if (_stricmp(command[0], "truncate") == 0)
                {
                    ret = truncate_File(command[1]);
                    if (ret == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                    }
                }
                else
                {
                    printf("ERROR : Command not Found !!!\n");
                    continue;
                }
            }
            else if (count == 3)
            {
                if (_stricmp(command[0], "create") == 0)
                {
                    ret = CreateFile(command[1],
                                     atoi(command[2]));
                    if (ret >= 0)
                    {
                        printf("File Succesfully Created with File Discriptor i: %d\n", ret);
                    }
                    else if (ret == -1)
                    {
                        printf("ERROR : Invalid PArameter\n");
                    }
                    else if (ret == -2)
                    {
                        printf("ERROR : There is no such nodes\n");
                    }
                     else if (ret == -3)
                    {
                        printf("ERROR : File is already exist\n");
                    }
                    else if (ret == -4)
                    {
                        printf("EROOR : Memory Allocation Failed\n");
                    }
                }
                else if (_stricmp(command[0], "open") == 0)
                {
                    ret = OpenFile(command[1], atoi(command[2]));
                    if (ret >= 0)
                    {
                        printf("File is succesfully opened with file Discriptor i : %d", ret);
                    }
                    else if (ret == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!!\n");
                    }
                    else if (ret == -2)
                    {
                        printf("ERROR : File not Present\n");
                    }
                    else if (ret == -3)
                    {
                        printf("ERROR : Permission Denied !!!\n");
                    }
                    continue;
                }
                else if (_stricmp(command[0], "read") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if (fd == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                    }
                    ptr = (char *)malloc(sizeof(atoi(command[2])) + 1);
                    if (ptr == NULL)
                    {
                        printf("ERROR : Memory allocation Failed !!!\n");
                    }
                    ret = ReadFile(fd, ptr, atoi(command[2]));
                    if (ret == -1)
                    {
                        printf("ERROR : File not exist !!!\n");
                    }
                    else if (ret == -2)
                    {
                        printf("ERROR : Permission Denied !!!!\n");
                    }
                    else if (ret == -3)
                    {
                        printf("ERROR : Reached at the end of file\n");
                    }
                    else if (ret == -4)
                    {
                        printf("ERROR : It is not a Regular File !!!\n");
                    }
                    if (ret > 0)
                    {
                        _write(2, ptr, ret);
                    }
                    continue;
                }
                else
                {
                    printf("ERROR : Command not Found !!!\n");
                    continue;
                }
            }
            else if (count == 4)
            {
                printf("Count == %d\n", count);
                if (_stricmp(command[0], "lseek") == 0)
                {
                    fd = GetFDFromName(command[1]);
                    if (fd == -1)
                    {
                        printf("ERROR : Incorrect Parameter !!!\n");
                        continue;
                    }
                    ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                    if (ret == -1)
                    {
                        printf("ERROR : Unable to perform lseek !!!\n");
                    }
                }
                else
                {
                    printf("ERROR : Command not Found!!!\n");
                    continue;
                }
            }
            else
            {
                printf("ERROR : Command not Found!!!\n");
                continue;
            }
        }
    }
    else if (ret == 2)
    {
        printf("\nToo many Attempts\n");
        printf("You are not autharised to acces the System \n\n");
    }
    return 0;
}
////////////////////////////////////////////////////
//
//
// Function Name = Password
// Input Parameters = character array ( contains
/// password )
// Return Value = it returns integer value
//
// Discription = it accepts one character array and
// it gets one array from user
// and compaire both the arrays ,
// with respective to its return an value
//
////////////////////////////////////////////////////

int Password(char arr[])
{
    char brr[50];
    int i = 0;
    int cnt = 0;
    printf("Enter Password\n");
    //printf("arr = %s\n",arr);
    while (cnt <= 5)
    {
        fgets(brr, 50, stdin);
        while (arr[i] != '\0')
        {
            if (arr[i] != brr[i])
            {
                break;
            }
            i++;
        }
        if (arr[i] != '\0')
        {
            printf("Incorrect Passwpord\n");
            i = 0;
            cnt++;
            continue;
        }
        else if (arr[i] == '\0')
        {
            break;
        }
    }
    if (cnt == 6)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}
// SueprBlock and DILB ()

//////////////////////////////////////////////////////
//
// Function Name : InitialiseSuperBlock
// Input Parameters : it accepts nothing
// Return Value : It returns nothing
// Discriptions : Function assigns NULL value to
// the structure of array
//
//
////////////////////////////////////////////////////
////
void InitialiseSuperBlock()
{
    int i = 0;
    //printf("Inside Initialise SuperBlock\n");
    while (i < MAXINODE)
    {
        //printf("\ti = %d",i);
        UFDTArr[i].ptrfiletable = NULL;
        //printf("%d\t",i);
        i++;
    }
    printf("\n");
    SUPERBLOCKobj.FreeInode = MAXINODE;
    SUPERBLOCKobj.TotalInode = MAXINODE;
    printf("Initialise Succesfully\n");
}
///////////////////////////////////////////////////
//
// Function Name : CreateDILB ()
//
// Input Parameter : nothing
//
// Return Value : nothing
//
// Dicription : It creates Singly Linlear Linked
// List of structure INODE
// and assigns value to the
// significant structure
//
//
////////////////////////////////////////////////////
void CreateDILB()
{
    int i = 1;
    PINODE temp = head;
    PINODE newn = head;
    //printf("\nInside createDILB\n");
    while (i <= MAXINODE)
    {
        //printf("head=%x\ttemp=%x\t i=%d\n",head,temp,i);
        newn = (PINODE)malloc(sizeof(INODE));
        //strcpy_s(newn->FileName,"");
        newn->InodeNumber = i;
        newn->FileSize = 0;
        newn->FileActualSize = 0;
        newn->FileType = 0;
        newn->LinkCount = 0;
        newn->RefferenceCount = 0;
        newn->Permission = 0;
        newn->Buffer = NULL;
        newn->next = NULL;
        if (temp == NULL)
        {
            // printf("\nif\n");
            head = newn;
            temp = head;
        }
        else
        {
            //printf("\nelse\n");
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    printf("DILB created succesfully\n");
}
////////////////////////////////////////////////////
///////////////
//
// Function Name = GetInode()
//
// Input Parameter = it accepts character array
//
// Return Value = return type of our Function is
// struct * , and it returns significant address of the
// structure inode
//
// Discription = it compairs accepted character
// array with the
// FileName[]
// array present structure
//
////////////////////////////////////////////////////
//////////////////////////////
PINODE GetInode(char *name)
{
    PINODE temp = head;
    if (name == NULL)
    {
        return NULL;
    }
    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0)
        {
            break;
        }
        temp = temp->next;
    }
    return temp;
}
////////////////////////////////////////////////////
///////////////////
//
// Funtion Name = GetFDFromName()
//
// Input Parameter = it Accepts character array
//
// Return Value = it returns significant file
// discriptor number i.e. index of array of structure UFDT
//
// Discription = it compairs accepted array with the
// FileName[] array present in inode
// structure Linked list
//
////////////////////////////////////////////////////
/////////////////////
int GetFDFromName(char *name)
{
    int i = 0;
    while (i < 50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            if (_stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
            {
                break;
            }
        }
        i++;
    }
    if (i == 50)
    {
        return -1;
    }
    else
    {
        return i;
    }
}
// count = 1 fuctions
////////////////////////////////////////////////////
/////////
//
// Function Name = ls_File()
//
// Input Parameter = nothing
//
// Return value = nothing
//
// Discription = ls_file () function list all the
// files which we created
//
////////////////////////////////////////////////////
/////////
void ls_File()
{
    int i = 0;
    PINODE temp = head;
    if (SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("\nERROR : There is no Files \n");
        return;
    }
    printf("\nFile Name \t Inode Number\tFile Size\tLink Count\n");
    printf("-----------------------------------------------------\n");
    while (temp != NULL)
    {
        if (temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize, temp->LinkCount);
        }
        temp = temp->next;
    }
    printf("-------------------------------------------------------\n");
}
////////////////////////////////////////////////////
//
// Function Name = CloseAllFile()
//
// Input Parameter = nothing
//
// Return value = nothing
//
// Discription = CloseAllFile() function decreases
// the refference count of the
// file and other entries
// like readoffset and writeoffset set to zero in structure
// array of ufdt
////////////////////////////////////////////////////
/////////////////////////
void CloseAllFiles()
{
    int i = 0;
    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->RefferenceCount)--;
            break;
        }
        i++;
    }
}
/////////////////////////////////////////////////
//
// Function Name = DisplayHelp()
//
// Input Parameter = nothing
//
// Return Value = nothing
//
// Discription = It Display Fuction name and its
// use in file system for help
//
////////////////////////////////////////////////////
////////
void DisplayHelp()
{
    printf("\nls : to list all the Files\n");
    printf("clear : to clear the console\n");
    printf("open : to open the File\n");
    printf("close : To close the File\n");
    printf("closeall : To close all the opened files\n");
    printf("read : To read the contents of the File\n");
    printf("write : To write contents into file\n");
    printf("stat : To Display information file using name\n");
    printf("fstat : To Display information of file using Discriptor\n");
    printf("truncate : To remove all data from file\n");
    printf("rm : To delete the file\n");
}
// count = 2 functions
////////////////////////////////////////////
//
// Finction Name = stat_File()
//
// Input Parameter = one character array
//
// Return Value = return value is integer
//
// Discription = Function printf statistical
// information about File with the help of File name , and
// if their is any type
// of error is occured it return
// s specific int value
//
////////////////////////////////////////////////////
////////////////////
int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;
    if (name == NULL)
    {
        return -1;
    }
    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0)
        {
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL)
    {
        return -2;
    }
    printf("\n-------Statistical information about File-----------\n");
    printf("File Name : %s \n", temp->FileName);
    printf("Inode Number : %d \n", temp->InodeNumber);
    printf("File Size : %d\n", temp->FileSize);
    printf("Actual File Size : %d \n", temp->FileActualSize);
    printf("Link Count : %d\n", temp->LinkCount);
    printf("Reffernce Count : %d \n", temp->RefferenceCount);
    if (temp->Permission == 1)
    {
        printf("File Permission : Read only\n");
    }
    else if (temp->Permission == 2)
    {
        printf("File Permission : Write Only\n");
    }
    else if (temp->Permission == 3)
    {
        printf("File Permission : Read and Write\n");
    }
    printf("---------------------------------------------------------------\n");
    return 0;
}
////////////////////////////////////////////
//
// Finction Name = fstat_File()
//
// Input Parameter = one integer
//
// Return Value = return value is integer
//
// Discription = Function printf statistical information about File with refferenced with the
// fd (file discriptor), and if
// their is any type
// of error is occured it return
// s specific int value
//
////////////////////////////////////////////////////
////////////////////
int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;
    if (fd < 0)
    {
        return -1;
    }
    if (UFDTArr[fd].ptrfiletable == NULL)
    {
        return -2;
    }
    temp = UFDTArr[fd].ptrfiletable->ptrinode;
    printf("-------------Statistical information about  file----------------\n");
    printf("File Name : %s\n", temp->FileName);
    printf("Inode Number : %d\n", temp->InodeNumber);
    printf("File Actual Size : %d\n", temp->FileActualSize);
    printf("File Size : %d\n", temp->FileSize);
    printf("Refference Count : %d\n", temp->RefferenceCount);
    printf("Link Count : %d\n", temp->LinkCount);
    if (temp->Permission == 1)
    {
        printf("File Permission : Read Only\n");
    }
    else if (temp->Permission == 3)
    {
        printf("File Permission : Read and Write\n");
    }
    else if (temp->Permission == 2)
    {
        printf("File Permission : Write only\n");
    }
    printf("\n----------------------------------------\n");
    return 0;
}
////////////////////////////////////////////////////
//
// Function Name = CloseFileByName()
//
// Input Parameter = one character array
//
// Return Value = return value is integer
//
// Discription = it decreases refference count of
//the File and sets the other entities of
// ufdt structure array to zero
//
////////////////////////////////////////////////////
//////////////////
int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if (i == -1)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->RefferenceCount)--;
    return 0;
}
////////////////////////////////////////////
//
// Function Name = rm_File()
//
// Input Parameter = one character array
//
// Return Value = integer
//
// Discription = it free the memory which we are
//allocating for file by searching the name of File
// to the input array
//
////////////////////////////////////////////////////
////////////////
int rm_File(char *name)
{
    int fd = 0;
    fd = GetFDFromName(name);
    if (fd == -1)
    {
        return -1;
    }
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
    if (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        free(UFDTArr[fd].ptrfiletable);
    }
    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;
    return 0;
}
///////////////////////////////////
//
// Function Name = man()
//
// Input Parameter = character array
//
// return value = nothing
//
// Discription = it comapires character array with
//the string and if they are equal it
// prints the way to write
//the command
//
// ex. man ls
////////////////////////////////////////////////////
//////////////
void man(char *name)
{
    if (name == NULL)
    {
        return;
    }
    if (_stricmp(name, "create") == 0)
    {
        printf("\nDiscription : Used to Create Newregular File\n");
        printf("usage : create File_Name Permission\n");
    }
    else if (_stricmp(name, "read") == 0)
    {
        printf("\nDiscription : Used to Read data from regular File\n");
        printf("usage : read File_Name No_Of_Bytes_to_read\n");
    }
    else if (_stricmp(name, "write") == 0)
    {
        printf("\nDiscription : Used to write into regular File\n");
        printf("usage : write File_Name\n After this enter the data that you want to write\n");
    }
    else if (_stricmp(name, "ls") == 0)
    {
        printf("\nDiscription : Used to List all regular File\n");
        printf("usage :ls\n");
    }
    else if (_stricmp(name, "stat") == 0)
    {
        printf("\nDiscription : Used to Display information of File\n");
        printf("usage : stat File_Name\n");
    }
    else if (_stricmp(name, "fstat") == 0)
    {
        printf("\nDiscription : Used to Display information of File\n");
        printf("usage : fstat FIle_Disciptor\n");
    }
    else if (_stricmp(name, "truncate") == 0)
    {
        printf("\nDiscription : Used to remove data from File\n");
        printf("usage : truncate File_Name\n");
    }
    else if (_stricmp(name, "open") == 0)
    {
        printf("\nDiscription : Used to used to open existing File\n");
        printf("usage : open File_Name mode\n");
    }
    else if (_stricmp(name, "close") == 0)
    {
        printf("\nDiscription : Used to Close opened File\n");
        printf("usage : close File_Name\n");
    }
    else if (_stricmp(name, "closeall") == 0)
    {
        printf("\nDiscription : Used to close all opened of File\n");
        printf("usage : closeall\n");
    }
    else if (_stricmp(name, "lseek") == 0)
    {
        printf("\nDiscription : Used to change the File_Offset\n");
        printf("usage : lseek File_Name ChangeInOffset startPoint\n");
    }
    else if (_stricmp(name, "rm") == 0)
    {
        printf("\nDiscription : Used to Dlete the File\n");
        printf("usage : rm File_Name\n");
    }
    else
    {
        printf("\nNo Mannual Entry Available\n");
    }
}
////////////////////////////////////////////
//
// Function Name = WriteFile()
//
// Input Parameter = two integers , one character
// array
//
// Return value = integer
//
// Discription = the function accepts
// fileDiscriptor value , file name , and the
// how many byte we have to
// write to the file i.e isize
// and it accepts data
// from user and write it into file
// if conditions are
// satisfied then returns an a specific integer value
//
////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int WriteFile(int fd, char *arr, int isize)
{
    if (((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != (READ + WRITE)))
    {
        return -1;
    }
    if (((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != (WRITE)) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission) != (READ + WRITE)))
    {
        return -1;
    }
    if ((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
    {
        return -2;
    }
    if ((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
    {
        return -3;
    }
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset), arr, isize);
    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;
    return isize;
}
///////////////////////////////////////////
//
// Function Name = truncate_File()
//
// Input Parameter = one character array
//
// Return Value = integer
//
// Discription = it Deletes all Data from the file
// which we store
// and returns an specific value
//
////////////////////////////////////////////////////
//////////////
int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if (fd == -1)
    {
        return -1;
    }
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, MAXFILESIZE);
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
    return 0;
}
// count == 3 functions
///////////////////////////////////////////////
//
// Function Name = CreateFile()
//
// Input Parameter = integer , character array
//
// Return Value = integer
//
// Discription = CreateFile() function insert the
// data into nodes which are created by the
// CreateDILB() function
//
////////////////////////////////////////////////////
//////////////////
int CreateFile(char *name, int Permission)
{
    int i = 0;
    PINODE temp = head;
    if ((temp == NULL) || (Permission == 0) || (Permission > 3))
    {
        return -1;
    }
    if (SUPERBLOCKobj.FreeInode == 0)
    {
        return -2;
    }
    if (GetInode(name) != NULL)
    {
        return -3;
    }
    (SUPERBLOCKobj.FreeInode)--;
    while (temp != NULL)
    {
        if (temp->FileType == 0)
        {
            break;
        }
        temp = temp->next;
    }
    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }
    UFDTArr[i].ptrfiletable = (pfiletable)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)
    {
        return -4;
    }
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = Permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName, 10, name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->RefferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->Permission = Permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);
    memset((UFDTArr[i].ptrfiletable->ptrinode->Buffer), 0, MAXFILESIZE);
    return i;
}
////////////////////////////////////////////////////
///////////////
//
// Function Name = OpenFile()
//
// Input Parameter = character array , int
//
// Return Value = int
//
// Discription = It open the file which we are
// already created and by opening the file it
// notes the entries that are in
// the file table i.e. writeoffset
// and readoffset , mode reff
// count
//
////////////////////////////////////////////////////
/////////////////////
int OpenFile(char *name, int mode)
{
    int i = 0;
    PINODE temp = NULL;
    if ((name == NULL) || (mode <= 0))
    {
        return -1;
    }
    temp = GetInode(name);
    //printf("temp = %x\n",temp);
    if (temp == NULL)
    {
        return -2;
    }
    if ((temp->Permission) < mode)
    {
        return -3;
    }
    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
        {
            break;
        }
        i++;
    }
    UFDTArr[i].ptrfiletable =
        (pfiletable)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;
    if (mode == READ + WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if (mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if (mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    (UFDTArr[i].ptrfiletable->ptrinode->RefferenceCount)++;
    return i;
}
////////////////////////////////////////////////////
////
//
// Function Name = ReadFile()
//
// Input Parameter = int , int , char arr[]
//
// Return Value = int
//
// Discription = Function is used to read the data
//from the existing files by giving fileDiscriptor
//number
// and number of bytes which we
// have to read
//
////////////////////////////////////////////////////
////////////////////////////////////
int ReadFile(int fd, char *arr, int isize)
{
    int readsize = 0;
    if (UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    if ((UFDTArr[fd].ptrfiletable->mode != READ) &&
        (UFDTArr[fd].ptrfiletable->mode != READ + WRITE))
    {
        return -2;
    }
    if ((UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ) && (UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ + WRITE))
    {
        return -2;
    }
    if (UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
    {
        return -3;
    }
    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
    {
        return -4;
    }
    readsize = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if (readsize < isize)
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), readsize);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + isize;
    }
    else
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), isize);
        (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + isize;
    }
    return isize;
}
// count == 4 functions
////////////////////////////////////////////////////
///////////
//
// Function Name = LseekFile()
//
// Input Parameter = int ,int ,int
//
// Return Value = int
//
// Discription = it is used to change the
// Fileoffset i.e start point of the File data
//
////////////////////////////////////////////////////
////////////////////////
int LseekFile(int fd, int size, int from)
{
    if ((fd < 0) || (fd > 2))
    {
        return -1;
    }
    if (UFDTArr[fd].ptrfiletable == NULL)
    {
        return -1;
    }
    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
        }
        else if (from == START)
        {
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if (size < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }
            if ((UFDTArr[fd].ptrfiletable->readoffset) + size < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < MAXFILESIZE)
            {
                return -1;
            }
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
            {
                return -1;
            }
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
            }
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if (from == START)
        {
            if (size > MAXFILESIZE)
            {
                return -1;
            }
            if (size < 0)
            {
                return -1;
            }
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            }
            UFDTArr[fd].ptrfiletable->writeoffset = size;
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
            {
                return -1;
            }
            if ((UFDTArr[fd].ptrfiletable->writeoffset) + size < 0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    return 0;
}
