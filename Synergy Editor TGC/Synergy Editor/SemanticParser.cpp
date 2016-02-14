#include "StdAfx.h"
#include "SemanticParser.h"

// IRM 20090920 - Need to check if a label is actually a keyword
#include "Keywords.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<SemanticMessage*> SemanticParser::m_vMessages;
std::vector<SemanticFunction*> SemanticParser::m_vFunctions;
std::vector<SemanticType*> SemanticParser::m_vTypes;
std::vector<SemanticVariable*> SemanticParser::m_vVariables;
std::vector<SemanticVariable*> SemanticParser::m_vConstants;
std::vector<SemanticLabel*> SemanticParser::m_vLabels;

std::vector<SemanticMessage*>::iterator SemanticParser::mySEVectorIterator; // Errors
std::vector<SemanticFunction*>::iterator SemanticParser::mySFVectorIterator; // Functions
std::vector<SemanticType*>::iterator SemanticParser::mySTVectorIterator; // Types
std::vector<SemanticLabel*>::iterator SemanticParser::mySLVectorIterator; // Labels
std::vector<SemanticVariable*>::iterator SemanticParser::mySVVectorIterator; // Variables
std::vector<SemanticVariable*>::iterator SemanticParser::mySCVectorIterator; // Constants

HWND SemanticParser::m_ClassView;
CMutex SemanticParser::m_Mutex;
CMutex SemanticParser::m_Running;
bool SemanticParser::kill;

// IRM 20090910 - Storage for the current function name (if in a function)
CString SemanticParser::currentFunction;

CString SemanticParser::pathName;
int SemanticParser::mode;
std::vector<CString> SemanticParser::messages;
int SemanticParser::lineNumber;
std::vector<SemanticVariable*> SemanticParser::vTempVariables; // Used by endfunction to close variable definitions
bool SemanticParser::inFunction; // Only add temp variables if we are in a function

SemanticType* SemanticParser::currentType; // The current type

// Gives the class the class view hwnd
void SemanticParser::SetClassView(HWND hwnd)
{
	m_ClassView = hwnd;
	kill = false;
}

