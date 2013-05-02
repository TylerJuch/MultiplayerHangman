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
			void removeUser(int clientFD);
			bool isOneUser();
			void setNewChooser(int clientFD);
			std::string getWordFromChooser(	);
			char getLetterFromGuesser();
			User* getGuesser();
			void writeToSocket(int clientFD, std::string text);
			std::string readFromSocket(int clientFD);
			User* getChooser();
			void sendMessageToAllClients(std::string message);
			bool userIsActive(int clientFD);
			static void *getGuess(void* arg);
			static void *timedWait(void* arg);
			void setNextGuesser();
			User* getCurrentGuesser();
			void setCurrentGuesser(User *newGuesser);

		private:
			User *userList;
			User *chooser;
			User *currentGuesser;
			int numOfUsers;
			void addNewUserToUserList(int clientFD);
			void removeUserFromList(int clientFD);
			
	};
};

#endif