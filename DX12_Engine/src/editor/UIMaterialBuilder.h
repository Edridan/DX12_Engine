// material builder editor
// this allow to edit and build a material

#pragma once

#include "engine/Defines.h"


#ifdef WITH_EDITOR

#include "ui/UIWindow.h"
#include "ui/UI.h"

class UIMaterialBuilder : public UIWindow
{
public:
	UIMaterialBuilder();
	~UIMaterialBuilder();

	// window management

private:

	// struct
	struct Node
	{
		int     Id;
		char    Name[32];
		ImVec2  Pos, Size;
		float   Value;
		ImVec4  Color;
		int     InputsCount, OutputsCount;

		Node(int id, const char* name, const ImVec2 & pos, float value, const ImVec4 & color, int inputs_count, int outputs_count)
			:Id(id)
			,Pos(pos)
			,Color(color)
			,Value(value)
			,InputsCount(inputs_count)
			,OutputsCount(outputs_count)
		{
			strncpy_s(Name, name, 31);
			Name[31] = '\0';
		}

		inline ImVec2 GetInputSlotPos(int i_SlotNo) const
		{
			return ImVec2(Pos.x, Pos.y + Size.y * ((float)i_SlotNo + 1) / ((float)InputsCount + 1));
		}

		inline ImVec2 GetOutputSlotPos(int i_SlotNo) const
		{
			return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)i_SlotNo + 1) / ((float)OutputsCount + 1));
		}
	};

	struct NodeLink
	{
		int     InputIdx, InputSlot, OutputIdx, OutputSlot;

		NodeLink(int input_idx, int input_slot, int output_idx, int output_slot)
			:InputIdx(input_idx)
			,InputSlot(input_slot)
			,OutputIdx(output_idx)
			,OutputSlot(output_slot)
		{}
	};

	// load/save material
	void	SaveCurrentMaterial(const char * i_OutputFile);
	void	LoadMaterial(const char * i_FilePath);	
	void	ClearCurrentMaterial();

	// internal structure for node management
	

	// Inherited via UIWindow
	virtual void DrawWindow() override;

	ImVector<Node>		m_Nodes;
	ImVector<NodeLink>	m_Links;
};



#endif