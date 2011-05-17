/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_festergut
SD%Complete:
SDComment: by /dev/rsa, IOV
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    // Spells
    SPELL_GASEOUS_BLIGHT_1          = 69157,
    SPELL_GASEOUS_BLIGHT_2          = 69162,
    SPELL_GASEOUS_BLIGHT_3          = 69164,
    SPELL_BLIGHT_VISUAL_1           = 69126,
    SPELL_BLIGHT_VISUAL_2           = 69152,
    SPELL_BLIGHT_VISUAL_3           = 69154,
    SPELL_INHALE_BLIGHT             = 69165,
    SPELL_INHALED_BLIGHT            = 69166,
    SPELL_PUNGENT_BLIGHT_10_N       = 69195,
    SPELL_PUNGENT_BLIGHT_10_H       = 73031,
    SPELL_PUNGENT_BLIGHT_25_N       = 71219,
    SPELL_PUNGENT_BLIGHT_25_H       = 73032,
    SPELL_GAS_SPORE                 = 69278,
    SPELL_SPORE_AURA_0              = 69279,
    SPELL_BLIGHTED_SPORES_10_N      = 69290,
    SPELL_BLIGHTED_SPORES_10_H      = 71222,
    SPELL_BLIGHTED_SPORES_25_N      = 73033,
    SPELL_BLIGHTED_SPORES_25_H      = 73034,
    SPELL_INOCULATE                 = 69291,
    SPELL_REMOVE_UNOCULATE          = 69298, // not used
    SPELL_GASTRIC_BLOAT_10_N        = 72219,
    SPELL_GASTRIC_BLOAT_10_H        = 72551,
    SPELL_GASTRIC_BLOAT_25_N        = 72552,
    SPELL_GASTRIC_BLOAT_25_H        = 72553,
    SPELL_VILE_GAS_10_N             = 69240,
    SPELL_VILE_GAS_10_H             = 73019,
    SPELL_VILE_GAS_25_N             = 71218,
    SPELL_VILE_GAS_25_H             = 73020,
    SPELL_VILE_GAS_AURA             = 69244, // not used
    SPELL_VILE_GAS_AURA_0           = 69248, // not used
    SPELL_BERSERK                   = 47008,
    SPELL_GASEOUS_SPIGOT            = 71379, // not used
    SPELL_SUMMON_VILE_STALKER       = 72287, // not used

    // NPCs
    NPC_VILE_GAS_STALKER            = 38548,
    NPC_BLIGHT_STALKER              = 36659,
    NPC_PUDDLE_STALKER              = 37013,

    // Texts
    //-1631201,
    //-1631202,
    SAY_AGGRO                       = -1631203,
    SAY_KILL1                       = -1631204,
    SAY_KILL2                       = -1631205,
    SAY_DEATH                       =- 1631206,
    SAY_BERSERK                     = -1631207,
    SAY_PURGENT_BLIGHT              = -1631208,
    SAY_STINKY_DEATH                = -1631209,
    SAY_INHALE1                     = -1631210,
    SAY_INHALE2                     = -1631211,
    SAY_INHALE3                     = -1631212,
    SAY_GASTRIC_BLOAT               = -1631213,

    // Achievements
    ACHIEV_FLU_SHOT_10              = 4577,
    ACHIEV_FLU_SHOT_25              = 4615,
};

static Locations SpawnLoc[] =
{
    {4267.9399f,   3137.32f,     360.385986f, 0.0f},   // 0 (start point)
    {4317.067383f, 3136.99f,     360.385590f, 3.21f},  // 1 right
    {4220.801758f, 3136.99f,     360.385590f, 0.39f},  // 2 left
    {4269.084473f, 3186.306641f, 360.385590f, 4.72f},  // 3 rear
};

struct MANGOS_DLL_DECL boss_festergutAI : public ScriptedAI
{
    boss_festergutAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint8 m_uiMode;

    bool m_bIntro;
    bool m_bPet;
    uint8 m_uiInhaleCount;
    uint8 m_uiGasSporeCount;
    uint32 m_uiBerserkTimer;
    uint32 m_uiGastricBloatTimer;
    uint32 m_uiVileGasTimer;
    uint32 m_uiGaseosBlightTimer;
    uint32 m_uiPurgentBlightTimer;
    uint32 m_uiGasSporeTimer;
    uint32 m_uiInhalteBlightTimer;
    uint64 m_uiBlightTargetGUID;
    uint64 m_uiGasSporeTargetGUID[3];
//    uint64 pPuddleStalkerGUID[3];

