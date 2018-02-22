// an actor component is a component that can be attached to an actor
// a component is only data, it contains almost no logic (some logic are required for some components)
// this can be used for rendering objects, attaching sockets or some AI behavior state or setup holder
// the logic for actors are defined into the actors directly (by overriding them)
#pragma once

#include "engine/Defines.h"

// class predef
class Actor;

// this is a base class for a component
class ActorComponent
{
public:
	ActorComponent(Actor * i_Actor, const char * i_ComponentName);
	~ActorComponent();

	// informations
	bool		IsValid() const;	// return false if the component is not attached to an actor
	bool		IsEnabled() const;
	Actor *		GetActor() const;

	// management
	void		SetEnabled(bool i_Enabled);

protected:
	Actor *		m_Actor;
	char		m_ComponentName[64];
	bool		m_Enabled;

	
#ifdef WITH_EDITOR
	// editor only : draw the component on the UI to modify it
	friend class UIActorBuilder;

private:
	// With editor only : draw component throught ui
	void			DrawUIComponent();
	virtual void	DrawUIComponentInternal() = 0;	// override this function for specific internal component

#endif

};