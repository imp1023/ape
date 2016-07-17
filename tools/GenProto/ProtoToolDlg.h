// BombProtoToolDlg.h : header file
//

#pragma once

#include <vector>
#include <string>
#include <map>
using namespace std;
// CProtoToolDlg dialog

enum DirType
{
	C2S = 0,
	S2C,
};

class CProtoToolDlg : public CDialog
{
// Construction
public:
	CProtoToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BOMBPROTOTOOL_DIALOG };

	CString m_strRootDir;
public:
	//读取之前生成的文件，用于保证生成顺序
	void ReadMessageDefFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	//为读取顺序重新排序
	void SortMsgLst(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr,map<string,int>& mapMsgStr,map<string,int>& mapDelMsgStr);
	void GetFileList(vector<string>& lstFileName);
	bool DealConvert(map<string,int>& mapMsgStr,CString strFileName);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void CallBatFile();
    void CallBatFile(const CString& dir, const CString& file);
	void CreateEventProto(vector<string>& lstFileName,vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr,map<string,int> mapDelMsgStr);
	void CreateMessageDefFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateAs3File(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
    void CreateLuaFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateAutoCreateInstance(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateClientHandlerAuto(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
    void CreateSseCmdAuto(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateAutoEventHeadFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateC2SMessage(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr);
	void CreateProtoHeadFile(vector<string>& lstFileName);
	void CreateC2SFile_H(CString strFileName,CString strMsg,bool bBce = true);
	void CreateC2SFile_CPP(CString strFileName,CString strMsg,bool bBce = true);
	void AddString(CString strInfo);
	virtual void OnOK();
	virtual void OnCancel();
	CString m_strStep;
};
