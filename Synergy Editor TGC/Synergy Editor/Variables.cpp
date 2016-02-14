#include "StdAfx.h"
#include "Variables.h"
#include "Utilities.h"
#include "Settings.h"

CString Variables::m_ProjectName;
CString Variables::m_ProjectPath;
CString Variables::m_ProjectFile;
CString Variables::m_BackupLocation;

CString Variables::m_DBPropertyScreenType;
CString Variables::m_DBPropertyTitle;
CString Variables::m_DBPropertyIcon;
CString Variables::m_DBPropertyExe;

CString Variables::m_DBPropertyFullResolution;
CString Variables::m_DBPropertyWindowResolution;
bool Variables::m_DBPropertyCompressMedia;
bool Variables::m_DBPropertyEncryptMedia;
CString Variables::m_DBPropertyFileVersion;
CString Variables::m_DBPropertyInternalVersion;
CString Variables::m_DBPropertyBuildType;
CString Variables::m_DBPropertyFileComments;
CString Variables::m_DBPropertyFileCompany;
CString Variables::m_DBPropertyFileDescription;
CString Variables::m_DBPropertyFileCopyright;

int Variables::m_NumberOfLines;
std::vector<int> Variables::Lines;

// Each file stored by the full path!
std::vector <SourceFile*> Variables::m_IncludeFiles;
std::vector <CString> Variables::m_DBPropertyMedia;
std::vector <CString> Variables::m_DBPropertyCursors;
std::vector <CString> Variables::m_ToDo;
std::vector <CString> Variables::m_Comments;

bool Variables::m_ProjectChanged;

CString Variables::m_CommandLineArgs;

// Flag to indicate that file saving should use the ! symbol as we are saving
bool Variables::m_SavingBecauseStep;
bool Variables::m_SavingBecauseDebug;

int Variables::m_OpenFileLine;

void Variables::Init()
{
	Variables::m_ProjectName = Settings::m_DBPropertyTitle;
	Variables::m_ProjectPath = Utilities::GetTemporaryPath();
	Variables::m_ProjectFile = _T("");


	Variables::m_DBPropertyScreenType = Settings::m_DBPropertyScreenType;
	Variables::m_DBPropertyTitle = Settings::m_DBPropertyTitle;
	Variables::m_DBPropertyIcon = Settings::m_DBPropertyIcon;
	Variables::m_DBPropertyExe = Settings::m_DBPropertyExe;

	Variables::m_DBPropertyFullResolution = Settings::m_DBPropertyFullResolution;
	Variables::m_DBPropertyWindowResolution = Settings::m_DBPropertyWindowResolution;
	Variables::m_DBPropertyCompressMedia = Settings::m_DBPropertyCompressMedia;
	Variables::m_DBPropertyEncryptMedia = Settings::m_DBPropertyEncryptMedia;
	Variables::m_DBPropertyFileVersion = Settings::m_DBPropertyFileVersion;
	Variables::m_DBPropertyInternalVersion = Settings::m_DBPropertyInternalVersion;
	Variables::m_DBPropertyBuildType = Settings::m_DBPropertyBuildType;
	Variables::m_DBPropertyFileComments = Settings::m_DBPropertyFileComments;
	Variables::m_DBPropertyFileCompany = Settings::m_DBPropertyFileCompany;
	Variables::m_DBPropertyFileDescription = Settings::m_DBPropertyFileDescription;	
	Variables::m_DBPropertyFileCopyright = Settings::m_DBPropertyFileCopyright;	

	Variables::m_CommandLineArgs = _T("");

	ClearIncludes();
	Variables::m_DBPropertyMedia.clear();
	Variables::m_ToDo.clear();
	Variables::m_Comments.clear();
	Variables::m_DBPropertyCursors.clear();

	Variables::m_ProjectChanged = false;
}

