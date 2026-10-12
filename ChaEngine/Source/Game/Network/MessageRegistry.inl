#include "MessageRegistry.h"

template<typename T>
inline MessageRegistry& MessageRegistry::AddMember(T& _v)
{
	members.push_back(&_v);

	return *this;
}
