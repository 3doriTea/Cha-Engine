#pragma once

namespace cha
{
	/// @brief マネージャーのインタフェース
	class IManager
	{
	public:
		IManager() = default;
		virtual ~IManager() = default;

		/// @brief 解放処理
		virtual void Release() {}
	};
}
