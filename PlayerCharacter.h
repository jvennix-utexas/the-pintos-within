#ifndef __PlayerCharacter_h_
#define __PlayerCharacter_h_

#include "common.h"

#include "GameObject.h"
#include "FPSBoxController.h"
#include "PlayerBox.h"
#include "Mesh.h"
#include "Weapon.h"
#include "Debouncer.h"
#include "GuiManager.h"
// #include "HitBox.h"

#include <OgreAnimationState.h>

#define WEAPON_NUM 5
#define HAIR_NUM 5

class HitBox;
class Weapon;
class Hair;

class PlayerCharacter : GameObject
{
protected:
	void regen_health();

public:
	uint32_t player_id;

	uint32_t team_id;

	bool is_yourself;

	bool in_pinto_form;
	
	bool is_dead;
	bool is_shooting;
	bool is_moving;
	bool is_running;
	bool is_idle;
	bool is_reloading;
	bool is_jet_packing;
	bool is_jumping;
	bool is_invisible;

	float run_animation_time;
	float shoot_animation_time;
	float idle_animation_time;
	float reload_animation_time;
	float jump_animation_time;
	float head_animation_time;
	float die_animation_time;

	Ogre::AnimationState *running_animation_state;
	Ogre::AnimationState *idle_animation_state;
	Ogre::AnimationState *shooting_animation_states[WEAPON_NUM];
	Ogre::AnimationState *reload_animation_states[WEAPON_NUM];
	Ogre::AnimationState *jumping_animation_state;
	Ogre::AnimationState *head_animation_state;
	Ogre::AnimationState *die_animation_state;

	Ogre::AnimationState *jet_pack_running_animation_state;
	Ogre::AnimationState *jet_pack_idle_animation_state;
	Ogre::AnimationState *jet_pack_shooting_animation_states[WEAPON_NUM];
	Ogre::AnimationState *jet_pack_reload_animation_states[WEAPON_NUM];
	Ogre::AnimationState *jet_pack_jumping_animation_state;
	Ogre::AnimationState *jet_pack_die_animation_state;

	Ogre::AnimationState *hair_running_animation_state;
	Ogre::AnimationState *hair_idle_animation_state;
	Ogre::AnimationState *hair_shooting_animation_state;
	Ogre::AnimationState *hair_reload_animation_state;
	Ogre::AnimationState *hair_jumping_animation_state;
	Ogre::AnimationState *hair_die_animation_state;

	Ogre::AnimationState *current_jet_pack_reload_animation_state;
	Ogre::AnimationState *current_jet_pack_shooting_animation_state;

	Ogre::AnimationState *current_hair_reload_animation_state;
	Ogre::AnimationState *current_hair_shooting_animation_state;

	Ogre::AnimationState *current_reload_animation_state;
	Ogre::AnimationState *current_shooting_animation_state;

	Ogre::AnimationState *weapon_running_animation_state;
	Ogre::AnimationState *weapon_idle_animation_state;
	Ogre::AnimationState *weapon_shooting_animation_state;
	Ogre::AnimationState *weapon_reload_animation_state;
	Ogre::AnimationState *weapon_jumping_animation_state;
	Ogre::AnimationState *weapon_die_animation_state;

	int max_health;
	int health;

	int health_regen;
	double health_regen_rate;

	FPSBoxController* controller;

	Mesh* mesh;
	Mesh* pinto_mesh;

	PlayerBox* jet_pack;

	Weapon* weapons[WEAPON_NUM];
	Weapon* current_weapon;

	Hair* hairs[HAIR_NUM];
	Hair* current_hair;

	HitBox* head_box;
	HitBox* body_box;

	HitBox* pinto_box;

	PlayerBox* jet_pack_shoot_pos1;
	PlayerBox* jet_pack_shoot_pos2;

	Debouncer *walk_sound_debouncer;
	Debouncer *jump_debouncer;

	Debouncer *jet_pack_sound_debouncer;
	Debouncer *jet_pack_particle_debouncer;

	Debouncer *health_regen_debouncer;

	PlayerCharacter(bool, Scene*, std::string,
	float, float, float, float, float, float, float,
	float, float, float, uint32_t, uint32_t, bool);

	virtual ~PlayerCharacter();

	virtual void update();
	virtual void changeWeapon(int);
	virtual void changeHair(int);

	virtual void changeToPinto();
	void setToOriginalStatus();

	void switchToBlueTeam();

	Transform* transform;
	Transform* tr;
};

#endif // #ifndef __PlayerCharacter_h_
