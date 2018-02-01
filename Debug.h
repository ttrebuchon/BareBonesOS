#ifndef INCLUDED_DEBUG_H
#define INCLUDED_DEBUG_H

//Includes and whatnot





#endif




#ifdef DEBUG

#ifndef INCLUDED_DEBUG_H_ENABLED
#define INCLUDED_DEBUG_H_ENABLED
#undef INCLUDED_DEBUG_H_DISABLED
#undef kassert

//TODO: kassert
#define kassert(X) 

#endif
#else

#ifndef INCLUDED_DEBUG_H_DISABLED
#define INCLUDED_DEBUG_H_DISABLED
#undef INCLUDED_DEBUG_H_ENABLED
#undef kassert


#define kassert(X) 

#endif
#endif