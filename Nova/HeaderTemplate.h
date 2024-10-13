#ifndef NOVA_TEMPLATE_HEADER_H
#define NOVA_TEMPLATE_HEADER_H

namespace Nova // -----------------------------------------------------------------------------------------------------------------
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

} // namespace Nova ---------------------------------------------------------------------------------------------------------------

#endif//NOVA_TEMPLATE_HEADER_H
