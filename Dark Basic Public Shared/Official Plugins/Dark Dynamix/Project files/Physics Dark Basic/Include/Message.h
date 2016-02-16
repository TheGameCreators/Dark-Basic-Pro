#ifndef MESSAGE
#define MESSAGE

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>
using namespace std;

class Message
{
public:
	static void errorBox(string message, string caption){
		if(errorFlagged < maxErrors){
			MessageBox(NULL,(LPCSTR)message.c_str(), (LPCSTR)caption.c_str(), MB_OK | MB_ICONERROR);
			errorFlagged++;// = true;
		}
	}

    static void ignitionError(){
	   Message::errorBox(string("Physics not initialised."), string("Error"));
	}
    static void startCalledError(){
	   Message::errorBox(string("Multiple initialisation."), string("Error"));
    }


	static void objectNotExist(int objectID, string function){
		string str("Object:");

		stringstream lStream; 
		lStream << objectID;
		str += lStream.str();
		
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void limbNotExist(int objectID, int limbID, string function){
		string str("Limb:");

		stringstream lStream; 
		lStream << limbID;
		str += lStream.str();
		str.append(" does not exist in object:");
    
		stringstream lStream2;
		lStream2 << objectID;
		str += lStream2.str();


		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void meshNotExist(int objectID, string function){
		string str("Object:");

		stringstream lStream; 
		lStream << objectID;
		str += lStream.str();
		
		str.append(" has no root mesh data.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void meshNotExist(int objectID, int limbID, string function){
		string str("Limb:");

		stringstream lStream; 
		lStream << limbID;
		str += lStream.str();
		str.append(" in object:");

		stringstream lStream2;
		lStream2 << objectID;
		str += lStream2.str();

		str.append(" has no mesh data.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}


	static void fileNotExist(string file, string function){
		string str("File: ");

		//stringstream lStream; 
		//lStream << objectID;
		//str += lStream.str();
		
		str.append(file);
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void actorNotExist(int objectID, string function){
		string str("Object:");

		stringstream lStream; 
		lStream << objectID;
		str += lStream.str();
		
		str.append(" does not have an associated actor.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	//static void actorNotExist(int objectID, int limbID, string function){
	//	string str("Object:");

	//	stringstream lStream; 
	//	lStream << objectID;
	//	str += lStream.str();
	//	
	//	//limb
	//	string str2("Limb:");
	//	stringstream lStream2; 
	//	lStream2 << limbID;
	//	str2 += lStream2.str();
	//	str.append(str2);

	//	str.append(" does not have an associated actor.");

	//	string str2("Error: ");
	//	str2.append(function);
	//	errorBox(str, str2);
	//}

	static void actorAlreadyExist(int objectID, string function){
		string str("Object:");

		stringstream lStream; 
		lStream << objectID;
		str += lStream.str();
		
		str.append(" already has an associated actor.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}


	static void actorNotExist(int objectID, int limbID, string function){
		string str("Limb: ");

		stringstream lStream; 
		lStream << limbID;
		str += lStream.str();
		str.append(" in Object: ");
    
		stringstream lStream2;
		lStream2 << objectID;
		str += lStream2.str();

		str.append(" does not have an associated actor.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void actorAlreadyExist(int objectID, int limbID, string function){
		string str("Limb: ");

		stringstream lStream; 
		lStream << limbID;
		str += lStream.str();
		str.append(" in Object: ");
    
		stringstream lStream2;
		lStream2 << objectID;
		str += lStream2.str();

		str.append(" already has an associated actor.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}



	static void jointDescNotExist(int jointDesc, string function){
		string str("Joint desc: ");

		stringstream lStream; 
		lStream << jointDesc;
		str += lStream.str();
		
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void tireFuncDescNotExist(int tireFuncDesc, string function){
		string str("TireFuncDesc: ");

		stringstream lStream; 
		lStream << tireFuncDesc;
		str += lStream.str();
		
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void jointNotExist(int joint, string function){
		string str("Joint: ");

		stringstream lStream; 
		lStream << joint;
		str += lStream.str();
		
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void jointDescAlreadyExists(int jointDesc, string function){
		string str("Joint desc: ");

		stringstream lStream; 
		lStream << jointDesc;
		str += lStream.str();
		
		str.append(" already exists.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void tireFuncDescAlreadyExists(int tireFuncDesc, string function){
		string str("TireFuncDesc: ");

		stringstream lStream; 
		lStream << tireFuncDesc;
		str += lStream.str();
		
		str.append(" already exists.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void jointAlreadyExists(int joint, string function){
		string str("Joint: ");

		stringstream lStream; 
		lStream << joint;
		str += lStream.str();
		
		str.append(" already exists.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void meshAlreadyExists(int mesh, string function){
		string str("Mesh: ");

		stringstream lStream; 
		lStream << mesh;
		str += lStream.str();
		
		str.append(" already exists.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}

	static void meshActorNotExist(int mesh, string function){
		string str("Mesh: ");

		stringstream lStream; 
		lStream << mesh;
		str += lStream.str();
		
		str.append(" does not exist.");

		string str2("Error: ");
		str2.append(function);
		errorBox(str, str2);
	}


	static int errorFlagged;
	static int maxErrors;
};



#endif