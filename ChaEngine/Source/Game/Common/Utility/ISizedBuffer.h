#pragma once
#include <cstddef>

/// @brief サイズを持つバッファのインタフェース
class ISizedBuffer
{
public:
	virtual ~ISizedBuffer() = default;

	/// @brief 生データへのポインタを取得
	/// @return 生データへのポインタ
	virtual void* Data() = 0;
	/// @brief 生データへのポインタを取得
	/// @return 生データへのポインタ
	virtual const void* Data() const = 0;

	/// @brief 要素数を取得する
	/// @return 要素数
	virtual std::size_t Size() const = 0;

	/// @brief 全体のバイト数を取得する
	/// @return 全体のバイト数 ( Size() * sizeof(T) )
	virtual std::size_t ByteSize() const = 0;

	/// @brief サイズ変更
	/// @param _size 新しいサイズ
	virtual void Resize(const size_t _size) = 0;
	
	/// @brief サイズをバイト単位で変更
	/// @param _size 新しいサイズ(バイト数)
	virtual void ResizeBytes(const size_t _sizeBytes) = 0;
};