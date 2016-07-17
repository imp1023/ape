#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include "tcrdb_win.h"

#pragma warning(disable : 4996)

const char *tcrdberrmsg(int ecode)
{
	return "miscellaneous error";
}

void tcrdbsetecode(TCRDB *rdb, int ecode)
{
	rdb->ecode = ecode;
}

TCRDB *tcrdbnew(void)
{
	TCRDB* rdb = new TCRDB;
	tcrdbsetecode(rdb, TTESUCCESS);
	if(access("./data/", 0) == -1)
	{
		mkdir("./data/");
	}
	return rdb;
}

void tcrdbdel(TCRDB *rdb)
{
	delete rdb;
}

int tcrdbecode(TCRDB *rdb)
{
	return rdb->ecode;
}

bool tcrdbopen(TCRDB *rdb, const char *host, int port)
{
	tcrdbsetecode(rdb, TTESUCCESS);
	return true;
}

bool tcrdbclose(TCRDB *rdb)
{
	tcrdbsetecode(rdb, TTESUCCESS);
	return true;
}

bool tcrdbput(TCRDB *rdb, const void *kbuf, int ksiz, const void *vbuf, int vsiz)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kbuf);
	FILE *pFile;
	int error = fopen_s(&pFile, szPath, "wb");
	if(NULL != pFile)
	{
		fwrite(vbuf, 1, vsiz, pFile);
		fclose(pFile);
		tcrdbsetecode(rdb, TTESUCCESS);
		return true;
	}
	tcrdbsetecode(rdb, TTEMISC);
	return false;
}

bool tcrdbput2(TCRDB *rdb, const char *kstr, const char *vstr)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kstr);
	FILE *pFile;
	fopen_s(&pFile, szPath, "wb");
	if(NULL != pFile)
	{
		fprintf(pFile, "%s", vstr);
		fclose(pFile);
		tcrdbsetecode(rdb, TTESUCCESS);
		return true;
	}
	tcrdbsetecode(rdb, TTEMISC);
	return false;
}

bool tcrdbout(TCRDB *rdb, const void *kbuf, int ksiz)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kbuf);
	if(remove(szPath) == 0)
	{
		tcrdbsetecode(rdb, TTESUCCESS);
		return true;
	}
	tcrdbsetecode(rdb, TTENOREC);
	return false;
}

bool tcrdbout2(TCRDB *rdb, const char *kstr)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kstr);
	if(remove(szPath) == 0)
	{
		tcrdbsetecode(rdb, TTESUCCESS);
		return true;
	}
	tcrdbsetecode(rdb, TTENOREC);
	return false;
}

void *tcrdbget(TCRDB *rdb, const void *kbuf, int ksiz, int *sp)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kbuf);
	*sp = 0;

	FILE *pFile;
	fopen_s(&pFile, szPath, "rb");
	if (pFile == NULL)
	{
		tcrdbsetecode(rdb, TTENOREC);
		return NULL;
	}

	fseek(pFile, 0, SEEK_END);
	int nSize = ftell(pFile);
	rewind(pFile);

	char* buffer = (char*)malloc(sizeof(char)*nSize);
	if (buffer == NULL)
	{
		tcrdbsetecode(rdb, TTEMISC);
		return NULL;
	}

	size_t result = fread(buffer,1,nSize,pFile);
	if (result != nSize)
	{
		free(buffer);
		tcrdbsetecode(rdb, TTEMISC);
		return NULL;
	}
	fclose(pFile);

	*sp = nSize;
	tcrdbsetecode(rdb, TTESUCCESS);
	return buffer;
}

char *tcrdbget2(TCRDB *rdb, const char *kstr)
{
	char szPath[256];
	sprintf_s(szPath, "./data/%s", kstr);

	FILE * pFile;
	fopen_s(&pFile, szPath, "rb");
	if (pFile == NULL)
	{
		tcrdbsetecode(rdb, TTENOREC);
		return NULL;
	}

	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	rewind(pFile);

	char* buffer = (char*)malloc(sizeof(char)*lSize);
	if (buffer == NULL)
	{
		tcrdbsetecode(rdb, TTEMISC);
		return NULL;
	}

	size_t result = fread(buffer,1,lSize,pFile);
	if (result != lSize)
	{
		tcrdbsetecode(rdb, TTEMISC);
		free(buffer);
		return NULL;
	}
	fclose(pFile);

	tcrdbsetecode(rdb, TTESUCCESS);
	return buffer;
}

#endif
