#pragma once

#include <vector>
#include "View.h"
#include "Doc.h"

class SemanticMessage
{
public:
	CString file;
	CString message;
	int line;

	SemanticMessage(CString newFile, CString newMessage, int newLine)
	{
		file = newFile;
		message = newMessage;
		line = newLine;
	}
};

class SemanticFunction
{
public:
	CString file;
	CString function;
	CString definition;
	int line;

	SemanticFunction(CString newFile, CString newFunction, CString newDefintion, int newLine)
	{
		file = newFile;
		function = newFunction;
		definition = newDefintion;
		line = newLine;
	}
};

class SemanticVariable
{
public:
	CString file;
	CString variable;
	CString match; // uppercase
	CString help; // Full variable definition
	int line;
	int endline;
	CString type; // Variable type specified by the as keyword
	CString typeMatch; // Variable type specified by the as keyword in uppercase

	// Standard Constructor
	SemanticVariable(CString newFile, CString newVariable, CString newMatch, CString newHelp, int newLine, CString newType = _T(""))
	{
		file = newFile;
		variable = newVariable;
		match = newMatch;
		line = newLine;
		help = newHelp;
		endline = -1;
		type = newType;
		typeMatch = newType;
		typeMatch.MakeUpper();
	}

	// Type variable constructor
	SemanticVariable(CString newFile, CString newVariable, int newLine, CString newType)
	{
		file = newFile;
		variable = newVariable;
		match = newVariable;
		match.MakeUpper();
		help = newVariable + _T(" (Type Argument)");
		line = newLine;
		endline = -1;
		type = newType;
		typeMatch = newType;
		typeMatch.MakeUpper();
	}

	// Function argument constructor
	SemanticVariable(CString newFile, CString newVariable, int newLine)
	{
		file = newFile;
		variable = newVariable;
		match = newVariable;
		match.MakeUpper();
		help = newVariable + _T(" (Function Argument)");
		line = newLine;
		endline = -1;
	}
};

class SemanticType
{
public:
	CString file; // The owning file
	CString type; // The type
	CString match; // uppercase
	int line; // The line
	std::vector<SemanticVariable*> variables; // The variables

	SemanticType(CString newFile, CString newType, CString newMatch, int newLine)
	{
		file = newFile;
		type = newType;
		match = newMatch;
		line = newLine;
	}
};

class SemanticLabel
{
public:
	CString file;
	CString label;
	CString match; // uppercase
	int line;

	SemanticLabel(CString newFile, CString newLabel, CString newMatch, int newLine)
	{
		file = newFile;
		label = newLabel;
		match = newMatch;
		line = newLine;
	}
};

struct SemanticData
{
	CString Text;
	CString Filename;
	int mode; // 0=Classview; 1=Msgbox
};

class SemanticParser
{
public:
	const enum MODE { Classview, Msgbox };

	static UINT ParseThread(LPVOID pParam);
	static void CleanUp(bool ignoreLock);
	static void SetClassView(HWND view);

	static std::vector<SemanticMessage*> m_vMessages;
	static std::vector<SemanticFunction*> m_vFunctions;
	static std::vector<SemanticType*> m_vTypes;
	static std::vector<SemanticVariable*> m_vVariables;
	static std::vector<SemanticVariable*> m_vConstants;
	static std::vector<SemanticLabel*> m_vLabels;

	static std::vector<SemanticMessage*>::iterator mySEVectorIterator; // Errors
	static std::vector<SemanticFunction*>::iterator mySFVectorIterator; // Functions
	static std::vector<SemanticType*>::iterator mySTVectorIterator; // Types
	static std::vector<SemanticLabel*>::iterator mySLVectorIterator; // Labels
	static std::vector<SemanticVariable*>::iterator mySVVectorIterator; // Variables
	static std::vector<SemanticVariable*>::iterator mySCVectorIterator; // Constants

	static HWND m_ClassView;

	static bool kill;
	static CMutex m_Mutex;	
	static CMutex m_Running;

    // IRM 20090910 - Storage for the current function name (if in a function)
    static CString currentFunction;

	static CString pathName;
	static int mode;
	static std::vector<CString> messages;
	static int lineNumber;
	static std::vector<SemanticVariable*> vTempVariables;
	static bool inFunction;

	static SemanticType* currentType;
private:
	static void HandleGlobal(CString &line, CString &lineUpper, int offset, bool local, bool isConstant);
	static void HandleDim(CString &line, CString &lineUpper, int offset);
    static void HandleFunctionArgs(CString &line, int start, int end);
	static bool IsLabel(CString &line);
	static void HandleTypeVariable(CString &line, CString &lineUpper);
};
