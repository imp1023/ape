// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//

#ifndef BASE_SINGLETON_H
#define BASE_SINGLETON_H

#include <pthread.h>
#include <stdlib.h> // atexit


template<typename T>
class Singleton
{
public:
	static T& instance()
	{
		pthread_once(&ponce_, &Singleton::init);
		return *value_;
	}

private:
	Singleton();
	~Singleton();

	static void init()
	{
		value_ = new T();
		::atexit(destroy);
	}

	static void destroy()
	{
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		delete value_;
	}

private:
	static pthread_once_t ponce_;
	static T*             value_;
};

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;


#endif

