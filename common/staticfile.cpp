/* 
 * File:   staticfile.cpp
 * Author: Kidd
 * 
 * Created on 2012年2月20日, 上午10:46
 */

#include <string>

#include "staticfile.h"

StaticFile::StaticFile()
{
    m_pFile     = NULL;
    m_strName   = "";
    m_nWriteCnt = 0;
    m_nWriteMod = 1;
}

StaticFile::~StaticFile()
{
    if (m_pFile != NULL)
    {
        fclose(m_pFile);
    }
}

bool StaticFile::Open(const std::string& filename)
{
    m_strName = filename;
    if (m_pFile != NULL)
    {
        fclose(m_pFile);
    }
    m_pFile = fopen(m_strName.c_str(), "w+");
    return m_pFile != NULL;
}

bool StaticFile::Write(const std::string& str)
{
    if (m_pFile == NULL)
    {
        return false;
    }
    fprintf(m_pFile, "%s", str.c_str());
    OnWrite();
    return true;
}

bool StaticFile::Write(int num)
{
    if (m_pFile == NULL)
    {
        return false;
    }
    fprintf(m_pFile, "%d", num);
    OnWrite();
    return true;
}

bool StaticFile::WriteCF()
{
    if (m_pFile == NULL)
    {
        return false;
    }
    fprintf(m_pFile, "\n");
    OnWrite();
    return true;
}

bool StaticFile::WriteLine(const std::string& str)
{
    return Write(str) && WriteCF();
}

bool StaticFile::WriteFirstLine(const std::string& str)
{
    if (m_pFile == NULL)
    {
        return false;
    }
    fseek(m_pFile, 0, 0);
    return Write(str);
}

bool StaticFile::WriteFirstLine(int num)
{
    if (m_pFile == NULL)
    {
        return false;
    }
    fseek(m_pFile, 0, 0);
    return Write(num);
}
