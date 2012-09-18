
#include "ComponentMenuItem.h"


ComponentMenuItem::ComponentMenuItem(ComponentMenuItem * parent)
{
	m_name = "";
	m_parent = parent;
	m_priority = 2147483647;
	m_styleOnTree = "";
}

ComponentMenuItem::~ComponentMenuItem(void)
{
}
