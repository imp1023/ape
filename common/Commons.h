#ifndef CORE_COMMONS_H
#define CORE_COMMONS_H

template <typename T>
inline
void SafeDelete(T*& ptr)
{
	if(ptr)
	{
		delete ptr;
	}

	ptr = NULL;
}

template <typename T>
inline
void SafeDeleteArray(T*& ptr)
{
	if(ptr)
	{
		delete[] ptr;
	}

	ptr = NULL;
}

#endif

