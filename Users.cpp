#include "Users.hpp"
#include "sockhelper.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <ncurses.h>
#include <algorithm>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
char guess;
bool fromInput;

//constructor/destructor
osproj::Users::Users() {
	this->userList = NULL;
	this->chooser = NULL;
	this->numOfUsers = 0;
	this->currentGuesser = NULL;
}

osproj::Users::~Users() {
	delete this->userList;
	delete this->chooser;
	delete this->currentGuesser;
}

///////////////////////////////////////////
//Public Methods
///////////////////////////////////////////
void osproj::Users::addUser(int clientFD)
{
	// Adds a new user with given clientFD to list by calling private method
	this->addNewUserToUserList(clientFD);
}
osproj::User* osproj::Users::getChooser()
{
	// Returns the current chooser
	return (this->chooser);
}

osproj::User* osproj::Users::getCurrentGuesser() {
	return (this->currentGuesser);
}

void* osproj::Users::getGuess(void* arg) {
	// This is ugly, I know
	// Refactor if you want
	// That malloc crap was annoying
	int targetFD = *(int *)arg;
	Users u;
	u.writeToSocket(targetFD, "Enter a letter to guess: ");
	std::string word = u.readFromSocket(targetFD);

	guess = word.at(0);
	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}
osproj::User* osproj::Users::getGuesser()
{
	// Returns first eligible guessor in user list
	return (this->userList->status == GUESSER) ? this->userList : this->userList->next;
}


char osproj::Users::getLetterFromGuesser()
{
	// Returns a single letter entered by guessor
	fromInput = false;
	do {
		pthread_t inputThread;
		pthread_create(&inputThread, NULL, &(getGuess), (void *)&(this->currentGuesser->clientFD));
		pthread_t timerThread;
		pthread_create(&timerThread, NULL, &(timedWait), NULL);
		
		//This waits for one of the threads to finish
		pthread_join(timerThread, NULL);


		if (!fromInput) {
			User *temp = this->currentGuesser;
			setNextGuesser();
			removeUserFromList(temp->clientFD);

			if (isOneUser()) {
				std::cout<<"Only one user!"<<std::endl;
				//ghetto rigged to return '~' when only 1 player is left. This will exit game loop
				return '~';
			}
		}
	} while(!fromInput);

	return guess;
}

std::string osproj::Users::getWordFromChooser() 
{
	// Returns the word chosen by the chooser
	writeToSocket((this->chooser)->clientFD, "Enter a word for the guessers to guess: ");
	std::string word = readFromSocket((this->chooser)->clientFD);
	return word.substr(0, word.length()-2);
}

bool osproj::Users::isOneUser()
{
	// Returns true if there is only one user currently connectesd
	return this->numOfUsers == 1; 
}

std::string osproj::Users::readFromSocket(int clientFD)
{
	// Reads some input from user with given clientFD and returns as string.
	char buf[1024];
	read(clientFD, buf, sizeof(buf));
	return buf;
}

void osproj::Users::removeUser(int clientFD)
{
	// Removes user with given clientFD from list by calling private method
	this->removeUserFromList(clientFD);
}

void osproj::Users::sendMessageToAllClients(std::string message)
{
	// Sends given message to all users in user list
	User *curPtr = this->userList;
	do {
		writeToSocket(curPtr->clientFD, message);
		curPtr = curPtr->next;
	} while(curPtr!=NULL && curPtr->clientFD != this->userList->clientFD);
}

void osproj::Users::setCurrentGuesser(User *newGuesser) {
	this->currentGuesser = newGuesser;
}

void osproj::Users::setNewChooser(int winningUserFD) 
{
	// Sets old chooser to guessor
	// Sets winning user of last round with given clientFD to new chooser
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
}

void osproj::Users::setNextGuesser() {
	this->currentGuesser = this->currentGuesser->next;
	if (this->currentGuesser->status == CHOOSER) this->currentGuesser = this->currentGuesser->next;
	sendMessageToAllClients("A new user has been selected to guess a letter.\n");
}

void* osproj::Users::timedWait(void* arg) {
	
	struct timespec timeToWait;
  	struct timeval now;
	int timeInSec = 10;

	gettimeofday(&now,NULL);


	timeToWait.tv_sec = now.tv_sec+timeInSec;

	pthread_mutex_lock(&mutex);
	int rc = pthread_cond_timedwait(&cond, &mutex, &timeToWait);
	pthread_mutex_unlock(&mutex);

	if (rc != 0) {
		std::cout<<"User timed out. rc:"<<rc<<std::endl;
		fromInput = false;
	}
	else {
		fromInput = true;
	}


	pthread_exit(0);
}

void osproj::Users::updateAllClientsScreen(int numWrong, std::string wordGuessed, std::string wrongGuesses)
{
	// Sends given message to all users in user list
	User *curPtr = this->userList;
	do {		
		close(0);
		close(1);
		dup(curPtr->clientFD);
		dup(curPtr->clientFD);
		
		std::string body = "";
		if(numWrong > 1) {
			body += "\t|      (_)\n";
		}
		if(numWrong > 2) {
			body += "\t|      \\|/\n";
		}
		if(numWrong > 3) {
			body += "\t|       |\n";
		}
		if(numWrong > 4) {
			body += "\t|      / \\\n";
		}
		
		for(int i = numWrong; i < 5; i++) {
			body += "\t|\n";
		}
		
		std::string s = wordGuessed;
		std::replace( s.begin(), s.end(), '$', '_'); // replace all '$' to '_'
	
		std::cout << std::string(50,'\n');
		std::cout << "Multiplayer Hangman:\n";
		std::cout << "____________________\n\n";
		std::cout << 
			 "\t________\n" <<
			 "\t|/      |\n" <<
		     body <<
		     "\t|\n" <<
		 	"\t|___\n";
		
		curPtr = curPtr->next;
		std::cout << "____________________\n";
		std::cout << "Mystery Word: " << s << "\n";
		std::cout << "Used Letters: " << wrongGuesses << "\n";
		std::cout << "____________________\n";
	} while(curPtr!=NULL && curPtr->clientFD != this->userList->clientFD);
}

bool osproj::Users::userIsActive(int clientFD) {
	// Returns true if user is active
	int result = write(clientFD, "", 0);
	if (result == -1) {
			return false;
	}
	return true;
}

void osproj::Users::writeToSocket(int clientFD, std::string text)
{
	// Writes text to client with given clientFD
	write(clientFD, text.c_str(), text.length());

}


///////////////////////////////////////////
//Private Methods
///////////////////////////////////////////
void osproj::Users::addNewUserToUserList(int clientFD) 
{
	// Adds a new user to user list with given clientFD
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
	std::cout << "Client Connected: " << clientFD << std::endl;
}

void osproj::Users::removeUserFromList(int clientFD) {

	User *previousUser;
	User *curPtr = this->userList;

	previousUser = curPtr;
	curPtr = curPtr->next;

	if (curPtr->clientFD == this->userList->clientFD) {		
		this->userList = curPtr->next;	
	}
	while (curPtr->clientFD != clientFD) {
		previousUser = curPtr;
		curPtr = curPtr->next;
	}
	
	previousUser->next = curPtr->next;
	
	delete curPtr;
	this->numOfUsers--;

	return;

}
