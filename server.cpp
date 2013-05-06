#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>
#include "Users.hpp"
#include "sockhelper.hpp"
#include <algorithm>
#include <sstream>
#include "Timing.hpp"
#include <ncurses.h>


using namespace std;

bool checkIfLetterIsInWord(char letter);
bool checkIfUserIsStillConnected(int clientFD);
void setWordGuessed();
void updateClientScreens();
void *playGame(void *);
bool gameIsOver();

//static pthread_mutex_t userListMutex = PTHREAD_MUTEX_INITIALIZER;
int numIncorrectGuesses;
vector<char> lettersGuessed;
vector<char> incorrectLettersGuessed;
vector<char> illegalSymbols(4);

osproj::Users *userList = new osproj::Users();
/* This is going to make it easy to keep track of
 * the letters if there are duplicates in a word
 * So like word = "beet"
 * Then: wordUnguessed = "beet"
 * 		 wordGuessed = "$$$$"
 * Guess 'e'
 *		 wordUnguessed = "b$$t"
 *		 wordGuessed = "$ee$"
 * Can easily do this with indexOf
 */

string wordUnguessed;	
string wordGuessed;

int main() {
 	// This is gross, but the quick ways didn't work and I don't feel like looking for a better solution
 	illegalSymbols.push_back(' ');
 	illegalSymbols.push_back('!');
 	illegalSymbols.push_back('?');
 	illegalSymbols.push_back(',');
 	illegalSymbols.push_back(' ');
 	illegalSymbols.push_back('$');

	//Handle user connection crap
	int sockfd, newsockfd, portno, clilen;
    //char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    //int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
    /* Now start listening for the clients, here 
     * process will go in sleep mode and will wait 
     * for the incoming connection
     */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    pthread_t gameLoopThread;
    while (1) 
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        userList->addUser(newsockfd);
        if(userList->isOneUser()) pthread_create(&gameLoopThread, NULL, playGame, NULL);
    } /* end of while */
	
	return 0;
}

void *playGame(void *placeholder) {
	// Need to check and see if users are still connected in there at some point? Maybe often?
	int winningUserFD = -1;
	while(true) {
		if(userList->isOneUser()) userList->sendMessageToAllClients("Waiting for another player...\n");
		while(userList->isOneUser());
		userList->sendMessageToAllClients("Enough players found!\n");

	 	userList->setNewChooser(winningUserFD);
	 	if (winningUserFD == -1) userList->setCurrentGuesser(userList->getChooser()->next);
	 	wordUnguessed = userList->getWordFromChooser();
	 	setWordGuessed();
		bool gameOver = false;
		lettersGuessed.clear();
		incorrectLettersGuessed.clear();
		osproj::User *guesser = userList->getGuesser();

		bool userWasCorrect = false;
		numIncorrectGuesses = 0;

	 	while (!gameOver) {
	 		if(!userWasCorrect){
	 			userList->setNextGuesser();
	 		}
	 		else {
	 			userList->sendMessageToAllClients("Current guesser gets to guess again.\n");
	 		}
	 		userWasCorrect = false;

	 		char guess;
	 		bool alreadyGuessed = false;
	 		bool guessedIllegalSymbol = false;
	 		do {
	 			alreadyGuessed = false;
	 			guessedIllegalSymbol = false;
	 			guess = userList->getLetterFromGuesser();
	 			if(find(lettersGuessed.begin(), lettersGuessed.end(), guess) != lettersGuessed.end()) {
	 				userList->writeToSocket(guesser->clientFD, "That letter has already been guessed.\n");
	 				alreadyGuessed = true;
	 			}
	 			else if(find(illegalSymbols.begin(), illegalSymbols.end(), guess) != illegalSymbols.end()) {
	 				userList->writeToSocket(userList->getCurrentGuesser()->clientFD, "You cannot guess that symbol.\n");
	 				guessedIllegalSymbol = true; 
	 			}
	
	 		} while(alreadyGuessed || guessedIllegalSymbol);
	 		lettersGuessed.push_back(guess);

	 		if (checkIfLetterIsInWord(guess) == false){
	 			numIncorrectGuesses++;
	 			incorrectLettersGuessed.push_back(guess);
	 		}
	 		else {
	 			userWasCorrect = true;
	 		}

	 		cout << "Word: " << wordUnguessed <<endl << "Hidden Word: " << wordGuessed << endl;

			//ghetto rigged to look for '~' when only 1 player is left
	 		gameOver = gameIsOver() || (guess == '~');
			
	 		if(numIncorrectGuesses == 5) {
	 			userList->sendMessageToAllClients("That's five wrong guesses! The guessers lose! The chooser wins!\n");
	 			gameOver = true;
	 			winningUserFD = userList->getChooser()->clientFD;	//This line may not be necessary, but oh well.
				updateClientScreens();
				continue;
	 		}

	 		updateClientScreens();
	 		if (gameOver) winningUserFD = userList->getCurrentGuesser()->clientFD;
	 	}
	}
	cout << "Leaving Game Loop" << endl;
	pthread_exit(0);
}

void setWordGuessed() {
	// Builds a string of correct length filled with $s.
	wordGuessed="";

	for(unsigned int i =0; i < wordUnguessed.length(); i++) {
		if ( wordUnguessed.at(i) == ' ') wordGuessed+=" ";
		else if ( wordUnguessed.at(i) == '!') wordGuessed+="!";
		else if ( wordUnguessed.at(i) == '?') wordGuessed+="?";
		else if ( wordUnguessed.at(i) == ',') wordGuessed+=",";
		else if ( wordUnguessed.at(i) == '.') wordGuessed+=".";
		else
			wordGuessed+="$";
	}
	cout << "Word: " << wordUnguessed <<endl << "Hidden Word: " << wordGuessed << endl;
}

bool checkIfLetterIsInWord(char letter) {
	bool foundInWord = false;

	int position = wordUnguessed.find(letter);
	cout<<"Found "<<letter<<endl;
	while(position != -1) {
		foundInWord = true;

		wordGuessed.replace(position, 1, 1, wordUnguessed.at(position)); //Reveals letter if guessed correctly
		wordUnguessed.replace(position, 1, 1, '$');						//And puts a $ in to mark the letter has been guessed.

		position = wordUnguessed.find(letter);
	}

	std::stringstream ss;
	ss << "User guessed "<<letter<<".\n";
	userList->sendMessageToAllClients(ss.str());

	ss.str(std::string());
	if(!foundInWord) {	
		ss<<"The letter "<<letter<<" was not found in the word.\n";
	} else {
		ss<<"The letter "<<letter<<" was found in the word.\n";
	}
	userList->sendMessageToAllClients(ss.str());
	return foundInWord;
}

void updateClientScreens() {
	// Encurses magic here to all the users
	// You should use all of the global variables
	
	// initscr();			/* Start curses mode 		  */
	// printw("Hello World !!!");	/* Print Hello World		  */
	// refresh();			/* Print it on to the real screen */
	// getch();			/* Wait for user input */
	// endwin();			/* End curses mode		  */
	
	std::string wrongGuesses = "";
	
	for(unsigned int i =0; i < incorrectLettersGuessed.size(); i++) {
		wrongGuesses.push_back(incorrectLettersGuessed.at(i));
		wrongGuesses += " | ";
	}
	
	userList->updateAllClientsScreen(incorrectLettersGuessed.size(), wordGuessed, wrongGuesses);
	return;
}

bool gameIsOver() {
	// Returns true is game is over
	return (wordGuessed.find('$') == string::npos);
}