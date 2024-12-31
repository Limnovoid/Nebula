#ifndef NEBULA_RANDOM_H
#define NEBULA_RANDOM_H

namespace Nebula // ---------------------------------------------------------------------------------------------------------------
{

class Random
{
public:
	template<typename T>
	static void Shuffle(std::vector<T> & vector);

	/// <returns> A vector populated with the integers [min, max] in a random order. </returns>
	static std::vector<size_t> GetRandomSequence(size_t const min, size_t const max);

	/// <returns> A vector populated with the integers [0, max] in a random order. </returns>
	static std::vector<size_t> GetRandomSequence(size_t const max);

	template<IsInt T = size_t>
	T Integer(T min, T max);

private:
	static std::random_device	g_randomDevice;
	static std::mt19937			g_generator;
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
inline void Random::Shuffle(std::vector<T> & vector)
{
	std::shuffle(vector.begin(), vector.end(), g_generator);
}

// --------------------------------------------------------------------------------------------------------------------------------

inline std::vector<size_t> Random::GetRandomSequence(size_t const max)
{
	return GetRandomSequence(0, max);
}

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_RANDOM_H
