////////////////////////////////////////////////////////
///     @FileName      : JSONParser.h
///     @Author        : Sagar Barai
///     @Date          : 07/15/2014
///     @Description   : JSON Object Parser Class
///     @Copyright     : Copyright@2014 Sagar Barai
///	@Email		   : azadsagar@hotmail.com
///     @Version       : 0.1
////////////////////////////////////////////////////////

#ifndef JSONPARSER_H_INCLUDED
#define JSONPARSER_H_INCLUDED

#include<fstream>
#include<string>
#include<stdlib.h>
#include<stack>
#include "JSONException.h"

using namespace std;

class JSONParser
{
private:

    string fileToString(ifstream& objJSONFile)
    {
        if(!objJSONFile.is_open())
        {
            throw JSONException("IO Error: Input file is not opened");
        }

        objJSONFile.clear();
        objJSONFile.seekg(0L,ios::beg);

        string objJsonString;
        objJsonString="";

        char sztmpChar;

        while(objJSONFile.read((char*)&sztmpChar,sizeof(char)))
        {
            objJsonString+=sztmpChar;
        }

        return objJsonString;

    }


	void reachTillIndex(const string& objJson,bool& is_insidequote,stack<char>& objStack,
		char& sztmpPreivousChar,char& sztmpCurrentChar,unsigned int& nPos,const unsigned int& n_index,
		unsigned int& n_currentIndex,const char szdelimator)
	{
		const unsigned int nLength=objJson.length();

		sztmpPreivousChar=sztmpCurrentChar;

		while((n_currentIndex<n_index) && nPos<nLength)
		{
			sztmpCurrentChar=objJson[nPos];
			nPos++;
			if(sztmpCurrentChar==',' && (!is_insidequote))
			{
				if(objStack.size()==1)
				n_currentIndex++;
			}
			else if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
			{
				is_insidequote=is_insidequote?false:true;
			}
			else if((sztmpCurrentChar=='{' || sztmpCurrentChar=='[' || sztmpCurrentChar=='}' || sztmpCurrentChar==']') && (!is_insidequote))
			{
				switch(sztmpCurrentChar)
				{
				case '{':
				case '[':
					objStack.push(sztmpCurrentChar);
					break;
				case '}':
					if(objStack.top()!='{')
					{
						throw JSONException("Invalid Json Object");
					}
					else
					{
						objStack.pop();
						if(szdelimator==sztmpCurrentChar)
						{
							if(objStack.empty())
							{
								throw JSONException("Index out of bounds");
							}
						}
					}
					break;
				case ']':
					if(objStack.top()!='[')
					{
						throw JSONException("Invalid Json Object");
					}
					else
					{
						objStack.pop();
						if(szdelimator==sztmpCurrentChar)
						{
							if(objStack.empty())
							{
								throw JSONException("Index out of bounds");
							}
						}
					}
					break;
				}
			}

			sztmpPreivousChar=sztmpCurrentChar;

		}

		/// check if loop was broken coz of end of file.
		/// if so throw invalid json file exception

		if(n_currentIndex!=n_index)
			throw JSONException("Invalid JSON Object");
	}

