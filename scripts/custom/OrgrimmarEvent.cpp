#include "precompiled.h"
#include "OrgrimmarEvent.h"

struct MANGOS_DLL_DECL npc_bolvar_fordragonAI : public ScriptedAI
{
    npc_bolvar_fordragonAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    int32 m_uiMovementTimer;
    uint16 m_uiNextMovementId;
    uint8 m_uiPhase;
    // Spell timers
    int32 m_uiSpellTimers[5];
    // This coords are used for evade to a specific location
    float m_uiEvadePosition[3];

    void Reset()
    {
        m_uiMovementTimer = 20 * IN_MILLISECONDS;
        m_uiNextMovementId = 0;
        m_uiPhase = BOLVAR_PHASE_INTRO;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
        for (uint8 i = 0; i < 5; ++i)
            m_uiSpellTimers[i] = 10 * (i + 1) * IN_MILLISECONDS;
        m_uiEvadePosition[0] = m_creature->GetPositionX();
        m_uiEvadePosition[1] = m_creature->GetPositionY();
        m_uiEvadePosition[2] = m_creature->GetPositionZ();
    }
    
    Player* SelectRandomFriendlyCCedPlayerAtRange(float range)
    {
        Map *pMap = m_creature->GetMap();
        Map::PlayerList const &pPlayers = pMap->GetPlayers();
        std::list<Player*> pList;

        pList.clear();

        MaNGOS::FriendlyCCedInRangeCheck u_check(m_creature, range);
        MaNGOS::PlayerListSearcher<MaNGOS::FriendlyCCedInRangeCheck> searcher(pList, u_check);

        Cell::VisitGridObjects(m_creature, searcher, range);

        if (pList.empty())
            return NULL;

        return pList.front();
    }

    Player* SelectRandomPlayerAtRange(float range, bool alive, SelectionType uiType)
    {
        Map *pMap = m_creature->GetMap();
        Map::PlayerList const &pPlayers = pMap->GetPlayers();
        std::vector<Player*> pList;

        pList.clear();

        for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
        {
            if (Player *player = itr->getSource())
            {
                if (!player->IsInMap(m_creature))
                    continue;

                if (player->isGameMaster())
                    continue;

                bool isValid = true;
                switch(uiType)
                {
                    case TARGET_ENEMY:
                        if (player->IsFriendlyTo(m_creature))
                            isValid = false;
                        break;
                    case TARGET_FRIENDLY:
                        if (!player->IsFriendlyTo(m_creature))
                            isValid = false;
                        break;
                    case TARGET_ALL:
                    default:
                        break;
                }

                if (!isValid)
                    continue;

                if (alive && player->isAlive() && player->IsWithinDistInMap(m_creature, range))
                    pList.push_back(player);
                else if (!alive && !player->isAlive() && player->IsWithinDistInMap(m_creature, range))
                    pList.push_back(player);
            }
        }

        if (pList.empty())
            return NULL;

        return pList[urand(0, pList.size() - 1)];
    }

    void ResetCombatPosition()
    {
        SetMoving(true);
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->GetMotionMaster()->MovePoint(17, m_uiEvadePosition[0], m_uiEvadePosition[1], m_uiEvadePosition[2]);
    }

