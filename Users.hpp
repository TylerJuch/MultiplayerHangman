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
			bool isOneUser();
			void setNewChooser();
			std::string getWordFromChooser();
			char getLetterFromGuesser();
			User *getGuesser();
			void writeToSocket(int clientFD, std::string text);
			int getChooserFD();

		private:
			User *userList;
			User *chooser;
			int numOfUsers;
			void addNewUserToUserList(int clientFD);
			void removeUserFromList(int clientFD);
	};
};

#endif