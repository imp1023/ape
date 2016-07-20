#include <string>
#include "gdhandler.h"


GDHandler GDHandler::m_Instance;

GDHandler::GDHandler(void)
{
}

GDHandler::~GDHandler(void)
{
}

bool GDHandler::Init(const char * pszFileName, const char *pszBack, int i4Width, int i4Height)
{
	SetEnable(true);

	memset(m_szPathTTF, 0, sizeof(m_szPathTTF));
	memset(m_brect, 0, sizeof(m_brect));
	memset(m_szBack, 0, sizeof(m_szBack));

	if (NULL == pszFileName || '\0' == pszFileName[0])
		return false;
	if (NULL != m_szPathTTF)
	{
		// 复制宏
		strncpy(m_szPathTTF, pszFileName, sizeof(m_szPathTTF)-1);
		m_szPathTTF[sizeof(m_szPathTTF)-1] = '\0';
	}
	if (NULL != pszBack)
	{
		// 复制宏
		strncpy(m_szBack, pszBack, sizeof(m_szBack)-1);
		m_szBack[sizeof(m_szBack)-1] = '\0';
	}
	m_i4Width = i4Width;//80;
	m_i4Height = i4Height;//40;
#ifndef _WIN32
	m_pIM = gdImageCreate(m_i4Width, m_i4Height);
	if (NULL == m_pIM)
		return false;

	// 第一次获取是设置背景色
	m_i4BackColor = gdImageColorAllocate(m_pIM, 0xE0, 0xFF, 0xFF);

	m_i4FontColor = gdImageColorAllocate(m_pIM, 0xCD, 0x00, 0x00);

	m_pBackFont = gdFontGetMediumBold();
#endif
	return true;
}

void GDHandler::Release()
{
#ifndef _WIN32
	if (NULL != m_pIM)
		gdImageDestroy(m_pIM);
#endif
}

void GDHandler::SetFontColor(int i4R, int i4G, int i4B)
{
#ifndef _WIN32
	m_i4FontColor = gdImageColorAllocate(m_pIM, i4R, i4G, i4B);
#endif
}

char* GDHandler::GetPngData(int &nSize)
{
	char *pszData = NULL;
#ifndef _WIN32
	pszData = (char *)gdImagePngPtr(m_pIM, &nSize);
	if (NULL == pszData || 0 == nSize)
		return NULL;
#endif
	return pszData;
}

void GDHandler::FreePngData(char *pszData)
{
#ifndef _WIN32
	if (pszData != NULL)
	{
		gdFree(pszData);
	}
#endif
}

bool GDHandler::MakePngData(char *pszShow, int i4Len)
{
	if (NULL == pszShow || i4Len < 0 || strlen(pszShow) < (unsigned int)i4Len)
		return false;
#ifndef _WIN32
	gdImageFilledRectangle(m_pIM,0,0,100,40,m_i4BackColor);
	if (!MakePngNoiseBack())
		return false;
	//gdImageChar(m_pIM, pGDFP, 30, 30, 67, black);
	// 字体大小
	const double i4FTSIZE = 19;
	// 倾斜角度
	const double dANGLE = 12;
	// 字间距
	const int i4DIS = 25;
	// 字体起始坐标
	const int i4O_X = 5;
	const int i4O_Y = 25;
	// 右斜坐标修正
	const int i4MOD_X = 2;
	const int i4MOD_Y = 2;
	// 是否右斜
	bool bRight = false;

	int i4X = 0;
	int i4Y = 0;
	char szTempShow[2] = "";
	double bTempAngle = dANGLE;
	// 左斜右斜交替
	for(int i=0; i<i4Len; i++)
	{
		if (bRight)
		{
			i4X = i4O_X+i*i4DIS+i4MOD_X;
			i4Y = i4O_Y+i4MOD_Y;
		}
		else
		{
			i4X = i4O_X+i*i4DIS;
			i4Y = i4O_Y;
		}
		szTempShow[0] = pszShow[i];
		gdImageStringTTF(m_pIM, &m_brect[0], m_i4FontColor, m_szPathTTF, i4FTSIZE, bTempAngle, i4X, i4Y, szTempShow);
		bRight = !bRight;
		bTempAngle = -bTempAngle;
	}
#endif
	return true;
}

bool GDHandler::MakePngNoiseBack()
{
	if (NULL == m_pBackFont)
		return false;
#ifndef _WIN32
	for (int i=1; i<=18; i++)
	{
		gdImageChar(m_pIM, m_pBackFont, rand()%m_i4Width, rand()%m_i4Height, '*', gdImageColorAllocate(m_pIM,235, 200, 255));
	}
#endif
	return true;
}



