#include "Shape.h"
#include<iostream>
using std::cout;
using std::endl;


Shape::Shape()
{
	cout << "Shape" << endl;
}
Shape::Shape(bool isfilled, C color)
{
	this->isfilled = isfilled;
	this->color = color;
}

string Shape::tostring()
{
	return("shape"+"color:"++"isfilled-"+isfilled?"true":"false")
}

Shape::~Shape()
{
}
