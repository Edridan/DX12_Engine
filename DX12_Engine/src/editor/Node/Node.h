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
	Node(ImVec2 i_Position, const char * i_Name, int i_InputCount, int i_OutputCount, const ImVec4 & i_Color = ImVec4(0.4f, 0.4f, 0.4f, 0.4f));
	~Node();

	// informations
	int					GetId() const;
	const char *		GetName() const;

	// compute offset positions
	ImVec2				GetOutputSlotPos(int i_OutSlot) const;
	ImVec2				GetInputSlotPos(int i_InputSlot) const;

	// link management
	void				PushInputLink(NodeLink * i_Link);
	void				PushOutputLink(NodeLink * i_Link);

	void				PopInputLink(NodeLink * i_Link);
	void				PopOutputLink(NodeLink i_Link);

	void				Draw();
private:

	virtual void	DrawInternal() = 0;	// virtual function called

	// informations
	const int   m_Id;
	char		m_Name[32];
	int			m_InputsCount, m_OutputsCount;

	// rendering
	ImVec2		m_Pos, m_Size;		
	ImVec4		m_Color;

};

#endif