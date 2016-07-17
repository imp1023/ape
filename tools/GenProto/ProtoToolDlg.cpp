// BombProtoToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProtoTool.h"
#include "ProtoToolDlg.h"

#include <string>
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProtoToolDlg dialog


const char SZMSGDIR[] = "ProtoMessage";
const char SZEVENTDIR[] = "event";
const char SZASDIR[] = "as-ProtoMessage";
const char SZLUADIR[] = "client_export\\lua_gen";
const char SZEVENTPROTO[] = "event.proto";
const char SZSCE[] = "Dce";	//c2s
const char SZSSE[] = "Dse";	//s2c
using namespace std;
CProtoToolDlg::CProtoToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProtoToolDlg::IDD, pParent)
	, m_strStep(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProtoToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_STEP, m_strStep);
}

BEGIN_MESSAGE_MAP(CProtoToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CProtoToolDlg message handlers

BOOL CProtoToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowText("农场——proto文件转换工具");
	GetDlgItem(IDOK)->SetWindowText("转换");
	GetDlgItem(IDCANCEL)->SetWindowText("退出");
	CString strInfo;
	strInfo = "说明：\r\n";
	strInfo +="\t1.该程序只能放在src根目录，event，gamed文件夹的上一层\r\n";
	strInfo +="\t2.待转换的proto文件必须放在该程序同级文件夹中，文件夹名称：";
	strInfo += SZMSGDIR;
	strInfo += "\r\n";
	strInfo +="\t3.Sce开头的文件，将自动生成一系列信息处理文件\r\n";
	strInfo +="\t4.点击“转换”生成文件\r\n";
	GetDlgItem(IDC_STATIC_TIP)->SetWindowText(strInfo);
	char szDir[1024];
	GetCurrentDirectory(1024,szDir);
	m_strRootDir = szDir;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProtoToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProtoToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProtoToolDlg::AddString(CString strInfo)
{
	m_strStep += strInfo;
	m_strStep += "\r\n";
	UpdateData(false);
}

void CProtoToolDlg::ReadMessageDefFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s//gamed//event//MessageDef.h",m_strRootDir);
	fstream fRead(strFileName.GetBuffer(0),ios::in |ios::binary);
	if (fRead.fail())
	{
		CString str;
		str.Format("未找到文件：%s,重新生成所有文件顺序！！",strFileName);
		AddString(str);
		return;
	}	
	char szInfo[2048];
	CString strInfo;
	while(fRead.getline(szInfo,2048))
	{
		strInfo = szInfo;
		int nPos1 = strInfo.Find("//^c^");
		int nPos2 = strInfo.Find("//^s^"); 
		if(nPos1>0)
		{
			strInfo = strInfo.Mid(nPos1+5);
			strInfo.TrimLeft();
			int nPosEnd = strInfo.Find("#");
			strInfo = strInfo.Mid(0,nPosEnd);
			lstC2SMsgStr.push_back(strInfo.GetBuffer(0));
		}
		else if(nPos2>0)
		{
			strInfo = strInfo.Mid(nPos2+5);
			strInfo.TrimLeft();
			int nPosEnd = strInfo.Find("#");
			strInfo = strInfo.Mid(0,nPosEnd);
			lstS2CMsgStr.push_back(strInfo.GetBuffer(0));
		}
	}
}

void CProtoToolDlg::SortMsgLst(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr,map<string,int>& mapMsgStr,map<string,int>& mapDelMsgStr)
{
	map<string,int>::iterator iter;
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		string strName = lstC2SMsgStr[i];
		iter = mapMsgStr.find(strName);
		if(iter != mapMsgStr.end())
		{
			mapMsgStr.erase(iter);
		}
		else
		{//已删除
			mapDelMsgStr.insert(make_pair(strName,1));
		}
	}
	for(int i=0;i<lstS2CMsgStr.size();i++)
	{
		string strName = lstS2CMsgStr[i];
		iter = mapMsgStr.find(strName);
		if(iter != mapMsgStr.end())
		{
			mapMsgStr.erase(iter);
		}
		else
		{//已删除
			mapDelMsgStr.insert(make_pair(strName,1));
		}
	}
	for(iter = mapMsgStr.begin();iter!=mapMsgStr.end();iter++)
	{
		string strName = iter->first;
		if(strName.size()>3&&strName.substr(0,3)==SZSCE)
		{
			lstC2SMsgStr.push_back(strName);
		}
		else if(strName.size()>3&&strName.substr(0,3)==SZSSE)
		{
			lstS2CMsgStr.push_back(strName);
		}
	}
}