    void Reset()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, NOT_STARTED);

        m_bIntro = false;
        m_bPet = false;
        m_uiInhaleCount = 0;
        m_uiBerserkTimer = 6*MINUTE*IN_MILLISECONDS;
        m_uiGastricBloatTimer = 12*IN_MILLISECONDS;
        m_uiVileGasTimer = 40*IN_MILLISECONDS;
        m_uiGaseosBlightTimer = 0*IN_MILLISECONDS;
        m_uiPurgentBlightTimer = 130*IN_MILLISECONDS;
        m_uiGasSporeTimer = 20*IN_MILLISECONDS;
        m_uiInhalteBlightTimer = 30*IN_MILLISECONDS;
        m_uiBlightTargetGUID = 0;

        for (uint8 i = 0; i < 3; ++i)
            m_uiGasSporeTargetGUID[i] = 0;

        switch (m_uiMode)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
            case RAID_DIFFICULTY_10MAN_HEROIC:
                m_uiGasSporeCount = 2;
                break;
            case RAID_DIFFICULTY_25MAN_NORMAL:
            case RAID_DIFFICULTY_25MAN_HEROIC:
                m_uiGasSporeCount = 3;
                break;
            default:
                break;
        }

/*        for(uint8 i = 0; i < 3; ++i)
             if (!pPuddleStalkerGUID[i])
             {
                 Unit* pTemp = doSummon(NPC_PUDDLE_STALKER,SpawnLoc[i].x, SpawnLoc[i].y, SpawnLoc[i].z, TEMPSUMMON_MANUAL_DESPAWN);
                 if (pTemp)
                 {
                     pPuddleStalkerGUID[i] = pTemp->GetGUID();
                     pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                     pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                 }
             }
*/

        if (Creature* pBlightTarget = GetClosestCreatureWithEntry(m_creature, NPC_BLIGHT_STALKER, 120.0f))
        {
            if (!pBlightTarget->isAlive())
                pBlightTarget->Respawn();

            m_uiBlightTargetGUID = pBlightTarget->GetGUID();

            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_bIntro)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        // used for event with Professor Putricide
        m_pInstance->SetData(TYPE_EVENT, 500);
        debug_log("EventMGR: creature %u send signal %u ",m_creature->GetEntry(),m_pInstance->GetData(TYPE_EVENT));

        m_bIntro = true;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1))
        {
            case 0:
               DoScriptText(SAY_KILL1, m_creature, pVictim);
               break;
            case 1:
               DoScriptText(SAY_KILL2, m_creature, pVictim);
               break;
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, FAIL);

        if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
        {
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature, pWho);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);

        if (Creature* pBlightTarget = GetClosestCreatureWithEntry(m_creature, NPC_BLIGHT_STALKER, 120.0f))
        {
            if (!pBlightTarget->isAlive())
                pBlightTarget->Respawn();

            m_uiBlightTargetGUID = pBlightTarget->GetGUID();

            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
        }

/*        for(uint8 i = 0; i < 3; ++i)
             if (pPuddleStalkerGUID[i])
                 doCast(SPELL_GASEOUS_SPIGOT, m_creature->GetMap()->GetCreature(pPuddleStalkerGUID[i]));
*/
    }

    void JustDied(Unit *killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FESTERGUT, DONE);

        // used for event with Professor Putricide
        m_pInstance->SetData(TYPE_EVENT, 550);

        DoScriptText(SAY_DEATH, m_creature);

        if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
        {
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
            pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
        }

