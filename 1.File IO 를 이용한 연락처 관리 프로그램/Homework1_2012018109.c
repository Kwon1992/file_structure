#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

//record struct, offset define
#define OFFSET 160
typedef struct record{
	char name[32];
	char addr[64];
	char telnum[32];
	char email[32];
}record;

//function prototype define
int menuDisplay();
void setOption(int*, int*, int*);
void insertItem(int);
void deleteItem(int);
void updateItem(int);
void searchItem(int);
void showAllItem(int);

void main(){
	int select;
	int filedes;

	if(0 > (filedes = open("./ContactList.dat", O_RDWR|O_CREAT,0644))){
		printf("file open error\n");
		return;
	}


	system("clear");
	while((select = menuDisplay()) != 6){
		switch(select){
			case 1 :
				insertItem(filedes);
				break;
			case 2 :
				deleteItem(filedes);
				break;
			case 3 :
				updateItem(filedes);
				break;
			case 4 :
				searchItem(filedes);
				break;
			case 5 :
				showAllItem(filedes);
				break;
			default :
				break;
		}
	}

	close(filedes);
}

// Print the menu screen
int menuDisplay(){
	int select;

	system("clear");
	printf("\n  CONTACT MANAGER\n");
	printf("=====================\n");
	printf("    1. Insert\n");
	printf("    2. Delete\n");
	printf("    3. Update\n");
	printf("    4. Search\n");
	printf("    5. Show All\n");
	printf("    6. Exit\n");
	printf("=====================\n");
	printf("Press Menu Number>>");

	select = getchar() - 48;
	system("clear");

	return select;
}

// Set basic information before performing the operation
void setOption(int* fieldSize, int* fieldPoint, int* offset){
	int select;
	int fieldSizeSet[4] = {32, 64, 32, 32};
	int fieldPointSet[4] = {0, 32, 96, 128};
	int offsetSet[4] = {128, 96, 128, 128};

	system("clear");
	printf("\n     OPTION MENU\n");
	printf("=====================\n");
	printf(" 1. By name\n");
	printf(" 2. By address\n");
	printf(" 3. By phone number\n");
	printf(" 4. By email address\n");
	printf("=====================\n");
	printf("Press Menu Number>>");

	scanf("%d", &select);

	*fieldSize = fieldSizeSet[select-1];
	*fieldPoint = fieldPointSet[select-1];
	*offset = offsetSet[select-1];

}

// Insert the item in a record unit
void insertItem(int filedes){
	record temp;
	char lookAround[160];
	int isEof = (int)lseek(filedes, 0, SEEK_END);
	lseek(filedes, 0, SEEK_SET);

	system("clear");
	printf("name : ");
	scanf("%s", temp.name);
	printf("addr : ");
	scanf("%s", temp.addr);
	printf("telnum : ");
	scanf("%s", temp.telnum);
	printf("email : ");
	scanf("%s", temp.email);

	if(lseek(filedes, 0, SEEK_SET) == lseek(filedes, 0, SEEK_END)) {
		lseek(filedes, 0, SEEK_SET);
	}
	else {
		lseek(filedes, 0, SEEK_END); 
	}

	write(filedes, temp.name, 32);
	write(filedes, temp.addr, 64);
	write(filedes, temp.telnum, 32);
	write(filedes, temp.email, 32);


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}

// Delete the item in a record unit by the appropriate field
void deleteItem(int filedes){
	int fieldSize, fieldPoint, offset;
	int isEof = (int)lseek(filedes, 0, SEEK_END);
	lseek(filedes, 0, SEEK_SET);

	char searchInfo[64],lookAround[64];
	setOption(&fieldSize, &fieldPoint, &offset);

	printf("Enter the information you want to delete\n");
	printf(">>");
	scanf("%s",searchInfo);

	lseek(filedes, fieldPoint, SEEK_CUR);
	while(lseek(filedes, 0, SEEK_CUR) < isEof) {
		read(filedes, lookAround, fieldSize);
		if(!strcmp(searchInfo, lookAround)){
			lseek(filedes, (-1*(fieldPoint+fieldSize)), SEEK_CUR);
			write(filedes, "", 160);
			break;
		}
		lseek(filedes, offset, SEEK_CUR);
	}
	lseek(filedes, 0, SEEK_SET);

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}

