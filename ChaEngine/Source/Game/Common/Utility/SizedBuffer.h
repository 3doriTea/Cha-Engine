#pragma once
#include <vector>
#include <utility>
#include <initializer_list>
#include "ISizedBuffer.h"

template<typename T>
class SizedBuffer : public ISizedBuffer
{
	// std::memcpy で安全に複製できる型（プリミティブ型や単純な構造体）のみ許可
	static_assert(std::is_trivially_copyable_v<T>,
		"SizedBuffer<T> には trivially copyable な型（プリミティブ型や単純な構造体）のみ指定可能です。std::string 等は使用できません。");

public:
	using value_type = T;
	using size_type = typename std::vector<T>::size_type;

	// ---------------------------------------------------------
	// ISizedBuffer インターフェースの実装
	// ---------------------------------------------------------
	void* Data() override { return buffer_.data(); }
	const void* Data() const override { return buffer_.data(); }

	void Resize(const size_t _size) override { buffer_.resize(_size); }
	void Resize(const size_type _size, const value_type _fillValue) { buffer_.resize(_size, _fillValue); }
	void ResizeBytes(const size_t _sizeBytes) override
	{
		constexpr size_t elementSize = sizeof(value_type);
		// (A + B - 1) / B で切り上げ除算
		const size_t elementCount = (_sizeBytes + elementSize - 1) / elementSize;
		buffer_.resize(elementCount);
	}

	std::size_t Size() const override { return buffer_.size(); }
	std::size_t ByteSize() const override { return buffer_.size() * sizeof(T); }

	// ---------------------------------------------------------
	// 型安全なアクセス用メソッド
	// ---------------------------------------------------------
	T* TypedData() { return buffer_.data(); }
	const T* TypedData() const { return buffer_.data(); }

	// ---------------------------------------------------------
	// コンストラクタ群（波括弧初期化 & アンダースコア引数）
	// ---------------------------------------------------------
	SizedBuffer() = default;

	explicit SizedBuffer(const size_type _count)
		: buffer_{ _count }
	{
	}

	SizedBuffer(const size_type _count, const T& _value)
		: buffer_{ _count, _value }
	{
	}

	template <typename InputIt>
	SizedBuffer(InputIt _first, InputIt _last)
		: buffer_{ _first, _last }
	{
	}

	SizedBuffer(const SizedBuffer& _other)
		: buffer_{ _other.buffer_ }
	{
	}

	SizedBuffer(SizedBuffer&& _other) noexcept
		: buffer_{ std::move(_other.buffer_) }
	{
	}

	SizedBuffer(std::initializer_list<T> _init)
		: buffer_{ _init }
	{
	}

	explicit SizedBuffer(const std::vector<T>& _vec)
		: buffer_{ _vec }
	{
	}

	explicit SizedBuffer(std::vector<T>&& _vec) noexcept
		: buffer_{ std::move(_vec) }
	{
	}

	// 代入演算子
	SizedBuffer& operator=(const SizedBuffer&) = default;
	SizedBuffer& operator=(SizedBuffer&&) noexcept = default;
	SizedBuffer& operator=(std::initializer_list<T> _ilist)
	{
		buffer_ = _ilist;
		return *this;
	}

	// アクセサ・イテレータ
	auto begin() { return buffer_.begin(); }
	auto end() { return buffer_.end(); }
	auto begin() const { return buffer_.begin(); }
	auto end() const { return buffer_.end(); }

	T& operator[](size_type _i) { return buffer_[_i]; }
	const T& operator[](size_type _i) const { return buffer_[_i]; }

	const std::vector<T>& get_internal_vector() const { return buffer_; }

private:
	std::vector<T> buffer_{};
};
