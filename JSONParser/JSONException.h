#ifndef JSONEXCEPTION_H
#define JSONEXCEPTION_H

#include<string>

class JSONException
{
private:
	std::string objErrorText;
public:
	JSONException(std::string objErrorText)
	{
		this->objErrorText=objErrorText;
	}

	std::string what()
	{
		return objErrorText;
	}
};

#endif
