#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

using namespace std;

enum UserStatus {
        CHOOSER,
        GUESSER
};

typedef struct User_ {
        int clientFD;
        enum UserStatus status;
        struct User_ *next;
} User;

void askUserForWord();
char askUserForLetter(User* guessor);
bool checkIfLetterIsInWord(char letter);
void addNewUserToUserList(int clientFD);
bool checkIfUserIsStillConnected(int clientFD);
void removeUserFromList(int clientFD);
void setWordGuessed();
void setNewChooser(User *newChooser);
void updateClientScreens();
void playGame();
bool checkIfGameOver();
void doprocessing (int sock);

//Meant to be a circularly linked list
User *userList;
User *chooser;

int numIncorrectGuesses;
vector<char> lettersGuessed;
vector<char> incorrectLettersGuessed;

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

	//initialize the userlist
	userList = NULL;
	
	//Handle user connection crap
	int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

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
    while (1) 
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }

        doprocessing(newsockfd);
        std::cout << "Client Connected" << std::endl;

    } /* end of while */
	
	//playGame();
	return 0;
}

void doprocessing (int sock)
{
	addNewUserToUserList(sock);
	
	/* Client needs to wait here */
    
}

void playGame() {
	// Need to check and see if users are still connected in there at some point? Maybe often?
 
	User *winningUser = userList;
	while(true) {
		// Busy wait here for number connected players to be at least 2?
		setNewChooser(winningUser);
		askUserForWord();
		setWordGuessed();

		bool gameOver = false;
		lettersGuessed.clear();
		incorrectLettersGuessed.clear();
		User *guesser = userList;
		numIncorrectGuesses = 0;
		
		while (!gameOver) {
			guesser = guesser->next;
			if (guesser->status == CHOOSER) guesser = guesser->next;		//Prevent guesser from ever being chooser;

			char guess = askUserForLetter(guesser);
			if (checkIfLetterIsInWord(guess) == false){
				numIncorrectGuesses++;

				incorrectLettersGuessed.push_back(guess);
			} 
			// push letter to letters guessed
			lettersGuessed.push_back(guess);
			gameOver = checkIfGameOver();
			updateClientScreens();
			if (gameOver) winningUser = guesser;
		}
	}
}

void setNewChooser(User *newChooser) {
	chooser->status = GUESSER;
	newChooser->status = CHOOSER;
}

void askUserForWord() {
	// chooser is global var

	/*send "Enter a word for the guessors to guess: "
	wordUnguessed = libreadline

	error checking
	must not contain $
	no spaces? would be easier. could change later.
	*/
	
	int sock = chooser->clientFD;
	int n;
    char buffer[256];
	
	n = write(sock,"Enter a word for the guessors to guess: ",255);
    if (n < 0) 
    {
        perror("ERROR writing to socket");
        exit(1);
    }
	
    bzero(buffer,256);

    n = read(sock,buffer,255);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
	wordUnguessed = buffer;
	cout << "Server recieved word: " << wordUnguessed << " \n";

	return;
}

void setWordGuessed() {
	// Builds a string of correct length filled with $s.
	for(int i =0; i < wordUnguessed.length(); i++) {
		wordUnguessed+="$";
	}
}

char askUserForLetter(User* guessor) {
	// send to guessor "please enter a character to guess"
	// error check
	return 'a';
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
void addNewUserToUserList(int clientFD) {
	User *newUser = (User*)malloc(sizeof(User));
	newUser->status = GUESSER;
	newUser->clientFD = clientFD;
	newUser->next = userList;		//So it is circularlly linked

	if(userList == NULL) {
		newUser->next = newUser;
	}
	else {
		User *curPtr = userList;
		int headId = curPtr->clientFD;
		while((curPtr->next)->clientFD != headId) curPtr = curPtr->next;
		curPtr->next = newUser;
	}
}
bool checkIfUserIsStillConnected(int clientFD) {
	// Can reuse code from remote shell projected
	// Would use mine, but it sucks --Tyler

	return true;
}
void removeUserFromList(int clientFD) {
	User *curPtr = userList;
	if (clientFD == curPtr->clientFD) { //Case removing head of list
		User *tempUserList = userList;
		userList = userList->next;
		while((curPtr->next)->clientFD != tempUserList->clientFD) curPtr = curPtr->next;
		curPtr->next = userList;
		free(tempUserList);
	}
	else {
		while((curPtr->next)->clientFD != clientFD) curPtr = curPtr->next;
		User *temp = curPtr->next;
		curPtr->next = (curPtr->next)->next;
		free(temp);
	}
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
	return (wordGuessed.find('$') == -1);
}
