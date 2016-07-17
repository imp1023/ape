// tblTool.cpp : Defines the entry point for the console application.
//
#include "DBC.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <string.h>
using namespace std;
void usage();
string GetBaseClassName();
string GetTblClassName();
string GetVarName(DBCFile *pDBCFile, int nField);
void CreateHeadFile();
void CreateSourceFile();
void CreateSystemHeadFile();
void CreateSystemSourceFile();
string strFileName;
string strClassName;
int nSingleIndex = 0;
int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		usage();
		return 1;
	}

	strFileName = argv[1];
	strClassName = argv[2];

	if (argc > 3)
	{
		nSingleIndex = atoi(argv[3]);
	}

	if (strFileName.empty() || strClassName.empty())
	{
		usage();
		return 1;
	}

	CreateHeadFile();
	CreateSourceFile();
	return 0;
}

void usage()
{
	cout << "usage like : tblTool filename classname singleindex!!";
}

string GetBaseClassName()
{
	return "CFG_"+strClassName;
}

string GetTblClassName()
{
	return strClassName + "Tbl";
}

string GetFieldType(DBCFile *pDBCFile, int nField)
{
	if (pDBCFile == NULL)
	{
		return string();
	}

	int nFieldType =  pDBCFile->GetFieldType(nField);
	if (nFieldType  < 0)
	{
		return string();
	}

	string strFieldType;
	if (nFieldType == 0)
	{
		strFieldType = "int";
	}
	else if (nFieldType == 1)
	{
		strFieldType = "float";
	}
	else if (nFieldType == 2)
	{
		strFieldType = "string";
	}

	return strFieldType;
}

string GetFieldName(DBCFile *pDBCFile, int nField)
{
	if (pDBCFile == NULL)
	{
		return string();
	}

	return pDBCFile->GetFieldName(nField);
}

string GetVarName(DBCFile *pDBCFile, int nField)
{
	if (pDBCFile == NULL)
	{
		return string();
	}

	int nFieldType =  pDBCFile->GetFieldType(nField);
	if (nFieldType  < 0)
	{
		return string();
	}

	string strFieldName = pDBCFile->GetFieldName(nField);

	string strVarName;
	if (nFieldType == 0)
	{
		strVarName = "m_n_";
	}
	else if (nFieldType == 1)
	{
		strVarName = "m_f_";
	}
	else if (nFieldType == 2)
	{
		strVarName = "m_str_";
	}

	strVarName += strFieldName;
	transform(strVarName.begin(), strVarName.end(), strVarName.begin(), ::tolower);
	return strVarName;

}

