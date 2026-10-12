#pragma once
#include <Base/Base.h>

namespace Utility::Topological
{
	/// @brief トポロジカルソートでソートする
	/// @param _input 
	/// @param _result 
	void Sort(
		const std::vector<std::vector<size_t>>& _input,
		std::vector<size_t>& _result);
}