// Parsers the passed text
UINT SemanticParser::ParseThread(LPVOID pParam)
{
	SemanticData* sd = (SemanticData*)pParam;

	int copyLength = sd->Text.GetLength() + 1;
	TCHAR *buffer = new TCHAR[copyLength];
	wcscpy_s(buffer, copyLength, sd->Text.GetBuffer());
	CString text = buffer;
	delete buffer;

	pathName = sd->Filename;
	mode = sd->mode;
	delete sd;

	if(!m_Mutex.Lock(10))
	{
		return 0;
	}

	if(mode == 0)
	{
		CleanUp(true); // Empty vectors
	}
	else
	{
		messages.clear(); // Empty message vector
	}

	bool inString = false, justInComment = false, inComment = false, inBlockComment = false;
	int bracketBalance = 0;
	lineNumber = 0;
	int linePos = 0;

	CString line, lineUpper;

	TCHAR ch, pch = '\n';

	int length = text.GetLength();
	for(int i=0; i < length; i++)
	{
		if(kill)
		{
			break;
		}

		ch = text.GetAt(i);

		if(ch != '\n')
		{
			line.AppendChar(ch);
			lineUpper.AppendChar((TCHAR)toupper(ch));
		}

		if(!inString && !inComment && !inBlockComment)
		{
			if(linePos > 2 && lineUpper.Mid(linePos - 3, 3) == _T("REM"))
			{
				inComment = true;
			}
			else if((linePos > 1 && lineUpper.Mid(linePos - 2, 2) == _T("//"))) // This comment does not need a preceeding newline or :
			{
				if(line.GetLength() > 2)
				{
					justInComment = true;
				}
				else
				{
					inComment = true;
				}
			}
			else if(ch == '`') // This comment does not need a preceeding newline or :
			{
				if(line.GetLength() > 1)
				{
					justInComment = true;
				}
				else
				{
					inComment = true;
				}
			}
			else if(ch == '(')
			{
				bracketBalance++;
			}
			else if(ch == ')')
			{
				bracketBalance--;
			}
			else if(ch == '{' || ch == '}' || ch == '@')
			{
				if(mode == 0)
				{
					m_vMessages.push_back(new SemanticMessage(pathName, _T("'") + text.Mid(i,1) + _T("' invalid character"), lineNumber));										
				}
				else
				{
					messages.push_back(_T("'") + text.Mid(i,1) + _T("' invalid character"));						
				}
			}
		}

		if((ch == '\n' || justInComment || ch == ':') && !inString && !inComment)
		{
			int lineLength = line.GetLength();
			CString trimmedUpper(lineUpper);
			trimmedUpper.TrimLeft();
			int offset = lineUpper.Find(trimmedUpper, 0);
			if(lineLength >5 && wcsncmp(trimmedUpper, _T("REMEND"), 6) == 0)
			{
				inBlockComment = false;
			}
			if(!inBlockComment)
			{
				if(justInComment) // Give the line chance to be analysed if it ended in a comment
				{
					inComment = true;
					justInComment = false;
				}
				if(inString)
				{
					if(mode == 0)
					{
						m_vMessages.push_back(new SemanticMessage(pathName, _T("Unterminated string"), lineNumber));										
					}
					else
					{
						messages.push_back(_T("Unterminated string"));						
					}
				}
				else if(ch == ':' && IsLabel(line))
				{
					int trimmedLength = trimmedUpper.GetLength() - 1;
					bool pass = true;
					for(int i = 0; i < trimmedLength; i++)
					{
						TCHAR test = trimmedUpper.GetAt(i);
						if(!isalpha(test) && !isdigit(test) && test != '_')
						{
							pass = false;
							break;
						}
					}
					if(pass)
					{
						m_vLabels.push_back(new SemanticLabel(pathName, line.Mid(offset, (lineLength - 1) - offset), lineUpper.Mid(offset, (lineLength - 1) - offset), lineNumber));
					}
				}
				else if(lineLength > 10 && wcsncmp(trimmedUpper, _T("ENDFUNCTION"), 11) == 0)
				{
					for(UINT endf=0; endf < vTempVariables.size(); endf++)
					{
						vTempVariables.at(endf)->endline = lineNumber; // Variable has gone out of scope
					}
					vTempVariables.clear();
					inFunction = false;
				}
				else if(lineLength > 8 && wcsncmp(trimmedUpper, _T("FUNCTION "), 9) == 0)
				{
					int pos = -1, posEnd = line.GetLength(), varStart = -1;
					for(int func=(offset + 9); func<lineLength; func++)
					{
						TCHAR funcChr = line.GetAt(func);
						if(pos == -1 && (funcChr == '(' || funcChr == ' '))
						{
							pos = func;
						}
						else if(funcChr == ')')
						{
							posEnd = func+1;
							break;
						}

						if(varStart == -1 && funcChr == '(') // Look for first variable
						{
							for(int temp = (func + 1); temp < lineLength; temp++)
							{
								TCHAR tempChr = line.GetAt(temp);
								if(isalpha(tempChr) || tempChr == '_')
								{
									varStart = temp;
									break;
								}
							}
							
						}
					}
					if(pos > -1)
					{
						if(mode == Classview)
						{
                            // IRM 20090910 - Store the function name, for use when naming local variables
                            currentFunction = line.Mid((offset + 9), pos - (offset + 9));

							m_vFunctions.push_back(new SemanticFunction(pathName, lineUpper.Mid((offset + 9), pos - (offset + 9)), line.Mid((offset + 9), posEnd - (offset + 9)), lineNumber));
							inFunction = true;

							// Try and find variable names
							if(varStart > -1)
							{
								HandleFunctionArgs(line, varStart, posEnd);
							}
						}
					}
					else
					{
						if(mode == Classview)
						{
							m_vMessages.push_back(new SemanticMessage(pathName, _T("Incomplete function definition"), lineNumber));
						}
						else
						{
							messages.push_back(_T("Incomplete function definition"));
						}
					}
				}
				else if(lineLength > 4 && wcsncmp(trimmedUpper, _T("TYPE "), 5) == 0)
				{
					if(mode == Classview)
					{
						SemanticType* type = new SemanticType(pathName, line.Mid(offset + 5), lineUpper.Mid(offset + 5), lineNumber);
						currentType = type;
						m_vTypes.push_back(type);
					}
				}
				else if(lineLength > 6 && wcsncmp(trimmedUpper, _T("ENDTYPE"), 7) == 0)
				{
					currentType = NULL;
				}
				//else if(currentType != NULL) // If we are in a type then handle the type definitions
				//{
				//	HandleTypeVariable(line, lineUpper);
				//}
				else if(lineLength > 6 && wcsncmp(trimmedUpper, _T("GLOBAL "), 7) == 0)
				{
					HandleGlobal(line, lineUpper, offset + 7, false, false);
				}
				else if(lineLength > 9 && wcsncmp(trimmedUpper,_T("#CONSTANT "), 10) == 0)
				{
					HandleGlobal(line, lineUpper, offset + 10, false, true);
				}
				else if(lineLength > 5 && wcsncmp(trimmedUpper, _T("LOCAL "), 6) == 0)
				{
					HandleGlobal(line, lineUpper, offset + 6, true, false);
				}
				else if(lineLength > 3 && wcsncmp(trimmedUpper, _T("DIM "), 4) == 0)
				{
					HandleDim(line, lineUpper, offset + 4);
				}
				else if(lineLength > 7 && wcsncmp(trimmedUpper, _T("REMSTART"), 8) == 0)
				{
					inBlockComment = true;
				}
				else if(bracketBalance != 0)
				{
					CString bracketError;
					bracketError.Format(_T("Bracket inbalance (%d)"), lineNumber);
					if(mode == 0)
					{
						m_vMessages.push_back(new SemanticMessage(pathName, bracketError, lineNumber));
					}
					else
					{
						messages.push_back(bracketError);
					}
				}
			}
			line.Empty(); // Reset line variable
			bracketBalance = 0; // Reset bracket balance
			lineUpper.Empty();			
		}
		else if(ch == '"')
		{
			inString = !inString;
		}
		linePos++;
		if(ch == '\n')
		{
			inComment = false; // End of line, so quit the line comment
			inString = false; // End of line, so can't be in a string
			line.Empty(); // Reset line variable
			bracketBalance = 0; // Reset bracket balance
			lineUpper.Empty();			
			lineNumber++;
			linePos = 0;
		}
		pch = ch;
	}

	m_Mutex.Unlock();

	if(!kill)
	{
		if(mode == 0)
		{
			SendMessage(m_ClassView, WM_USER, 0, 0); // Update
		}
		else if(messages.size() > 0) // mode = 1
		{
			CString errorlist;
			for(UINT err=0; err<messages.size();err++)
			{
				errorlist.Append(messages.at(err) + _T("\n"));
			}	
			errorlist.Append(_T("\nThe error(s) shown above were detected on the line you have just completed"));
			TCHAR *message = new TCHAR[errorlist.GetLength() + 1];
			wcscpy_s(message, errorlist.GetLength() + 1, errorlist.GetBuffer());
			::SendMessage(SemanticParser::m_ClassView, WM_USER + 2, 0, (LPARAM)message);
		}
	}
	vTempVariables.clear();

	m_Running.Unlock();

	return 0;
}

