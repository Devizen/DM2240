#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H

enum class ECEntityTypes
{
	TOP = 0,
	STATIC,
	OBJECT,
	TERRAIN,
	OTHERS,
	BOTTOM,

	TYPE_COUNT
};

/*Increment*/
ECEntityTypes& operator++(ECEntityTypes &_option);
ECEntityTypes operator++(ECEntityTypes &_option, int);
/*Decrement*/
ECEntityTypes& operator--(ECEntityTypes &_option);
ECEntityTypes operator--(ECEntityTypes &_option, int);



#endif