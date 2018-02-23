// this include scene graph and object viewer

#include "engine/Defines.h"

#ifdef WITH_EDITOR

#include "ui/UIWindow.h"
#include "game/Actor.h"

// class predef
class World;
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
	void			AddActor(const Actor::ActorDesc & i_Desc, const Transform & i_Transform);
	void			DeleteActor(Actor * i_Actor, bool i_DeleteChild);
	void			AttachToParent(Actor * i_Child, Actor * i_Parent);
	void			SelectActor(Actor * i_Actor);

	// internal render
	void			DrawActor(Actor * i_Actor);

	// Inherited via UIWindow
	virtual void	DrawWindow() override;

	// other modules
	UIActorBuilder *		m_ActorBuilder;

	// management
	Actor *					m_SelectedActor;
	Actor *					m_ActorToSetup;


	// internal data
	static const char *			s_ActorSpawnType[];
	static Actor::ActorDesc *	s_ActorDesc;
};

#endif