    void SetMoving(bool bMove)
    {
        if (bMove)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_uiPhase = BOLVAR_PHASE_MOVEMENT;
        }
        else
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_uiPhase = BOLVAR_PHASE_COMBAT;
        }
    }

    void JustDied(Unit *killer)
    {
        Map *pMap = m_creature->GetMap();
        const Quest *pQuest = GetQuestTemplateStore(QUEST_HORDE);
        const CreatureInfo *cInfo = GetCreatureTemplateStore(NPC_THRALL);

        if (!pMap || !pQuest || !cInfo)
            return;

        for (Map::PlayerList::const_iterator itr = pMap->GetPlayers().begin(); itr != pMap->GetPlayers().end(); ++itr)
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
                m_creature->SetOrientation(BolvarPositions[id][3]);
                break;
            case BOLVAR_WAYPOINT_INTRO_2:
                DoScriptText(BOLVAR_SAY_INTRO_2, m_creature);
                m_creature->SetOrientation(BolvarPositions[id][3]);
                break;
            case BOLVAR_WAYPOINT_INTRO_3:
                DoScriptText(BOLVAR_SAY_INTRO_3, m_creature);
                m_creature->SetOrientation(BolvarPositions[id][3]);
                break;
            case BOLVAR_WAYPOINT_INTRO_4:
                DoScriptText(BOLVAR_SAY_INTRO_4, m_creature);
                m_creature->SetOrientation(BolvarPositions[id][3]);
                break;
            case BOLVAR_WAYPOINT_INTRO_5:
                DoScriptText(BOLVAR_SAY_INTRO_5, m_creature);
                m_creature->SetOrientation(BolvarPositions[id][3]);
                break;
            case BOLVAR_WAYPOINT_FRONT_DOOR:
                DoScriptText(BOLVAR_SAY_FRONT_DOOR, m_creature);
                SetMoving(false);
                break;
            case BOLVAR_WAYPOINT_TUNNEL:
                DoScriptText(BOLVAR_SAY_TUNNEL, m_creature);
                SetMoving(false);
                break;
            case BOLVAR_WAYPOINT_SQUARE:
                DoScriptText(BOLVAR_SAY_SQUARE, m_creature);
                SetMoving(false);
                break;
            case BOLVAR_WAYPOINT_EVADE:
                SetMoving(false);
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
            if (m_uiNextMovementId <= BOLVAR_WAYPOINT_SQUARE)
            {
                if (m_uiMovementTimer <= 0)
                {
                    m_creature->GetMotionMaster()->MovePoint(m_uiNextMovementId, BolvarPositions[m_uiNextMovementId][0], BolvarPositions[m_uiNextMovementId][1], BolvarPositions[m_uiNextMovementId][2]);
                    switch(m_uiNextMovementId)
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
                            break;
                        case BOLVAR_WAYPOINT_FRONT_DOOR:
                            m_uiMovementTimer = 120 * /*MINUTE **/ IN_MILLISECONDS;
                            SetMoving(true);
                            break;
                        case BOLVAR_WAYPOINT_TUNNEL:
                            m_uiMovementTimer = 120 * /*MINUTE **/ IN_MILLISECONDS;
                            DoScriptText(BOLVAR_SAY_FRONT_DOOR_TO_TUNNEL, m_creature);
                            SetMoving(true);
                            break;
                        case BOLVAR_WAYPOINT_SQUARE:
                            DoScriptText(BOLVAR_SAY_TUNNEL_TO_SQUARE, m_creature);
                            SetMoving(true);
                            break;
                        default:
                            error_log("SD2: Unhandled waypoint %i in npc_bolvar_fordragon script.", m_uiNextMovementId);
                            break;
                    }
                    for (int i = 0; i < 3; ++i)
                        m_uiEvadePosition[i] = BolvarPositions[m_uiNextMovementId][i];
                    ++m_uiNextMovementId;
                }
                else
                    m_uiMovementTimer -= uiDiff;
            }
            
            // Combat handling
            if (m_uiPhase == BOLVAR_PHASE_COMBAT)
            {
                for (uint8 i = 0; i <= BOLVAR_TIMER_RESURRECTION; ++i)
                {
                    if (m_uiSpellTimers[i] <= 0)
                    {
                        switch(i)
                        {
                            case BOLVAR_TIMER_STRIKE:
                                DoCast(m_creature->getVictim(), BOLVAR_SPELL_STRIKE);
                                m_uiSpellTimers[i] = urand(5, 10) * IN_MILLISECONDS;
                                break;
                            case BOLVAR_TIMER_DIVINE_STORM:
                                DoCast(m_creature->getVictim(), BOLVAR_SPELL_DIVINE_STORM);
                                m_uiSpellTimers[i] = urand(7, 10) * IN_MILLISECONDS;
                                break;
                            case BOLVAR_TIMER_SACRED_LIGHT:
                                DoCast(DoSelectLowestHpFriendly(30.0f), BOLVAR_SPELL_SACRED_LIGHT);
                                m_uiSpellTimers[i] = urand(10, 15) * IN_MILLISECONDS;
                                break;
                            case BOLVAR_TIMER_HAND_OF_FREEDOM:
                                // DoCast(SelectRandomFriendlyCCedPlayerAtRange(40.0f), BOLVAR_SPELL_HAND_OF_FREEDOM); -> Is not working
                                m_uiSpellTimers[i] = urand(20, 25) * IN_MILLISECONDS;
                                break;
                            case BOLVAR_TIMER_RESURRECTION:
                                Player *pTarget = SelectRandomPlayerAtRange(50.0f, false, TARGET_FRIENDLY);
                                if (pTarget)
                                    DoCast(pTarget, BOLVAR_SPELL_RESURRECTION, true);
                                m_uiSpellTimers[i] = urand(25, 30) * IN_MILLISECONDS;
                                break;
                        }
                    }
                    else
                        m_uiSpellTimers[i] -= uiDiff;
                }
                DoMeleeAttackIfReady();
            }

            if (m_creature->GetDistance(m_uiEvadePosition[0], m_uiEvadePosition[1], m_uiEvadePosition[2]) >= EVADE_DISTANCE && m_uiPhase != BOLVAR_PHASE_MOVEMENT)
                ResetCombatPosition();
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

    int32 m_uiYellTimer;
    int32 m_uiPhaseTimer;
    uint8 m_uiPhase;

    void Reset()
    {
        m_uiYellTimer = urand(25, 30) * IN_MILLISECONDS;
        m_uiPhase = SOLDIER_PHASE_INTRO;
        m_uiPhaseTimer = 67 * IN_MILLISECONDS;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho->GetTypeId() == TYPEID_UNIT)
            if (((Creature*)pWho)->GetEntry() == NPC_HORDE_SOLDIER)
            {
                DoStartMovement(pWho);
                m_creature->SetInCombatWith(pWho);
            }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (IsEventActive(ORGRIMMAR_EVENT))
        {
            switch(m_uiPhase)
            {
                case SOLDIER_PHASE_INTRO:
                {
                    // They start charging 2 seconds after Bolvar
                    if (m_uiPhaseTimer <= 0)
                        m_uiPhase = SOLDIER_PHASE_CHARGE;
                    else
                        m_uiPhaseTimer -= uiDiff;

                    // Random yells during intro
                    if (m_uiYellTimer <= 0)
                    {
                        if (urand(1, 20) == 1) // There are about 60 soldiers, so only 5% chance to yell something.
                        {
                            switch(urand(0, 4))
                            {
                                case 0:
                                    DoScriptText(SOLDIER_SAY_1, m_creature);
                                    break;
                                case 1:
                                    DoScriptText(SOLDIER_SAY_2, m_creature);
                                    break;
                                case 2:
                                    DoScriptText(SOLDIER_SAY_3, m_creature);
                                    break;
                                case 3:
                                    DoScriptText(SOLDIER_SAY_4, m_creature);
                                    break;
                                case 4:
                                    DoScriptText(SOLDIER_SAY_5, m_creature);
                                    break;
                                default:
                                    break;
                            }
                        }
                        m_uiYellTimer = urand(10, 20) * IN_MILLISECONDS;
                    }
                    else
                        m_uiYellTimer -= uiDiff;
                    break;
                }
                case SOLDIER_PHASE_CHARGE:
                {
                    uint8 randomPos = urand(0, 7);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->GetMotionMaster()->MovePoint(0, SoldiersPositions_FrontDoor[randomPos][0] + frand(-3.0f, 3.0f), SoldiersPositions_FrontDoor[randomPos][1] + urand(-3.0f, 3.0f), SoldiersPositions_FrontDoor[randomPos][2]);
                    m_uiPhase = SOLDIER_PHASE_FIGHT;
                    break;
                }
                case SOLDIER_PHASE_FIGHT:
                {
                    DoMeleeAttackIfReady();
                    break;
                }
                default:
                    error_log("SD2: Unhandled phase %i for npc_alliance_soldier.", m_uiPhase);
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