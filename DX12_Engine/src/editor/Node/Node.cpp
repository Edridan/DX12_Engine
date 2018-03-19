#include "Node.h"

#include "engine/Debug.h"
#include <string.h>

Node::Node(ImVec2 i_Postion, const NodeDesc & i_Desc)
	:m_Id((int)this)
	, m_InputsCount(i_Desc.InputCount)
	, m_OutputsCount(i_Desc.OutputCount)
	, m_Color(i_Desc.Color)
{
	strncpy_s(m_Name, i_Desc.Name, 31);
	m_Name[31] = '\0';

	// generate array
	m_InputLinks = new NodeLink *[m_InputsCount];
	m_OutputLinks = new NodeLink *[m_OutputsCount];

	// copy the slot
	m_InputSlots = new InputSlot[m_InputsCount];
	m_OutputSlots = new ENodeSlotType[m_OutputsCount];

	// generate inputs
	for (int i = 0; i < m_InputsCount; ++i)
	{
		m_InputSlots[i].Type = i_Desc.InputSlots[i];

		switch (m_InputSlots[i].Type)
		{
		case eFloat:	m_InputSlots[i].Float = 0.f;	break;
		case eInt:		m_InputSlots[i].Int = 0;		break;
		default:	break;
		}

		// initialize input links
		m_InputLinks[i] = nullptr;
	}

	// generate outputs
	for (int i = 0; i < m_OutputsCount; ++i)
	{
		m_OutputSlots[i] = i_Desc.OutputSlots[i];

		// initialize output links
		m_OutputLinks[i] = nullptr;
	}
}

Node::~Node()
{
	delete[]	m_Name;
	delete[]	m_InputSlots;
	delete[]	m_OutputSlots;
}

int Node::GetId() const
{
	return m_Id;
}

const char * Node::GetName() const
{
	return m_Name;
}

bool Node::IsInputFree(int i_Slot) const
{
	return false;
}

ImVec2 Node::GetInputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_InputsCount + 1));
}

void Node::PushInputLink(NodeLink * i_Link, int i_InputSlot)
{
}

void Node::PushOutputLink(NodeLink * i_Link, int i_OutSlot)
{
}

void Node::PopInputLink(NodeLink * i_Link)
{
}

void Node::PopOutputLink(NodeLink * i_Link)
{
}

bool Node::IsOutputFree(int i_Slot) const
{
	return false;
}

Node::ENodeSlotType Node::GetInputNodeType(int i_Slot) const
{
	if (i_Slot < m_InputsCount)
	{
		ASSERT_ERROR("");
		return eNone;
	}
	return m_InputSlots[i_Slot].Type;
}

Node::ENodeSlotType Node::GetOutputNodeType(int i_Slot) const
{
	if (i_Slot < m_OutputsCount)
	{
		ASSERT_ERROR("");
		return eNone;
	}
	return m_OutputSlots[i_Slot];
}

NodeLink * Node::GetInputLink(int i_Slot) const
{
	return nullptr;
}

NodeLink * Node::GetOutputLink(int i_Slot) const
{
	return nullptr;
}

ImVec2 Node::GetOutputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x + m_Size.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_OutputsCount + 1));
}

void Node::Draw()
{
}
