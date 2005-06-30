#include "c_object.h"
#include "globals.h"





c_object::c_object ()
	: f_references(0)
{
}

c_object::~c_object ()
{
}





void c_object::reference_increase ()
{
	f_references++;
}

void c_object::reference_decrease ()
{
	f_references--;
}

bool c_object::reference_zero () const
{
	return f_references == 0;
}
