#pragma once
#include "Dependency/DependencyLinker.h"

namespace cha
{
	/// @brief システムオブジェクトのインタフェース
	class ISystemObject
	{
	public:
		ISystemObject() = default;
		virtual ~ISystemObject() = default;

		/// @brief 遅い更新処理
		virtual void LateUpdate() = 0;

		/// @brief 参照関係を登録、リンクする
		/// @param _linker リンクするやつ
		virtual void OnDependencyLink(DependencyLinker& _linker) = 0;
	};
}
