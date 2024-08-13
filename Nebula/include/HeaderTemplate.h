#ifndef NEBULA_TEMPLATE_HEADER_H
#define NEBULA_TEMPLATE_HEADER_H

namespace Nebula
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

} // namespace Nebula -------------------------------------------------------------------------------------------------------------

#endif//NEBULA_TEMPLATE_HEADER_H
