// Customised Virtual Files System Project (CVFS)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <io.h>
using namespace std;

#define MAXINODE 50      // We can create the 50 maximum files due to 50 Inodes
#define MAXFILESIZE 1024 // size of 1 Block

#define READ 1
#define WRITE 2

#define REGULAR 1 // They store user data, text, binary information, or program code.
#define SPECIAL 2 // Special files are created by the operating system or device drivers to provide a standardized interface for interacting with hardware.

// For performing the Lseek operation
#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
} SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];  // name of the file 
    int InodeNumber;    // when new file is created newInode number is given to it
    int FileSize;       // number block in which data is stored * data block size
    int FileActualSize; // number of bytes of data in file
    int FileType;       // 1 for regular file
    char *Buffer;       // It is charcter pointer which points to Actual Data of the file where it is stored
    int ReferenceCount; // It shows the How many other entities are referring to the file
    int permission;     // 1 read, 2 write , 3 read and write(both) when we create the file it is set
    struct inode *next; // pointer of inode pointing to another Inode
} INODE, *PINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int mode;        // 1 read, 2 write , 3 read and write(both) (mode in which file is opened)
    PINODE ptrinode; // ptrinode is pointer that points to the Inode
} FILETABLE, *PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable; // pointer points to the FileTable
} UFDT;

// Global variables
UFDT UFDTArr[MAXINODE]; // (UFDTArr is an array of UFDT structure objects) ufdt structure which contains pointers of fileTable
                        // 0 stdin 1 stdout 2 stderr  Indexes are reserved so the  UFDTArr indexd from 3 used as FD (FD is handle to access the file)

SUPERBLOCK SUPERBLOCKobj; // Creating the objects of the SuperBlock
PINODE head = NULL;       //  head is pointer of type struct INODE currently initilize with NULL (head of linked list)

void man(char *name)
{
    if (name == NULL)
        return;

    // strcmp is function used to compare two string if they are equls return 0
    if (strcmp(name, "create") == 0)
    {
        printf("Description : Used to create new regular file\n");
        printf("Usage: create File_name Permission\n");
    }
    else if (strcmp(name, "read") == 0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage: read File_name NO_Of_Bytes_To_Read\n");
        printf("Usage: read File_name\n");
        printf("Note : If you not specifies the no of bytes to read it will read all the data\n");
    }
    else if (strcmp(name, "write") == 0)
    {
        printf("Description : Used to write data into regular file\n");
        printf("Usage: write File_name\n After this enter the data that we want to write\n");
    }
    else if (strcmp(name, "ls") == 0)
    {
        printf("Description : Used to list all information of file\n");
        printf("Usage: ls\n");
    }
    else if (strcmp(name, "stat") == 0)
    {
        printf("Description : Used Display information of file\n");
        printf("Usage: stat File_name\n");
    }
    else if (strcmp(name, "fstat") == 0)
    {
        printf("Description :  Used Display information of file using file descriptor\n");
        printf("Usage: fstat File_Descriptor\n"); //////////////////////////////////////
    }
    else if (strcmp(name, "truncate") == 0)
    {
        printf("Description : Used to remove data from file\n");
        printf("Usage: truncate File_name\n");
    }
    else if (strcmp(name, "open") == 0)
    {
        printf("Description : Used to open existing file\n");
        printf("Usage: open File_name mode\n");
    }
    else if (strcmp(name, "close") == 0)
    {
        printf("Description : Used to close opened file\n");
        printf("Usage: close File_name\n");
    }
    else if (strcmp(name, "closeall") == 0)
    {
        printf("Description : Used to close all opened file\n");
        printf("Usage: closeall\n");
    }
    else if (strcmp(name, "lseek") == 0)
    {
        printf("Description : Used to change file offset\n");
        printf("Usage: lseek File_name ChangeInOffset StartPoint\n");
    }
    else if (strcmp(name, "rm") == 0)
    {
        printf("Description : Used to delete the file\n");
        printf("Usage: rm File_name\n");
    }
    else
    {
        printf("ERROR: No manual entry available.\n");
    }
}

