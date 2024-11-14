#pragma once
#include "MDS/Defines/MDS.h"
#include <memory>
namespace mds
{
	class IRProgramMemNode;
	class IRProgramRoot;
	
	class IRProgramMemTreeRoot;

	class IRProgramMemElem
	{
	protected:
		bool m_initializedMemElem;
		bool m_isMemNode;
		/// <summary>
		/// WeakPtr to parent/Owning Node
		/// </summary>
		std::weak_ptr<IRProgramMemNode> m_WPtrParentNode;
		/// <summary>
		/// Direct acess to parent adress, lifespan depend of m_WPtrParentNode.expired() state
		/// if parentElem is a MemNode, then the m_parentElem point to the same adress as m_WPtrParentNode.lock().get() does.
		/// </summary>
		IRProgramMemElem* m_parentElem;

		/// <summary>
		/// WeakPtr to the IRProgramMemTreeRoot, this enable a ProgramRoot to offer a view to it's memory live state, whenever it's a Heap or Stack Root
		/// </summary>
		std::weak_ptr<IRProgramMemTreeRoot> m_WPtrProgramMemTreeRoot;
		/// <summary>
		/// Direct acess to program adress, if in doubt about program state, use m_WPtrProgramRoot
		/// </summary>
		IRProgramRoot* m_programRoot;

	protected:
		IRProgramMemElem();
		IRProgramMemElem(IRProgramMemElem* InParentElem);
		IRProgramMemElem(std::shared_ptr<IRProgramMemNode> InParentNode);
		virtual ~IRProgramMemElem();

	protected:
		IRProgramMemElem* InitMemElem(std::shared_ptr<IRProgramMemNode> InParentNode);
		IRProgramMemElem* InitMemElem(IRProgramMemElem* InParentElem);
		virtual void ResetMemTreeData();
	
	public:
		inline bool LiveTree() const { return ((bool)this) && !m_WPtrParentNode.expired(); }

		template<class TProgramRoot = mds::IRProgramRoot>
		TProgramRoot* GetSafeRoot() const
		{
			if (LiveTree())
			{
				return static_cast<TProgramRoot*>(m_programRoot);
			}
			return nullptr;
		}

		template<class TProgramRoot = mds::IRProgramRoot>
		TProgramRoot* GetRoot() const
		{
			return static_cast<TProgramRoot*>(m_programRoot);
		}

	};

	class IRProgramMemNode : public IRProgramMemElem
	{
	protected:
		/// <summary>
		/// Weak SelfPtr to create child MemNode
		/// </summary>
		std::weak_ptr<IRProgramMemNode> m_WPtrSelfNode;
	protected:
		IRProgramMemNode();
		R_VIRTUAL_IMPLICIT ~IRProgramMemNode() R_OVERRIDE_IMPLICIT;
	protected:
		std::shared_ptr<IRProgramMemNode> InitMemNode(std::shared_ptr<IRProgramMemNode> InParentNode, std::shared_ptr<IRProgramMemNode> InSelfNode);
		std::shared_ptr<IRProgramMemNode> InitMemNode(IRProgramMemElem* InParentElem, std::shared_ptr<IRProgramMemNode> InSelfNode);

		virtual void ResetMemTreeData() override;

	public:
		template<class TMemNode = mds::IRProgramMemNode>
		std::shared_ptr<TMemNode> LockSelf()
		{
			if (std::shared_ptr<IRProgramMemNode> SPtrSelf = m_WPtrSelfNode.lock())
			{
				return std::static_pointer_cast<TMemNode>(SPtrSelf);
			}
			return nullptr;
		}
	};

	class IRProgramMemTreeRoot : public IRProgramMemNode
	{
	public:
		IRProgramMemTreeRoot();
		R_VIRTUAL_IMPLICIT ~IRProgramMemTreeRoot() R_OVERRIDE_IMPLICIT;
	protected:
		virtual void ResetMemTreeData() override;
	public:
		static std::shared_ptr<IRProgramMemTreeRoot> CreateProgramMemRoot(IRProgramRoot* InProgramRoot);
	};
};