	void extractValue(const string& objJson,string& objJsonString,stack<char>& objStack,char& sztmpCurrentChar,
		char& sztmpPreivousChar,bool& is_insidequote,unsigned int& nPos)
	{
		unsigned int nLength=objJson.length();

		is_insidequote=false;

		while(nPos<nLength)
		{
			sztmpCurrentChar=objJson[nPos];
			nPos++;

			if(sztmpCurrentChar=='[' || sztmpCurrentChar=='{' ||sztmpCurrentChar=='\"' || ((sztmpCurrentChar>='0' && sztmpCurrentChar<='9') || sztmpCurrentChar=='-' || sztmpCurrentChar=='+')
                || sztmpCurrentChar=='t' || sztmpCurrentChar=='f' || sztmpCurrentChar=='n')
				break;
			else if(sztmpCurrentChar==' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t')
				continue;
			else
				throw JSONException("Invalid Json object");
		}

		/// check if loop was broken coz of end of file
        if(!(sztmpCurrentChar=='[' || sztmpCurrentChar=='{' ||sztmpCurrentChar=='\"' || ((sztmpCurrentChar>='0' && sztmpCurrentChar<='9') || sztmpCurrentChar=='-' || sztmpCurrentChar=='+')
             || sztmpCurrentChar=='t' || sztmpCurrentChar=='f' || sztmpCurrentChar=='n'))
        {
            throw JSONException("Invalid JSon object");
        }

		///empty the stack
		while(!objStack.empty())
		{
			objStack.pop();
		}

		/// collect value and return it

		/// if the the char found " then we only return contents without quote
		if(sztmpCurrentChar=='\"')
		{
			is_insidequote=true;

			sztmpPreivousChar=sztmpCurrentChar;

			bool is_evenEscapeSequence=false;

			while(nPos<nLength)
			{
				sztmpCurrentChar=objJson[nPos];
				nPos++;

				if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
				{
					is_insidequote=false;
					break;
				}
				else if(sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t' || sztmpCurrentChar=='\b'
                        || sztmpCurrentChar=='\f')
                {
                    /// as per json.org these chars can not be involved directly. json encoder should generate escape sequence for
                    /// such char
                    /// at present \u encoding is not supported
                    throw JSONException("Invalid Json object");
                }
                else if(sztmpCurrentChar=='\\')
                {

                    if(is_evenEscapeSequence)
                    {
                        objJsonString+='\\';
                        is_evenEscapeSequence=false;
                        sztmpPreivousChar=0;    /// some dummy char
                        continue;
                    }
                    else
                    {
                        is_evenEscapeSequence=true;
                    }
                }
				else
				{
				    /// parse the escape sequence if any
				    if(sztmpPreivousChar=='\\')         /// escape sequence is found
                    {
                        switch(sztmpCurrentChar)
                        {
                        case 'n':
                            objJsonString+='\n';        /// add new line char
                            break;
                        case 'r':
                            objJsonString+='\r';        /// add carriage return
                            break;
                        case 't':
                            objJsonString+='\t';        /// add tab space
                            break;
                        case 'f':
                            objJsonString+='\f';        /// add form feed
                            break;
                        case 'b':
                            objJsonString+='\b';        /// add backspace char
                            break;
                        case 'u':
                            objJsonString+="\\u";        /// currently not parsing \u hex code
                            break;
                        case '/':
                            objJsonString+='/';        /// add forward slash
                            break;
                        default:
                            throw JSONException("Invalid Json object");
                        }

                        is_evenEscapeSequence=false;
                    }
                    else
                    {
                        objJsonString+=sztmpCurrentChar;
                    }
				}

				sztmpPreivousChar=sztmpCurrentChar;
			}

			/// if loop was broken coz of end of file
			if(sztmpCurrentChar!='\"')
			{
				throw JSONException("Invalid Json object");
			}

		}
		else if((sztmpCurrentChar>='0' && sztmpCurrentChar<='9') || sztmpCurrentChar=='-' || sztmpCurrentChar=='+')
		{
			/// digit is found
			int tmpPeriodCount=0;
			int tmpExponentCount=0;
			objJsonString+=sztmpCurrentChar;

			/// we collect as long as comma separator or space or
			/// tab char or new line char or carriage return or
			/// terminating ']' is not found

			sztmpPreivousChar=sztmpCurrentChar;

			while(nPos<nLength)
			{
				sztmpCurrentChar=objJson[nPos];
				nPos++;

				if(sztmpCurrentChar=='.')
				{
					tmpPeriodCount++;
					if(tmpPeriodCount>1)
					{
						throw JSONException("Invalid Json object");
					}
					else
                    {
                        /// period is not allowed just after exponent
                        if(sztmpPreivousChar=='e' || sztmpPreivousChar=='E' || sztmpPreivousChar=='-' || sztmpPreivousChar=='+')
                            throw JSONException("Invalid Json object");
                    }

					objJsonString+=sztmpCurrentChar;

				}
				else if(sztmpCurrentChar=='-' || sztmpCurrentChar=='+')
                {
                    // - or + is always followed by e
                    if(sztmpPreivousChar=='e' || sztmpPreivousChar=='E')
                        objJsonString+=sztmpCurrentChar;
                    else
                        throw JSONException("Invalid Json object");

                }
				else if(sztmpCurrentChar=='e' || sztmpCurrentChar=='E')
                {
                    tmpExponentCount++;
                    /// exponent not expected just after + - or . and also if exponent is available
                    /// before period thats invalid
                    if(tmpExponentCount>1 || !(sztmpPreivousChar>='0' && sztmpPreivousChar<='9'))
                        throw JSONException("Invalid Json object");

                    objJsonString+=sztmpCurrentChar;


                }
				else if(sztmpCurrentChar==' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t' || sztmpCurrentChar==','
                        || sztmpCurrentChar=='}' || sztmpCurrentChar==']' )
				{
					break;
				}
				else if(sztmpCurrentChar>='0' && sztmpCurrentChar<='9')
				{
					objJsonString+=sztmpCurrentChar;
				}
				else
				{
					/// anything other than obove condition is invalid json data
					throw JSONException("Invalid Json object");
				}
				sztmpPreivousChar=sztmpCurrentChar;
			}

			/// if loop was not broken ']' or ',' we check for validity of json object
			if(!(sztmpCurrentChar=='}' || sztmpCurrentChar==',' || sztmpCurrentChar==']'))
			{
			    /// applicable for numbers only
                /// just make sure last char was not . e E + -

			    if(sztmpPreivousChar=='e' || sztmpPreivousChar=='E' || sztmpPreivousChar=='-' || sztmpPreivousChar=='+' || sztmpPreivousChar=='.')
                    throw JSONException("Invalid Json object");

				while(nPos<nLength)
				{
					sztmpCurrentChar=objJson[nPos];
					nPos++;

					if(sztmpCurrentChar==',' || sztmpCurrentChar=='}' || sztmpCurrentChar==']')
						break;
				}

				/// make sure loop was not broken coz of end of file

				if(!(sztmpCurrentChar=='}' || sztmpCurrentChar==','))
					throw JSONException("Invalid Json object");
			}
			else
            {
                /// applicable for numbers only
                /// just make sure last char was not . e E + -
                if(sztmpPreivousChar=='e' || sztmpPreivousChar=='E' || sztmpPreivousChar=='-' || sztmpPreivousChar=='+' || sztmpPreivousChar=='.')
                    throw JSONException("Invalid Json object");

            }
		}
		else if(sztmpCurrentChar=='t' || sztmpCurrentChar=='f' || sztmpCurrentChar=='n')
        {
            /// true false or null is expected

            objJsonString+=sztmpCurrentChar;


            while(nPos<nLength)
			{
				sztmpCurrentChar=objJson[nPos];
				nPos++;

				if(sztmpCurrentChar==' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t' || sztmpCurrentChar==','
                        || sztmpCurrentChar=='}' || sztmpCurrentChar==']' )
                    break;

                objJsonString+=sztmpCurrentChar;
            }

            /// compare the string with true false and null
            if(!(objJsonString.compare("true")==0 || objJsonString.compare("false")==0 || objJsonString.compare("null")==0))
                throw JSONException("Invalid Json Object");


        }
		else
		{
			objStack.push(sztmpCurrentChar);
			objJsonString+=sztmpCurrentChar;

			sztmpPreivousChar=sztmpCurrentChar;

			is_insidequote=false;

			while(nPos<nLength)
			{
				sztmpCurrentChar=objJson[nPos];
				nPos++;

				if((sztmpCurrentChar=='[' || sztmpCurrentChar=='{') && (!is_insidequote))
				{
					objStack.push(sztmpCurrentChar);
				}
				else if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
				{
					/// togle inside quote
					is_insidequote=is_insidequote?false:true;
				}
				else if((sztmpCurrentChar=='}' || sztmpCurrentChar==']') && (!is_insidequote))
				{
					switch(sztmpCurrentChar)
					{
					case '}':
						if(objStack.top()=='{')
						{
							objStack.pop();
							if(objStack.empty())
							{
								objJsonString+=sztmpCurrentChar;
								goto returnJsonString;
							}
						}
						else
						{
							throw JSONException("Invalid Json object");
						}
						break;
					case ']':
						if(objStack.top()=='[')
						{
							objStack.pop();
							if(objStack.empty())
							{
								objJsonString+=sztmpCurrentChar;
								goto returnJsonString;
							}
						}
						else
						{
							throw JSONException("Invalid Json object");
						}
						break;
					}
				}

				objJsonString+=sztmpCurrentChar;
				sztmpPreivousChar=sztmpCurrentChar;
			}
		}

returnJsonString:
		return;
	}

public:
	unsigned int getSize(ifstream& objJSONFile)
	{
	    return getSize(fileToString(objJSONFile));
	}

