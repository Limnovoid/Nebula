#ifndef NEBULA_ORION_TEMPLATE_HEADER_H
#define NEBULA_ORION_TEMPLATE_HEADER_H

namespace Nebula
{

namespace Orion
{

template<typename T>
class TemplateClass
{
public:
	TemplateClass();
};

// --------------------------------------------------------------------------------------------------------------------------------

template<typename T>
TemplateClass<T>::TemplateClass()
{
}

} // namespace Orion

} // namespace Nebula

#endif//NEBULA_ORION_TEMPLATE_HEADER_H
