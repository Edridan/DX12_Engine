#pragma once

#include "engine/Defines.h"

#ifdef WITH_EDITOR

#include <DirectXMath.h>
#include "ui/UIWindow.h"

// class predef
class Actor;
class Transform;

class UIActorBuilder : public UIWindow
{
public:
	UIActorBuilder();
	~UIActorBuilder();

	// management
	void	SetActor(Actor * i_Actor);
	Actor *		GetActor() const;

private:
	// internal callbacks
	void		DrawTransform(Transform * i_Transform);
	void		DrawVector(DirectX::XMFLOAT3 & i_Vector, const char * i_Name, DirectX::XMFLOAT3 * o_Save = nullptr);

	// Inherited via UIWindow
	virtual void DrawWindow() override;

	Actor *		m_Actor;
};

#endif