// Update the item in a record unit by the appropriate field
void updateItem(int filedes){
	record temp;
	char lookAround[64], searchInfo[64];
	int isEof = (int)lseek(filedes, 0, SEEK_END);

	int fieldSize, fieldPoint, offset;
	setOption(&fieldSize, &fieldPoint, &offset);

	printf("Enter the information you want to update\n");
	printf(">>");
	scanf("%s", searchInfo);


	lseek(filedes, fieldPoint, SEEK_SET);
	while(lseek(filedes, 0, SEEK_CUR) < isEof){
		read(filedes, lookAround, fieldSize);
		if(!strcmp(searchInfo, lookAround)){
			lseek(filedes, (-1*(fieldPoint+fieldSize)),SEEK_CUR);
			printf("name    : ");
			scanf("%s", temp.name);
			printf("address : ");
			scanf("%s", temp.addr);
			printf("contact : ");
			scanf("%s", temp.telnum);
			printf("mail    : ");
			scanf("%s", temp.email);

			write(filedes, temp.name, 32);
			write(filedes, temp.addr, 64);
			write(filedes, temp.telnum, 32);
			write(filedes, temp.email, 32);
		}
	}
	lseek(filedes, 0, SEEK_SET);

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

}

// Search the item in a record unit by the appropriate field
void searchItem(int filedes){
	record temp;
	char lookAround[64], searchInfo[64];
	int fieldSize, fieldPoint, offset;
	int isEof = (int)lseek(filedes, 0, SEEK_END);
	lseek(filedes, 0, SEEK_SET);

	setOption(&fieldSize, &fieldPoint, &offset);

	printf("Enter the information you want to search\n");
	printf(">>");
	scanf("%s", searchInfo);

	lseek(filedes, fieldPoint, SEEK_CUR);
	while(lseek(filedes, 0, SEEK_CUR) < isEof) {
		read(filedes, lookAround, fieldSize);
		if(!strcmp(searchInfo, lookAround)){
			lseek(filedes, (-1*(fieldPoint+fieldSize)), SEEK_CUR);
			read(filedes, temp.name, 32);
			read(filedes, temp.addr, 64);
			read(filedes, temp.telnum, 32);
			read(filedes, temp.email, 32);
			break;
		}
		lseek(filedes, offset, SEEK_CUR);
	}

	if(!strcmp(searchInfo, lookAround)) {
		printf("\n     The result you want\n");
		printf("=============================\n");
		printf("name    : %s\n",temp.name);
		printf("address : %s\n",temp.addr);
		printf("contact : %s\n",temp.telnum);
		printf("mail    : %s\n",temp.email);
		printf("=============================\n");
	} else {
		printf("\nYour Information doesn't exist\n");
	}

	lseek(filedes, 0, SEEK_SET);


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}

// Print all Items existing in file
void showAllItem(int filedes){
	record temp;
	int isEof = (int)lseek(filedes, 0, SEEK_END);
	lseek(filedes, 0, SEEK_SET);

	printf("         SHOW ALL         \n");
	printf("==============================\n");
	while(lseek(filedes, 0, SEEK_CUR) < isEof){
		read(filedes, temp.name, 32);
		read(filedes, temp.addr, 64);
		read(filedes, temp.telnum, 32);
		read(filedes, temp.email, 32);

		if(strcmp(temp.name,"") == 0) continue;

		printf("name    : %s\n",temp.name);
		printf("address : %s\n",temp.addr);
		printf("contact : %s\n",temp.telnum);
		printf("mail    : %s\n",temp.email);
		printf("==============================\n");
	}

	lseek(filedes, 0, SEEK_SET);

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}