void CreateHeadFile()
{
	string strBaseClassName = GetBaseClassName();
	string strTblClassName = GetTblClassName();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(strFileName.c_str());
	int nCol = fileDBC.GetFieldsNum();

	string strHeadName = strTblClassName + ".h";
	ofstream fileHead;
	fileHead.open(strHeadName.c_str());

	string strUpperTbl = strTblClassName;
	transform(strUpperTbl.begin(), strUpperTbl.end(), strUpperTbl.begin(), ::toupper);

	fileHead << "#ifndef _" << strUpperTbl << "_H_" << "\n";
	fileHead << "#define _" << strUpperTbl << "_H_" << "\n";

	fileHead << "#include <map>" << "\n";
	fileHead << "#include <string>" << "\n";
	fileHead << "#include \"../common/const_def.h\"" << "\n";
	fileHead << "#include \"../common/Singleton.h\""<<"\n\n";

	fileHead << "using namespace std;\n";

	fileHead << "class "<< strBaseClassName << "\n";
	fileHead << "{" << "\n";
	fileHead << "public:" << "\n";
	
	//construct
	fileHead << "\t" << strBaseClassName << "()" << "\n";

	for ( int i = 0; i < nCol; i++ )
	{
		if (GetFieldType(&fileDBC, i) == "int")
		{
			if (i == 0)
			{
				fileHead<<"\t\t:";
			}
			else
			{
				fileHead <<",\n\t\t";
			}
			fileHead<<GetVarName(&fileDBC, i) << "(0)";
		}
		else if (GetFieldType(&fileDBC, i) == "float")
		{
			if (i == 0)
			{
				fileHead<<"\t\t:";
			}
			else
			{
				fileHead <<",\n\t\t";
			}
			fileHead<<GetVarName(&fileDBC, i) << "(0.0)";
		}
		else
		{
			continue;
		}


	}
	fileHead <<"\n\t{};\n";

	fileHead << "\t" << "~" << strBaseClassName << "(){};" << "\n\n";
	
	for ( int i = 0; i < nCol; i++ )
	{
		if (GetFieldType(&fileDBC, i) == "int")
		{
			fileHead << "\t" << GetFieldType(&fileDBC, i) << "\t\t" << "get_" << GetFieldName(&fileDBC, i) << "() const { return " << GetVarName(&fileDBC, i) << "; }" << "\n";
		}
		else
		{
			fileHead << "\t" << GetFieldType(&fileDBC, i) << "\t" << "get_" << GetFieldName(&fileDBC, i) << "() const { return " << GetVarName(&fileDBC, i) << "; }" << "\n";
		}
		
		fileHead << "\t" << "void" << "\t" << "set_" << GetFieldName(&fileDBC, i) << "("<< GetFieldType(&fileDBC, i) <<" val) { " << GetVarName(&fileDBC, i) << " = val; }" << "\n\n";
	}

	fileHead << "protected:\n";
	for ( int i = 0; i < nCol; i++ )
	{
		if (GetFieldType(&fileDBC, i) == "int")
		{
			fileHead << "\t" << GetFieldType(&fileDBC, i) << "\t\t" << GetVarName(&fileDBC, i) << ";\n";
		}
		else
		{
			fileHead << "\t" << GetFieldType(&fileDBC, i) << "\t" << GetVarName(&fileDBC, i) << ";\n";
		}
	}

	fileHead << "};\n\n";

	fileHead << "class " << GetTblClassName() << "\n";
	fileHead << "{\n";
	fileHead << "public:\n";
	fileHead << "\t" << GetTblClassName() << "();" << "\n";
	fileHead << "\t" << "~" << GetTblClassName() << "();" << "\n\n";

	fileHead << "\t" << "void			LoadInfo();" << "\n";
	fileHead << "\t" << "void			Clear();" << "\n\n";

	fileHead << "\t" << strBaseClassName << "*\tGetInfo"<< "(int nIndex);"<< "\n\n";

	fileHead << "protected:\n";

	fileHead << "\t" << "static const char*  szConfigFile;" << "\n";
	fileHead << "\t" << "map<int,"<<GetBaseClassName()<<"*>	m_map"<<GetBaseClassName()<< ";\n";

	fileHead << "};\n\n";

	//typedef Singleton<CopyEquipCardMgr> CopyEquipCardMgrInst;
	fileHead <<"typedef Singleton<"<<GetTblClassName()<<"> "<<GetTblClassName()<<"Inst;\n\n";
	fileHead << "#endif\n\n";

	fileHead.flush();
	fileHead.close();
}

