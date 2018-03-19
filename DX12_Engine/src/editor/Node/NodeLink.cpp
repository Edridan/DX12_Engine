#include "NodeLink.h"

#include "Node.h"

NodeLink::NodeLink()
	:m_InputNode(nullptr)
	,m_OutputNode(nullptr)
	,m_InputSlot(0)
	,m_OutputSlot(0)
{
}

NodeLink::~NodeLink()
{
	// Unlink the node if necessary
	UnlinkInput();
	UnlinkOutput();
}

int NodeLink::GetInputSlot() const
{
	return m_InputSlot;
}

int NodeLink::GetOutputSlot() const
{
	return m_OutputSlot;
}

Node * NodeLink::GetInputNode() const
{
	return m_InputNode;
}

Node * NodeLink::GetOutputNode() const
{
	return m_OutputNode;
}

void NodeLink::UnlinkOutput()
{
	if (m_OutputNode != nullptr)
	{
		m_OutputNode->PopInputLink(this);
	}
}

void NodeLink::UnlinkInput()
{
	if (m_InputNode != nullptr)
	{
		m_InputNode->PopOutputLink(this);
	}
}
