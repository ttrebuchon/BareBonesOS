#ifndef INCLUDED_STD_FILE_H
#define INCLUDED_STD_FILE_H

#include <Common.h>

C_CODE

typedef struct __sFILE
{
	unsigned char* _p;
	int _r;
	int _w;
	
	int _flags;
	int _file;
	
	
	
	
	struct __sbuf _bf;
	int _lbfsize;
	
	
	void* _cookie;
	int (*_close)(void*);
	int (*_read)(void*, char*, int);
	fpos_t(*_seek)(void*, fpos_t, int);
	int(*_write)(void*, const char*, int);
	
	
	struct __sbuf _ext;

	unsigned char* _up;
	int _ur;
	
	
	unsigned char _ubuf[3];
	unsigned char _nbuf[1];
	
	
	struct __sbuf _lb;


	int _blksize;
	fpos_t _offset;
	
	
} FILE;


C_END