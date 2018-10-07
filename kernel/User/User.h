#ifndef INCLUDED_USER_USER_H
#define INCLUDED_USER_USER_H

#include <Common.h>
#include <kernel/Security/types.h>

#define USERNAME_MAX_LENGTH 255

START_NS(Kernel)
C_CODE

typedef struct _user_t
{
	uid_t id;
	char name[USERNAME_MAX_LENGTH+1];
	gsid_t groups_id;
}
user_t;

user_t* getusrnam(const char* name);
user_t* getusrid(uid_t id);

struct groupset* user_get_groups_id(gsid_t id);
struct groupset* user_get_groups(const user_t*);


C_END
END_NS

#endif