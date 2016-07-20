#ifndef _GDHANDLER_H_
#define _GDHANDLER_H_


#include <gd.h>
#include <gdfontmb.h>
class GDHandler
{
public:
	static GDHandler *GetInstance(void) { return &m_Instance; }
	bool Init(const char * pszFileName, const char *pszBack, int i4Width, int i4Height);
	void Release(void);

	void SetFontColor(int i4R, int i4G, int i4B);
	bool MakePngData(char *pszShow, int i4Len);
	char* GetPngData(int &nSize);
	void FreePngData(char *pszData);

	bool IsEnable(void) const { return m_bEnable; }
	void SetEnable(bool bEnable) { m_bEnable = bEnable; }
private:
	// 生成干扰雪花背景
	bool MakePngNoiseBack();
private:
	GDHandler(void);
	virtual ~GDHandler(void);
private:
	// 功能开关
	bool m_bEnable;

	int m_i4Width;
	int m_i4Height;
	// 记录生成图片的四个矩形坐标点
	int m_brect[8];
	// 字体文件地址
	char m_szPathTTF[48];
	// 背景色
	int m_i4BackColor;
	// 验证码字体颜色
	int m_i4FontColor;
	// 干扰背景字符
	char m_szBack[2];
	// 干扰背景字体
	gdFontPtr m_pBackFont;

	gdImagePtr m_pIM;
	static GDHandler m_Instance;
};

#endif

