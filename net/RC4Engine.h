/****************************************************************************
*
* RC4 Encyption Class
* Copyright (c) 2007 Burlex
* Adapted from rc4.c to C++
*
* This file may be distributed under the terms of the Q Public License
* as defined by Trolltech ASA of Norway and appearing in the file
* COPYING included in the packaging of this file.
*
* This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
* WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#ifndef _SHARED_RC4ENGINE_H
#define _SHARED_RC4ENGINE_H

#include <string>

#ifdef WIN32
	typedef unsigned __int8 uint8;
#else
	typedef uint8_t uint8;
#endif

class RC4Engine
{
	unsigned char perm[256];
	unsigned char index1, index2;
	bool Initialized;

public:

	//! RC4Engine constructor. Must supply a key and the length of that array.
	RC4Engine(const unsigned char * keybytes, unsigned int keylen,bool bC2S)
	{
		Setup(keybytes, keylen,bC2S);
	}
	bool IsInit(){return Initialized;}
#ifdef _WIN32
    void ResetInitialized(){Initialized = false;}
#endif
	RC4Engine() : Initialized(false) {}

	//! Destructor
	~RC4Engine() { }

	//! Initializes permutation, etc.
	void Setup(const unsigned char * keybytes, const unsigned int keylen,bool bC2S)
	{
		unsigned int i = 0;
		unsigned char j = 0, k;

		// Initialize RC4 state (all bytes to 0)
		if(bC2S)
		{
			for(; i < 256; ++i)
				perm[i] = (uint8)i;
		}
		else
		{
			for(; i < 256; ++i)
				perm[i] = (uint8)(255-i);
		}

		// Set index values
		index1 = index2 = 0;

		// Randomize the permutation
		for(j = 0, i = 0; i < 256; ++i)
		{//ÈÅÂÒÃÜÔ¿ 
			j += perm[i] + keybytes[i % keylen];
			k = perm[i];
			perm[i] = perm[j];
			perm[j] = k;
		}

		Initialized = true;
	}

	//! Processes the specified array. The same function is used for both
	//  encryption and decryption.

	void Process(const unsigned char * input, unsigned char * output, unsigned int len)
	{
		//static int ai = 0;
		//ai++;
		if(!Initialized || !input)
		{
			return;
		}
		unsigned int i = 0;
		unsigned char j, k;
		for(; i < len; ++i)
		{//index1 ,index2ÊÇ unsigned char
			index1++;
			index2 += perm[index1];

			k = perm[index1];
			perm[index1] = perm[index2];
			perm[index2] = k;

			j = perm[index1] + perm[index2];
			output[i] = input[i] ^ perm[j];
		}
	}

	void ReadProcess(string& input)
	{
		//static int ai = 0;
		//ai++;
		if(!Initialized || input.length() <= 0)
		{
			return;
		}
		unsigned int len = input.length();
		unsigned int i = 0;
		unsigned char j, k;
		for(; i < len; ++i)
		{//index1 ,index2ÊÇ unsigned char
			index2++;
			index1 += perm[index2];

			k = perm[index1];
			perm[index1] = perm[index2];
			perm[index2] = k;

			j = perm[index1] + perm[index2];
			input[i] = input[i] ^ perm[j];
		}
	}
};

#endif  // _SHARED_RC4ENGINE_H
