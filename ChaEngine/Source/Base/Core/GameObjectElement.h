#pragma once
#include "../BasePch.h"
#include "GameObject.h"
#include "ObjectBase.h"
#include "IObject.h"
#include "../CoreType/ComponentFlag.h"

namespace cha
{
	/// @brief ゲームオブジェクトのコレクションに保存されるゲームオブジェクトの要素
	struct GameObjectElement
	{
		/// @brief ゲームオブジェクト要素の初期化処理
		GameObjectElement() = default;

		/// @brief ゲームオブジェクト要素の初期化処理
		inline GameObjectElement(
			std::shared_ptr<IObject> _pObject,
			std::type_index _typeIndex,
			const ComponentFlag& _componentFlag) :
			pObject{ std::move(_pObject) },
			typeIndex{ _typeIndex },
			componentFlag{ _componentFlag }
		{
		}

		/// @brief コピー禁止
		/// @param _src 
		GameObjectElement(const GameObjectElement& _src) = delete;
		/// @brief コピー禁止
		/// @param _src 
		/// @return 
		GameObjectElement& operator=(const GameObjectElement& _src) = delete;

		/// @brief ムーブOk
		/// @param _src 
		GameObjectElement(GameObjectElement&& _src) noexcept = default;
		/// @brief ムーブOk
		/// @param _src 
		/// @return 
		GameObjectElement& operator=(GameObjectElement&& _src) noexcept = default;

		std::shared_ptr<IObject> pObject{};         ///< オブジェクトのシェアポインタ
		std::type_index typeIndex{ typeid(void) };  ///< オブジェクトの型インデクス
		ComponentFlag componentFlag{};              ///< コンポーネントの所有状況をフラグ管理
	};
}
