#ifndef NEUTRON_TEMPLATE_HEADER_H
#define NEUTRON_TEMPLATE_HEADER_H

namespace Neutron // --------------------------------------------------------------------------------------------------------------
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

} // namespace Neutron ------------------------------------------------------------------------------------------------------------

#endif//NEUTRON_TEMPLATE_HEADER_H