// Clears the include vector (and deletes the memory used in each one)
void Variables::ClearIncludes()
{
	std::vector<SourceFile*>::iterator mySFVectorIterator;
	for(mySFVectorIterator = m_IncludeFiles.begin(); mySFVectorIterator != m_IncludeFiles.end(); mySFVectorIterator++)
	{
		delete *mySFVectorIterator;
	}
	Variables::m_IncludeFiles.clear();
}

// Removes the include file from the vector
void Variables::RemoveInclude(CString old)
{
	if(Variables::m_IncludeFiles.size() == 0)
	{
		return;
	}

	CString path;
	if(old == Utilities::ExtractFilename(old))
	{
		path = m_ProjectPath + old;
	}
	else
	{
		path = old;
	}

	std::vector <SourceFile *>::iterator Iter;
	for ( Iter = Variables::m_IncludeFiles.begin( ) ; Iter != Variables::m_IncludeFiles.end( ) ; Iter++ )
	{
		if((*Iter)->filename == path)
		{
			m_ProjectChanged = true;
			delete *Iter;
			Variables::m_IncludeFiles.erase(Iter);
			break;
		}		
	}
}

// Add's a new include file to the list
void Variables::AddInclude(CString file)
{
	m_ProjectChanged = true;
	m_IncludeFiles.push_back(new SourceFile(file, -1));
}

// Checks whether the specified include file exists already in the vector
int Variables::IncludeExists(CString path)
{
	std::vector <SourceFile*>::iterator Iter;
	int pos = 0;
	for ( Iter = Variables::m_IncludeFiles.begin( ) ; Iter != Variables::m_IncludeFiles.end( ) ; Iter++ )
	{
		if((*Iter)->filename == path)
		{
			return pos;
		}	
		pos++;
	}
	return -1;
}

// Changes the include file at the specified index
void Variables::UpdateInclude(int index, CString newFile)
{
	ASSERT(index > -1 && index < (int)Variables::m_IncludeFiles.size());
	SourceFile *sf = Variables::m_IncludeFiles.at(index);
	sf->filename = newFile;
}

// Sets the include as the main include
void Variables::MakeIncludeMain(CString path)
{
	if(Variables::m_IncludeFiles.size() < 2)
	{
		return;
	}
	
	int replaced = IncludeExists(path);
	if(replaced == -1)
	{
		return;
	}

	SourceFile* oldMain = Variables::m_IncludeFiles.at(0);
	Variables::m_IncludeFiles.at(0) = Variables::m_IncludeFiles.at(replaced);
	Variables::m_IncludeFiles.at(replaced) = oldMain;

	m_ProjectChanged = true;
}

// Removes the media item from the vector
void Variables::RemoveMedia(CString old)
{
	std::vector <CString>::iterator Iter;
	for ( Iter = Variables::m_DBPropertyMedia.begin( ) ; Iter != Variables::m_DBPropertyMedia.end( ) ; Iter++ )
	{
		if(*Iter == old)
		{
			m_ProjectChanged = true;
			Variables::m_DBPropertyMedia.erase(Iter);
			break;
		}			
	}
}

// Adds a media item to the vector
void Variables::AddMedia(CString item)
{
	m_ProjectChanged = true;
	Variables::m_DBPropertyMedia.push_back(item);
}

// Checks whether the specified media file exists already in the vector
int Variables::MediaExists(CString file)
{
	std::vector <CString>::iterator Iter;
	int i = 0;
	for ( Iter = Variables::m_DBPropertyMedia.begin( ) ; Iter != Variables::m_DBPropertyMedia.end( ) ; Iter++ )
	{
		if(*Iter == file)
		{
			return i;
		}
		i++;
	}
	return -1;
}

// Removes the cursor item from the vector
void Variables::RemoveCursor(CString old)
{
	std::vector <CString>::iterator Iter;
	for ( Iter = Variables::m_DBPropertyCursors.begin( ) ; Iter != Variables::m_DBPropertyCursors.end( ) ; Iter++ )
	{
		if(*Iter == old)
		{
			m_ProjectChanged = true;
			Variables::m_DBPropertyCursors.erase(Iter);
			break;
		}			
	}
}

