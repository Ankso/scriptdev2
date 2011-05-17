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
SDName: Boss_Gortok
SD%Complete: 90%
SDComment: TODO: spell for stasis orb activating mob visual (red lightning thing)
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_DEATH               = -1575018,

    SPELL_FREEZE_ANIM       = 16245,

    SPELL_IMPALE            = 48261,
    SPELL_IMPALE_H          = 59268,

    SPELL_WITHERING_ROAR    = 48256,
    SPELL_WITHERING_ROAR_H  = 59267,

    SPELL_ARCING_SMASH      = 48260,

    // orb
    NPC_ORB                 = 26688, 
    SPELL_ORB_VISUAL        = 48044,
    SPELL_ORB_CHANNEL       = 48048, // this spell does not have a visual effect, there seems to be one spell with the visual effect missing

    ORB_EVENT_NOT_SPAWNED   = 0,
    ORB_EVENT_MOVING        = 1,
    ORB_EVENT_CAST_STARTED  = 2,

};

/*######
## boss_gortok
######*/

static const float orbWP[2][3] = 
{
    {248.5f,-451.1f,110.f},
    {274.1f,-451.7f,110.f}
};

struct MANGOS_DLL_DECL boss_gortokAI : public ScriptedAI
{
    boss_gortokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiCheckTimer;
    uint64 m_uiCurrentMobGUID;
    std::vector<uint32> mobList;

    uint32 m_uiImpaleTimer;
    uint32 m_uiRoarTimer;
    uint32 m_uiSmashTimer;

    uint64 m_uiOrbGUID;
    uint8 m_uiOrbEventProgress;
    uint32 m_uiOrbTimer;

    void Reset()
    {
        m_uiCheckTimer = 1000;
        m_uiCurrentMobGUID = 0;
        m_creature->setFaction(35);
        mobList.clear();
        mobList.push_back(NPC_WORGEN);
        mobList.push_back(NPC_FURBOLG);
        mobList.push_back(NPC_JORMUNGAR);
        mobList.push_back(NPC_RHINO);

        m_uiImpaleTimer = 3000;
        m_uiRoarTimer = 500;
        m_uiSmashTimer = 5000;

        m_uiOrbGUID = 0;
        m_uiOrbEventProgress = ORB_EVENT_NOT_SPAWNED;
        m_uiOrbTimer = 0;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        mobList.clear();
        mobList.push_back(NPC_WORGEN);
        mobList.push_back(NPC_FURBOLG);
        mobList.push_back(NPC_JORMUNGAR);
        mobList.push_back(NPC_RHINO);

        
        for (std::vector<uint32>::iterator itr = mobList.begin();itr != mobList.end();++itr)
        {
            if (Creature* pCreature = GetClosestCreatureWithEntry(m_creature, *itr, 100.f))
            {
                pCreature->Respawn();
                pCreature->setFaction(35);
            }
        }
    
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, DONE);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        // If Nefarian has reached combat area, let him attack
        if (pSummoned->GetEntry() == NPC_ORB && uiMotionType == POINT_MOTION_TYPE)
            if (uiPointId == 1)
            {
                pSummoned->GetMotionMaster()->MovePoint(2, orbWP[1][0], orbWP[1][1], orbWP[1][2]);
            } 
            else if (uiPointId == 2)
            {
                if (Creature* pTarget = m_creature->GetMap()->GetCreature(m_uiCurrentMobGUID))
                {
                    // waypoint 3 is not needed if visual spell target works
                    pSummoned->GetMotionMaster()->MovePoint(3,pTarget->GetPositionX(),pTarget->GetPositionY(),pTarget->GetPositionZ()+5.f);
                }
            }
            else if (uiPointId == 3)
            {
                if (Creature* pTarget = m_creature->GetMap()->GetCreature(m_uiCurrentMobGUID))
                    {
                        //pSummoned->CastSpell(pTarget, SPELL_ORB_CHANNEL, true); // spell has no effect? visual spell needed
                        m_uiOrbEventProgress = ORB_EVENT_CAST_STARTED;
                        pSummoned->ForcedDespawn();
                    }
            }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (!m_creature->HasAura(SPELL_FREEZE_ANIM))
                m_creature->CastSpell(m_creature, SPELL_FREEZE_ANIM,true);

