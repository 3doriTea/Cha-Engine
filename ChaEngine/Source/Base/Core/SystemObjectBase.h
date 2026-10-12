#pragma once
#include "ObjectBase.h"
#include "SystemRegistry.h"
#include "NullObjectData.h"
#include "ISystemObject.h"

namespace cha
{
	/// @brief システムオブジェクトの基底クラス
	/// @tparam SystemT システムの型
	/// @tparam DataT データ構造体
	template <typename SystemT, typename DataT = NullObjectData>
	class SystemObjectBase : public ObjectBase<DataT>, public ISystemObject
	{
	public:
		/// @brief 参照関係を登録、リンクする
		/// @param _linker リンクするやつ
		virtual void OnDependencyLink(DependencyLinker& _linker) override {}

		/// @brief 遅い更新処理
		void LateUpdate() override {}

	private:
		inline static uint64_t INDEX_ = 
			SystemRegistry::GetSystemIndex<SystemT>();  // システムのインデクス
	};
}
