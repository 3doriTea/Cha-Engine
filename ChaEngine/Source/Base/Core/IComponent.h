#pragma once

namespace cha
{
	/// @brief コンポーネントのインタフェース
	class IComponent
	{
	public:
		IComponent() = default;
		virtual ~IComponent() = default;
	};
}
