#include "QA.hh"
#include <sys/stat.h>
#include <errno.h>

static void create_folder(const char* name)
{
	if (mkdir(name, 0777) != 0)
	{
		std::cerr << "[" << __func__ << "] " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
	}
}

static void create_folder_if_not_exists(const char* name)
{
	struct stat st;
	if (stat(name, &st) != 0)
	{
		if (errno == ENOENT)
		{
			create_folder(name);
		}
		else
		{
			std::cerr << "[" << __func__ << "] " << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	else if (!S_ISDIR(st.st_mode))
	{
		std::cerr << "There is already a FILE present called '" << name << "'" << std::endl;
		exit(0);
	}
}


void QA::SetupFolders()
{
	create_folder_if_not_exists("Images");
	create_folder_if_not_exists("DBs");
	create_folder_if_not_exists("Tmp");
}