	unsigned int getSize(const string& objJson)
	{
	    const unsigned int nLength=objJson.length();
	    unsigned int nPos=0;

	    unsigned int nSize=1;

		char sztmpCurrentChar;
		char sztmpPreivousChar=0;
		stack<char> objStack;

		bool is_insidequote=false;

		while(nPos<nLength)
        {
            sztmpCurrentChar=objJson[nPos];
            nPos++;

            if((sztmpCurrentChar=='[') || (sztmpCurrentChar=='{'))
            {
                break;
            }
            else if(!((sztmpCurrentChar==' ') || (sztmpCurrentChar=='\t') || (sztmpCurrentChar=='\n') || (sztmpCurrentChar=='\r')))
            {
                /// if other than these character at the beginning of file this is invalid json data
                throw JSONException("Invalid JSON File");
            }
            else
            {
                continue;
            }
        }

		objStack.push(sztmpCurrentChar);

		switch(sztmpCurrentChar)
        {
        case '[':
            /// contents inside can [] can either begin with "
            /// or { or even [
            /// we look for such pair seperated by commas ,
            /// till we reach requested index
            sztmpPreivousChar=sztmpCurrentChar;

			while((!objStack.empty()) && nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(sztmpCurrentChar==',' && (!is_insidequote))
                {
					if(objStack.size()==1)
						nSize++;
                }
                else if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
                {
                    is_insidequote=is_insidequote?false:true;
                }
                else if((sztmpCurrentChar=='{' || sztmpCurrentChar=='[' || sztmpCurrentChar=='}' || sztmpCurrentChar==']') && (!is_insidequote))
                {
                    switch(sztmpCurrentChar)
                    {
                    case '{':
                    case '[':
                        objStack.push(sztmpCurrentChar);
                        break;
                    case '}':
                        if(objStack.top()!='{')
                        {
                            throw JSONException("Invalid Json Object");
                        }
                        else
                        {
                            objStack.pop();
                        }
                        break;
                    case ']':
                        if(objStack.top()!='[')
                        {
                            throw JSONException("Invalid Json Object");
                        }
                        else
                        {
                            objStack.pop();
                        }
                        break;
                    }
                }

                sztmpPreivousChar=sztmpCurrentChar;

            }

		case '{':

			sztmpPreivousChar=sztmpCurrentChar;

			while((!objStack.empty()) && nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(sztmpCurrentChar==',' && (!is_insidequote))
                {
					if(objStack.size()==1)
						nSize++;
                }
                else if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
                {
                    is_insidequote=is_insidequote?false:true;
                }
				else if((sztmpCurrentChar=='{' || sztmpCurrentChar=='[' || sztmpCurrentChar=='}' || sztmpCurrentChar==']') && (!is_insidequote))
                {
                    switch(sztmpCurrentChar)
                    {
                    case '{':
                    case '[':
                        objStack.push(sztmpCurrentChar);
                        break;
                    case '}':
                        if(objStack.top()!='{')
                        {
                            throw JSONException("Invalid Json Object");
                        }
                        else
                        {
                            objStack.pop();
                        }
                        break;
                    case ']':
                        if(objStack.top()!='[')
                        {
                            throw JSONException("Invalid Json Object");
                        }
                        else
                        {
                            objStack.pop();
                        }
                        break;
                    }
                }

                sztmpPreivousChar=sztmpCurrentChar;
            }

			break;

		default:
			throw JSONException("Invalid json object");
		}

		if(!objStack.empty())
		{
			throw JSONException("Invalid Json object");
		}

		/// also check if file reached to the end
		while(nPos<nLength)
		{
		    sztmpCurrentChar=objJson[nPos];
            nPos++;

			if(!(sztmpCurrentChar==' ' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t'  || sztmpCurrentChar=='\n'))
				throw JSONException("Invalid Json object");
		}

		return nSize;
	}

    string get(ifstream& objJSONFile,const int n_index)
    {
        return get(fileToString(objJSONFile),n_index);
    }

    string get(ifstream& objJSONFile,const string key)
    {
        return get(fileToString(objJSONFile),key);
    }

    string get(const string& objJson,const int n_index)
    {
        /// if this overloaded function is called
        /// we are looking inside array
        /// and therefore array can either begin with
        /// '[' or '{'
        /// items inside [] or {} can eiteher begin with
        /// quote " or { or even '['
        stack<char> objStack;
        string objJsonString="";
        char sztmpPreivousChar=0;
        char sztmpCurrentChar;

        const unsigned int nLength=objJson.length();
        unsigned int nPos=0;


        bool is_insidequote=false;

        while(nPos<nLength)
        {
            sztmpCurrentChar=objJson[nPos];
            nPos++;

            if((sztmpCurrentChar=='[') || (sztmpCurrentChar=='{'))
            {
                break;
            }
            else if(!((sztmpCurrentChar==' ') || (sztmpCurrentChar=='\t') || (sztmpCurrentChar=='\n') || (sztmpCurrentChar=='\r')))
            {
                /// if other than these character at the beginning of file this is invalid json data
                throw JSONException("Invalid JSON File");
            }
            else
            {
                continue;
            }
        }


        /// check if char was '[' or '{'

        unsigned int n_currentIndex=0;

        objStack.push(sztmpCurrentChar);

        switch(sztmpCurrentChar)
        {
        case '[':
            /// contents inside can [] can either begin with "
            /// or { or even [
            /// we look for such pair seperated by commas ,
            /// till we reach requested index
			reachTillIndex(objJson,is_insidequote,objStack,sztmpPreivousChar,sztmpCurrentChar,nPos,n_index,n_currentIndex,']');

			/// now we scan till we get [ or { or " or digit
            /// if digit is found only one period is permitted
            /// in between we scan till the end of digit

			extractValue(objJson,objJsonString,objStack,sztmpCurrentChar,sztmpPreivousChar,is_insidequote,nPos);

            break;

        case '{':
            /// contents inside {} are always a key value pair
            /// where value can be yet another json object
            /// or json array
            /// the the first char we look for is quote "

			reachTillIndex(objJson,is_insidequote,objStack,sztmpPreivousChar,sztmpCurrentChar,nPos,n_index,n_currentIndex,'}');

            /// we search for : and which is not inside key
            sztmpPreivousChar=sztmpCurrentChar;

            /// eliminate all tab spaces white spaces new line carriage return
            /// etc

            while(nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(sztmpCurrentChar==' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t')
                {
                    continue; /// ignore them
                }
                else if(sztmpCurrentChar=='\"')
                {
                    break;
                }
                else
                {
                    /// other than this throw exception
                    /// coz key is always inside qote ""
                    throw JSONException("Invalid Json Object");
                }
            }

            /// if loop was not broken coz quote then it must be coz of end of file
            /// check if we reached end of file and if so throw invalid json object exception

            if(sztmpCurrentChar!='\"')
                throw JSONException("Invalid Json Object");

            is_insidequote=true;
            sztmpPreivousChar=sztmpCurrentChar;

            while(nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
                {
                    is_insidequote=false;
                    break;
                }

                sztmpPreivousChar=sztmpCurrentChar;
            }

            /// check if loop was broken coz of end of file
            if(sztmpCurrentChar!='\"')
                throw JSONException("Invalid Json Object");

            /// now we search for : and then we store the value which may itself be object

            while(nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(sztmpCurrentChar==' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t')
                {
                    continue;
                }
                else if(sztmpCurrentChar==':')
                {
                    break;
                }
                else
                {
                    throw JSONException("Invalid Json Object");
                }
            }

            /// check if loop was broken coz of end of file
            if(sztmpCurrentChar!=':')
                throw JSONException("Invalid Json Object");


			extractValue(objJson,objJsonString,objStack,sztmpCurrentChar,sztmpPreivousChar,is_insidequote,nPos);

            break;

			default:
				throw JSONException("Invalid Json Object");
        }

        //returnJsonString:

        return objJsonString;

    }

    string get(const string& objJson,const string key)
    {
        string objJsonString="";

        /// if this overloaded function is called
        /// we are definitely looking for key value pair
        /// and key value pair are always inside {}
        /// so if this json object doesn't start with
        /// opening { bracket then this is invalid json object

        const unsigned int nLength=objJson.length();
        unsigned int nPos=0;

        char sztmpCurrentChar;
		char sztmpPreivousChar=0;
		stack<char> objStack;

		bool is_insidequote=false;

		///we are looking for '{'
		while(nPos<nLength)
        {
            sztmpCurrentChar=objJson[nPos];
            nPos++;

            if((sztmpCurrentChar==' ') || (sztmpCurrentChar=='\t') || (sztmpCurrentChar=='\r') || (sztmpCurrentChar=='\n'))
                continue;
            else if(sztmpCurrentChar=='{')
                break;
            else
                throw JSONException("Not a JSON object or Invalid json object");
        }

        if(sztmpCurrentChar!='{')
            throw JSONException("Invalid json object");

        objStack.push(sztmpCurrentChar);

        bool is_keyFound=false;
        bool is_ParsingValue=false;         /// parsing value is toggled by : and ,

        is_insidequote=false;
        sztmpPreivousChar=sztmpCurrentChar;

        while((!objStack.empty()) && nPos<nLength)
        {
            sztmpCurrentChar=objJson[nPos];
            nPos++;

            if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
            {
                /// toggle insidequote
                is_insidequote=is_insidequote?false:true;
            }
            else if(sztmpCurrentChar==':' && (!is_insidequote))     /// this one needs testing i am not sure for possible inputs
            {
                is_ParsingValue=true;
            }
            else if(sztmpCurrentChar==',' && (!is_insidequote))
            {
                if(objStack.size()==1)
                    is_ParsingValue=false;
            }
            else if((sztmpCurrentChar=='{' || sztmpCurrentChar=='[') && (!is_insidequote))
            {
                objStack.push(sztmpCurrentChar);
            }
            else if((sztmpCurrentChar=='}' || sztmpCurrentChar==']') && (!is_insidequote))
            {
                switch(sztmpCurrentChar)
                {
                case '}':
                    if(objStack.top()=='{')
                    {
                        objStack.pop();
                    }
                    else
                    {
                        /// this is invalid json object
                        throw JSONException("Invalid Json object");
                    }
                    break;
                case ']':
                    if(objStack.top()=='[')
                    {
                        objStack.pop();
                    }
                    else
                    {
                        /// this is invalid json object
                        throw JSONException("Invalid Json object");
                    }
                    break;

                }
            }

            if(is_insidequote && (!is_ParsingValue))
            {
                /// if we are inside quote and
                /// currently not parsing value
                /// we store the key in temporary string
                /// and then compare it with a given input string
                /// if the key is matched with input string
                /// then we break this loop
                /// where we return the value assosiated with that
                /// key which might itself be json (object or array or
                /// number or simply a string)

                string tmpKey="";

                while(nPos<nLength)
                {
                    sztmpCurrentChar=objJson[nPos];
                    nPos++;

                    if(sztmpCurrentChar=='\"' && sztmpPreivousChar!='\\')
                    {
                        is_insidequote=false;
                        if(key.compare(tmpKey)==0)
                        {
                            /// this is required key
                            is_keyFound=true;
                            goto keyMatched;
                        }
                        break;
                    }

                    tmpKey+=sztmpCurrentChar;
                    sztmpPreivousChar=sztmpCurrentChar;
                }

                /// check how this loop was broken

                if(sztmpCurrentChar!='\"')          /// reached end of file
                    throw JSONException("Invalid json object");

            }

            sztmpPreivousChar=sztmpCurrentChar;

        }

        /// check how we came here from the above loop

        if(is_keyFound)
        {
            /// thats good sign
            /// nothing to do here
            /// actually never reached here
        }
        else if(objStack.empty())
        {
            /// key not found
            /// beofre throwing key not found exception we can check
            /// validity of json object
            /// if there are char other than whitespace tab new line
            /// or carriage return this is invalid object

            while(nPos<nLength)
            {
                sztmpCurrentChar=objJson[nPos];
                nPos++;

                if(!(sztmpCurrentChar== ' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t'))
                    throw JSONException("Invalid Json Object");
            }

            throw JSONException("Key not found : " + key);
        }
        else
        {
            /// we reached end of the file and this is invalid json file/string
            throw JSONException("Invalid Json Object");
        }


        keyMatched:

        /// look for colon

        /// remove all the white spaces if before : and after : if any

        while(nPos<nLength)
        {
            sztmpCurrentChar=objJson[nPos];
            nPos++;

            if((sztmpCurrentChar== ' ' || sztmpCurrentChar=='\n' || sztmpCurrentChar=='\r' || sztmpCurrentChar=='\t'))
                continue;
            else if(sztmpCurrentChar==':')
            {
                break;
            }
            else
            {
                /// other than above char before : is invalid json object
                throw JSONException("Invalid Json Object");
            }
        }

        if(sztmpCurrentChar!=':')                       /// reached end of file
            throw JSONException("Invalid Json Object");

        /// now we scan till we get [ or { or " or digit
        /// if digit is found only one period is permitted
        /// in between we scan till the end of digit

		extractValue(objJson,objJsonString,objStack,sztmpCurrentChar,sztmpPreivousChar,is_insidequote,nPos);

        return objJsonString;

    }

	string debugJSONData(ifstream& objJsonFile)
	{
		return fileToString(objJsonFile);
	}

	string debugJSONData(const string& objJson)
	{
		return objJson;
	}

	string formatJSON(const string& objJson)
	{
	    const unsigned int nLength=objJson.length();
        unsigned int nPos=0;
        string formatedJson;
        char tmpCurrentChar;
        char tmpPreviosChar=0;
        bool is_insidequote=false;
        unsigned int tabCount=0;

        while(nPos<nLength)
        {
            tmpCurrentChar=objJson[nPos];
            nPos++;

            if(tmpCurrentChar=='\"' && tmpPreviosChar!='\\')
            {
                is_insidequote=is_insidequote?false:true;
                formatedJson+='\"';
            }
            else if((tmpCurrentChar=='[' || tmpCurrentChar=='{') && !is_insidequote)
            {
                formatedJson+='\n';
                for(unsigned int i=0;i<tabCount;i++)
                {
                    formatedJson+="  ";
                }
                formatedJson+=tmpCurrentChar;
                formatedJson+='\n';

                tabCount++;
                for(unsigned int i=0;i<tabCount;i++)
                {
                    formatedJson+="  ";
                }
            }
            else if((tmpCurrentChar==']' || tmpCurrentChar=='}') && !is_insidequote)
            {
                formatedJson+='\n';
                tabCount--;
                for(unsigned int i=0;i<tabCount;i++)
                {
                    formatedJson+="  ";
                }
                formatedJson+=tmpCurrentChar;
                formatedJson+='\n';
                for(unsigned int i=0;i<tabCount;i++)
                {
                    formatedJson+="  ";
                }
            }
            else if(tmpCurrentChar==',' && !is_insidequote)
            {
                formatedJson+=',';
                formatedJson+='\n';
                for(unsigned int i=0;i<tabCount;i++)
                {
                    formatedJson+="  ";
                }
            }
            else if((tmpCurrentChar=='\t' || tmpCurrentChar=='\n' || tmpCurrentChar=='\r') && (!is_insidequote))
            {
                tmpPreviosChar=tmpCurrentChar;
                continue;
            }
            else
            {
                formatedJson+=tmpCurrentChar;
            }
        }

        return formatedJson;
	}

};

#endif // JSONPARSER_H_INCLUDED
