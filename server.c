#include <stdio.h>

enum UserStatus {
        CHOOSER,
        GUESSER
};

typedef struct User_ {
        int clientFD;
        enum UserStatus status;
        struct User_ *next;
} User;

char* askUserForWord(int clientFD);
char askUserForLetter(int clientFD);
int checkIfLetterIsInWord(char letter);
int getNextGuessorClientFD(int lastClientFD);
void addNewUserToUserList(int clientFD);
int checkIfUserIsStillConnected(int clientFD);
int removeUserFromList(int clientFD);
int setNewChooser(int clientFD);
void updateClientScreens(User* clientList);

//Meant to be a circularly linked list
User *userList;
int currentNumberOfIncorrectGuesses;
char *lettersGuessed;
char *incorrectLettersGuessed;

int main() {

	return 0;
}