// Adds a media item to the vector
void Variables::AddCursor(CString item)
{
	m_ProjectChanged = true;
	Variables::m_DBPropertyCursors.push_back(item);
}

// Checks whether the specified cursor file exists already in the vector
int Variables::CursorExists(CString file)
{
	std::vector <CString>::iterator Iter;
	int i = 0;
	for ( Iter = Variables::m_DBPropertyCursors.begin( ) ; Iter != Variables::m_DBPropertyCursors.end( ) ; Iter++ )
	{
		if(*Iter == file)
		{
			return i;
		}
		i++;
	}
	return -1;
}

// Add a new to do to the vector (includes ticked flag)
void Variables::AddToDo(CString item)
{
	m_ProjectChanged = true;
	Variables::m_ToDo.push_back(item);
}

// Remove an existing to do item (includes ticked flag)
void Variables::RemoveToDo(CString old)
{
	std::vector <CString>::iterator Iter;
	for ( Iter = Variables::m_ToDo.begin( ) ; Iter != Variables::m_ToDo.end( ) ; Iter++ )
	{
		if((*Iter).Mid(1) == old)
		{
			m_ProjectChanged = true;
			Variables::m_ToDo.erase(Iter);
			break;
		}			
	}
}

// Changes the to do item's checked state
void Variables::ChangeToDo(CString item, BOOL Checked)
{
	CString flag = Checked ? _T("1") : _T("0");
	std::vector <CString>::iterator Iter;
	for ( Iter = Variables::m_ToDo.begin( ) ; Iter != Variables::m_ToDo.end( ) ; Iter++ )
	{
		if((*Iter).Mid(1) == item)
		{
			m_ProjectChanged = true;
			*Iter = flag + item;
			break;
		}
	}
}

// Renames the to do (both names do not include ticked flag)
void Variables::RenameToDo(CString oldToDo, CString newToDo)
{
	std::vector <CString>::iterator Iter;
	for ( Iter = Variables::m_ToDo.begin( ) ; Iter != Variables::m_ToDo.end( ) ; Iter++ )
	{
		if((*Iter).Mid(1) == oldToDo)
		{
			m_ProjectChanged = true;
			*Iter = (*Iter).Mid(0, 1) + newToDo;
			break;
		}
	}
}

// Moves a to do up the list
void Variables::MoveToDoUp(CString item)
{
	for(UINT i=1; i < Variables::m_ToDo.size(); i++)
	{
		CString todo = Variables::m_ToDo.at(i);
		if(todo.Mid(1) == item)
		{
			m_ProjectChanged = true;
			CString old = Variables::m_ToDo.at(i - 1);
			Variables::m_ToDo.at(i - 1) = Variables::m_ToDo.at(i);
			Variables::m_ToDo.at(i) = old;
			break;
		}
	}
}

// Moves a to do down the list
void Variables::MoveToDoDown(CString item)
{
	for(UINT i=0; i < (Variables::m_ToDo.size() - 1); i++)
	{
		CString todo = Variables::m_ToDo.at(i);
		if(todo.Mid(1) == item)
		{
			m_ProjectChanged = true;
			CString old = Variables::m_ToDo.at(i + 1);
			Variables::m_ToDo.at(i + 1) = Variables::m_ToDo.at(i);
			Variables::m_ToDo.at(i) = old;
			break;
		}
	}
}

// Adds a comment line to the comment vector
void Variables::AddComment(CString item)
{
	m_ProjectChanged = true;
	Variables::m_Comments.push_back(item);
}

// Empties the comment vector
void Variables::ClearComment()
{
	m_ProjectChanged = true;
	Variables::m_Comments.clear();
}
