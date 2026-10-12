#include "NetworkSessionManager.h"
#include "MessageConverter.h"

template<typename Func>
inline void NetworkSessionManager::Receive(Func&& _func)
{
	std::vector<ENetTransport::Event> receivedEvents = transport_.PollEvents();

	for (auto& recievedEvent : receivedEvents)
	{
		_func(recievedEvent);
	}
}
