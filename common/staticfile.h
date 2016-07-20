/* 
 * File:   staticfile.h
 * Author: Kidd
 *
 * Created on 2012年2月20日, 上午10:46
 */

#ifndef STATICFILE_H
#define	STATICFILE_H
#include "stdio.h"

class StaticFile
{
public:
    StaticFile();
    ~StaticFile();
public:
    bool            Open(const std::string& filename);
    bool            Write(const std::string& str);
    bool            Write(int num);
    bool            WriteLine(const std::string& str);
    bool            WriteFirstLine(const std::string& str);
    bool            WriteFirstLine(int num);
    bool            WriteCF();
    void            OnWrite();
public:
    int             m_nWriteMod;
private:
    FILE*           m_pFile;
    std::string     m_strName;
    int             m_nWriteCnt;
} ;

//////////////////////////////////////////

inline void StaticFile::OnWrite()
{
    if (m_nWriteCnt > m_nWriteMod && m_pFile != NULL)
    {
        fflush(m_pFile);
        m_nWriteCnt = 0;
    }
    m_nWriteCnt++;
}
#endif	/* STATICFILE_H */

