
//********************************************
//	Ini 相关函数
//********************************************

#ifndef __INI_H__
#define __INI_H__

//#include "Type.h"

#include <stdlib.h>

#define ERROR_DATA (-99999999)
#define MAX_INI_VALUE (1024)
#define MAX_PATH1 (260)
//根据指针值删除内存
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	if( (x)!=NULL ) { delete (x); (x)=NULL; }
#endif
//根据指针值删除数组类型内存
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	if( (x)!=NULL ) { delete[] (x); (x)=NULL; }
#endif
//根据指针调用free接口
#ifndef SAFE_FREE
#define SAFE_FREE(x)	if( (x)!=NULL ) { free(x); (x)=NULL; }
#endif
//根据指针调用Release接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	if( (x)!=NULL ) { (x)->Release(); (x)=NULL; }
#endif
 
#ifndef _MAX_PATH
#define _MAX_PATH (4096)
#endif
//配置文件类
class Ini
{
////////////////////////////////////////////////
// 内部数据
////////////////////////////////////////////////
private:
	char			m_strFileName[MAX_PATH1];	//文件名
	long			m_lDataLen;					//文件长度
	char*			m_strData;					//文件内容

	int				IndexNum;					//索引数目（[]的数目）
	int*			IndexList;					//索引点位置列表
	int				Point;						//当前指针
	int				Line, Word;					//当前行列

	char			m_szValue[MAX_INI_VALUE] ;
	char			m_szRet[MAX_INI_VALUE];

////////////////////////////////////////////////
// 通用接口
////////////////////////////////////////////////
public:
	Ini();
	Ini(const char* filename);								//初始化打开配置文件
	virtual ~Ini();									//释放内存
	char			*GetData();							//返回文件内容
	int				GetLines(int);						//返回文件的行数
	int				GetLines();						//返回文件的行数

	bool			Open(const char* filename);				//打开配置文件
	void			Close();							//关闭配置文件
	bool			Save(char *filename=NULL);			//保存配置文件
	int				FindIndex(char *);					//返回标题位置

////////////////////////////////////////////////
// 内部函数
////////////////////////////////////////////////
private:
	void		InitIndex();						//初始化索引
	int			FindData(int, char *);				//返回数据位置
	int			GotoNextLine(int); 					//提行
	char*		ReadDataName(int &);				//在指定位置读一数据名称
	char*		ReadText(int);						//在指定位置读字符串

	bool		AddIndex(char *);					//加入一个索引
	bool		AddData(int, char *, char *);		//在当前位置加入一个数据
	bool		ModityData(int, char *, char *);	//在当前位置修改一个数据的值
	int			GotoLastLine(char *section);			//把指针移动到本INDEX的最后一行

////////////////////////////////////////////////
// 公用接口
////////////////////////////////////////////////
public:
	//读一个整数
	int				Readint(char *section, char *key);
	//如果存在，则读一个整数
	bool			ReadintIfExist(char *section, char *key, int& nResult);
	//在指定的行读一整数
	int				Readint(char *section, int lines);	
	//读一个字符串
	char*			ReadText(char *section, char *key, char* str, int size);
	//如果存在则读取
	bool			ReadTextIfExist(char *section, char *key, char* str, int size);
	//在指定的行读一字符串
	char*			ReadText(char *section, int lines, char* str, int size);	
	//在指定行读一字符名称
	char*			ReadCaption(char *section, int lines, char* str, int size);
	//写一个整数
	bool			Write(char *section, char *key, int num);			
	//写一个字符串
	bool			Write(char *section, char *key, char *string);		
	//返回连续的行数（从INDEX到第一个空行）
	int				GetContinueDataNum(char *section);	
	//在指定位置读字符串
	char*			ReadOneLine(int);
	int				FindOneLine(int);
	//返回指定字符所在的行数
	int				ReturnLineNum(char*);
};



#endif



