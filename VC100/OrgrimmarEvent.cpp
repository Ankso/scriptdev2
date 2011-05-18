#include "precompiled.h"
#include "OrgrimmarEvent.h"

struct MANGOS_DLL_DECL npc_bolvar_fordragonAI : public ScriptedAI
{
    npc_bolvar_fordragonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    int32 movement_timer;
    uint16 nextMovementId;
    uint8 phase;

    void Reset()
    {
        movement_timer = 20 * IN_MILLISECONDS;
        nextMovementId = 0;
        phase = BOLVAR_PHASE_ASSAULT;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit *killer)
    {
        Map *map = m_creature->GetMap();
        const Quest *pQuest = GetQuestTemplateStore(QUEST_HORDE);
        const CreatureInfo *cInfo = GetCreatureTemplateStore(NPC_KILL_CREDIT_HORDE);

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
            switch(phase)
            {
                case BOLVAR_PHASE_ASSAULT:
                {
                    if (movement_timer <= 0)
                    {
                        if (nextMovementId <= BOLVAR_WAYPOINT_SQUARE)
                        {
                            m_creature->GetMotionMaster()->MovePoint(nextMovementId, BolvarPositions[nextMovementId][0], BolvarPositions[nextMovementId][1], BolvarPositions[nextMovementId][2]);
                            switch(nextMovementId)
                            {
                                case BOLVAR_WAYPOINT_INTRO_1:
                                    movement_timer = urand(10, 12) * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_INTRO_2:
                                    movement_timer = urand(10, 12) * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_INTRO_3:
                                    movement_timer = urand(10, 12) * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_INTRO_4:
                                    movement_timer = urand(10, 12) * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_INTRO_5:
                                    movement_timer = 4 * IN_MILLISECONDS;
                                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    break;
                                case BOLVAR_WAYPOINT_FRONT_DOOR:
                                    movement_timer = 60 /* MINUTE*/ * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_TUNNEL:
                                    movement_timer = 60 /* MINUTE*/ * IN_MILLISECONDS;
                                    break;
                                case BOLVAR_WAYPOINT_SQUARE:
                                    movement_timer = 60 /* MINUTE*/ * IN_MILLISECONDS;
                                    phase = BOLVAR_PHASE_SQUARE;
                                    break;
                                default:
                                    break;
                            }
                            ++nextMovementId;
                        }
                    }
                    else
                        movement_timer -= uiDiff;
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

void AddSC_Orgrimmar_Event()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_bolvar_fordragon";
    pNewScript->GetAI = &GetAI_npc_bolvar_fordragon;
    pNewScript->pGossipHello = &GossipHello_npc_bolvar_fordragon;
    pNewScript->RegisterSelf();
}