void DisplayHelp()
{
    printf("clear       :   To clear console\n");
    printf("create      :   To create the file\n");
    printf("open        :   To open the file\n");
    printf("read        :   To read the contents from file\n");
    printf("write       :   To write contents into file\n");
    printf("ls          :   To List out all files\n");
    printf("lseek       :   To change file offset\n");
    printf("stat        :   To Display information of file using name\n");
    printf("fstat       :   To Display information of file using file descriptor\n");
    printf("truncate    :   To remove all data from file\n");
    printf("close       :   To close the file\n");
    printf("rm          :   To Delete the file\n");
    printf("closeall    :   To close all opened files\n");
    printf("exit        :   To Terminate file system\n");
}

int GetFDFromName(char *name)
{
    int i = 0; // here we are startting the FD(file descriptor) from 0

    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable != NULL && strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
            break;

        i++;
    }

    if (i == MAXINODE)
        return -1; // fileName is not exist in the INODES

    else
        return i;
}

PINODE Get_Inode(char *name) // get the Inode address according to the file name
{
    PINODE temp = head;
    int i = 0;

    if (name == NULL)
        return NULL;

    while (temp != NULL)
    {
        if (strcmp(name, temp->FileName) == 0) // if name and INODE name are mathced return address of INODE
            break;

        temp = temp->next;
    }
    return temp;
}

/*
    for creating the LinkList of the Inode
    the DILB block contains the all INODE equal to MAXINODES

    created singly Linear linkedList using Insert Last Logic
*/
void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    // MAXINODE are 50 so we are created the DILB contins 50 Empty INODE
    while (i <= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE)); // alloacting memory for INODE dynamically

        // Initilizing members of INODE structur

        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;
        newn->Buffer = NULL;
        newn->next = NULL;
        newn->InodeNumber = i; // Assigning the INODE number to each newly created INODE

        if (temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;
    }
    // printf("DILB Created Successfully\n");
}

/*
    1)Initilizing super block with the toatal number of Inodes and free Inodes
    2) Initialzing elements of ufdt array with NUll
*/
void InitialiseSuperBlock()
{
    int i = 0;
    while (i < MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL; // pointer from the UFDTarray to the filetable set to NULL
        i++;
    }

    // Initializing the members of the SUPERBLOCK structure
    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;
}

int CreateFile(char *name, int permission)
{
    int i = 0;
    PINODE temp = head;

    if ((name == NULL) || (permission <= 0) || (permission > 3))
        return -1;

    if (SUPERBLOCKobj.FreeInode == 0)
        return -2;

    (SUPERBLOCKobj.FreeInode)--; // when one file is created then one inode is used for it so we reduce free inodes by 1

    if (Get_Inode(name) != NULL) // File is already exist of the give given name
    {
        (SUPERBLOCKobj.FreeInode)++; // As we decrement the FreeInode count above and here we know same file is exist we increment FreeInode count to it's original value
        return -3;                   // File is alredy exist
    }

    // Finding the Empty Inode whose FileType is 0 so it can be used for file
    while (temp != NULL)
    {
        if (temp->FileType == 0)
            break;

        temp = temp->next;
    }

    // Finding the Empty position in UFDT Array
    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable == NULL) // array elements madhe NULL (element empty) aahe mhnjech to fieltable la point kart nahi
            break;

        i++;
    }

    // allocating the Filetable memroy dynamically and UFDTArr elements(pointer) points to the fieltable
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    // Initilizing the fileTable members
    UFDTArr[i].ptrfiletable->mode = permission; // if permission is 1 read mode , 2 write node ,3 read+write mode
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;

    UFDTArr[i].ptrfiletable->ptrinode = temp; // pointer from the filetable  pointing to the Inode

    // Initilizing the member of INODE

    // destination                              //source
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName, name); // name copy kel InodeName madhe
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->permission = permission;              // 3 giving file permission for read and write both
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE); // alloacting the memory of Block dynamically to store the data

    return i;
}

