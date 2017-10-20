#include "EntityTypes.h"

ECEntityTypes & operator++(ECEntityTypes & _option)
{
	_option = static_cast<ECEntityTypes>(static_cast<int>(_option) + 1);
	if (_option == ECEntityTypes::BOTTOM)
		_option = static_cast<ECEntityTypes>(1);
	return _option;
}

ECEntityTypes operator++(ECEntityTypes & _option, int)
{
	ECEntityTypes result = _option;
	++_option;
	return result;
}

ECEntityTypes & operator--(ECEntityTypes & _option)
{
	_option = static_cast<ECEntityTypes>(static_cast<int>(_option) - 1);
	if (_option == ECEntityTypes::TOP)
		_option = static_cast<ECEntityTypes>(3);
	return _option;
}

ECEntityTypes operator--(ECEntityTypes & _option, int)
{
	ECEntityTypes result = _option;
	--_option;
	return result;
}
