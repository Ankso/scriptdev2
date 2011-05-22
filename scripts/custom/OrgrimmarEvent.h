#include "precompiled.h"
#include "ObjectMgr.h"
#include "GameEventMgr.h"

#define ORGRIMMAR_EVENT             78
#define ORGRIMMAR_PREPARATION_EVENT 79
#define QUEST_ALLIANCE              80000
#define QUEST_HORDE                 80001

#define EVADE_DISTANCE              50.0f

enum NPCs
{
    NPC_BOLVAR_FORDRAGON = 80000,
    NPC_THRALL = 80001,

    NPC_ALLIANCE_SOLDIER = 80002,
    NPC_HORDE_SOLDIER = 80003,

    NPC_ALLIANCE_HEAVY_SOLDIER = 80004,
    NPC_HORDE_HEAVY_SOLDIER = 80005,
};

enum BolvarTexts
{
    BOLVAR_SAY_INTRO_1 = -2000000,
    BOLVAR_SAY_INTRO_2 = -2000001,
    BOLVAR_SAY_INTRO_3 = -2000002,
    BOLVAR_SAY_INTRO_4 = -2000003,
    BOLVAR_SAY_INTRO_5 = -2000004,

    BOLVAR_SAY_FRONT_DOOR           = -2000005,
    BOLVAR_SAY_FRONT_DOOR_TO_TUNNEL = -2000008,
    BOLVAR_SAY_TUNNEL               = -2000006,
    BOLVAR_SAY_TUNNEL_TO_SQUARE     = -2000009,
    BOLVAR_SAY_SQUARE               = -2000007,

    SOLDIER_SAY_1 = -2000010,
    SOLDIER_SAY_2 = -2000011,
    SOLDIER_SAY_3 = -2000012,
    SOLDIER_SAY_4 = -2000013,
    SOLDIER_SAY_5 = -2000014,
};

enum BolvarWaypoints
{
    BOLVAR_WAYPOINT_INTRO_1    = 0,
    BOLVAR_WAYPOINT_INTRO_2    = 1,
    BOLVAR_WAYPOINT_INTRO_3    = 2,
    BOLVAR_WAYPOINT_INTRO_4    = 3,
    BOLVAR_WAYPOINT_INTRO_5    = 4,

    BOLVAR_WAYPOINT_FRONT_DOOR = 5,
    BOLVAR_WAYPOINT_TUNNEL     = 7,
    BOLVAR_WAYPOINT_SQUARE     = 10,

    BOLVAR_WAYPOINT_EVADE      = 20,
};

enum Phases
{
    BOLVAR_PHASE_INTRO      = 0,
    BOLVAR_PHASE_MOVEMENT   = 1,
    BOLVAR_PHASE_COMBAT     = 2,

};

enum Spells
{
    BOLVAR_SPELL_STRIKE       = 41975,
    BOLVAR_SPELL_DIVINE_STORM = 53385,
    BOLVAR_SPELL_SACRED_LIGHT = 68008,
    BOLVAR_SPELL_RESURRECTION = 9232,
    BOLVAR_SPELL_HAND_OF_FREEDOM = 1044,
};

enum Timers
{
    BOLVAR_TIMER_STRIKE          = 0,
    BOLVAR_TIMER_DIVINE_STORM    = 1,
    BOLVAR_TIMER_SACRED_LIGHT    = 2,
    BOLVAR_TIMER_HAND_OF_FREEDOM = 3,
    BOLVAR_TIMER_RESURRECTION    = 4,
};

enum SelectionType
{
    TARGET_ENEMY    = 0,
    TARGET_FRIENDLY = 1,
    TARGET_ALL      = 2,
};

float BolvarPositions[11][4] = {
    { 1272.475830f, -4399.773438f, 26.321188f, 3.488860f },
    { 1267.679321f, -4378.562012f, 28.625492f, 4.458753f },
    { 1278.283691f, -4422.939941f, 26.592169f, 3.479697f },
    { 1272.475830f, -4399.773438f, 26.321188f, 0.323486f },
    { 1272.475830f, -4399.773438f, 26.321188f, 3.488860f },
    { 1341.866333f, -4378.541992f, 26.186592f, 0.227517f },
    { 1429.739746f, -4364.395996f, 25.463297f, 4.884936f },
    { 1433.062988f, -4393.112305f, 25.463297f, 4.824723f },
    { 1438.786133f, -4422.902832f, 25.463297f, 0.128036f },
    { 1477.557129f, -4418.270996f, 25.461380f, 0.118000f },
    { 1517.291016f, -4413.885254f, 16.941128f, 0.052768f },
};

enum SoldiersPhases
{
    SOLDIER_PHASE_INTRO =  0,
    SOLDIER_PHASE_CHARGE = 1,
    SOLDIER_PHASE_FIGHT =  2,
};

float SoldiersPositions_FrontDoor[8][3] = {
    // { 1346.921265f, -4327.463867f, 27.281607f },
    // { 1347.774048f, -4333.683105f, 27.303045f },
    { 1348.748413f, -4342.461426f, 27.230156f },
    { 1350.410645f, -4351.984375f, 27.107201f },
    { 1352.367676f, -4360.899902f, 26.744757f },
    { 1354.290039f, -4369.205566f, 26.269405f },
    { 1355.729492f, -4375.058105f, 26.125025f },
    { 1357.612915f, -4382.715332f, 26.124069f },
    { 1357.747192f, -4392.923828f, 29.361778f },
    { 1357.109619f, -4406.307129f, 28.641808f },
};