// rm_File("Demo.txt")
int rm_File(char *name)
{
    int fd = 0;

    fd = GetFDFromName(name);

    if (fd == -1)
        return -1; // FileName is not present in Inodes

    UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0; // making the Inode Value 0 means it can be used for another Files
    strcpy(UFDTArr[fd].ptrfiletable->ptrinode->FileName, " ");
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->permission = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount = 0;
    
    free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer); // removing the dynamically allocated memory of Block
    free(UFDTArr[fd].ptrfiletable);                   // removing the Filetable that is pointing to the INODE which having name

    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++; // when file is remove then increse the FreeInodes counts

    printf("File is successfully deleted\n");
}

int ReadFile(int fd, char *arr) // if no of bytes to read is not provided then read all the data
{
    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1; // file is not there for given FD

    if (UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ + WRITE)
        return -2;

    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)
        return -2;
    

    // As we are rading the complete the data of the file so make the readOffset to 0
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    strcpy(arr,UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
    int noOfBytesRead = UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize;

    return noOfBytesRead;
}
int ReadFile(int fd, char *arr, int isize)
{
    int reamaining_read_size = 0;

    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1; // file is not there for given FD

    if (UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ + WRITE)
        return -2;

    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)
        return -2;

    if (UFDTArr[fd].ptrfiletable->readoffset >= UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
        return -3; // Reaching to the end of the file

    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
        return -4;

    reamaining_read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset); // we have to start read of file from the readOffset

    if (reamaining_read_size < isize)
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), reamaining_read_size); // INode(destination) madhla read_size byte data copy kela arr madhe

        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + reamaining_read_size; // readOffset jevdh byte cha data read kela tevdha increse kela

        return reamaining_read_size;

    }
    else
    {
        strncpy(arr, (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset), isize); // all data read
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset + isize;
        return isize;
    }

}

/*
    char *strncpy(char *dest, const char *src, size_t n)

    Parameters
    dest − This is the pointer to the destination array where the content is to be copied.

    src − This is the string to be copied.

    n − The number of characters to be copied from source.

    Return Value
    This function returns the pointer to the copied string.


    in writeFile funciton arr considerd as source so we have to copy data from the source to INODE->Buffer
*/
int WriteFile(int fd, char *arr, int isize) // 1 read , 2 write, 3  read+write
{
    if ((UFDTArr[fd].ptrfiletable->mode) != WRITE && (UFDTArr[fd].ptrfiletable->mode) != READ + WRITE)
        return -1;

    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != WRITE && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ + WRITE)
        return -1;

    if ((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
        return -2;

    if ((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
        return -3;

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset), arr, isize); // we are copying the data in the inode buffer after the writeoffset  so we adding the value of writeOFfset

    (UFDTArr[fd].ptrfiletable->writeoffset) = ((UFDTArr[fd].ptrfiletable->writeoffset) + isize); // updating the value of writeOffset

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize); // Initilize file actual size of INODE

    return isize;
}

int OpenFile(char *name, int mode)
{
    int i = 0; // here we are set i = 0 so we are not reserve first 3 block
    PINODE temp = NULL;

    if (name == NULL || mode <= 0)
        return -1;

    temp = Get_Inode(name);

    if (temp == NULL) // name cha Inode exist kart nahi
        return -2;

    if (temp->permission < mode) // Invalid mode
        return -3;

    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;

        i++;
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    if (UFDTArr[i].ptrfiletable == NULL)
        return -1;

    UFDTArr[i].ptrfiletable->mode = mode;

    if (mode == READ + WRITE) // 3
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    else if (mode == READ) // 1
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if (mode == WRITE) // 2
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }

    UFDTArr[i].ptrfiletable->ptrinode = temp;              // File table is pointing to the iNode
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++; // file is open increment the ReferenceCount

    return i; // returning the FD
}

