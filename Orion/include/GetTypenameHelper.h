#ifndef NEBULA_ORION_GET_TYPENAME_HELPER_H
#define NEBULA_ORION_GET_TYPENAME_HELPER_H

namespace Nebula
{

namespace Orion
{

/// <summary>
/// Class for getting a c-style string representation of a typename.
/// Stores the string in a statically allocated char buffer. Computes and copies to the buffer on every call to Get().
/// </summary>
template <typename T>
class GetTypenameHelper
{
public:
	/// <summary> Computed and get a pointer to the c-style typename string. </summary>
	static char const* Get();

	/// <summary> Function-call operator, equivalent to calling Get() on this object. </summary>
	char const* operator()() const;

private:
	static constexpr size_t		FRONT_LENGTH = sizeof("Nebula::Orion::GetTypeNameHelper<") - 1;
	static constexpr size_t		BACK_LENGTH = sizeof(">::Get") - 1;
};

// --------------------------------------------------------------------------------------------------------------------------------

template <typename T>
char const* GetTypenameHelper<T>::Get()
{
	static const size_t nameLength = sizeof(__FUNCTION__) - FRONT_LENGTH - BACK_LENGTH - 1;
	static char typeName[nameLength + 1] = {};

	memcpy(typeName, __FUNCTION__ + FRONT_LENGTH, nameLength);

	return typeName;
}

// --------------------------------------------------------------------------------------------------------------------------------

template <typename T>
char const* GetTypenameHelper<T>::operator()() const
{
	return Get();
}

} // namespace Orion --------------------------------------------------------------------------------------------------------------

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_ORION_GET_TYPENAME_HELPER_H
