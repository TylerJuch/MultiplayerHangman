#include "Users.hpp"
#include "sockhelper.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>

osproj::Users::Users()
{
	this->userList = NULL;
}

osproj::Users::~Users()
{
	delete this->userList;
}

void osproj::Users::addUser(int clientFD)
{
	this->addNewUserToUserList(clientFD);
}

bool osproj::Users::isOneUser()
{
	return this->userList->next == this->userList;
}


// void osproj::setNewChooser(User *newChooser) 
// {
// 	chooser->status = GUESSER;
// 	newChooser->status = CHOOSER;
// 	chooser = newChooser;
// 	cout << "User " << chooser->clientFD << " is now the chooser" << endl;
// }

void osproj::Users::addNewUserToUserList(int clientFD) {
	User *newUser = (User*)malloc(sizeof(User));
	newUser->status = GUESSER;
	newUser->clientFD = clientFD;
	newUser->next = this->userList;		//So it is circularlly linked

	if(this->userList == NULL) {
		newUser->next = newUser;
		this->userList = newUser;
	}
	else {
		User *curPtr = this->userList;
		int headId = curPtr->clientFD;
		while((curPtr->next)->clientFD != headId) curPtr = curPtr->next;
		curPtr->next = newUser;
	}
}

// void removeUserFromList(int clientFD) 
// {
// 	User *curPtr = userList;
// 	if (clientFD == curPtr->clientFD) { //Case removing head of list
// 		User *tempUserList = userList;
// 		userList = userList->next;
// 		while((curPtr->next)->clientFD != tempUserList->clientFD) curPtr = curPtr->next;
// 		curPtr->next = userList;
// 		free(tempUserList);
// 	}
// 	else {
// 		while((curPtr->next)->clientFD != clientFD) curPtr = curPtr->next;
// 		User *temp = curPtr->next;
// 		curPtr->next = (curPtr->next)->next;
// 		free(temp);
// 	}
// }