void CloseByFD(int i)
{
    // when we again repopen file after close read and write offset becomes 0
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);
    if (i == -1)
        return -1;

    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

void CloseALLFile()
{
    int i = 0;
    while (i < MAXINODE)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            break;
        }

        i++;
    }
}

// kashyat  //Kitine Displacement   //Kuthun
int LseekFile(int fd, int size, int from)
{
    if ((fd < 0) || (from > 2))
        return -1;
    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1;

    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
    {
        if (from == CURRENT) // 1
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) // if displacement is larger than file size we can't lseek it
                return -1;

            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0) // if displcament is less than zero we can't lseek it
                return -1;

            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size; // updating read offset
        }
        else if (from == START) // 0
        {
            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                return -1;
            if (size < 0)
                return -1;

            (UFDTArr[fd].ptrfiletable->readoffset) = size;
        }
        else if (from == END) // 2
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;

            if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
                return -1;

            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
        }
    }
    else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
                return -1;

            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
                return -1;

            if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;

            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size; // updating the writeOffset
        }
        else if (from == START)
        {
            if (size > MAXFILESIZE)
                return -1;

            if (size < 0)
                return -1;

            if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
                (UFDTArr[fd].ptrfiletable->writeoffset) = size; // updating the writeOffset
            }
        }
        else if (from == END)
        {
            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;

            if ((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size < 0)
                return -1;

            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size; // updating the writeOffset
        }
    }
    printf("Successfully Changed\n");
}

void ls_file() // Listing All the Files created
{
    int i = 0;
    PINODE temp = head; // temp is pointing to the Inode head

    if (SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("ERROR : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode number\tFile Size\n");
    printf("----------------------------------------------------\n");
    while (temp != NULL)
    {
        if (temp->FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize);
        }
        temp = temp->next;
    }
    printf("----------------------------------------------------\n");
}

