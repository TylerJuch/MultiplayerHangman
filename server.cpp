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

using namespace std;

bool checkIfLetterIsInWord(char letter);
bool checkIfUserIsStillConnected(int clientFD);
void setWordGuessed();
void updateClientScreens();
void *playGame(void *);
bool checkIfGameOver();

static pthread_mutex_t userListMutex = PTHREAD_MUTEX_INITIALIZER;
int numIncorrectGuesses;
vector<char> lettersGuessed;
vector<char> incorrectLettersGuessed;
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
	
	//playGame();
	return 0;
}

void *playGame(void *placeholder) {
	// Need to check and see if users are still connected in there at some point? Maybe often?

	// while(true) {
	while(userList->isOneUser()); 

 	userList->setNewChooser();
 	wordUnguessed = userList->getWordFromChooser();
 	setWordGuessed();
	bool gameOver = false;
	lettersGuessed.clear();
	incorrectLettersGuessed.clear();
	osproj::User *guesser = userList->getGuesser();

	numIncorrectGuesses = 0;
		
	// 	while (!gameOver) {
	// 		guesser = guesser->next;
	// 		if (guesser->status == CHOOSER) guesser = guesser->next;		//Prevent guesser from ever being chooser;

	// 		char guess = askUserForLetter(guesser);
	// 		if (checkIfLetterIsInWord(guess) == false){
	// 			numIncorrectGuesses++;

	// 			incorrectLettersGuessed.push_back(guess);
	// 		} 
	// 		// push letter to letters guessed
	// 		lettersGuessed.push_back(guess);
	// 		gameOver = checkIfGameOver();
	// 		updateClientScreens();
	// 		if (gameOver) winningUser = guesser;
	// 	}
	// }
	cout << "Leaving Game Loop" << endl;
	pthread_exit(0);
}

void setWordGuessed() {
	// Builds a string of correct length filled with $s.

	for(unsigned int i =0; i < wordUnguessed.length(); i++) {
		wordGuessed+="$";
	}
	cout << "Word: " << wordUnguessed << " Hidden Word: " << wordGuessed << endl;
}

bool checkIfLetterIsInWord(char letter) {
	bool foundInWord = false;

	int position = wordUnguessed.find(letter);
	while(position != -1) {
		foundInWord = true;

		wordGuessed.replace(position, 1, 1, wordUnguessed.at(position)); //Reveals letter if guessed correctly
		wordUnguessed.replace(position, 1, 1, '$');						//And puts a $ in to mark the letter has been guessed.

		position = wordUnguessed.find(letter);
	}

	return foundInWord;
}

void updateClientScreens() {
	// Encurses magic here to all the users
	// You should use all of the global variables

	return;
}

bool checkIfGameOver() {
	// Returns true is game is over
	//		i.e. All $ have been removed from wordGuesses
	//			i.e. All letters have been revealed to the players
	// Returns false otherwies
	//return (wordGuessed.find('$') == -1);
	return false;
}