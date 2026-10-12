#pragma once

enum struct ENetSendMode
{
	Reliable,     ///< 確実に相手に送る かけたら困る
	Unreliable,   ///< 届かなかったらそのまま捨てるが順序は正しく
	Unsequenced,  ///< 確実性も順序も関係ない
};