// Display file information using FD
int fstat_file(int fd)
{
    PINODE temp = head;
    int i = 0;

    if (fd < 0)
        return -1;

    if (UFDTArr[fd].ptrfiletable == NULL)
        return -2;

    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n--------Statistical Informantion about file----------\n");
    printf("File Name : %s\n", temp->FileName);
    printf("Inode Number : %d\n", temp->InodeNumber);
    printf("File Size : %d\n", temp->FileSize);
    printf("Actual File Size : %d\n", temp->FileActualSize);
    printf("Reference Count : %d\n", temp->ReferenceCount);

    if (temp->permission == 1)
        printf("File Permission : Read Only\n");
    else if (temp->permission == 2)
        printf("File Permission : Write\n");
    else if (temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("-----------------------------------------------------------\n\n");

    return 0;
}

// Display file information using file name
int stat_file(char *name)
{
    PINODE temp = head;
    int i = 0;

    if (name == NULL)
        return -1;

    while (temp != NULL) // travel all Inodes till we name same as INODE Name
    {
        if (strcmp(name, temp->FileName) == 0)
            break;

        temp = temp->next;
    }

    if (temp == NULL)
        return -2; // name file is not exist

    printf("\n--------Statistical Informantion about file----------\n");
    printf("File Name : %s\n", temp->FileName);
    printf("Inode Number : %d\n", temp->InodeNumber);
    printf("File Size : %d\n", temp->FileSize);
    printf("Actual File Size : %d\n", temp->FileActualSize);
    printf("Reference Count : %d\n", temp->ReferenceCount);
    if (temp->permission == 1)
        printf("File Permission : Read Only\n");
    else if (temp->permission == 2)
        printf("File Permission : Write\n");
    else if (temp->permission == 3)
        printf("File Permission : Read & Write\n");
    printf("-----------------------------------------------------------\n\n");

    return 0;
}

// void *memset(void *str, int c, size_t n)
//    copies the character c (an unsigned char)
//   to the first n characters of the string pointed to, by the argument str

// For deleting all data from the given name of file
int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if (fd == -1)
        return -1;

    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer, 0, MAXFILESIZE);

    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;

    printf("Data is successfully Removed\n");
}

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;

    char command[4][80]; // command is 2D Array. which containing 4  One Dimensional array and each  1D Array containg 80 elements and each of type Char
    char str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);    // It attempts to flush (clear) the input buffer associated with the standard input stream (stdin)
        strcpy(str, " "); // copying the empty in str ;

        printf("\n$ ");

        fgets(str, 80, stdin); // scanf("%[^'\n']s",str);
                               // man open

        // sscanf() fucntion is takes input from string str and split according the format specifiers
        count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]); // (Tokenization)
        //  2                                // man         open

        if (count == 1)
        {
            if (strcmp(command[0], "ls") == 0)
            {
                ls_file();
            }
            else if (strcmp(command[0], "closeall") == 0) // making all files read offset & wirteOffset equal to 0 and decrementing the reference count
            {
                CloseALLFile();
                printf("All files closed successfully\n");
                continue;
            }
            else if (strcmp(command[0], "clear") == 0)
            {
                system("cls"); // This system call used to clear the terminal
                continue;
            }
            else if (strcmp(command[0], "help") == 0)
            {
                DisplayHelp();
                continue;
            }
            else if (strcmp(command[0], "exit") == 0)
            {
                printf("Terminating the Virtual File System\n");
                break;
            }
            else
            {
                printf("\nERROR : Command not found 1 !!!\n");
                continue;
            }
        }
        else if (count == 2)
        {
            if (strcmp(command[0], "stat") == 0) // command : stat demo.txt
            {
                ret = stat_file(command[1]); // fileName send to the stat_file function

                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");

                else if (ret == -2)
                    printf("ERROR : There is no such file\n ");

                continue;
            }
            else if (strcmp(command[0], "fstat") == 0) // command :  fstat 2 (2 file descriptor)
            {
                ret = fstat_file(atoi(command[1])); // converting 2 form charter to integer

                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");

                else if (ret == -2)
                    printf("ERROR : There is no such file\n ");

                continue;
            }
            else if (strcmp(command[0], "close") == 0) // command  : close demo.txt
            {
                ret = CloseFileByName(command[1]); // making all files read offset & wirteOffset equal to 0 and decrementing the reference count
                if (ret == -1)
                {
                    int fd = atoi(command[1]);

                    if (fd < MAXINODE)
                        CloseByFD(fd); // we closing file by FD

                    else
                        printf("ERROR: There is no such file\n");
                }
                continue;
            }
            else if (strcmp(command[0], "rm") == 0) // command  : rm demo.txt
            {
                ret = rm_File(command[1]); // rm_file will delete the file
                if (ret == -1)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0], "man") == 0) // command  : man  create
            {
                man(command[1]); // getting the manual of the command
            }
            else if (strcmp(command[0], "write") == 0) // command  : write demo.txt
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }

                if (UFDTArr[fd].ptrfiletable->ptrinode->permission == READ) // if file has only read permission we can't write data in it
                {
                    printf("ERROR : Permission denied\n");
                    continue;
                }

                printf("Enter the data : \n");
                scanf("%[^'\n']s", arr); // special scanf break when newLine Charcter in encountered

                ret = strlen(arr);

                if (ret == 0)
                    continue; // size of input is zero

                ret = WriteFile(fd, arr, ret); // in FD write ret bytes of data from the arr
                if (ret == -1)
                    printf("ERROR : Permission denied\n");

                else if (ret == -2)
                    printf("ERROR : There is no sufficient memory to write\n");

                else if (ret == -3)
                    printf("ERROR : It is not regular file\n");

                else if (ret > 0)
                    printf("%d bytes of data written successfully.", ret);
            }

            else if (strcmp(command[0], "truncate") == 0) // command  : truncate demo.txt
            {
                ret = truncate_File(command[1]); // truncate_file is clearing the all data from the file but it not delete file
                if (ret == -1)
                {
                    printf("ERROR : File is not exist \n");
                }
            }

            else if (strcmp(command[0], "read") == 0) // command : read demo.txt
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }

                int readSize = UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize;
                ptr = (char *)malloc(readSize+1); // memory allocation for read data from file
                ret = ReadFile(fd, ptr); // ptr is character array .  ptr madhe read kel

                if (ret == -1)
                {
                    printf("ERROR : File not existing\n");
                }
                if (ret == -2)
                {
                    printf("ERROR : Permission denied\n");
                }
                if (ret == -3)
                {
                    printf("ERROR : Reached at end of file\n");
                }

                if (ret == 0)
                {
                    printf("ERROR : File empty\n");
                }
                if (ret > 0)
                {
                    write(2, ptr, ret); // display all read contains on stdout 2 (monitor)
                }
            }
            else
            {
                printf("\nERROR : Command not found 2!!!\n");
                continue;
            }
        }

        else if (count == 3)
        {
            if (strcmp(command[0], "create") == 0) // command : create demo.txt 3    (3 is permission for both read and write)
            {
                ret = CreateFile(command[1], atoi(command[2]));

                if (ret >= 0)
                {
                    printf("File is successfully created with file descriptor : %d\n", ret);
                }

                if (ret == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }

                if (ret == -2)
                {
                    printf("ERROR : There is no inodes\n");
                }
                if (ret == -3)
                {
                    printf("ERROR : File already exists\n");
                }
                if (ret == -4)
                {
                    printf("ERROR : Memory allocation failure\n");
                }
                continue;
            }
            else if (strcmp(command[0], "open") == 0) // command : open demo.txt 3
            {
                ret = OpenFile(command[1], atoi(command[2]));

                if (ret >= 0)
                {
                    printf("File is successfully opened with file descriptor : %d\n", ret);
                }
                if (ret == -1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }
                if (ret == -2)
                {
                    printf("ERROR : File not present\n");
                }
                if (ret == -3)
                {
                    printf("ERROR : Permission denied\n");
                }
                continue;
            }
            else if (strcmp(command[0], "read") == 0) // command : read demo.txt 10
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }

                ptr = (char *)malloc((atoi(command[2])) + 1); // memory allocation for read data from file

                ret = ReadFile(fd, ptr, atoi(command[2])); // ptr is character array .  ptr madhe read kel

                if (ret == -1)
                {
                    printf("ERROR : File not existing\n");
                }
                if (ret == -2)
                {
                    printf("ERROR : Permission denied\n");
                }
                if (ret == -3)
                {
                    printf("ERROR : Reached at end of file\n");
                }
                if (ret == -4)
                {
                    printf("ERROR : It is not regular file\n");
                }
                if (ret == 0)
                {
                    printf("ERROR : File empty\n");
                }
                if (ret > 0)
                {
                    write(2, ptr, ret); // display all read contains on stdout 2 (monitor)
                }
                continue;
            }
            else
            {
                printf("ERROR : Command not found 3!!!\n");
                continue;
            }
        }

        /*
            lseek a.txt -4 2  (end pasun -4 ne displacement)
        */
        else if (count == 4) // command : lseek a.txt 2 0  (2 : is the no of bytes we want to displacement) (0(start) ,1(curr) , 2(end): are locations of the displacemet)
        {
            if (strcmp(command[0], "lseek") == 0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameters\n");
                    continue;
                }
                ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));
                if (ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else
            {
                printf("\nERROR : Command not found 4!!!!\n");
                continue;
            }
        }
        else
        {
            printf("\nERROR : Command not found 0!!!!\n");
            continue;
        }
    }
    return 0;
}
