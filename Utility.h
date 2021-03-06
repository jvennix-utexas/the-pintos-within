#ifndef __Utility_h_
#define __Utility_h_

// given an arbitrarily-sized struct, MEMALIGNED_SIZE ensures
// that the size of the struct is a multiple of a word boundary
// (16 bytes on linux) to prevent memcpy from dying on alignment
//
// XXX: If I wasn't such a dick about using pointer arithmetic,
// we wouldn't have to worry about this -joev
#define MEMALIGNED_SIZE(struct) \
  ((sizeof(struct)/16)+1)*16

#define CHANCE(chance) \
  ((chance * 10000) >= (rand() % 10000 + 1))

#define RAND (((float) rand()) / (float) RAND_MAX)

#define RAND_RANGE(from, to) \
  (from-to != 0) ? ((rand() % (to - from)) + from) : 0

#define CUBE_SCALE 1

#define BIT(x) (1<<(x))

enum collisiontypes {
    COL_NOTHING = 0,
    COL_STATIC = BIT(0),
    COL_CHARACTER = BIT(1),
    COL_BALL = BIT(2),
    COL_BULLET = BIT(3),
    COL_PICKUP = BIT(4),
    COL_HIT_BOX = BIT(5),
    COL_INACTIVE = BIT(6)
};

#define PLAYER_NAME_MAX_LEN 16
#define GAME_OVER_MSG_MAX_LEN 255

#define STATIC_COLLIDER_WITH (COL_BALL | COL_CHARACTER | COL_BULLET | COL_PICKUP)
#define CHARACTER_COLLIDER_WITH (COL_STATIC | COL_CHARACTER | COL_PICKUP)
#define BALL_COLLIDER_WITH (COL_STATIC | COL_CHARACTER | COL_BALL)
#define COL_BULLET_COLLIDER_WITH (COL_STATIC | COL_HIT_BOX | COL_BULLET)
#define COL_PICKUP_COLLIDER_WIH (COL_STATIC)
#define COL_HIT_BOX_COLLIDER_WITH (COL_BULLET)

#define HEARTBEATPACK 1
#define ASSIGNPLAYERID 2

#define PARTICLEPACK 3
#define JOINGAME 4
#define PLAYERNUM 5
#define TAKEDAMAGE 6
#define PLAYER_RESPAWN 7
#define PLAYER_DIE 8
#define WEAPON_CHANGE 9
#define WEAPON_SPAWN 10
#define CHATPACK 11
#define BLOOD 12
#define DUST 13
#define BLASTER_EXPLODE 14
#define PLAY_FIRE_SOUND 15
#define CHANGE_PINTO 16
#define INCREASE_SCORE 17
#define TIME_LEFT 18
#define GAMESTART 19
#define PLAYER_JOIN 20
#define PLAYER_DISCONNECT 21
#define PING 22
#define GAME_OVER 23
#define HAIR_CHANGE 24

// network stuff
#define PING_INTERVAL 1000
#define CLIENT_TIMEOUT 5

#define WALL "Wall"
#define FLOORGRASS "FloorGrass"
#define FLOORTILE "FloorTiled"
#define PLATFORMDIRT "PlatformDirt"
#define PLATFORMGREY "PlatformGrey"
#define PLAYERREDSPAWNER "Spawner.Player.Red"
#define PLAYERBLUESPAWNER "Spawner.Player.Blue"
#define PLAYERFREESPAWNER "Spawner.Player.Free"
#define LIGHT "Sun"
#define SKY "SkyHemi"
#define WEAPONSPAWNER "Spawner.Weapon"
#define FLOORSAND "FloorSand"

//current_state
#define MAIN_MENU 1
#define HOST_MENU 2
#define CLIENT_MENU 3
#define LOBBY_AS_HOST 4
#define LOBBY_AS_CLIENT 5
#define LOADING 6
#define IN_GAME 7

//game_mode
#define ELIMINATION 1
#define DEATHMATCH 2
#define PINTO 3

//team_mode
#define FFA 1
#define TEAM 2

//map
#define THEGAUNTLET 1
#define DUSTTWO 2

#define PISTOL_ID 0
#define SHOTGUN_ID 1
#define ASSAULTRIFLE_ID 2
#define BLASTER_ID 3
#define MELEE_ID 4

#define NORMAL_HAIR_ID 0
#define JOE_HAIR_ID 1
#define MIKE_HAIR_ID 2
#define KEEGAN_HAIR_ID 3
#define JACKY_HAIR_ID 4

//team id
#define RED_TEAM 0
#define BLUE_TEAM 1
#define PINTO_TEAM 1

#define WEAPON_PICKUP 0
#define HAIR_PICKUP 1

#endif // #ifndef __Utility_h_
