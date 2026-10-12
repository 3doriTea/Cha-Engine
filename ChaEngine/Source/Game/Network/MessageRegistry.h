#pragma once
#include <Game/Game.h>
#include <variant>
#include <Game/Common/Utility/ISizedBuffer.h>

class MessageRegistry
{
	friend class MessageConverter;

public:
	template<typename T>
	MessageRegistry& AddMember(T& _v);

private:
	MessageRegistry() = default;
	~MessageRegistry() = default;
	
private:
	std::vector<std::variant<
		int8_t*,  uint8_t*,
		int16_t*, uint16_t*,
		int32_t*, uint32_t*,
		int64_t*, uint64_t*,
		float*,
		double*,
		std::string*,
		ISizedBuffer*
		>> members{};
};

#include "MessageRegistry.inl"