            if (m_uiOrbEventProgress == ORB_EVENT_CAST_STARTED)
            {
                if (m_uiOrbTimer < uiDiff)
                {
                    if (Creature* pOrb = m_creature->GetMap()->GetCreature(m_uiOrbGUID))
                        pOrb->ForcedDespawn();
                                
                        m_uiOrbEventProgress = ORB_EVENT_NOT_SPAWNED;

                        if (Creature* pMob = m_creature->GetMap()->GetCreature(m_uiCurrentMobGUID))
                        {
                            pMob->setFaction(14);
                            pMob->SetInCombatWithZone();
                        }
                }
                else
                    m_uiOrbTimer -= uiDiff;
                return;
            }

            if (m_uiCheckTimer < uiDiff)
            {
                if (m_pInstance && m_pInstance->GetData(TYPE_GORTOK) == IN_PROGRESS && !m_uiCurrentMobGUID)
                {
                    if (m_uiOrbEventProgress == ORB_EVENT_NOT_SPAWNED && ((!mobList.empty() && !m_bIsRegularMode) || (mobList.size() > 2 && m_bIsRegularMode)))
                    {

                        std::vector<uint32>::iterator itr = mobList.begin();
                        std::advance(itr,rand()%mobList.size());

                        if (Creature* pCreature = GetClosestCreatureWithEntry(m_creature,*itr,100.f))
                        {
                            m_uiCurrentMobGUID = pCreature->GetGUID();
                            if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_STASIS_GENERATOR, 120.f))
                                if (Creature* pOrb = m_creature->SummonCreature(NPC_ORB, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ()+5, 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                                {
                                    pOrb->CastSpell(pOrb, SPELL_ORB_VISUAL, true);
                                    pOrb->AddSplineFlag(SPLINEFLAG_FLYING);
                                    pOrb->GetMotionMaster()->MovePoint(1, orbWP[0][0], orbWP[0][1], orbWP[0][2]);
                                    m_uiOrbEventProgress = ORB_EVENT_MOVING;
                                }
                            //m_uiOrbEventProgress = ORB_EVENT_CAST_STARTED; // remove this line when mob npc exists in db
                        }
                        mobList.erase(itr);
                        m_uiCheckTimer = 1000;
                    }
                    else // all mobs dead, boss starts fight
                    {
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_GORTOK, SPECIAL);

                        m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                        m_creature->setFaction(14);
                        m_creature->SetInCombatWithZone();
                    }
                }
                else if (m_uiCurrentMobGUID)
                {
                    if (Creature* pMob = m_creature->GetMap()->GetCreature(m_uiCurrentMobGUID))
                        if (!pMob->isAlive())
                        {
                            m_uiCurrentMobGUID = 0;
                            m_uiCheckTimer = 5000;
                        }
                }
            }
            else
                m_uiCheckTimer -= uiDiff;

            return;
        }

        if (m_uiImpaleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H);
            m_uiImpaleTimer = urand(4500,6000);
        }
        else
            m_uiImpaleTimer -= uiDiff;

        if (m_uiSmashTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCING_SMASH);
            m_uiSmashTimer = urand(3500,7000);
        }
        else
            m_uiSmashTimer -= uiDiff;

        if (m_uiRoarTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WITHERING_ROAR : SPELL_WITHERING_ROAR_H);
            m_uiRoarTimer = urand(25000,40000);
        }
        else
            m_uiRoarTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

bool GOHello_go_stasis_generator(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (m_pInstance->GetData(TYPE_GORTOK) == NOT_STARTED)
            m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);
    }

    return true;
}

void AddSC_boss_gortok()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gortok";
    pNewScript->GetAI = &GetAI_boss_gortok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_stasis_generator";
    pNewScript->pGOUse = &GOHello_go_stasis_generator;
    pNewScript->RegisterSelf();
}
