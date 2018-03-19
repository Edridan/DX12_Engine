// Node editor
// this define a UI node that can be drawed

#pragma once

#include "engine/Defines.h"

#ifdef WITH_EDITOR

#include "ui/UI.h"

class NodeLink;

class Node
{
public:
	// Max slots input/output per node
#define MAX_SLOT		8

	enum ENodeSlotType
	{
		eInt,
		eFloat,
		eVector3,
		eVector4,
		eTexture,

		eNone,
	};

	struct NodeParam
	{
		ENodeSlotType		Type;
		bool				Free;
	};

	struct InputParam : public NodeParam
	{
		
		// initial value for basic types
		union
		{
			int				Int;
			float			Float;
		};
	};

	struct NodeDesc
	{
		// Node desc
		const char *	Name;
		ImVec4 &		Color;

		// count
		int				InputCount;
		int				OutputCount;

		// node definition
		ENodeSlotType	InputSlots[MAX_SLOT];
		ENodeSlotType	OutputSlots[MAX_SLOT];

	};

	Node(ImVec2 i_Postion, const NodeDesc & i_Desc);
	~Node();

	// informations
	int					GetId() const;
	const char *		GetName() const;
	bool				IsInputFree(int i_Slot) const;
	bool				IsOutputFree(int i_Slot) const;
	ENodeSlotType		GetInputNodeType(int i_Slot) const;
	ENodeSlotType		GetOutputNodeType(int i_Slot) const;
	NodeLink *			GetInputLink(int i_Slot) const;
	NodeLink *			GetOutputLink(int i_Slot) const;

	// compute offset positions (input/output slot)
	virtual ImVec2		GetOutputSlotPos(int i_OutSlot) const;	// can be overriden for specific node (as behavior tree nodes for example)
	virtual ImVec2		GetInputSlotPos(int i_InputSlot) const;

	// link management
	void				PushInputLink(NodeLink * i_Link, int i_InputSlot);
	void				PushOutputLink(NodeLink * i_Link, int i_OutSlot);

	void				PopInputLink(int i_InputSlot);
	void				PopOutputLink(int i_OutputSlot);

	void				PopInputLink(NodeLink * i_Link);
	void				PopOutputLink(NodeLink * i_Link);

	void				Draw();
private:

	virtual void	DrawInternal() = 0;	// virtual function called

	// informations
	const int   m_Id;
	char		m_Name[32];
	int			m_InputsCount, m_OutputsCount;

	// slots for the node
	InputParam *	m_InputSlots;
	NodeParam *		m_OutputSlots;

	// links
	NodeLink **			m_InputLinks;
	NodeLink **			m_OutputLinks;

	// rendering
	ImVec2		m_Pos, m_Size;
	ImVec4		m_Color;

};

#endif