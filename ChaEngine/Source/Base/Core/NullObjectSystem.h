#pragma once
#include "ObjectBase.h"
#include "NullObjectData.h"

namespace cha
{
	/// @brief null参照エラーを解決するためのnull object
	struct NullObjectSystem : ObjectBase<>
	{
		// NOTE: NullObjectはSystemObjectBaseを継承せずともシステム扱い
	};
}