void CProtoToolDlg::GetFileList(vector<string>& lstFileName)
{
	SetCurrentDirectory(m_strRootDir);
	if(!SetCurrentDirectory(SZMSGDIR))
	{
		CString str;
		str.Format("未找到文件夹：%s!",SZMSGDIR);
		AddString(str);
		return;
	}
	CFileFind fFile;
	CString strFileName;
	BOOL bFind = fFile.FindFile();
	while(bFind)
	{
		bFind = fFile.FindNextFile();
		strFileName = fFile.GetFileName();
		
		int nPos = strFileName.ReverseFind('.');
		if(nPos<=0)
			continue;
		CString strTmp = strFileName.Mid(nPos+1);
		strTmp.MakeLower();
		if(strTmp=="proto")
		{
			lstFileName.push_back(fFile.GetFileName().GetBuffer(0));
		}
	}
	SetCurrentDirectory(m_strRootDir);
}

void CProtoToolDlg::OnOK()
{
	m_strStep = ""; 
	
	vector<string> lstFileName;
	GetFileList(lstFileName);

	map<string,int> mapMsgStr;
	map<string,int> mapDelMsgStr;
	for(int i=0;i<lstFileName.size();i++)
	{
		DealConvert(mapMsgStr,lstFileName[i].c_str());
	}
	
	//读取已生成的文件(读取MessageDef.h)
	vector<string> lstC2SMsgStr;
	vector<string> lstS2CMsgStr;
	ReadMessageDefFile(lstC2SMsgStr,lstS2CMsgStr);
	SortMsgLst(lstC2SMsgStr,lstS2CMsgStr,mapMsgStr,mapDelMsgStr);
	//生成Event.proto
	CreateEventProto(lstFileName,lstC2SMsgStr,lstS2CMsgStr,mapDelMsgStr);
	//调用bat文件
	CallBatFile();
	//生成MessageDef.h
	CreateMessageDefFile(lstC2SMsgStr,lstS2CMsgStr);
	//生成GenAs3File.sh
	//CreateAs3File(lstC2SMsgStr,lstS2CMsgStr);
    CreateLuaFile(lstC2SMsgStr, lstS2CMsgStr);
	//生成AutoEvent_PDHead.h
	CreateProtoHeadFile(lstFileName);
	//生成AutoEventHead.h
	CreateAutoEventHeadFile(lstC2SMsgStr,lstS2CMsgStr);
	//生成AutoCreateInstance.cpp
	CreateAutoCreateInstance(lstC2SMsgStr,lstS2CMsgStr);
	//生成ClientHandler_Auto.h
	CreateClientHandlerAuto(lstC2SMsgStr,lstS2CMsgStr);
    //生成SseCmd_Auto.h
    CreateSseCmdAuto(lstC2SMsgStr,lstS2CMsgStr);
	AddString("--------------------------------------------------------------------------------");
	//生成C2S的Deal+Message类
	CreateC2SMessage(lstC2SMsgStr,lstS2CMsgStr);	
	
	AfxMessageBox("完成！");
}

void CProtoToolDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CProtoToolDlg::CallBatFile(const CString& dir_, const CString& file)
{
    CString dir = dir_;
    SetCurrentDirectory(dir.GetBuffer(0));
    SHELLEXECUTEINFO ShExecInfo1 = {0};
    ShExecInfo1.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo1.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo1.hwnd = NULL;
    ShExecInfo1.lpVerb = NULL;
    ShExecInfo1.lpFile = file;		
    ShExecInfo1.lpParameters = "";	
    ShExecInfo1.lpDirectory = dir;
    ShExecInfo1.nShow = SW_HIDE;
    ShExecInfo1.hInstApp = NULL;	
    ShellExecuteEx(&ShExecInfo1);
    WaitForSingleObject(ShExecInfo1.hProcess,INFINITE);
    
    CString strInfo;
    strInfo.Format("调用%s完毕！", file);
    AddString(strInfo);
}

