//IDNumbers: IT19022666
//Name: P.T.D.Thissera
//SOS_Assignment1_Task2

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

//********************* Header_files *********************************************************************************
#include <string.h>//Various memory handling function
#include <sys/types.h>//To defines data type used in the sytem source code
#include <sys/ipc.h>//To use messagers semaphores and shared memory
#include <sys/shm.h>//To difined symbolic constants and structures
#include <stdio.h>//For standerd input and output
#include <stdlib.h>//Include functions involving memory allocation, process control, conversions and others
#include <unistd.h>//To insamous NULL pointer definition
#include <semaphore.h>//To performing semaphore oparations
#include <sys/stat.h>//That contains constructs that facilitate getting information about files attributes
#include <fcntl.h>//That contains constructs that refer to file control

//****** Basic Functions ******
int download_s();
void all();

//***** Functions to access Shared Memory ********
char* get_Shared_Memory(int x);
int add_To_Shared_Memory(char sid[], int id);
int validate_Shared_Memory(int x);

//******** *Functions to modify the data *******
char *get_Title(char * b);
char *get_Content(char * b);
char *combine_Strings(const char *oldstring, const char c);

#define SHMSIZE 1024  //Make it a 1K shared memory segment

int iKey = 6969;
int sKey = 5500;

//char SEM_NAME[]= "vik";

int main(int argc,char *argv[])
{
  //Initializing the counter for firsttime use!

    if (validate_Shared_Memory(iKey) == 0)
    {
	add_To_Shared_Memory("0",iKey);
    }

    if(argc == 1)
          printf("No arguments passed\n");
    if(argc == 2)
    {  
        if (strcmp(argv[1], "view") == 0)
        { 
           all();
        }
        if (strcmp(argv[1], "download") == 0)
        {
           download_s();
        }

    }

    return 0;

}

int download_s()
{

  char sid[5];
  FILE *fp1;//File pointer
  char fname[50];

  printf("Enter The subject_ID: ");  //Asking the user to input Subject_ID
  scanf("%s", sid);

  //Getting the content from the shared Memory related to Subject_ID provided
	char *dataBlock = get_Shared_Memory(atoi(sid));

  // Passing it through the seperation function to separate the title and it's contents
	char *title = get_Title(dataBlock);
	char *content = get_Content(dataBlock);

	chdir("downloads/"); //cd into the downloads folder

	snprintf(fname, sizeof(fname), "%s", title); //Getting the subject name as filename
        strcat(fname,".txt"); // Adding the extension

	fp1 = fopen(fname, "w"); //Opening the file to write
        fprintf(fp1, "%s", content);

        fflush(fp1);//output stream only
        fclose(fp1);//close file

  printf("File Downloaded Successfully\n");
  return 0;
}

char *get_Title(char * b)
{
	 char *b1;
         char delim[] = "*"; //Delimiter used to seperate the name of the subject
	 char temp[50]; //Array to temporarily store the data

	strcpy(temp, b);  //  Copy the full block to array
	b1  = strtok(temp, delim); // Seperate by delimiter and passs to block1

	// printf("in func %s\n", block1);
  return b1;
}
char *get_Content(char * b)
{
	char temp2[999]; // Array to temporarily store the full block from Shared Memory
        char delim1[] = "*";  // Delimiter to seperate the name of subject
	char delim2[] = "#"; // Delimiter to seperate the content

	strcpy(temp2, b);
	char *b2 = strtok(temp2, delim1);
	// printf("in func 2 %s\n", block2);  //This printf prints out the title
	// printf("in func 2 %s\n", temp2);

	b2 = strtok(NULL,delim2);
	// printf("in func2 %s\n", block2);
	
	return b2;
}

char *combine_Strings(const char *oldstring, const char c)
{
	//declaring variables
	int result;
        char *newstring;

    //Combines the special character used as the delimiter with the string passed into it
    result = asprintf(&newstring, "%s%c", oldstring, c);

    if (result == -1){
    newstring = NULL;
    }
    return newstring;
}
int add_To_Shared_Memory(char data[], int id)
{
	//declaring variables
	int shmid;
	key_t key;
	char *shm;
	key = id;

	if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666)) < 0) //Creating the shared Memory block
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Attaching to shared Memory
	{
		printf("shmat failed\n");
		exit(1);
	}

	strncpy(shm, data, strlen(data));  //Passing the content

	return 1;

}

char* get_Shared_Memory(int x)
{
	//declaring variables
	int shmid;
	key_t key;
	char *shm;
	key = x;

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0) //Checking if the segment exists or not
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Checking whether you can attach to it or not
	{
		printf("shmat failed\n");
		exit(1);
	}

	return shm; //return the segment
}

int validate_Shared_Memory(int x)
{
	//Declaring variables
	int shmid;
	key_t key;
	key = x;

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0) //Checking if the segment exists
	{
		return 0;
	}

	return 1;
}

void all()
{
	int item_count = atoi(get_Shared_Memory(iKey));

        printf("Subject_ID  | Name of the Subject\n");

	for(int i = sKey; i < (sKey + item_count); i++)
		
	{
		if (validate_Shared_Memory(i) == 0)
		{
                     continue;
                }

	char *data = get_Shared_Memory(i);
	char *tag = get_Title(data);

	printf("%i        | %s\n", i, tag);

	}
}
//Reference:
//https://www.programiz.com/c-programming
//https://www.tutorialspoint.com/cprogramming/index.htm
//https://www.tutorialspoint.com/c_standard_library/string_h.htm
//https://www.w3schools.in/c-tutorial/c-header-files/
//Labsheet4,6

