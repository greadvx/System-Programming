#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sstream>

//namespace patch
//{
//    template < typename T > std::string to_string(const T& n) {
//        std::ostringstream stm ;
//        stm << n ;
//        return stm.str() ;
//    }
//}
const static std::string MESSAGE =
		"AZINO777!!! Problem with "
				"access?? Add into URL: ";

const int WAIT_TIME = 1;

template <typename T>
std::string toString(T val)
{
	std::ostringstream oss;
	oss<< val;
	return oss.str();
}

int main()
{
	int status = 0;
	int thisPID = getpid();

	std::string childMessage = MESSAGE;
	childMessage += toString(thisPID);

	sigset_t set;

	//init of sig set and cleaning
	sigemptyset(&set);

	//adding to set signals
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGKILL);

	sigprocmask(SIG_BLOCK, &set, NULL);

	while (true)
	{
		sigwait(&set, &status);

		std::cout << childMessage.c_str() << std::endl;;

		kill(getppid(), SIGUSR2);
	}
	return 0;
}