void CreateSourceFile()
{
	string strBaseClassName = GetBaseClassName();
	string strTblClassName = GetTblClassName();

	DBCFile fileDBC(0);
	fileDBC.OpenFromTXT(strFileName.c_str());
	int nCol = fileDBC.GetFieldsNum();

	string strSourceName = strTblClassName + ".cpp";
	ofstream fileSource;
	fileSource.open(strSourceName.c_str());

	fileSource << "#include \""<< GetTblClassName() << ".h\"" << "\n";
	fileSource << "#include \"../common/DBC.h\"" << "\n";
	fileSource << "const char* "<< GetTblClassName() << "::szConfigFile = \"Config/" << strFileName << "\";" << "\n\n";

	fileSource << GetTblClassName() << "::" << GetTblClassName() << "(void)" << "\n";
	fileSource << "{" << "\n";
	fileSource << "}" << "\n\n";

	fileSource << GetTblClassName() << "::" <<"~"<< GetTblClassName() << "(void)" << "\n";
	fileSource << "{" << "\n";
	fileSource << "\tClear();" << "\n";
	fileSource << "}" << "\n\n";

	//loadinfo function
	fileSource << "void " << GetTblClassName() << "::LoadInfo()" << "\n";
	fileSource << "{" << "\n";
	fileSource << "\t" << "Clear();" << "\n\n";
	fileSource << "\t" << "DBCFile fileDBC(0);" << "\n";
	fileSource << "\t" << "if( fileDBC.OpenFromTXT(szConfigFile) == false)" << "\n";
	fileSource << "\t"<<"{\n";
	fileSource << "\t\t"<<"return;\n";
	fileSource << "\t" <<"}\n\n";

	fileSource << "\t" << "int nRow = fileDBC.GetRecordsNum();" << "\n";
	fileSource << "\t" <<"if( nRow <= 0 )\n";
	fileSource << "\t" <<"{\n";
	fileSource << "\t\t"<<"return;\n";
	fileSource << "\t" <<"}\n\n";

	fileSource << "\t" << "for (int i = 0; i < nRow; i++)" << "\n";
	fileSource << "\t" << "{" << "\n";
	fileSource << "\t\t" << GetBaseClassName() <<" *p" << GetBaseClassName() << "= new "<<GetBaseClassName() << ";" << "\n";
	fileSource << "\t\t" << "if (p"<< GetBaseClassName()<<" == NULL)" << "\n";
	fileSource << "\t\t" << "{" << "\n";
	fileSource << "\t\t\t" << "continue;" << "\n";
	fileSource << "\t\t" << "}" << "\n\n";
	for (int i = 0; i < nCol; i++)
	{
		int nFieldType =  fileDBC.GetFieldType(i);
		string strValueName;
		if (nFieldType == 0)
		{
			strValueName = "iValue";
		}
		else if (nFieldType == 1)
		{
			strValueName = "fValue";
		}
		else if (nFieldType == 2)
		{
			strValueName = "pString";
		}
		fileSource << "\t\t" << "p" << GetBaseClassName() << "->set_" << GetFieldName(&fileDBC, i) << "(fileDBC.Search_Posistion(i, "<<i<<")->" << strValueName << ");" << "\n";
	}
	fileSource << "\n\t\t" << "m_map" << GetBaseClassName()<<".insert(make_pair(p"<< GetBaseClassName() << "->get_"<< GetFieldName(&fileDBC,nSingleIndex) << "(), p" <<GetBaseClassName() <<"));" << "\n\n";
	fileSource << "\t" << "}" << "\n";
	fileSource << "\t" << "printf(\"Load %s cnt:%d\\n\", " << GetTblClassName() <<"::szConfigFile" << ", nRow);" << "\n";
	fileSource << "}" << "\n\n";

	//clear function
	fileSource << "void " << GetTblClassName() << "::Clear()" << "\n";
	fileSource << "{" << "\n";
	fileSource << "\tfor (map<int," << GetBaseClassName() << "*>::iterator iter = m_map" << GetBaseClassName() << ".begin(); iter != m_map" << GetBaseClassName() << ".end(); ++iter)" << "\n";
	fileSource << "\t" << "{" << "\n";
	fileSource << "\t\t" << GetBaseClassName() <<" *p"<< GetBaseClassName() << " = (" << GetBaseClassName() <<"*)iter->second;" << "\n";
	fileSource << "\t\t" << "if (p"<< GetBaseClassName()<<" != NULL)" << "\n";
	fileSource << "\t\t" << "{" << "\n";
	fileSource << "\t\t\t" << "delete p" << GetBaseClassName()<<";" << "\n";
	fileSource << "\t\t" << "}" << "\n";
	fileSource << "\t" << "}" << "\n";
	fileSource << "\t" << "m_map" << GetBaseClassName() << ".clear();" << "\n";
	fileSource << "}" << "\n\n";

	//get function
	fileSource << GetBaseClassName() <<"* " << GetTblClassName() << "::GetInfo"<< "(int nIndex)" << "\n";
	fileSource << "{" << "\n";
	fileSource << "\t" << "map<int," << GetBaseClassName() << "*>::iterator iterFind = m_map" << GetBaseClassName() << ".find(nIndex);" << "\n";
	fileSource << "\t" << "if (iterFind != m_map" << GetBaseClassName() << ".end())" << "\n";
	fileSource << "\t" << "{" << "\n";
	fileSource << "\t\t" << "return (" << GetBaseClassName()  << "*)iterFind->second;" << "\n";
	fileSource << "\t" << "}" << "\n";
	fileSource << "\t" << "return NULL;" << "\n";
	fileSource << "}" << "\n\n";

	fileSource.flush();
	fileSource.close();
}