/*        for(uint8 i = 0; i < 3; ++i)
             if (pPuddleStalkerGUID[i])
             {
                 Creature* pTemp = m_creature->GetMap()->GetCreature(pPuddleStalkerGUID[i]);
                 if (pTemp) pTemp->ForcedDespawn();
                 pPuddleStalkerGUID[i] = NULL;
             }
*/
    }

    Unit* VileGasTarget()
    {
        std::list<uint64> PlayerGuidList;
        std::list<uint64>::iterator itr;
        Unit* pTarget = NULL;

        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                if (pPlayer->isAlive())
                    if (!m_creature->IsWithinDistInMap(pPlayer, ATTACK_DISTANCE))
                        PlayerGuidList.push_back(pPlayer->GetGUID());
        }

        if (PlayerGuidList.empty())
        {
            if (pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                return pTarget;
        }
        else
        {
            itr = PlayerGuidList.begin();
            std::advance(itr, urand(0, PlayerGuidList.size() - 1));

            if (pTarget = m_creature->GetMap()->GetPlayer(*itr))
                return pTarget;
        }
    }

    void RemoveAuraFromAll(uint32 m_uiSpellId)
    {
        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
            {
                if (pPlayer->HasAura(m_uiSpellId))
                    pPlayer->RemoveAurasDueToSpell(m_uiSpellId);

                if (Pet* pPet = pPlayer->GetPet())
                    if (pPet->HasAura(m_uiSpellId))
                        pPet->RemoveAurasDueToSpell(m_uiSpellId);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if(!m_pInstance)
            return;

        if (!m_bPet)
        {
            if (Creature* pGuard = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_STINKY)))
                if (!pGuard->isAlive())
                {
                     m_bPet = true;
                     if (m_pInstance->GetData(TYPE_STINKY) == NOT_STARTED)
                     {
                         DoScriptText(SAY_STINKY_DEATH, m_creature);
                         m_pInstance->SetData(TYPE_STINKY, DONE);
                     }
                }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_creature->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
        m_creature->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
        m_creature->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
        RemoveAuraFromAll(SPELL_BLIGHT_VISUAL_1);
        RemoveAuraFromAll(SPELL_BLIGHT_VISUAL_2);
        RemoveAuraFromAll(SPELL_BLIGHT_VISUAL_3);

        if (m_uiInhalteBlightTimer < uiDiff)
        {
            switch (urand(0,2))
            {
                case 0:
                    DoScriptText(SAY_INHALE1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_INHALE2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_INHALE3, m_creature);
                    break;
            }

            DoCast(m_creature, SPELL_INHALE_BLIGHT);

            m_uiInhalteBlightTimer = 40*IN_MILLISECONDS;
            m_uiGaseosBlightTimer = 3.5*IN_MILLISECONDS;
            m_uiGastricBloatTimer += 3.5*IN_MILLISECONDS;
        }
        else
            m_uiInhalteBlightTimer -= uiDiff;

        if (m_uiGaseosBlightTimer < uiDiff)
        {
            switch (m_uiInhaleCount)
            {
                case 0:
                    if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
                    {
                        DoCast(m_creature, SPELL_GASEOUS_BLIGHT_1);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
                        pBlightTarget->CastSpell(pBlightTarget, SPELL_BLIGHT_VISUAL_1, false);
                        m_uiInhalteBlightTimer = 30*IN_MILLISECONDS;
                        m_uiGaseosBlightTimer= 35*IN_MILLISECONDS;
                    }
                    break;
                case 1:
                    if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_1);
                        DoCast(m_creature, SPELL_GASEOUS_BLIGHT_2);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
                        pBlightTarget->CastSpell(pBlightTarget, SPELL_BLIGHT_VISUAL_2, false);
                        m_uiInhalteBlightTimer = 30*IN_MILLISECONDS;
                        m_uiGaseosBlightTimer= 35*IN_MILLISECONDS;
                    }
                    break;
                case 2:
                    if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_2);
                        DoCast(m_creature, SPELL_GASEOUS_BLIGHT_3);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
                        pBlightTarget->CastSpell(pBlightTarget, SPELL_BLIGHT_VISUAL_3, false);
                        m_uiInhalteBlightTimer = 30*IN_MILLISECONDS;
                        m_uiGaseosBlightTimer= 35*IN_MILLISECONDS;
                    }
                    break;
                case 3:
                    if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(m_uiBlightTargetGUID))
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_GASEOUS_BLIGHT_3);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_1);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_2);
                        pBlightTarget->RemoveAurasDueToSpell(SPELL_BLIGHT_VISUAL_3);
                        m_uiInhalteBlightTimer = 70*IN_MILLISECONDS;
                        m_uiGaseosBlightTimer= 75*IN_MILLISECONDS;
                        m_uiPurgentBlightTimer = 40*IN_MILLISECONDS;
                    }
                    break;
                default:
                    break;
            }

            ++m_uiInhaleCount;
        }
        else
            m_uiGaseosBlightTimer -= uiDiff;

        if (m_uiPurgentBlightTimer < uiDiff)
        {
            DoScriptText(SAY_PURGENT_BLIGHT, m_creature);

            switch (m_uiMode)
            {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature, SPELL_PUNGENT_BLIGHT_10_N);
                    break;
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    DoCast(m_creature, SPELL_PUNGENT_BLIGHT_10_H);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    DoCast(m_creature, SPELL_PUNGENT_BLIGHT_25_N);
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature, SPELL_PUNGENT_BLIGHT_25_H);
                    break;
                default:
                    break;
            }

            m_creature->RemoveAurasDueToSpell(SPELL_INHALED_BLIGHT);
            RemoveAuraFromAll(SPELL_INOCULATE);

            m_uiGaseosBlightTimer = 3*IN_MILLISECONDS;
            m_uiGastricBloatTimer += 3*IN_MILLISECONDS;
            m_uiInhaleCount = 0;
            m_uiPurgentBlightTimer = 130*IN_MILLISECONDS;
            m_uiGasSporeTimer = 23*IN_MILLISECONDS;
        }
        else
            m_uiPurgentBlightTimer -= uiDiff;

        if (m_uiGasSporeTimer < uiDiff)
        {
            if (!m_creature->IsNonMeleeSpellCasted(false))
            {
                m_uiGasSporeTimer = 40*IN_MILLISECONDS;

                for (uint8 i = 0; i < m_uiGasSporeCount; ++i)
                {
                    if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        m_uiGasSporeTargetGUID[i] = pTarget->GetGUID();
                        DoCast(pTarget, SPELL_GAS_SPORE);
                        DoCast(pTarget, SPELL_SPORE_AURA_0);
                    }
                }
            }
        }
        else
            m_uiGasSporeTimer -= uiDiff;

        if (m_uiGastricBloatTimer < uiDiff)
        {
            switch (m_uiMode)
            {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_GASTRIC_BLOAT_10_N);
                    break;
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_GASTRIC_BLOAT_10_H);
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                    DoCast(m_creature->getVictim(), SPELL_GASTRIC_BLOAT_25_N);
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    DoCast(m_creature->getVictim(), SPELL_GASTRIC_BLOAT_25_H);
                    break;
                default:
                    break;
            }

            m_uiGastricBloatTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiGastricBloatTimer -= uiDiff;

        if (m_uiVileGasTimer < uiDiff)
        {
            if(Unit *pTarget = VileGasTarget())
            {
                if (Creature* pSummoned = m_creature->SummonCreature(NPC_VILE_GAS_STALKER, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    switch (m_uiMode)
                    {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            DoCast((Unit*)pSummoned, SPELL_VILE_GAS_10_N);
                            break;
                        case RAID_DIFFICULTY_10MAN_HEROIC:
                            DoCast((Unit*)pSummoned, SPELL_VILE_GAS_10_H);
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            DoCast((Unit*)pSummoned, SPELL_VILE_GAS_25_N);
                            break;
                        case RAID_DIFFICULTY_25MAN_HEROIC:
                            DoCast((Unit*)pSummoned, SPELL_VILE_GAS_25_H);
                            break;
                        default:
                            break;
                    }
                }
            }

            m_uiVileGasTimer = 40*IN_MILLISECONDS;
        }
        else
            m_uiVileGasTimer -= uiDiff;

        if (m_uiBerserkTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            DoScriptText(SAY_BERSERK, m_creature);

            m_uiBerserkTimer = 6*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiBerserkTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

struct MANGOS_DLL_DECL  mob_vile_gas_stalkerAI : public ScriptedAI
{
    mob_vile_gas_stalkerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint32 m_uiLifeTimer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetInCombatWithZone();
        m_creature->SetDisplayId(11686);
        SetCombatMovement(false);

        m_uiLifeTimer = 12000;
    }

    void AttackStart(Unit *pWho)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_FESTERGUT) != IN_PROGRESS)
              m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiLifeTimer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_uiLifeTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_vile_gas_stalker(Creature* pCreature)
{
    return new mob_vile_gas_stalkerAI(pCreature);
}

void AddSC_boss_festergut()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "boss_festergut";
    pNewScript->GetAI = &GetAI_boss_festergut;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_vile_gas_stalker";
    pNewScript->GetAI = &GetAI_mob_vile_gas_stalker;
    pNewScript->RegisterSelf();
}
