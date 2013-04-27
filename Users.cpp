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

void osproj::Users::setNewChooser(int winningUserFD) 
{
	std::cout << "Setting new user...\n";
	this->chooser->status = GUESSER;

	if(winningUserFD == -1) {
		this->userList->status = CHOOSER;
		this->chooser = this->userList;
	} else {
		User *curPtr = this->userList;
		while(curPtr->clientFD != winningUserFD) curPtr = curPtr->next;
		curPtr->status = CHOOSER;
		this->chooser = curPtr;
	}
	std::cout << "User " << this->chooser->clientFD << " is now the chooser" << std::endl;
}

int osproj::Users::getChooserFD()
{
	return (this->chooser)->clientFD;
}

std::string osproj::Users::getWordFromChooser() 
{
	// chooser is global var
	writeToSocket((this->chooser)->clientFD, "Enter a word for the guessors to guess: ");
	std::string word = readFromSocket((this->chooser)->clientFD);
	return word.substr(0, word.length()-2);

	/*error checking
	must not contain $
	no spaces? would be easier. could change later.
	*/
}

char osproj::Users::getLetterFromGuesser(int clientFD)
{
	writeToSocket(clientFD, "Enter a letter to guess: ");
	std::string word = readFromSocket(clientFD);
	return word.at(0);
}

osproj::User* osproj::Users::getGuesser()
{
	if(this->userList->status == GUESSER) return this->userList;
	else return this->userList->next;
}

void osproj::Users::writeToSocket(int clientFD, std::string text)
{
	//Based off of this: http://www.cplusplus.com/forum/unices/31692/
	write(clientFD, text.c_str(), text.length());
}

std::string osproj::Users::readFromSocket(int clientFD)
{
	char buf[1024];
	read(clientFD, buf, sizeof(buf));
	return buf;
}

void osproj::Users::sendMessageToAllClients(std::string message)
{
	User *curPtr = this->userList;
	do {
		writeToSocket(curPtr->clientFD, message);
		curPtr = curPtr->next;
	} while(curPtr!=NULL && curPtr->clientFD != this->userList->clientFD);
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