void CreateSystemHeadFile()
{
	string strHeadName = strClassName+"System"+ ".h";
	ofstream fileHead;
	fileHead.open(strHeadName.c_str());

	string strUpperTbl = strClassName+"System";
	string strClassTbl = strUpperTbl;
	string strDBTbl = "DB_" + strClassTbl;
	transform(strUpperTbl.begin(), strUpperTbl.end(), strUpperTbl.begin(), ::toupper);

	fileHead << "#ifndef _" << strUpperTbl << "_H_" << "\n";
	fileHead << "#define _" << strUpperTbl << "_H_" << "\n";

	fileHead << "#include <map>" << "\n";
	fileHead << "#include <string>" << "\n";
	fileHead << "#include \"../common/const_def.h\"" << "\n";
	fileHead << "#include \"../common/LogInclude.h\"\n";
	fileHead << "using namespace std;\n\n";

	fileHead << "class Player;\n";
	fileHead << "class "<< strDBTbl<<";\n\n";

	fileHead << "class "<< strClassTbl << "\n";
	fileHead << "{" << "\n";
	fileHead << "public:" << "\n";

	//construct
	fileHead << "\t" << strClassTbl << "();" << "\n";
	fileHead << "\t" << "~" << strClassTbl << "();" << "\n\n";
	
	fileHead << "\t" <<"void	SetDB( " <<strDBTbl<<" * pDB );\n";
	fileHead << "\t" <<strDBTbl<<"*	GetDB( void ) { return m_pDB;}\n";
	fileHead << "\t" <<"void	SetPlayer( Player *pPlayer ) { m_pPlayer = pPlayer; }\n";
	fileHead << "\t" <<"Player*	GetPlayer( void ) { return m_pPlayer; }\n\n";

	fileHead << "private:\n";
	fileHead << "\t"<<"Player *	m_pPlayer;\n";
	fileHead << "\t"<<strDBTbl<<"*	m_pDB;\n";
	fileHead << "\t"<<"static	log4cxx::LoggerPtr	logger;"<<"\n\n";

	fileHead << "public:\n";
	fileHead << "\t" <<"static const int	nSwitch = XXX;\n";
	fileHead << "};\n\n";

	fileHead << "#endif\n\n";

	fileHead.flush();
	fileHead.close();
}


void CreateSystemSourceFile()
{
	string strSourceName = strClassName+"System"+ ".cpp";
	ofstream fileSource;
	fileSource.open(strSourceName.c_str());

	string strClassTbl = strClassName+"System";
	string strDBTbl = "DB_" + strClassTbl;
	
	fileSource << "#include \"Player.h\"\n";
	fileSource << "#include \"dbinterface.pb.h\"\n";
	fileSource << "#include \""<<strClassTbl <<".h\"" << "\n";
	fileSource << "log4cxx::LoggerPtr "<<strClassTbl<<"::logger = log4cxx::Logger::getLogger(\""<<strClassTbl<<"\");\n\n";

	fileSource <<strClassTbl<<"::"<<strClassTbl<<"()\n";
	fileSource <<"{"<<"\n";
	fileSource <<"}"<<"\n\n";

	fileSource <<strClassTbl<<"::~"<<strClassTbl<<"()\n";
	fileSource <<"{"<<"\n";
	fileSource <<"}"<<"\n\n";

	fileSource <<"void "<<strClassTbl<<"::SetDB( "<<strDBTbl<<" *pDB )\n";
	fileSource <<"{"<<"\n";
	fileSource <<"\t"<<"m_pDB = pDB;\n";
	fileSource <<"}"<<"\n\n";

	fileSource.flush();
	fileSource.close();
}
