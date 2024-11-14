#include "MDS/Tools/System/IRProgramMemNode.h"
#include "MDS/Tools/System/IRProgramRoot.h"

using namespace mds;

IRProgramMemElem::IRProgramMemElem()
	: m_initializedMemElem(false), m_isMemNode(false)
{
}

IRProgramMemElem::IRProgramMemElem(IRProgramMemElem* InParentElem)
	: m_initializedMemElem(false), m_isMemNode(false)
{
	this->InitMemElem(InParentElem);
}

IRProgramMemElem::IRProgramMemElem(std::shared_ptr<IRProgramMemNode> InParentNode)
	: m_initializedMemElem(false), m_isMemNode(false)
{
	this->InitMemElem(InParentNode);
}

IRProgramMemElem::~IRProgramMemElem()
{
}

IRProgramMemElem* IRProgramMemElem::InitMemElem(std::shared_ptr<IRProgramMemNode> InParentNode)
{
	m_WPtrParentNode = InParentNode;
	m_parentElem = InParentNode.get();
	m_WPtrProgramMemTreeRoot = InParentNode->m_WPtrProgramMemTreeRoot;
	m_programRoot = InParentNode->m_programRoot;
	m_initializedMemElem = true;
	return this;

}
IRProgramMemElem* IRProgramMemElem::InitMemElem(IRProgramMemElem* InParentElem)
{
	if (InParentElem->m_isMemNode)
	{
		return InitMemElem(static_cast<IRProgramMemNode*>(InParentElem)->LockSelf());
	}
	else
	{
		m_WPtrParentNode = InParentElem->m_WPtrParentNode;
		m_parentElem = InParentElem;
		m_WPtrProgramMemTreeRoot = InParentElem->m_WPtrProgramMemTreeRoot;
		m_programRoot = InParentElem->m_programRoot;
		m_initializedMemElem = true;
		return this;
	}
}

void IRProgramMemElem::ResetMemTreeData()
{
	m_initializedMemElem = false;
	m_WPtrParentNode = std::weak_ptr<IRProgramMemNode>();
	m_parentElem = nullptr;
	m_WPtrProgramMemTreeRoot = std::weak_ptr<IRProgramMemTreeRoot>();
	m_programRoot = nullptr;
}

IRProgramMemNode::IRProgramMemNode()
: IRProgramMemElem()
{
	m_isMemNode = true;
}

IRProgramMemNode::~IRProgramMemNode()
{
}

std::shared_ptr<IRProgramMemNode> IRProgramMemNode::InitMemNode(std::shared_ptr<IRProgramMemNode> InParentNode, std::shared_ptr<IRProgramMemNode> InSelfNode)
{
	m_WPtrSelfNode = InSelfNode;
	this->InitMemElem(InParentNode);
	return InSelfNode;
}

std::shared_ptr<IRProgramMemNode> IRProgramMemNode::InitMemNode(IRProgramMemElem* InParentElem, std::shared_ptr<IRProgramMemNode> InSelfNode)
{
	m_WPtrSelfNode = InSelfNode;
	this->InitMemElem(InParentElem);
	return InSelfNode;
}

void mds::IRProgramMemNode::ResetMemTreeData()
{
	m_WPtrSelfNode = std::weak_ptr<IRProgramMemNode>();
	mds::IRProgramMemElem::ResetMemTreeData();
}

mds::IRProgramMemTreeRoot::IRProgramMemTreeRoot()
 : IRProgramMemNode()
{
}

mds::IRProgramMemTreeRoot::~IRProgramMemTreeRoot()
{
}

void mds::IRProgramMemTreeRoot::ResetMemTreeData()
{
	mds::IRProgramMemNode::ResetMemTreeData();
}
#include <iostream>
std::shared_ptr<IRProgramMemTreeRoot> mds::IRProgramMemTreeRoot::CreateProgramMemRoot(IRProgramRoot* InProgramRoot)
{
	std::shared_ptr<IRProgramMemTreeRoot> memRoot = std::make_shared<IRProgramMemTreeRoot>();
	std::cout << "Before " << (bool)memRoot << std::endl;
	memRoot->m_WPtrParentNode = std::weak_ptr<IRProgramMemNode>();
	std::cout << "After" << std::endl;
	memRoot->m_parentElem = nullptr;
	std::cout << "After2" << std::endl;
	memRoot->m_WPtrProgramMemTreeRoot = memRoot;
	memRoot->m_programRoot = InProgramRoot;
	memRoot->m_WPtrSelfNode = memRoot;
	memRoot->m_isMemNode = true;
	memRoot->m_initializedMemElem = true;

	return memRoot;
}