// Handles the variables within types
void SemanticParser::HandleTypeVariable(CString &line, CString &lineUpper)
{
	line.Trim();
	lineUpper.Trim();
	int pos = line.Find(' ');
	if(pos == -1) // Just a variable
	{
		SemanticVariable* sm = new SemanticVariable(pathName, line, lineNumber);
		currentType->variables.push_back(sm);
	}
	else
	{
		CString type = _T("");

		CString name = line.Mid(0, pos); // Extract the name
		line = line.Mid(pos + 1); // Go to the beginning of the next word
		lineUpper = lineUpper.Mid(pos + 1);
		if(lineUpper.Find(_T("AS")) == 0)
		{
			line = line.Mid(3); // Go to the beginning of the next word
			lineUpper = lineUpper.Mid(3);

			type = line;
		}

		SemanticVariable* sm = new SemanticVariable(pathName, name, lineNumber, type);
		currentType->variables.push_back(sm);
	}
}

// Handles the arguments in a function and turns them into variables
void SemanticParser::HandleFunctionArgs(CString &line, int start, int end)
{
	bool allowed = true;
	bool stored = false;
	bool ignoreSpace = false;
	CString VarName;
	for(int var=start; var<end; var++)
	{
		TCHAR varChr = line.GetAt(var);
		if(allowed && (isdigit(varChr) || isalpha(varChr) || varChr == '#' || varChr == '$' || varChr == '_'))
		{
			VarName.AppendChar(varChr);
			stored = false;
			ignoreSpace = false;
		}
		else if(varChr == ' ')
		{
			if(!stored)
			{
                // IRM 20090910 - Prefix functions arguments with the function name to differentiate them
                //                from other variables.
                VarName = currentFunction + _T(".") + VarName;
				SemanticVariable* sm = new SemanticVariable(pathName, VarName, lineNumber);
				m_vVariables.push_back(sm);
				vTempVariables.push_back(sm);
				VarName = _T("");
				stored = true;
			}
			if(!ignoreSpace)
			{
				allowed = false;
			}
		}
		else if(varChr == ',')
		{
			if(!stored)
			{
                // IRM 20090910 - Prefix functions arguments with the function name to differentiate them
                //                from other variables.
                VarName = currentFunction + _T(".") + VarName;
				SemanticVariable* sm = new SemanticVariable(pathName, VarName, lineNumber);
				m_vVariables.push_back(sm);
				vTempVariables.push_back(sm);
				VarName = _T("");
				stored = true;
			}										
			allowed = true;
			ignoreSpace = true;
		}
	}
	if(!stored && VarName.GetLength() > 0)
	{
        // IRM 20090910 - Prefix functions arguments with the function name to differentiate them
        //                from other variables.
        VarName = currentFunction + _T(".") + VarName;
		SemanticVariable* sm = new SemanticVariable(pathName, VarName, lineNumber);
		m_vVariables.push_back(sm);
		vTempVariables.push_back(sm);
	}
}

