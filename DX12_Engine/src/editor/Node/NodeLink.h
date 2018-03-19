// Node editor
// Node link between 2 nodes definition
// here the representation of a node link :
//	+--------+						+--------+
//	| Node 1 |Output---------->Input| Node 2 |
//	+--------+						+--------+

#pragma once

#include "engine/Defines.h"

class Node;

#ifdef WITH_EDITOR

class NodeLink
{
public:
	NodeLink();
	~NodeLink();

	// informations
	int		GetInputSlot() const;
	int		GetOutputSlot() const;
	Node *	GetInputNode() const;
	Node *	GetOutputNode() const;
	// unlink node
	void	UnlinkOutput();
	void	UnlinkInput();

	friend class Node;
private:
	// Nodes links
	Node *	m_InputNode;
	Node *	m_OutputNode;
	// slots
	int		m_InputSlot;
	int		m_OutputSlot;

};

#endif