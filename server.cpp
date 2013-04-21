#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

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
void checkIfGameOver();

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
	//Handle user connection crap here

	playGame();
	return 0;
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
		User *guesser = userList;
		numIncorrectGuesses = 0;
		while (!gameOver) {
			guesser = guesser->next;
			if (guesser->status == CHOOSER) guesser = guesser->next;		//Prevent guesser from ever being chooser;

			char guess = askUserForLetter(guesser);
			if (checkIfLetterIsInWord(guess) == false) numIncorrectGuesses++;
			checkIfGameOver();
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

	User *curPtr = userList;
	while(curPtr->next != userList) curPtr = curPtr->next;
	curPtr->next = newUser;
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

	return;
}

void checkIfGameOver() {

	return;
}