void CProtoToolDlg::CallBatFile()
{
	CString strDir;
	CString strFile;
	
	strFile.Format("%s\\%s\\%s",m_strRootDir,SZMSGDIR,"buildproto.bat");
	strDir.Format("%s\\%s",m_strRootDir,SZMSGDIR);
    CallBatFile(strDir, strFile);
	
	strFile.Format("%s\\%s\\%s",m_strRootDir,SZEVENTDIR,"buildproto.bat");
	strDir.Format("%s\\%s",m_strRootDir,SZEVENTDIR);
    CallBatFile(strDir, strFile);

//     strFile.Format("%s\\%s\\%s",m_strRootDir,SZASDIR,"buildproto.bat");
//     strDir.Format("%s\\%s",m_strRootDir,SZASDIR);
//     CallBatFile(strDir, strFile);

    strFile.Format("%s\\%s\\%s", m_strRootDir, SZLUADIR, "protoLua.bat");
    strDir.Format("%s\\%s", m_strRootDir, SZLUADIR);
    CallBatFile(strDir, strFile);
	
	CenterWindow();
	SetCurrentDirectory(m_strRootDir.GetBuffer(0));
}

bool CProtoToolDlg::DealConvert(map<string,int>& mapMsgStr,CString strFileName)
{
	CString strFile;
	strFile.Format("%s\\%s\\%s",m_strRootDir,SZMSGDIR,strFileName);
	fstream fRead(strFile.GetBuffer(0),ios::in |ios::binary);
	if (fRead.fail())
	{
		CString str;
		str.Format("未找到文件：%s!",strFile);
		AddString(str);
		return false;
	}	
	char szInfo[2048];
	CString strInfo;
	while(fRead.getline(szInfo,2048))
	{
		strInfo = szInfo;
		int nPos = strInfo.Find("message ");
		if(nPos>=0)
		{
			strInfo = strInfo.Mid(nPos+8);
			strInfo.TrimLeft();
			int i=0;
			for(i=0;i<strInfo.GetLength();i++)
			{
				char ch = strInfo.GetAt(i);
				if(	(ch >='a' && ch <='z') ||
					(ch >='A' && ch <='Z')||
					(ch >='0' && ch <='9'))
					continue;
				else
					break;
			}
			if(i>0)
			{
				strInfo = strInfo.Mid(0,i);
				mapMsgStr.insert(make_pair(strInfo.GetBuffer(0),1));
				//lstMsgStr.push_back(strInfo.GetBuffer(0));
			}
		}
	}
	fRead.close();

	return true;
}

