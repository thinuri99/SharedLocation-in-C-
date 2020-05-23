//IDNumber: IT19022666
//Name: P.T.D.Thissera
//SOS_Assignment1_Task2

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

//******************* Header_files ************************************************************************************

#include <string.h>//Various memory handling function
#include <sys/types.h>//To defines data types used in system source code
#include <sys/ipc.h>//To use messages,Semaphores and shared memory
#include <sys/shm.h>//To define symbolic constants and structure
#include <stdio.h>//For standard input and output
#include <stdlib.h>//Includes function involving memory allocation,process control,conversions and others
#include <unistd.h>//To infamouse NULL pointer definition
#include <semaphore.h>//To performing semaphore operations
#include <sys/stat.h>//That contains constructs that facilities getting information about files attributes
#include <fcntl.h>//That contains constructs that refer to file control
#include <pthread.h>//To create multiple threads for concurrent process flow

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//Basic Functions
int download_s();
int upload_s();
int delete_s();
void all();
int update_s();
void edit_s();

//Functions to access Shared Memory
char* get_Shared_Memory(int x);
int add_To_Shared_Memory(char sid[], int id);
int validate_Shared_Memory(int x);

//Functions to modify the data
char *get_Title(char * b);
char *get_Content(char * b);
char *combine_Strings(const char *oldstring, const char c);

#define SHMSIZE 1024  //1K shared memory segment for each record

int iKey = 6969;//Item_Key
int sKey = 5500;//Share_Key

//char SEM_NAME[]= "vik";

int main(int argc,char *argv[])//The begining of the function 'main'
{
	//Initializing the counter for firsttime use
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
	    if (strcmp(argv[1], "upload") == 0)
      	    {
      		upload_s();
            }	 
	    if (strcmp(argv[1], "download") == 0)
	    {
      		download_s();
            } 
	    if (strcmp(argv[1], "delete") == 0)
	    { 
       		delete_s();
            }
	    if (strcmp(argv[1], "update") == 0)
	    {
      		update_s();
            }
      	    if (strcmp(argv[1], "edit") == 0)
	    {
        	edit_s();
            }
    }
pthread_mutex_destroy(&lock);

    return 0;

}

void edit_s()//This function is created to edit the subjects
{

    char sid[5];
    FILE *fp1;//File pointer
    FILE *fp3;//File pointer
    char fname[50];

    printf("Subject_ID: ");  //Asking the user to input Subject_ID
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

    printf("File Downloaded Successfully and ready to be edited\n");
    printf("To update the content, use the update function\n");

    char *args[] = {"vi",fname,NULL};
    execvp("vi",args);

    printf("Edited Case is stored in your downloads folder\n");
}

int update_s()//This function is craeted to update the subjects
{
	//Declaring variable 
  	int k;
	int shmid;
	key_t key;
	char *shm;
	char fname[30];
	char ctitle[24];
	char *title;
	char *body;
	char *data;
	char str[999];

  printf("Enter Subject_ID to update Subject: ");
  scanf("%d", &k);

  key = k;

	FILE *fpr;//File pointer

	printf("Enter path to the edited file: \n");
	scanf("%s", fname);

	// chdir("downloads/");

	fpr = fopen(fname , "r");

	 if (fpr == NULL)
	 {
        	printf("Could not open file %s",fname);
        	return 1;
         }

	printf("Enter the Subject title: ");
	scanf("%s", ctitle);

	while (fgets(str, 1000, fpr) != NULL)

	printf("%s\n", str);

	title = combine_Strings(ctitle,'*');
	body = combine_Strings(str,'#');

	data = strcat(title,body);

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0)
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
	{
		printf("shmat failed\n");
		exit(1);
	}

	memcpy(shm,data,strlen(data));

}


int download_s()//This function is created to download the subjects
{

  char sid[5];
  FILE *fp1;//file pointer
  char fname[50];

  printf("Subject_ID: ");  //Asking the user to input subject_ID
  scanf("%s", sid);

  //Getting the content from the shared Memory related to subject_ID provided
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

  printf("File Downloaded Successfully!!!\n");
  return 0;
}

