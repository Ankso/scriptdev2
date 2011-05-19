#include "precompiled.h"
#include "OrgrimmarEvent.h"

struct MANGOS_DLL_DECL npc_bolvar_fordragonAI : public ScriptedAI
{
    npc_bolvar_fordragonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    int32 m_uiMovementTimer;
    uint16 nextMovementId;
    uint8 phase;

    void Reset()
    {
        m_uiMovementTimer = 20 * IN_MILLISECONDS;
        nextMovementId = 0;
        phase = BOLVAR_PHASE_INTRO;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
    }

    void JustDied(Unit *killer)
    {
        Map *map = m_creature->GetMap();
        const Quest *pQuest = GetQuestTemplateStore(QUEST_HORDE);
        const CreatureInfo *cInfo = GetCreatureTemplateStore(NPC_THRALL);

        if (!map || !pQuest || !cInfo)
            return;

        for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
        {
            Player *player = itr->getSource();
            
            if (player && player->GetQuestStatus(QUEST_HORDE) != QUEST_STATUS_NONE)
                player->KilledMonster(cInfo, ObjectGuid());
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
         if (type != POINT_MOTION_TYPE)
            return;

        switch(id)
        {
            case BOLVAR_WAYPOINT_INTRO_1:
                DoScriptText(BOLVAR_SAY_INTRO_1, m_creature);
                break;
            case BOLVAR_WAYPOINT_INTRO_2:
                DoScriptText(BOLVAR_SAY_INTRO_2, m_creature);
                break;
            case BOLVAR_WAYPOINT_INTRO_3:
                DoScriptText(BOLVAR_SAY_INTRO_3, m_creature);
                break;
            case BOLVAR_WAYPOINT_INTRO_4:
                DoScriptText(BOLVAR_SAY_INTRO_4, m_creature);
                break;
            case BOLVAR_WAYPOINT_INTRO_5:
                DoScriptText(BOLVAR_SAY_INTRO_5, m_creature);
                break;
            case BOLVAR_WAYPOINT_FRONT_DOOR:
                DoScriptText(BOLVAR_SAY_FRONT_DOOR, m_creature);
                break;
            case BOLVAR_WAYPOINT_TUNNEL:
                DoScriptText(BOLVAR_SAY_TUNNEL, m_creature);
                break;
            case BOLVAR_WAYPOINT_SQUARE:
                DoScriptText(BOLVAR_SAY_SQUARE, m_creature);
                break;
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (IsEventActive(ORGRIMMAR_EVENT))
        {
            // Movement handling
            if (nextMovementId <= BOLVAR_WAYPOINT_SQUARE)
            {
                if (m_uiMovementTimer <= 0)
                {
                    m_creature->GetMotionMaster()->MovePoint(nextMovementId, BolvarPositions[nextMovementId][0], BolvarPositions[nextMovementId][1], BolvarPositions[nextMovementId][2]);
                    switch(nextMovementId)
                    {
                        case BOLVAR_WAYPOINT_INTRO_1:
                            m_uiMovementTimer = 10 * IN_MILLISECONDS;
                            break;
                        case BOLVAR_WAYPOINT_INTRO_2:
                            m_uiMovementTimer = 9 * IN_MILLISECONDS;
                            break;
                        case BOLVAR_WAYPOINT_INTRO_3:
                            m_uiMovementTimer = 11 * IN_MILLISECONDS;
                            break;
                        case BOLVAR_WAYPOINT_INTRO_4:
                            m_uiMovementTimer = 10 * IN_MILLISECONDS;
                            break;
                        case BOLVAR_WAYPOINT_INTRO_5:
                            m_uiMovementTimer = 5 * IN_MILLISECONDS;
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            phase = BOLVAR_PHASE_FRONT_DOOR;
                            break;
                        case BOLVAR_WAYPOINT_FRONT_DOOR:
                            m_uiMovementTimer = 20 * /*MINUTE **/ IN_MILLISECONDS;
                            phase = BOLVAR_PHASE_TUNNEL;
                            break;
                        case BOLVAR_WAYPOINT_TUNNEL:
                            m_uiMovementTimer = 10 * /*MINUTE **/ IN_MILLISECONDS;
                            DoScriptText(BOLVAR_SAY_FRONT_DOOR_TO_TUNNEL, m_creature);
                            phase = BOLVAR_PHASE_SQUARE;
                            break;
                        case BOLVAR_WAYPOINT_SQUARE:
                            DoScriptText(BOLVAR_SAY_SQUARE, m_creature);
                            break;
                        default:
                            DEBUG_LOG("SD2: Unhandled waypoint %i in npc_bolvar_fordragon script.", nextMovementId);
                            break;
                    }
                    ++nextMovementId;
                }
                else
                    m_uiMovementTimer -= uiDiff;
            }
            
            // Combat handling
            switch(phase)
            {
                case BOLVAR_PHASE_INTRO:
                    break;
                case BOLVAR_PHASE_FRONT_DOOR:
                {
                    break;
                }
                case BOLVAR_PHASE_TUNNEL:
                {
                    break;
                }
                case BOLVAR_PHASE_SQUARE:
                {
                    break;
                }
                default:
                    break;
            }
        }
    }
};

bool GossipHello_npc_bolvar_fordragon(Player *pPlayer, Creature *pCreature)
{
    if (pCreature->isInCombat() || !IsEventActive(ORGRIMMAR_PREPARATION_EVENT))
        return false;

    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    return true;
}

CreatureAI* GetAI_npc_bolvar_fordragon(Creature* pCreature)
{
    return new npc_bolvar_fordragonAI(pCreature);
}

struct MANGOS_DLL_DECL npc_alliance_soldierAI : public ScriptedAI
{
    npc_alliance_soldierAI(Creature *pCreature) : ScriptedAI(pCreature) { Reset(); }

    int32 yellTimer;
    int32 phaseTimer;
    int32 deathTimer;
    uint8 phase;

    void Reset()
    {
        yellTimer = 25 * IN_MILLISECONDS;
        phase = SOLDIER_PHASE_INTRO;
        phaseTimer = 67 * IN_MILLISECONDS;
    }

    void UpdateAI(uint32 uiDiff)
    {
        if (IsEventActive(ORGRIMMAR_EVENT))
        {
            switch(phase)
            {
                case SOLDIER_PHASE_INTRO:
                {
                    // They start charging 2 seconds after Bolvar
                    if (phaseTimer <= 0)
                        phase = SOLDIER_PHASE_CHARGE;
                    else
                        phaseTimer -= uiDiff;

                    // Random yells during intro
                    if (yellTimer <= 0)
                    {
                        switch(urand(0, 99)) // There are about 60 soldiers, so only 5% chance to yell something.
                        {
                            case 0:
                                DoScriptText(SOLDIER_SAY_1, m_creature);
                                break;
                            case 1:
                                DoScriptText(SOLDIER_SAY_1, m_creature);
                                break;
                            case 2:
                                DoScriptText(SOLDIER_SAY_1, m_creature);
                                break;
                            case 3:
                                DoScriptText(SOLDIER_SAY_1, m_creature);
                                break;
                            case 4:
                                DoScriptText(SOLDIER_SAY_1, m_creature);
                                break;
                            default:
                                break;
                        }
                        yellTimer = urand(10, 20) * IN_MILLISECONDS;
                    }
                    else
                        yellTimer -= uiDiff;
                    break;
                }
                case SOLDIER_PHASE_CHARGE:
                {
                    uint8 randomPos = urand(0, 9);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->GetMotionMaster()->MovePoint(0, SoldiersPositions_FrontDoor[randomPos][0], SoldiersPositions_FrontDoor[randomPos][1], SoldiersPositions_FrontDoor[randomPos][2]);
                    phase = SOLDIER_PHASE_FIGHT;
                    break;
                }
                case SOLDIER_PHASE_FIGHT:
                {
                    if (!m_creature->isAlive())
                    {
                        if (deathTimer <= 0)
                        {
                            m_creature->Respawn();
                            phase = SOLDIER_PHASE_CHARGE;
                        }
                        else
                            deathTimer -= uiDiff;
                    }
                    break;
                }
                default:
                    DEBUG_LOG("SD2: Unhandled phase %i for npc_alliance_soldier.", phase);
                    break;
            }
        }
    }
};

CreatureAI *GetAI_npc_alliance_soldier(Creature *pCreature)
{
    return new npc_alliance_soldierAI(pCreature);
}

void AddSC_Orgrimmar_Event()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_bolvar_fordragon";
    pNewScript->GetAI = &GetAI_npc_bolvar_fordragon;
    pNewScript->pGossipHello = &GossipHello_npc_bolvar_fordragon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_alliance_soldier";
    pNewScript->GetAI = &GetAI_npc_alliance_soldier;
    pNewScript->RegisterSelf();
}