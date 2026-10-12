#pragma once

namespace cha
{
	/// @brief コンポーネントプールのインタフェース
	class IComponentPool
	{
	public:
		/// @brief コンポーネントプールの初期化処理
		IComponentPool() = default;
		/// @brief コンポーネントプールの解放処理
		virtual ~IComponentPool() = default;

		/// @brief 他コンポーネントプールと同調するためにサイズを指定する
		/// @param _size 指定のサイズ
		virtual void PaddingSize(const size_t _size) = 0;

		/// @brief 指定のコンポーネントを末端と入れ替える
		/// @param _registryIndex レジストリインデクス
		virtual void SwapToBack(const size_t _registryIndex) = 0;

		/// @brief 末端のコンポーネントをポイ捨てする
		virtual void PopBack() = 0;
	};
}
