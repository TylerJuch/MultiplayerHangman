#ifndef OSPROJ_USERS_H
#define OSPROJ_USERS_H


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

		private:
			User *userList;
			void setNewChooser(User *newChooser);
			void addNewUserToUserList(int clientFD);
			void removeUserFromList(int clientFD);
	};
};

#endif