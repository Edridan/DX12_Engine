// this include scene graph and object viewer

#include "engine/Defines.h"

#ifdef WITH_EDITOR

#include "ui/UIWindow.h"

// class predef
class World;
class Actor;
class Transform;
// other linked windows
class UIActorBuilder;

class UISceneBuilder : public UIWindow
{
public:
	UISceneBuilder(UIActorBuilder * i_ActorBuilder = nullptr);
	~UISceneBuilder();

	// management
	void SetWorld(World * i_World);

	// informations
	World * GetWorld() const;
	Actor * GetSelectedActor() const;

private:
	// world
	World *			m_World;

	// internal call
	void			AddEmptyActor(const Transform & i_Transform, const char * i_Name);	// add empty actor to the world
	void			SelectActor(Actor * i_Actor);

	// internal render
	void			DrawActor(Actor * i_Actor);

	// Inherited via UIWindow
	virtual void DrawWindow() override;

	UIActorBuilder *		m_ActorBuilder;
	Actor *					m_SelectedActor;
};

#endif