int upload_s()//This function is created to upload the subjects
{
   pthread_mutex_lock(&lock);

        char fname[50];
	FILE *fp2;
	char block[999];
	char tag[30];
	char *title;
	char *content;
	char *data;

  int jkey;
  int count; // Important Variables

  //Asking the user to enter the path of the file

  printf("Enter path to Subject to upload: ");
  scanf("%s", fname);

  jkey = sKey; //Passing the initializer key to temp variable
  count = atoi(get_Shared_Memory(iKey)); //Getting the current count of existing records
  jkey += count; //Adding the number of records to the initial key to get a new key value
  count++; //Increasing the count by 1

	fp2 = fopen(fname , "r");

	if (fp2 == NULL){
        printf("Could not open file %s",fname);
        return 1;
}

// Asking the user to enter a name for the subject to be saved in the shared memory
	printf("Enter the name for case study : ");
	scanf("%s", tag);

	while (fgets(block, 1000, fp2) != NULL)

	printf("%s\n", block);

//Combining the Name of the subject and it's content together
// A unique delimeter is used to seperate the two later
	title = combine_Strings(tag,'*');
  	content = combine_Strings(block,'#');
	data = strcat(title,content);

//Adding data to the shared memory
	add_To_Shared_Memory(data,jkey);

//Updating the count of records in shared memory 
	char string1[]="";
  	sprintf(string1,"%d",count);
  	add_To_Shared_Memory(string1,iKey);
//Updating Finished

	printf("Subject_ID is - %i\n",jkey);
  	printf("Please use the Subject_ID to access the files\n");

	fclose(fp2);//close file

	return 0;
  pthread_mutex_unlock(&lock);
  pthread_exit(0);
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
	// printf("in func 2 %s\n", b2);  //This printf prints out the title
	// printf("in func 2 %s\n", temp2);

	b2 = strtok(NULL,delim2);
	// printf("in func2 %s\n", b2);
	return b2;
}

char *combine_Strings(const char *oldstring, const char c)
{

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

	int shmid;
	key_t key;
	char *shm;
	key = id;

	if ((shmid = shmget(key, SHMSIZE, IPC_CREAT | 0666)) < 0) //Creating the shared Memory 
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

	return 1;  pthread_mutex_unlock(&lock);
  pthread_exit(0);

}


char* get_Shared_Memory(int x)
{
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
	int shmid;
	key_t key;
	key = x;

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0) //Checking if the segment exists
	{
		return 0;
	}

	return 1;
}

int delete_s()
{

  pthread_mutex_lock(&lock);
  char sid[4];

  printf("Enter Subject_ID to delete: ");
  scanf("%s", sid);

	int shmid;
	key_t key;
	char *shm;
	key = atoi(sid);

	if (validate_Shared_Memory(key) ==0) {
		printf("Invalid Subject_ID\n");
		return 0;
	}

	if ((shmid = shmget(key, SHMSIZE, 0666)) < 0)
	{
		printf("shmget failed\n");
		exit(1);
	}

	shmctl(shmid, IPC_RMID, NULL);

        printf("Subject Deleted Successfully\n");

	return 1;
	pthread_mutex_unlock(&lock);
    	pthread_exit(0);

}

void all() //This section is explained in the report -- ****** read_it ********
{
	int item_count = atoi(get_Shared_Memory(iKey));

  printf("Subject_ID  | Name of the Subject\n");

	for(int i = sKey; i < (sKey + item_count); i++)
	{
		if (validate_Shared_Memory(i) == 0){
      continue;
    }

	char *data = get_Shared_Memory(i);
	char *tag = get_Title(data);

	printf("%i        |%s\n", i, tag);
	}
}

//Reference:
//https://www.programiz.com/c-programming
//https://www.tutorialspoint.com/cprogramming/index.htm
//https://www.tutorialspoint.com/c_standard_library/string_h.htm
//https://www.w3schools.in/c-tutorial/c-header-files/
//Labsheet4,6
