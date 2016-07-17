#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "windows.h"

#include "xml/tinyxml.h"

#define str_eq(a, b) (::strcmp((a), (b))==0)

TiXmlElement* searchElement(TiXmlElement* root, const char* type, const char* name ){                                     
    if (! root)
        return NULL;

    for (TiXmlElement* element = root->FirstChildElement(); element != NULL; element = element->NextSiblingElement()) {
        const char* v = element->Value();
        const char* e = element->Attribute("Name");
        if (str_eq(v, type) && str_eq(e, name)) {
            return element;
        }
        TiXmlElement* child_found = searchElement(element, type, name);
        if (child_found)
            return child_found;
    }

    return NULL;
}

typedef bool (*SELECT_FILE)(const char*);

vector<string> listFile(const char* path, SELECT_FILE selectFunc)
{
    vector<string> files;
    
    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind=INVALID_HANDLE_VALUE;    

    hFind=FindFirstFile(path,&FindFileData);

    if(hFind==INVALID_HANDLE_VALUE)
    {  
        cout << "list file [" << path << "] failed." << endl;
        FindClose(hFind);   
        return files;    
    }  

    while(FindNextFile(hFind,&FindFileData)!=0)
    {  
        if(str_eq(FindFileData.cFileName,"..") || str_eq(FindFileData.cFileName, ".")) 
        {  
            continue;  
        }  

        if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }  

        if (! selectFunc(FindFileData.cFileName))
        {
            continue;
        }

        files.push_back(FindFileData.cFileName);

    }  
    FindClose(hFind);  


    return files;
}

int UpdateFile(const char* fileName,
                const char* type,
                const char* name,
                const char* dir,
                const char* path,
                SELECT_FILE selectFunc)
{
    TiXmlDocument doc(fileName);
    bool loadOk = doc.LoadFile();
    if (! loadOk) {
        cout << "load file [" << fileName << "] failed." << endl;
        return -1;
    }

    TiXmlElement* root = doc.RootElement();
    TiXmlElement* auto_event = searchElement(root, type, name);
    if (!auto_event) {
        cout << "cannot found auto_event node." << endl;
        return -1;
    }
    auto_event->Clear();


    vector<string> files = listFile(dir, selectFunc);
    for (size_t i = 0; i < files.size(); i++)
    {
        TiXmlElement* file_element = new TiXmlElement("File");
        string relativePath = path + files[i];
        file_element->SetAttribute("RelativePath", relativePath.c_str());
        auto_event->LinkEndChild(file_element);
    }


    doc.SaveFile(fileName);
    return 0;
}

bool selectCoreFile(const char* fileName)
{
    string strFileName(fileName);
    return strFileName.find("pb.h") != string::npos
        || strFileName.find("pb.cpp") != string::npos;
}



int updateCore() {
    const char* dir = "";


    return UpdateFile("core.vcproj",
                      "Filter",
                      "event_auto",
                      "event\\*",
                      ".\\event\\",
                      selectCoreFile);
}

bool selectGameFile(const char* fileName)
{
    string strFileName(fileName);
    return strFileName.find("DealDce") == 0;
}

int updateGame() {

    return UpdateFile("gamed.vcproj",
                      "Filter",
                      "event_auto_handle",
                      "gamed\\event\\*",
                      ".\\gamed\\event\\",
                      selectGameFile);

    return 0;
}

int main() {        
    
    int result = updateCore();
    if (result != 0)
        return -1;

    result = updateGame();
    if (result != 0)
        return -1;

    cout << "success" << endl;
    return 0;
}