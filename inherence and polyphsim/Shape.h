#pragma once
#include<string>
using std::string;
enum C {

	white,red,green,black,yellow,
};

class Shape
{
public:
	Shape();
	Shape(bool isfilled, C color);
	std::
	string tostring();
	~Shape();
private:
	C color{ red };
	bool isfilled=true;
};

