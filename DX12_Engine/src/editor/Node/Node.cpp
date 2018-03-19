#include "Node.h"

#include "NodeLink.h"
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
	m_InputLinks	= new NodeLink *[m_InputsCount];
	m_OutputLinks	= new NodeLink *[m_OutputsCount];

	// copy the slot
	m_InputSlots	= new InputParam[m_InputsCount];
	m_OutputSlots	= new NodeParam[m_OutputsCount];

	// generate inputs
	for (int i = 0; i < m_InputsCount; ++i)
	{
		m_InputSlots[i].Type = i_Desc.InputSlots[i];
		m_InputSlots[i].Free = true;

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
		m_OutputSlots[i].Type = i_Desc.OutputSlots[i];
		m_OutputSlots[i].Free = true;

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
	if (i_Slot >= m_InputsCount)
	{
		ASSERT_ERROR("");
		return false;
	}
	return m_InputSlots[i_Slot].Free;
}

bool Node::IsOutputFree(int i_Slot) const
{
	if (i_Slot >= m_OutputsCount)
	{
		ASSERT_ERROR("");
		return false;
	}
	return m_OutputSlots[i_Slot].Free;
}

ImVec2 Node::GetInputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_InputsCount + 1));
}

void Node::PushInputLink(NodeLink * i_Link, int i_InputSlot)
{
	if (IsInputFree(i_InputSlot))
	{
		// update link
		i_Link->m_InputNode = this;
		i_Link->m_InputSlot = i_InputSlot;
		// update param
		m_InputSlots[i_InputSlot].Free = false;
		m_InputLinks[i_InputSlot] = i_Link;
	}
}

void Node::PushOutputLink(NodeLink * i_Link, int i_OutSlot)
{
	if (IsOutputFree(i_OutSlot))
	{
		// update link
		i_Link->m_InputNode = this;
		i_Link->m_InputSlot = i_OutSlot;
		// update param
		m_OutputSlots[i_OutSlot].Free = false;
		m_OutputLinks[i_OutSlot] = i_Link;
	}
}

void Node::PopInputLink(int i_InputSlot)
{
	if (i_InputSlot >= m_InputsCount)
	{
		ASSERT_ERROR("");
		return;
	}
	
	if (m_InputLinks[i_InputSlot] != nullptr)
	{
		m_InputLinks[i_InputSlot]->m_InputNode = nullptr;
		m_InputLinks[i_InputSlot]->m_InputSlot = -1;
		m_InputLinks[i_InputSlot] = nullptr;
	}
}

void Node::PopOutputLink(int i_OutputSlot)
{
	if (i_OutputSlot >= m_OutputsCount)
	{
		ASSERT_ERROR("");
		return;
	}

	if (m_OutputLinks[i_OutputSlot] != nullptr)
	{
		m_OutputLinks[i_OutputSlot]->m_OutputNode = nullptr;
		m_OutputLinks[i_OutputSlot]->m_OutputSlot = -1;
		m_OutputLinks[i_OutputSlot] = nullptr;
	}
}

void Node::PopInputLink(NodeLink * i_Link)
{

}

void Node::PopOutputLink(NodeLink * i_Link)
{
}

Node::ENodeSlotType Node::GetInputNodeType(int i_Slot) const
{
	if (i_Slot >= m_InputsCount)
	{
		ASSERT_ERROR("");
		return eNone;
	}
	return m_InputSlots[i_Slot].Type;
}

Node::ENodeSlotType Node::GetOutputNodeType(int i_Slot) const
{
	if (i_Slot >= m_OutputsCount)
	{
		ASSERT_ERROR("");
		return eNone;
	}
	return m_OutputSlots[i_Slot].Type;
}

NodeLink * Node::GetInputLink(int i_Slot) const
{
	if (i_Slot >= m_InputsCount)
	{
		ASSERT_ERROR("");
		return nullptr;
	}
	return m_InputLinks[i_Slot];
}

NodeLink * Node::GetOutputLink(int i_Slot) const
{
	if (i_Slot >= m_OutputsCount)
	{
		ASSERT_ERROR("");
		return nullptr;
	}
	return m_OutputLinks[i_Slot];
}

ImVec2 Node::GetOutputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x + m_Size.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_OutputsCount + 1));
}

void Node::Draw()
{
}
