#include "Node.h"

#include <string.h>

Node::Node(ImVec2 i_Position, const char * i_Name, int i_InputCount, int i_OutputCount, const ImVec4 & i_Color)
	:m_Id((int)this)
	,m_InputsCount(i_InputCount)
	,m_OutputsCount(i_OutputCount)
	,m_Color(i_Color)
{
	strncpy_s(m_Name,i_Name, 31);
	m_Name[31] = '\0';
}

Node::~Node()
{
	delete[]	m_Name;
}

int Node::GetId() const
{
	return m_Id;
}

const char * Node::GetName() const
{
	return m_Name;
}

ImVec2 Node::GetInputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_InputsCount + 1));
}

ImVec2 Node::GetOutputSlotPos(int i_SlotNo) const
{
	return ImVec2(m_Pos.x + m_Size.x, m_Pos.y + m_Size.y * ((float)i_SlotNo + 1) / ((float)m_OutputsCount + 1));
}

void Node::Draw()
{
}