// Handles a dim statement
void SemanticParser::HandleDim(CString &line, CString &lineUpper, int offset)
{
	int lineLength = line.GetLength();
	int posEnd = lineLength;
	TCHAR varChr;
	int var;
	for(var=offset; var<lineLength; var++)
	{
		varChr = line.GetAt(var);
		//if(!isdigit(varChr) && !isalpha(varChr) && varChr != '#' && varChr != '$' && varChr != '_')
		if(var == lineLength || varChr == ' ' || varChr == ',' || varChr == '=' || varChr == '(')
		{
			posEnd = var;
			break;
		}
	}
	if(posEnd == offset)
	{
		if(mode == 0)
		{
			m_vMessages.push_back(new SemanticMessage(pathName, _T("Incomplete variable/array definition"), lineNumber));
		}
		else
		{
			messages.push_back(_T("Incomplete variable/array definition"));
		}
		return;
	}
	CString word = line.Mid(offset, posEnd - offset);
	CString upperWord = lineUpper.Mid(offset, posEnd - offset);
	CString help = line.Mid(offset - 4);
	if(mode == 0)
	{
		CString type = _T("");
		if(lineUpper.Mid(posEnd + 1, 2) == _T("AS")) // Variable is being declared as something
		{
			type = line.Mid(posEnd + 4);
		}

		SemanticVariable *sm = new SemanticVariable(pathName, word, upperWord, help, lineNumber, type);
		m_vVariables.push_back(sm);
		if(inFunction)
		{
			vTempVariables.push_back(sm);
		}
	}
}

