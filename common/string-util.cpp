#include "string-util.h"
#include <iostream>
#include <sstream>
#ifdef _WIN32
#include <Windows.h>
#include <WinNls.h>
#else
#include<iconv.h>
#include <stdio.h>
#include<iconv.h>
#endif

using namespace std;

bool safe_atoi(const char* str, int &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool safe_atoi(const string &str, int &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool safe_atoi(const string &str, time_t &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
#ifndef _WIN32
bool safe_atoi(const string &str, size_t &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
#endif // _WIN32

bool safe_atoi(const string &str, unsigned int &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool safe_atoi(const string &str, unsigned char &num) {
	int value;
	istringstream iss(str);
	bool ret = !(iss>>value).fail();
	num = (unsigned char)value;
	return ret;
}
bool safe_atoll(const char* str, long long &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool safe_atoll(const string &str, long long &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool safe_atof(const string &str, float &num) {
	istringstream iss(str);
	return !(iss>>num).fail();
}
bool tokenize(const string& str, vector<string>& tokens, string &delims) {
	tokens.clear();
	string::size_type lastPos = 0;
	string::size_type pos;
	while ((pos = str.find_first_of(delims, lastPos))!=str.npos) {
		tokens.push_back(str.substr(lastPos, pos-lastPos));
		lastPos = pos + 1;
	}
	tokens.push_back(str.substr(lastPos));
	return true;
}

bool tokenize_as_int(const string& str, vector<int>& tokens, const string &delims) {
	tokens.clear();
	if (str.length()==0) return true;
	string::size_type lastPos = 0;
	string::size_type pos;
	bool succ = true;
	int int_value;
	while ((pos = str.find_first_of(delims, lastPos))!=str.npos) {
		succ = succ && safe_atoi(str.substr(lastPos, pos-lastPos).c_str(), int_value);
		if (succ) tokens.push_back(int_value);
		lastPos = pos + 1;
	}
	succ = succ && safe_atoi(str.substr(lastPos).c_str(), int_value);
	if (succ) tokens.push_back(int_value);
	return succ;
}
bool tokenize_as_longlong(const string& str, vector<long long>& tokens, const string &delims) {
	tokens.clear();
	string::size_type lastPos = 0;
	string::size_type pos;
	bool succ = true;
	long long int_value;
	while ((pos = str.find_first_of(delims, lastPos))!=str.npos) {
		succ = succ && safe_atoll(str.substr(lastPos, pos-lastPos).c_str(), int_value);
		if (succ) tokens.push_back(int_value);
		lastPos = pos + 1;
	}
	succ = succ && safe_atoll(str.substr(lastPos).c_str(), int_value);
	if (succ) tokens.push_back(int_value);
	return succ;
}

static const char *codes =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 base64 Encode a buffer (NUL terminated)
 @param in      The input buffer to encode
 @param inlen   The length of the input buffer
 @param out     [out] The destination of the base64 encoded data
 @param outlen  [in/out] The max size and resulting size
 @return 0 if successful
*/
int base64_encode(const unsigned char *in,  unsigned long inlen,
				        unsigned char *out, unsigned long *outlen)
{
	unsigned long i, len2, leven;
	unsigned char *p;

	if(in == NULL)
	{
		return 1;
	}
	if(out == NULL)
	{
		return 1;
	}
	if(outlen == NULL)
	{
		return 1;
	}

	/* valid output size ? */
	len2 = 4 * ((inlen + 2) / 3);
	if (*outlen < len2 + 1) {
		return 1;
	}
	p = out;
	leven = 3*(inlen / 3);
	for (i = 0; i < leven; i += 3) {
		*p++ = codes[(in[0] >> 2) & 0x3F];
		*p++ = codes[(((in[0] & 3) << 4) + (in[1] >> 4)) & 0x3F];
		*p++ = codes[(((in[1] & 0xf) << 2) + (in[2] >> 6)) & 0x3F];
		*p++ = codes[in[2] & 0x3F];
		in += 3;
	}
	/* Pad it if necessary...  */
	if (i < inlen) {
		unsigned a = in[0];
		unsigned b = (i+1 < inlen) ? in[1] : 0;

		*p++ = codes[(a >> 2) & 0x3F];
		*p++ = codes[(((a & 3) << 4) + (b >> 4)) & 0x3F];
		*p++ = (i+1 < inlen) ? codes[(((b & 0xf) << 2)) & 0x3F] : '=';
		*p++ = '=';
	}

	/* append a NULL byte */
	*p = '\0';

	/* return ok */
	*outlen = p - out;
	return 0;
}

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c)
{
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(std::string const& input_str)
{
	unsigned char const* bytes_to_encode = (unsigned char const*)input_str.c_str();
	unsigned int in_len = input_str.length();

	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--)
	{
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3)
		{
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
			{
				ret += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
		{
			char_array_3[j] = '\0';
		}

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
		{
			ret += base64_chars[char_array_4[j]];
		}

		while((i++ < 3))
		{
			ret += '=';
		}
	}
	return ret;
}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i < 4; i ++) {
				char_array_4[i] = base64_chars.find(char_array_4[i]);
			}

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++) {
				ret += char_array_3[i];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++) {
			char_array_4[j] = 0;
		}
		for (j = 0; j <4; j++) {
			char_array_4[j] = base64_chars.find(char_array_4[j]);
		}
		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) {
			ret += char_array_3[j];
		}
	}

	return ret;
}

#ifndef _WIN32
int gb2utf(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	char **pin = &inbuf;
	char **pout = &outbuf;

	iconv_t cd = iconv_open("utf-8", "gb2312");
	if (cd==0)
	{
		return -1;
	}

	memset(outbuf,0,outlen);
	int rc = iconv(cd, pin, &inlen, pout, &outlen);
	iconv_close(cd);

	if (rc == -1)
	{
		return -1;
	}
	return 0;
}
/*
int gb2unicode(char *inbuf, size_t inlen, wchar_t *outbuf, size_t outlen) 
{
	char **pin = &inbuf;
	char **pout = &outbuf;

	iconv_t cd = iconv_open("unicode", "gb2312");
	if (cd==0)
	{
		return -1;
	}

	memset(outbuf,0,outlen);
	int rc = iconv(cd, pin, &inlen, pout, &outlen);
	iconv_close(cd);

	if (rc == -1)
	{
		return -1;
	}
	return 0;
}
*/
#endif

const int CHAR_BUFSIZE = 4096;       // 待转换名最大长度
int gb2312ToUtf8(std::string const& input_str,std::string& output_str)
{
	output_str = "";
#ifdef _WIN32
	//wchar_t pcUnicode[CHAR_BUFSIZE];

	//gb2312 to unicode

	int nUnicodeLen = MultiByteToWideChar(0, 0, input_str.c_str(), -1, NULL, 0);

	wchar_t * pcUnicode = new wchar_t[nUnicodeLen+1]; 
	memset(pcUnicode, 0, nUnicodeLen * 2 + 2);

	MultiByteToWideChar(0, 0, input_str.c_str(), -1, pcUnicode, nUnicodeLen);


	//unicode to utf8

	int nUtf8Len = WideCharToMultiByte(65001, 0, pcUnicode, -1, NULL, 0, NULL, NULL);

	char *pcUtf8=new char[nUtf8Len + 1]; 
	memset(pcUtf8, 0, nUtf8Len + 1);   

	WideCharToMultiByte(CP_UTF8, 0, pcUnicode, -1, pcUtf8, nUtf8Len, NULL, NULL);
	
	output_str=pcUtf8;
	delete [] pcUtf8; 
	delete [] pcUnicode; 
	return 0;
#else
	size_t nInputLength = input_str.size()+1;
	char* pGB2312 = new char[nInputLength];
	strcpy(pGB2312,input_str.c_str());

	size_t nDestUtf8Len = input_str.size()*4+1;
	char* pUtf8 = new char[nDestUtf8Len];
	memset(pUtf8,0,nDestUtf8Len);
	
	if(gb2utf(pGB2312,nInputLength,pUtf8,nDestUtf8Len)==0)
	{
		output_str = pUtf8;
	}

	delete [] pUtf8; 
	delete [] pGB2312; 
	return 0;
#endif // _WIN32
	return 0;
}

int gb2312ToUnicode(std::string const& input_str,std::wstring& output_str)
{
	output_str = L"";
#ifdef _WIN32

	//gb2312 to unicode

	

	int nUnicodeLen = MultiByteToWideChar(0, 0, input_str.c_str(), -1, NULL, 0);

	wchar_t* pcUnicode = new wchar_t[nUnicodeLen+1]; 
	memset(pcUnicode, 0, nUnicodeLen * 2 + 2);

	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,input_str.c_str(),2,pcUnicode,1);
	//MultiByteToWideChar(CP_ACP, 0, input_str.c_str(), -1, pcUnicode, nUnicodeLen);

	output_str = pcUnicode;
	delete [] pcUnicode; 
	return 0;
#else
/*
	size_t nInputLength = input_str.size()+1;
	char* pGB2312 = new char[nInputLength];
	strcpy(pGB2312,input_str.c_str());

	size_t nDestUnicodeLen = input_str.size()*4+1;
	wchar_t* pUnicode = new wchar_t[nDestUnicodeLen];
	memset(pUnicode,0,nDestUnicodeLen*sizeof(wchar_t));

	if(gb2unicode(pGB2312,nInputLength,pUnicode,nDestUnicodeLen)==0)
	{
		output_str = pUnicode;
	}

	delete [] pGB2312; 
	return 0;
*/
#endif // _WIN32
	return 0;
}
