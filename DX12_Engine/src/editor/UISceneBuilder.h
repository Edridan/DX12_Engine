// this include scene graph and object viewer

#include "engine/Engine.h"

#ifdef WITH_EDITOR

#include "ui/UIWindow.h"

// class predef
class World;
class Actor;
class Transform;

class UISceneBuilder : public UIWindow
{
public:
	UISceneBuilder();
	~UISceneBuilder();

	// management
	void SetWorld(World * i_World);
	World * GetWorld() const;



private:
	World *			m_World;

	std::vector<Actor *>	m_RootActors;
	std::vector<Actor *>	m_Actors;

	// Inherited via UIWindow
	virtual void DrawWindow() override;
	
	// internal callbacks
	void		DrawActorObject(Actor * i_Actor);
	void		DrawTransform(Transform * i_Transform);
	void		DrawVector(DirectX::XMFLOAT3 & i_Vector, const char * i_Name, DirectX::XMFLOAT3 * o_Save = nullptr);
};

#endif