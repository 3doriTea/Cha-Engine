#pragma once
#include "../BasePch.h"
#include "IManager.h"

namespace cha
{
	/// @brief 問題を管理する
	class IssueManager : public IManager
	{
	public:
		IssueManager() = default;
		~IssueManager() = default;

		/// @brief エラーが発生したか
		/// @return 発生した true / false
		inline bool HasError() const { return hasError_; }

		/// @brief 最終エラーメッセージを取得する
		/// @return 最終エラーメッセージ
		inline const std::wstring_view GetLastErrorMessage() const { return lastErrorMassage_; }

		/// @brief エラーを発行する
		/// @param _message エラー内容
		inline void Error(const std::wstring_view _message)
		{
			hasError_ = true;
			lastErrorMassage_ = _message;
		}

	private:
		std::wstring lastErrorMassage_{};  // 最後に発行されたエラーメッセージ
		bool hasError_{ false };          // エラーが発生したか
	};
}
