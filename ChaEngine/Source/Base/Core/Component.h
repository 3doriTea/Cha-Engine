#pragma once

namespace cha
{
	/// @brief コンポーネントをラップするゲームオブジェクトメンバー用のコンポーネント
	/// @tparam T コンポーネント型
	template<typename T>
	class Component : public T
	{
	public:
		/// @brief コンポーネントの初期化
		/// @tparam ...Args コンポーネント構造体のコンストラクタ引数の型
		/// @param ..._args コンポーネント構造体のコンストラクタ引数に渡す引数
		template<typename... Args>
			requires std::constructible_from<T, Args...>
		Component(Args&&... _args);

		/// @brief 解放処理
		virtual ~Component();

		/// @brief コピー禁止
		/// @param _src 
		Component(const Component& _src) = delete;
		/// @brief コピー禁止
		/// @param _src 
		/// @return 
		Component& operator=(const Component& _src) = delete;

		/// @brief ムーブOk
		/// @param _src 
		Component(Component&& _src) noexcept = default;
		/// @brief ムーブOk
		/// @param _src 
		/// @return 
		Component& operator=(Component&& _src) noexcept = default;
	};
}

#include "Component.inl"