// Handles a Global [dim] or Local [dim] statement
void SemanticParser::HandleGlobal(CString &line, CString &lineUpper, int offset, bool local, bool isConstant)
{
    // IRM 20090910 - Simplify test for prepending function name to the variable name
    bool addFunctionName = local && (!isConstant) && inFunction;

	int lineLength = line.GetLength();
	int posEnd = lineLength;
	TCHAR varChr;
	int var; // for loop variable [index]
	// Find the end of the word
	for(var=offset; var<lineLength; var++)
	{
		varChr = line.GetAt(var);
		if(var == lineLength || varChr == ' ' || varChr == ',' || varChr == '=')
		{
			posEnd = var;
			break;
		}
	}
	if(posEnd == offset) // Can't find name
	{
		if(mode == 0)
		{
			m_vMessages.push_back(new SemanticMessage(pathName, _T("Incomplete variable definition"), lineNumber));
		}
		else
		{
			messages.push_back(_T("Incomplete variable definition"));
		}
		return;
	}
	// Extract variable name [or possibly DIM]
	CString word = line.Mid(offset, posEnd - offset);
	CString upperWord = lineUpper.Mid(offset, posEnd - offset);
	CString help = line;
	help.Trim();
	if(upperWord == _T("DIM"))
	{
		posEnd = lineLength;
		for(int var=offset + 4; var<lineLength; var++)
		{
			TCHAR varChr = line.GetAt(var);
			if(!isdigit(varChr) && !isalpha(varChr))
			{
				posEnd = var;
				break;
			}
		}
		if(posEnd < (offset + 4))
		{
			if(mode == 0)
			{
				m_vMessages.push_back(new SemanticMessage(pathName, _T("Incomplete variable definition"), lineNumber));
			}
			else
			{
				messages.push_back(_T("Incomplete variable definition"));
			}
			return;
		}
		word = line.Mid(offset + 4, posEnd - (offset + 4));
		upperWord = lineUpper.Mid(offset + 4, posEnd - (offset + 4));
	}
	// Add it to the tree
	if(mode == Classview)
	{
        if (addFunctionName)
        {
            word = currentFunction + _T(".") + word;
        }
		SemanticVariable *sm = new SemanticVariable(pathName, word, upperWord, help, lineNumber);
		if(isConstant)
		{
			m_vConstants.push_back(sm);
		}
		else
		{
			m_vVariables.push_back(sm);
		}
		if(local && inFunction)
		{
			vTempVariables.push_back(sm);
		}
	}
	// Check for comma to do it again
	int commaCheck = posEnd;
	if(commaCheck < lineLength && line.GetAt(commaCheck) == ',')
	{
		// From the comma we need to find the next word
		int newOffset = -1;
		for(var = commaCheck; var < lineLength; var++)
		{
			varChr = line.GetAt(var);
			if(isalpha(varChr))
			{
				newOffset = var;
				break;
			}
		}
		if(newOffset > - 1)
		{
			HandleGlobal(line, lineUpper, newOffset, local, isConstant);
		}
		else
		{
			if(mode == 0)
			{
				m_vMessages.push_back(new SemanticMessage(pathName, _T("Incomplete variable definition"), lineNumber));
			}
			else
			{
				messages.push_back(_T("Incomplete variable definition"));
			}
		}
	}
}

