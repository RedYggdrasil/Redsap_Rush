#include "MDS/Tools/System/IRProgramRoot.h"

using namespace mds;
IRProgramRoot::IRProgramRoot()
: IRProgramMemElem()
{
	this->m_WPtrParentNode = std::weak_ptr<IRProgramMemNode>();
	this->m_parentElem = nullptr;
	this->m_memoryTreeRoot = mds::IRProgramMemTreeRoot::CreateProgramMemRoot(this);
	this->m_WPtrProgramMemTreeRoot = this->m_memoryTreeRoot;
	this->m_programRoot = this;
	this->m_isMemNode = false;
	this->m_initializedMemElem = true;
}

IRProgramRoot::~IRProgramRoot()
{
}

void mds::IRProgramRoot::ResetMemTreeData()
{
	m_memoryTreeRoot = nullptr;
	mds::IRProgramMemElem::ResetMemTreeData();
}
