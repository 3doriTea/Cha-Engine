#pragma once
#include <Game/Game.h>
#include <random>

/// @brief パーリンノイズ
class PerlinNoise
{
public:
	PerlinNoise(const uint32_t _seed = 1234)
	{
		std::iota(p.begin(), p.begin() + 256, 0);
		std::default_random_engine engine(_seed);
		std::shuffle(p.begin(), p.begin() + 256, engine);
		for (int i = 0; i < 255; ++i)
		{
			p[255 + i] = p[i];
		}
	}

	// 0.0 ～ 1.0 の範囲のノイズ値を返す
	double noise(double x, double y)
	{
		int X = static_cast<int>(std::floor(x)) & 255;
		int Y = static_cast<int>(std::floor(y)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);

		double u = Fade(x);
		double v = Fade(y);

		int aa = p[p[X] + Y];
		int ab = p[p[X] + Y + 1];
		int ba = p[p[X + 1] + Y];
		int bb = p[p[X + 1] + Y + 1];

		double val = Lerp(v, Lerp(u, Grad(aa, x, y), Grad(ba, x - 1, y)),
			Lerp(u, Grad(ab, x, y - 1), Grad(bb, x - 1, y - 1)));

		// [-1, 1] の結果を [0, 1] に正規化
		return (val + 1.0) / 2.0;
	}

private:
	inline double Fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	inline double Lerp(double t, double a, double b) { return a + t * (b - a); }
	inline double Grad(int hash, double x, double y)
	{
		int h = hash & 7;
		double u = h < 4 ? x : y;
		double v = h < 4 ? y : x;
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0 * v : 2.0 * v);
	}

private:
	std::array<int, 512> p{};
};