// Returns whether the text before the : is only characters or digits
bool SemanticParser::IsLabel(CString &line)
{
	int length = line.GetLength() - 1; // Don't include the ending :

    // IRM 20090906 - Skip leading whitespace, not treated as a part of the label
    int Current = 0;
    while ( isspace( line.GetAt(Current) ) )
        ++Current;

    int FirstPos = Current;

    // IRM 20090906 - First char of a label cannot be a digit
    if (isdigit(line.GetAt(Current)))
        return false;

	for( ; Current<length ; Current++)
	{
		TCHAR ch = line.GetAt(Current);
        // IRM 20090906 - Stop looking at the colon, that's the end of the label
        if (ch == ':')
            break;

        // IRM 20090906 - Allow underscore as a valid label character
        if(!(isdigit(ch) || isalpha(ch) || ch == '_'))
		{
			return false;
		}
	}

    // IRM 20090920 - Check if the label is actually a keyword
    std::vector <Keyword*> *LabelKeywords = 0;
    CString LabelName = line.Mid(FirstPos, Current - FirstPos).MakeUpper();

    switch(LabelName.GetAt(0))
	{
	case _T('A'):
		LabelKeywords = &Keywords::m_AKeywords;
		break;
	case _T('B'):
		LabelKeywords = &Keywords::m_BKeywords;
		break;
	case _T('C'):
		LabelKeywords = &Keywords::m_CKeywords;
		break;
	case _T('D'):
		LabelKeywords = &Keywords::m_DKeywords;
		break;
	case _T('E'):
		LabelKeywords = &Keywords::m_EKeywords;
		break;
	case _T('F'):
		LabelKeywords = &Keywords::m_FKeywords;
		break;
	case _T('G'):
		LabelKeywords = &Keywords::m_GKeywords;
		break;
	case _T('H'):
		LabelKeywords = &Keywords::m_HKeywords;
		break;
	case _T('I'):
		LabelKeywords = &Keywords::m_IKeywords;
		break;
	case _T('J'):
		LabelKeywords = &Keywords::m_JKeywords;
		break;
	case _T('K'):
		LabelKeywords = &Keywords::m_KKeywords;
		break;
	case _T('L'):
		LabelKeywords = &Keywords::m_LKeywords;
		break;
	case _T('M'):
		LabelKeywords = &Keywords::m_MKeywords;
		break;
	case _T('N'):
		LabelKeywords = &Keywords::m_NKeywords;
		break;
	case _T('O'):
		LabelKeywords = &Keywords::m_OKeywords;
		break;
	case _T('P'):
		LabelKeywords = &Keywords::m_PKeywords;
		break;
	case _T('Q'):
		LabelKeywords = &Keywords::m_QKeywords;
		break;
	case _T('R'):
		LabelKeywords = &Keywords::m_RKeywords;
		break;
	case _T('S'):
		LabelKeywords = &Keywords::m_SKeywords;
		break;
	case _T('T'):
		LabelKeywords = &Keywords::m_TKeywords;
		break;
	case _T('U'):
		LabelKeywords = &Keywords::m_UKeywords;
		break;
	case _T('V'):
		LabelKeywords = &Keywords::m_VKeywords;
		break;
	case _T('W'):
		LabelKeywords = &Keywords::m_WKeywords;
		break;
    case _T('X'):
		LabelKeywords = &Keywords::m_XKeywords;
		break;	
	case _T('Y'):
		LabelKeywords = &Keywords::m_YKeywords;
		break;
	case _T('Z'):
		LabelKeywords = &Keywords::m_ZKeywords;
		break;
	default:
		LabelKeywords = &Keywords::m_MiscKeywords;
		break;
	}

    for (unsigned int i = 0; i < LabelKeywords->size(); ++i)
    {
        if ( (*LabelKeywords)[i]->keyword == LabelName)
            return false;
    }

    return true;
}

// Waits until the lock is released, and then clears up
void SemanticParser::CleanUp(bool ignoreLock)
{
	if(!ignoreLock)
	{
		m_Mutex.Lock(); // Wait forever as this needs to be done
	}

	for(mySEVectorIterator = SemanticParser::m_vMessages.begin(); mySEVectorIterator != SemanticParser::m_vMessages.end(); mySEVectorIterator++)
	{
		delete *mySEVectorIterator;
	}
	SemanticParser::m_vMessages.clear();
	for(mySFVectorIterator = SemanticParser::m_vFunctions.begin(); mySFVectorIterator != SemanticParser::m_vFunctions.end(); mySFVectorIterator++)
	{
		delete *mySFVectorIterator;
	}
	SemanticParser::m_vFunctions.clear();
	for(mySTVectorIterator = SemanticParser::m_vTypes.begin(); mySTVectorIterator != SemanticParser::m_vTypes.end(); mySTVectorIterator++)
	{
		// Delete variables that are part of a type
		for(mySVVectorIterator = (*mySTVectorIterator)->variables.begin(); mySVVectorIterator != (*mySTVectorIterator)->variables.end(); mySVVectorIterator++)
		{
			delete *mySVVectorIterator;
		}
		delete *mySTVectorIterator;
	}
	SemanticParser::m_vTypes.clear();
	for(mySLVectorIterator = SemanticParser::m_vLabels.begin(); mySLVectorIterator != SemanticParser::m_vLabels.end(); mySLVectorIterator++)
	{
		delete *mySLVectorIterator;
	}
	SemanticParser::m_vLabels.clear();
	for(mySVVectorIterator = SemanticParser::m_vVariables.begin(); mySVVectorIterator != SemanticParser::m_vVariables.end(); mySVVectorIterator++)
	{
		delete *mySVVectorIterator;
	}
	SemanticParser::m_vVariables.clear();
	for(mySCVectorIterator = SemanticParser::m_vConstants.begin(); mySCVectorIterator != SemanticParser::m_vConstants.end(); mySCVectorIterator++)
	{
		delete *mySCVectorIterator;
	}
	SemanticParser::m_vConstants.clear();

	if(!ignoreLock)
	{
		m_Mutex.Unlock();
	}
}
