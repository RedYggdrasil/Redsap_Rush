#pragma once
#include "MDS/Tools/System/IRProgramMemNode.h"

namespace mds
{

	class IRProgramRoot : public mds::IRProgramMemElem
	{
	protected:
		std::shared_ptr<mds::IRProgramMemTreeRoot> m_memoryTreeRoot;
	protected:
		IRProgramRoot();
		/*implicit_virtual*/ ~IRProgramRoot()/*implicit_override*/;

		virtual void ResetMemTreeData() override;
	};
};