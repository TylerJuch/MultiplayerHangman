#ifndef OSPROJ_USERS_H
#define OSPROJ_USERS_H

#include <string>

namespace osproj
{
	enum UserStatus {
	        CHOOSER,
	        GUESSER
	};

	typedef struct User_ {
	        int clientFD;
	        enum UserStatus status;
	        struct User_ *next;
	} User;


	class Users
	{
		public:
			Users();
			~Users();
			
			void addUser(int clientFD);			
			User* getChooser();
			User* getCurrentGuesser();
			static void *getGuess(void* arg);
			User* getGuesser();
			char getLetterFromGuesser();
			std::string getWordFromChooser(	);
			bool isOneUser();
			std::string readFromSocket(int clientFD);
			void removeUser(int clientFD);
			void sendMessageToAllClients(std::string message);
			void setCurrentGuesser(User *newGuesser);
			void setNewChooser(int clientFD);
			void setNextGuesser();
			static void *timedWait(void* arg);
			void updateAllClientsScreen(int numWrong, std::string wordGuessed, std::string wrongGuesses);
			bool userIsActive(int clientFD);
			void writeToSocket(int clientFD, std::string text);
			
			
		private:
			User *chooser;
			User *currentGuesser;
			int numOfUsers;
			User *userList;
			void addNewUserToUserList(int clientFD);
			void removeUserFromList(int clientFD);
			
	};
};

#endif