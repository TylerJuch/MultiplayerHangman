#include "Users.hpp"
#include "sockhelper.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>

//constructor/destructor
osproj::Users::Users()
{
	this->userList = NULL;
	this->chooser = NULL;
	this->numOfUsers = 0;
}

osproj::Users::~Users()
{
	delete this->userList;
	delete this->chooser;
}

///////////////////////////////////////////
//Public Methods
///////////////////////////////////////////
void osproj::Users::addUser(int clientFD)
{
	this->addNewUserToUserList(clientFD);
}

bool osproj::Users::isOneUser()
{
	return this->numOfUsers == 1; 
}

void osproj::Users::setNewChooser() 
{
	std::cout << "Setting new user..." << std::endl;
	this->chooser->status = GUESSER;
	this->userList->status = CHOOSER;
	this->chooser = this->userList;
	std::cout << "User " << this->chooser->clientFD << " is now the chooser" << std::endl;
}

std::string osproj::Users::getWordFromChooser() 
{
	// chooser is global var

	/*send "Enter a word for the guessors to guess: "
	wordUnguessed = libreadline

	error checking
	must not contain $
	no spaces? would be easier. could change later.
	*/
	
	// int sock = chooser->clientFD;
	// wordGuessed = sockreadline(sock);
	return "cake";
}

char osproj::Users::getLetterFromGuesser()
{
	return 'a';
}

osproj::User* osproj::Users::getGuesser()
{
	if(this->userList->status == GUESSER) return this->userList;
	else return this->userList->next;
}

///////////////////////////////////////////
//Private Methods
///////////////////////////////////////////
void osproj::Users::addNewUserToUserList(int clientFD) 
{
	User *newUser = (User*)malloc(sizeof(User));
	newUser->status = GUESSER;
	newUser->clientFD = clientFD;
	newUser->next = this->userList;		//So it is circularlly linked

	if(this->userList == NULL) {
		newUser->next = newUser;
		newUser->status = CHOOSER;
		chooser = newUser;
		this->userList = newUser;
	}
	else {
		User *curPtr = this->userList;
		int headId = curPtr->clientFD;
		while((curPtr->next)->clientFD != headId) curPtr = curPtr->next;
		curPtr->next = newUser;
	}
	this->numOfUsers++;
	std::cout << "Client Connected" << std::endl;
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
//	this->numOfUsers--;
// }
