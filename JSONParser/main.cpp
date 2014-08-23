////////////////////////////////////////////////////////
///     @FileName      : JSONParser.h
///     @Author        : Sagar Barai
///     @Date          : 07/15/2014
///     @Description   : JSON Object Parser Class
///     @Copyright     : Copyright@2014 Sagar Barai
///	    @Email		     : azadsagar@hotmail.com
///     @Version       : 0.1
////////////////////////////////////////////////////////

#include<iostream>
#include "JSONParser.h"

using namespace std;

int main(int argc,char* argv[])
{
	try
	{
		/// display how to use this program if command line arguments are not enough
		if(argc<2)
			throw string("Usage: programn_name filename.txt");

		
		ifstream objinFile(argv[1],ios::in | ios::binary);

		JSONParser objJson;

		string jsonString=objJson.debugJSONData(objinFile);
		int choice;
		int index;
		string key;

		do
		{
			cout<<jsonString<<endl;
			cout<<endl<<"1. Retrive content by Index (Total Array Size : "<<objJson.getSize(jsonString)<<" )";
			cout<<endl<<"2. Retrive Content by key";
			cout<<endl<<"0. Exit";
			cout<<endl<<"--------------------------------------------------";
			cout<<endl<<"Enter your choice : ";
			cin>>choice;

			if(!choice)
			{
				objinFile.close();
				break;
			}

			switch(choice)
			{
			case 1:
				cout<<"Enter the Index : ";
				cin>>index;
				jsonString=objJson.get(jsonString,index);
				break;
			case 2:
				cout<<"Enter the key : ";
				cin>>key;
				jsonString=objJson.get(jsonString,key);
				break;
			default:
				cout<<"Invalid Choice";
			}
		}while(choice);	/// never reached here


	}
	catch(JSONException e)
	{
		cout<<e.what();
	}
	catch(string strError)
	{
		cout<<strError;
	}

	return 0;
}