void CProtoToolDlg::CreateProtoHeadFile(vector<string>& lstFileName)
{
	CString strFileName;
	strFileName.Format("%s/event/AutoEvent_PDHead.h",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strInfo;
	fs << "#ifndef _AUTOEVENT_PDHEAD_H_ "<<endl;
	fs << "#define _AUTOEVENT_PDHEAD_H_"<<endl;
	fs<<"#pragma once\n";
	fs<<"//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。"<<endl;
	for(int i=0;i<lstFileName.size();i++)
	{
		CString strFile;
		strFile = lstFileName[i].substr(0,lstFileName[i].find(".proto")).c_str();
		strFile.TrimLeft();
		strFile.TrimRight();
		{
			strInfo.Format("#include \"%s.pb.h\"",strFile);
			fs << strInfo.GetBuffer(0) << endl;
		}
	}
	fs << "#endif"<<endl;
	fs << ""<<endl<<endl;

	fs.close();

	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateEventProto(vector<string>& lstFileName,vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr,map<string,int> mapDelMsgStr)
{
	CString strOldEventFile;
	CString strNewEventFile;
	strOldEventFile.Format("%s\\%s\\%s_old",m_strRootDir,SZEVENTDIR,SZEVENTPROTO);
	strNewEventFile.Format("%s\\%s\\%s",m_strRootDir,SZEVENTDIR,SZEVENTPROTO);
	DeleteFile(strOldEventFile);
	if(!CopyFile(strNewEventFile,strOldEventFile,true))
	{
		CString str;
		str.Format("未找到文件：%s!",strNewEventFile);
		AddString(str);
		return;
	}
	fstream fRead(strOldEventFile.GetBuffer(0),ios::in |ios::binary);
	if (fRead.fail())
	{
		CString str;
		str.Format("未找到文件：%s!",strOldEventFile);
		AddString(str);
		return;

	}
	ofstream fWrite(strNewEventFile.GetBuffer(0),ios::binary);
	if (fWrite.fail())
	{
		CString str;
		str.Format("未找到文件：%s!",strOldEventFile);
		AddString(str);
		return;
	}

	//重新生成event.proto
	AddString("嵌入event.proto的import部分");
	//import 前边部分
	CString strInfo;
	char szInfo[2048];
	while(fRead.getline(szInfo,2048))
	{
		strInfo = szInfo;
		if(strInfo.Find("//start import")>=0)
		{
			fWrite.write(szInfo,strlen(szInfo));
			fWrite << "\n";
			break;
		}
		fWrite.write(szInfo,strlen(szInfo));
		fWrite << "\n";
	}
	int i=0;
	for(i=0;i<lstFileName.size();i++)
	{
		if(lstFileName[i].substr(0,3)==SZSCE||lstFileName[i].substr(0,3)==SZSSE)
		{
			strInfo.Format("import \"%s\"; \n",lstFileName[i].c_str());
			fWrite.write(strInfo.GetBuffer(0),strInfo.GetLength());
		}
	}
	//import 后边部分
	bool bFind = false;
	int nSceStart = 200;
	int nSseStart = 500;
	while(fRead.getline(szInfo,2048))
	{
		strInfo = szInfo;
		if(!bFind)
		{
			if(strInfo.Find("//end import")>=0)
			{
				bFind = true;
			}
		}

		if(bFind)
		{
			fWrite.write(szInfo,strlen(szInfo));
			fWrite << "\n";
			strInfo = szInfo;
			if(strInfo.Find("//auto start key =")>=0)
			{
				strInfo= strInfo.Mid(strInfo.Find("//auto start key =")+18);
				strInfo.TrimLeft();
				nSceStart = atoi(strInfo.GetBuffer(0));
				strInfo= strInfo.Mid(strInfo.Find(",")+1);
				nSseStart = atoi(strInfo.GetBuffer(0));
			}
			if(strInfo.Find("//start Event auto")>=0)
			{
				break;
			}
		}
	}
	//写Sce
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		string strName = lstC2SMsgStr[i];
		if(mapDelMsgStr.find(strName)==mapDelMsgStr.end())
		{
			strInfo.Format("\toptional %s ce_%s = %d;",strName.c_str(),strName.c_str(),nSceStart++);
			fWrite << strInfo.GetBuffer(0);
			fWrite << "\n";
		}
		else
		{
			strInfo.Format("\t//optional %s ce_%s = %d;",strName.c_str(),strName.c_str(),nSceStart++);
			fWrite << strInfo.GetBuffer(0);
			fWrite << "\n";
		}
	}
	fWrite << "\n\n\n";
	//写Sse
	for(int i=0;i<lstS2CMsgStr.size();i++)
	{
		string strName = lstS2CMsgStr[i];
		if(mapDelMsgStr.find(strName)==mapDelMsgStr.end())
		{
			strInfo.Format("\toptional %s se_%s = %d;",strName.c_str(),strName.c_str(),nSseStart++);
			fWrite << strInfo.GetBuffer(0);
			fWrite << "\n";
		}
		else
		{
			strInfo.Format("\t//optional %s se_%s = %d;",strName.c_str(),strName.c_str(),nSseStart++);
			fWrite << strInfo.GetBuffer(0);
			fWrite << "\n";
		}
	}

	fWrite << "}"<<endl;
	fWrite.close();
	fRead.close();
	DeleteFile(strOldEventFile);	

	strInfo.Format("生成%s完毕！",strNewEventFile);
	AddString(strInfo);
}
void CProtoToolDlg::CreateClientHandlerAuto(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s//gamed//ClientHandler_Auto.h",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strInfo;
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		if(lstC2SMsgStr[i].substr(0,3)==SZSCE)
		{
			CString strMsg = lstC2SMsgStr[i].c_str();
			strMsg.TrimLeft();
			strMsg.TrimRight();
			CString strMsg1;
			strMsg1.Format("mutable_ce_%s",strMsg);
			strMsg1.MakeLower();
			strInfo.Format("DEFINE_AUTO_EVENT(C2S_%s,%s,%s)",strMsg,strMsg,strMsg1.MakeLower());
			fs << strInfo.GetBuffer(0) << endl;
		}
	}
	fs.close();

	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateSseCmdAuto(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s//gamed//SseCmd_Auto.h",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strInfo;
	for(int i=0;i<lstS2CMsgStr.size();i++)
	{
		if(lstS2CMsgStr[i].substr(0,3)==SZSSE)
		{
			CString strMsg = lstS2CMsgStr[i].c_str();
			strMsg.TrimLeft();
			strMsg.TrimRight();
            strInfo.Format("DEFINE_SSE_CMD(%s, S2C_%s)", strMsg, strMsg);
			fs << strInfo.GetBuffer(0) << endl;
		}
	}
	fs.close();

	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateAutoEventHeadFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s/gamed/event/AutoEventHead.h",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strInfo;
	fs << "#ifndef _AUTOEVENTHEAD_H_"<<endl;
	fs << "#define _AUTOEVENTHEAD_H_"<<endl;
	fs<<"#pragma once"<<endl;
	fs<<"//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。\r"<<endl;
	
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		if(lstC2SMsgStr[i].substr(0,3)==SZSCE)
		{
			strInfo.Format("#include \"%sHandle.h\"",lstC2SMsgStr[i].c_str());
			fs << strInfo.GetBuffer(0) << endl;
		}
	}
	fs << "#endif"<<endl;
	fs << ""<<endl<<endl;
	
	fs.close();

	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateAutoCreateInstance(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s/gamed/GameEventAutoCreate.cpp",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strInfo;
	fs<<"#include \"event/AutoEventHead.h\""<<endl;
	fs<<"#include \"GameNetHandler.h\""<<endl;
	fs<<"#include \"GameEventHandler.h\""<<endl;
	fs<<"//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。"<<endl;
	
	fs << ""<<endl<<endl;
	fs << "void GameEventHandler::initAutoEventProcessors()"<<endl;
	fs << "{"<<endl;
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		if(lstC2SMsgStr[i].substr(0,3)==SZSCE)
		{
			strInfo.Format("\t%sHandle::createInstance(this);",lstC2SMsgStr[i].c_str());
			fs << strInfo.GetBuffer(0) << endl;
		}
	}
	fs << "}"<<endl;
	fs.close();

	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateAs3File(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s//FlashFile//ProtocolMgr.as",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);

	fs<<"package com.net {"<<endl;
	fs<<"    import flash.utils.getQualifiedClassName;"<<endl;
	fs<<"    import com.transport.PkgMap;"<<endl<<endl;

	fs<<"    /**"<<endl;
	fs<<"     * ..."<<endl;
	fs<<"     * @author Foreverflying"<<endl;
	fs<<"     */"<<endl;
	fs<<"    public class ProtocolMgr {"<<endl;
	fs<<"        public function ProtocolMgr() {"<<endl;
	fs<<"            _pkgMap = new PkgMap;"<<endl;
	fs<<"            InitPkgMap();"<<endl;
	fs<<"        }"<<endl<<endl;

	fs<<"        public function GetPkgMap() : PkgMap {"<<endl;
	fs<<"            return _pkgMap;"<<endl;
	fs<<"        }"<<endl<<endl;

	fs<<"        private function InitPkgMap() : void {"<<endl;
	
	CString strInfo;
	int i=0;
	int nStep = 0;
	for(i=0;i<lstC2SMsgStr.size();i++)
	{
		//if(lstMsgStr.size()>3&&lstMsgStr[i].substr(0,3)==SZBSE)
		{
			strInfo.Format("            _pkgMap.AddPkg(  1024 + %4d, %s );",nStep++,lstC2SMsgStr[i].c_str());
			fs << strInfo.GetBuffer(0)<<endl;
		}
	}
	fs << ""<<endl;
	nStep = 0;
	for(i=0;i<lstS2CMsgStr.size();i++)
	{
		//if(lstMsgStr.size()>3&&lstMsgStr[i].substr(0,3)==SZBCE)
		{
			strInfo.Format("            _pkgMap.AddPkg(  512 + %4d, %s );",nStep++,lstS2CMsgStr[i].c_str());
			fs << strInfo.GetBuffer(0)<<endl;
		}
	}
	fs <<  "        }"<<endl<<endl;
	fs <<  "        private var _pkgMap : PkgMap;"<<endl;
	fs <<  "    }"<<endl;
	fs <<  "}"<<endl;
	fs.close();
	
	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateLuaFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
    CString strFileName;
    strFileName.Format("%s//client_export//lua//ProtocolMgr.lua",m_strRootDir);
    ofstream fs(strFileName.GetBuffer(0),ios::binary);
    CString strInfo;
    int i=0;

    fs<<"ProtocolMgr = class(\"ProtocolMgr\")" << endl << endl;
    fs<<"require(\"totalwar/core/io/protocol/PkgMap.lua\")" << endl;
    for(i=0;i<lstC2SMsgStr.size();i++)
    {
        strInfo.Format("require(\"totalwar/core/io/protocol/%s_pb.lua\")",lstC2SMsgStr[i].c_str());
        fs << strInfo.GetBuffer(0) <<endl;
    }
    for(i=0;i<lstS2CMsgStr.size();i++)
    {
        strInfo.Format("require(\"totalwar/core/io/protocol/%s_pb.lua\")",lstS2CMsgStr[i].c_str());
        fs << strInfo.GetBuffer(0) <<endl;
    }
    fs<<""<<endl;
    int nStep = 0;
    for(i=0;i<lstC2SMsgStr.size();i++)
    {
        strInfo.Format("ID_%s = 1024 + %d",lstC2SMsgStr[i].c_str(),nStep++);
        fs << strInfo.GetBuffer(0)<<endl;
    }
    nStep = 0;
    for(i=0;i<lstS2CMsgStr.size();i++)
    {
        strInfo.Format("ID_%s = 512 + %d",lstS2CMsgStr[i].c_str(),nStep++);
        fs << strInfo.GetBuffer(0)<<endl;
    }

    fs<<endl<<"local _pkgMap = nil"<<endl;
    fs<<endl<<"function ProtocolMgr:initialize()" << endl << endl;
    fs<<"    _pkgMap = PkgMap:new()" << endl;
    fs<<"    self:InitPkgMap()" << endl;
    fs<<"end" << endl << endl;
    fs<<"function ProtocolMgr:GetPkgMap()"<<endl;
    fs<<"    return _pkgMap"<<endl;
    fs<<"end"<<endl<<endl;
    fs<<"function ProtocolMgr:InitPkgMap()"<<endl;
    nStep = 0;
    for(i=0;i<lstC2SMsgStr.size();i++)
    {
        strInfo.Format("    _pkgMap:AddPkg(ID_%s, %s_pb.%s())",lstC2SMsgStr[i].c_str(),lstC2SMsgStr[i].c_str(),lstC2SMsgStr[i].c_str());
        fs << strInfo.GetBuffer(0)<<endl;
    }
    nStep = 0;
    for(i=0;i<lstS2CMsgStr.size();i++)
    {
        strInfo.Format("    _pkgMap:AddPkg(ID_%s, %s_pb.%s())",lstS2CMsgStr[i].c_str(),lstS2CMsgStr[i].c_str(),lstS2CMsgStr[i].c_str());
        fs << strInfo.GetBuffer(0)<<endl;
    }
    fs<<"end"<<endl<<endl;
    fs.close();

    strInfo.Format("生成%s完毕！",strFileName);
    AddString(strInfo);
}

void CProtoToolDlg::CreateMessageDefFile(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	CString strFileName;
	strFileName.Format("%s//gamed//event//MessageDef.h",m_strRootDir);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);

	fs<<"#ifndef _MESSAGEDEF_H_"<<endl;
	fs<<"#define _MESSAGEDEF_H_"<<endl;
	fs<<"#pragma once"<<endl;
	fs<<"//该文件为使用工具自动生成，请不要在文件中修改，如需要修改，请与sskylin联系。"<<endl;

	fs<<"enum S2C_EVENT"<<endl;
	fs<<"{"<<endl;
	fs<<"\tS2C_EVENT_BASE = 511,"<<endl;
	int i=0;
	for(i=0;i<lstS2CMsgStr.size();i++)
	{
		//if(lstMsgStr.size()>3&&lstMsgStr[i].substr(0,3)==SZBSE)
		{
			fs << "\tS2C_";
			fs << lstS2CMsgStr[i].c_str();
			fs << ",\t\t//^s^";
			fs << lstS2CMsgStr[i].c_str();
			fs << "#"<<endl;
		}
	}
	fs<<"};"<<endl;

	fs<<"enum C2S_EVENT"<<endl;
	fs<<"{"<<endl;
	fs<<"\tC2S_EVENT_BASE = 1023,"<<endl;
	for(i=0;i<lstC2SMsgStr.size();i++)
	{
		//if(lstMsgStr.size()>3&&lstMsgStr[i].substr(0,3)==SZBCE)
		{
			fs << "\tC2S_";
			fs << lstC2SMsgStr[i].c_str();
			fs << ",\t\t//^c^";
			fs << lstC2SMsgStr[i].c_str();
			fs << "#"<<endl;
		}
	}
	fs<<"};"<<endl;
	fs<<"#endif"<<endl<<endl;
	fs.close();
	CString strInfo;
	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateC2SMessage(vector<string>& lstC2SMsgStr,vector<string>& lstS2CMsgStr)
{
	for(int i=0;i<lstC2SMsgStr.size();i++)
	{
		if(lstC2SMsgStr[i].substr(0,3)==SZSCE)
		{
			CString strFileName;
			strFileName.Format("%s//gamed//event//%sHandle.h",m_strRootDir,lstC2SMsgStr[i].c_str());
			CreateC2SFile_H(strFileName,lstC2SMsgStr[i].c_str());
			strFileName.Format("%s//gamed//event//%sHandle.cpp",m_strRootDir,lstC2SMsgStr[i].c_str());
			CreateC2SFile_CPP(strFileName,lstC2SMsgStr[i].c_str());

		}
	}
}
void CProtoToolDlg::CreateC2SFile_H(CString strFileName,CString strMsg,bool bBce /* = true */)
{
	fstream fRead(strFileName.GetBuffer(0),ios::in);
	if(!fRead.fail())
	{
		fRead.close();
		return;
	}
	strMsg.TrimLeft();
	strMsg.TrimRight();
	CString strDeal;
	strDeal.Format("%sHandle",strMsg);

	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	CString strDefine = "_" + strDeal + "_H_";
	strDefine.MakeUpper();
	fs << "#ifndef " << strDefine << ""<<endl;
	fs << "#define " << strDefine << ""<<endl;
	fs<<"#pragma once\n";
	fs<<"//该文件及.cpp文件为使用工具创建，可在文件中修改，工具如果发现该文件，将不再重新创建。"<<endl;
	fs<<"#include \"BaseEvent.h\""<<endl;

	fs<<"class "<<strDeal.GetBuffer(0)<< " : public CBaseEvent"<<endl;
	fs<<"{"<<endl;
	fs<<"public:"<<endl;
	fs<<"\t"<<strDeal.GetBuffer(0)<<"();"<<endl;
	fs<<"\t~"<<strDeal.GetBuffer(0)<<"();"<<endl;
	fs<<"\tBIND_1_EVENT("<<strDeal.GetBuffer(0)<<",C2S_"<<strMsg.GetBuffer(0)<<");"<<endl<<endl;
	fs<<"private:"<<endl;
	fs<<"\tvoid handle(Event* e);"<<endl;
	fs<<"};"<<endl;
	fs << "#endif"<<endl<<endl;
	fs.close();
	CString strInfo;
	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}

void CProtoToolDlg::CreateC2SFile_CPP(CString strFileName,CString strMsg,bool bBce /* = true */)
{
	fstream fRead(strFileName.GetBuffer(0),ios::in);
	if(!fRead.fail())
	{
		fRead.close();
		return;
	}
	strMsg.TrimLeft();
	strMsg.TrimRight();

	CString strMsg1;
	strMsg1.Format("mutable_ce_%s",strMsg);
	strMsg1.MakeLower();

	CString strDeal;
	strDeal.Format("%sHandle",strMsg);
	ofstream fs(strFileName.GetBuffer(0),ios::binary);
	fs<<"#include \"../GameNetHandler.h\""<<endl;
	fs<<"#include \"../GameDataHandler.h\""<<endl;
	fs<<"#include \"../../event/event.pb.h\""<<endl;
	fs<<"#include \"../../common/string-util.h\""<<endl;
	fs<<"#include \"../../logic/User.h\""<<endl;
	fs<<"#include \""<<strDeal.GetBuffer(0)<<".h\""<<endl<<endl<<endl;
	fs<<strDeal.GetBuffer(0)<<"::"<<strDeal.GetBuffer(0)<<"()"<<endl;
	fs<<":CBaseEvent()"<<endl;
	fs<<"{"<<endl<<endl;
	fs<<"}"<<endl;
	fs<<strDeal.GetBuffer(0)<<"::~"<<strDeal.GetBuffer(0)<<"()"<<endl;
	fs<<"{"<<endl<<endl;
	fs<<"}"<<endl<<endl;
	

	fs<<"void "<<strDeal.GetBuffer(0)<<"::handle(Event* e)"<<endl;
	fs<<"{"<<endl<<endl;
	if(!bBce)
	{
		fs<<"\tint64 nUserID = e->uid();"<<endl;
		fs<<"\tGameDataHandler* dh = eh_->getDataHandler();"<<endl;
		fs<<"\tUser* pUser = dh->getUser(nUserID);"<<endl;
		fs<<"\tif(pUser==NULL)"<<endl;
		fs<<"\t\treturn;"<<endl;
		fs<<"\t"<<strMsg.GetBuffer(0)<<"* p"<<strMsg.GetBuffer(0)<<" = e->"<<strMsg1.GetBuffer()<<"();"<<endl;
		fs<<"\t//write code start"<<endl<<endl<<endl<<endl;
		fs<<"\t//write code end"<<endl;
		fs<<"\t//here send proto to flash"<<endl;
		fs<<"\tstring text;"<<endl;
		fs<<"\tp"<<strMsg.GetBuffer(0)<<"->SerializeToString(&text);"<<endl;
		fs<<"\teh_->sendDataToUser(pUser->fd(), S2C_"<<strMsg.GetBuffer(0)<<", text);"<<endl;
	}
    else
    {
        fs<<"\tint64 nUserID = e->uid();"<<endl;
        fs<<"\tGameDataHandler* dh = eh_->getDataHandler();"<<endl;
        fs<<"\tUser* pUser = dh->getUser(nUserID);"<<endl;
        fs<<"\tif(pUser==NULL)"<<endl;
        fs<<"\t\treturn;"<<endl;
        fs<<"\t"<<strMsg.GetBuffer(0)<<"* p"<<strMsg.GetBuffer(0)<<" = e->"<<strMsg1.GetBuffer()<<"();"<<endl;
        fs<<"\tif(p" << strMsg.GetBuffer() <<"==NULL)"<<endl;
        fs<<"\t\treturn;"<<endl;
        fs<<"\tLOG4CXX_INFO(logger_, \""<<strMsg.GetBuffer(0)<< "[u\" << nUserID << \"]\\n\" << p" << strMsg.GetBuffer(0) << "->ShortDebugString());" << endl;
        fs<<endl;
    }

	fs<<"}"<<endl;
	fs.close();
	CString strInfo;
	strInfo.Format("生成%s完毕！",strFileName);
	AddString(strInfo);
}
