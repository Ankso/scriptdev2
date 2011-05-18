/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Tharonja
SD%Complete: 80%
SDComment: Timers
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_KING_DRED_TALON                = -1600020,
    SAY_CALL_FOR_RAPTOR                = -1600021,
    
    SPELL_BELLOWING_ROAR               = 22686,
    SPELL_FEARSOME_ROAR                = 48849,
    SPELL_FEARSOME_ROAR_H              = 59422,
    SPELL_GRIEVOUS_BITE                = 48920,
    SPELL_MANGLING_SLASH               = 48873,
    SPELL_PIERCING_SLASH               = 48878,
    SPELL_RAPTOR_CALL                  = 59416,            //not yet implemented
};

const float PosSummon1[3] = {-528.8f, -690.58f, 30.25f};

/*######
## boss_dred
######*/

struct MANGOS_DLL_DECL boss_dredAI : public ScriptedAI
{
    boss_dredAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bCalledForRaptorSpawn;
    uint32 m_uiFearsomeRoarTimer;
    uint32 m_uiManglingSlashTimer;
    uint32 m_uiPiercingSlashTimer;
    uint32 m_uiGrievousBiteTimer;
    uint32 m_uiBellowingRoarTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiCallForRaptorTimer;
    uint32 m_uiCallForRaptorSpawnTimer;
    
    void Reset()
    {
        m_uiFearsomeRoarTimer = 15*IN_MILLISECONDS;
        m_uiManglingSlashTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        m_uiPiercingSlashTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        m_uiGrievousBiteTimer = urand (15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        m_uiBellowingRoarTimer = 1*MINUTE*IN_MILLISECONDS;
        m_uiCheckTimer = 15*IN_MILLISECONDS;
        m_uiCallForRaptorTimer = 25*IN_MILLISECONDS;
        m_bCalledForRaptorSpawn = false;
    }

    void Aggro(Unit* pWho)
    {
    }

    void KilledUnit(Unit* pVictim)
    {
    }

    void JustDied(Unit* pKiller)
    {
    }
    
    void CallForRaptorSpawnCheck()
    {
        m_uiCallForRaptorSpawnTimer = 1*IN_MILLISECONDS;
        m_bCalledForRaptorSpawn = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFearsomeRoarTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FEARSOME_ROAR : SPELL_FEARSOME_ROAR_H, true);
            m_uiFearsomeRoarTimer = 15*IN_MILLISECONDS;
        }
        else
            m_uiFearsomeRoarTimer -= uiDiff;

        if (m_uiPiercingSlashTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_PIERCING_SLASH, true);
            m_uiPiercingSlashTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }
        else
            m_uiPiercingSlashTimer -= uiDiff;

        if (m_uiManglingSlashTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_MANGLING_SLASH, true);
            m_uiManglingSlashTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }
        else
            m_uiManglingSlashTimer -= uiDiff;

        if (m_uiGrievousBiteTimer < uiDiff)
        {    
            DoCast(m_creature->getVictim(), SPELL_GRIEVOUS_BITE, true);
            m_uiGrievousBiteTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
        }
        else
            m_uiGrievousBiteTimer -= uiDiff;

        if (m_uiCheckTimer < uiDiff)
        {
            if (Unit* pPlayer = m_creature->getVictim())
                if (pPlayer->GetHealth() == pPlayer->GetMaxHealth())
                    if (pPlayer->HasAura(SPELL_GRIEVOUS_BITE))
                        pPlayer->RemoveAura(SPELL_GRIEVOUS_BITE, EFFECT_INDEX_0);

            m_uiCheckTimer = 1*IN_MILLISECONDS;
        }
        else
            m_uiCheckTimer -= uiDiff;
        
        if (m_uiBellowingRoarTimer < uiDiff)
        {    
            DoCast(m_creature, SPELL_BELLOWING_ROAR);
            m_uiBellowingRoarTimer = 1*MINUTE*IN_MILLISECONDS;
        }
        else
            m_uiBellowingRoarTimer -= uiDiff;

        if (m_uiCallForRaptorTimer < uiDiff)
        {    
            DoScriptText(SAY_CALL_FOR_RAPTOR, m_creature);
            m_creature->CastSpell(m_creature, SAY_CALL_FOR_RAPTOR, true);
            m_uiCallForRaptorTimer = 25*IN_MILLISECONDS;
            CallForRaptorSpawnCheck();
        }
        else
            m_uiCallForRaptorTimer -= uiDiff;

        if (m_uiCallForRaptorSpawnTimer < uiDiff && m_bCalledForRaptorSpawn)
        {    
            std::list<Creature*> assistList;

            GetCreatureListWithEntryInGrid(assistList, m_creature, NPC_DRAKKARI_GUTRIPPER, 30.0f);
            if (assistList.empty())
                GetCreatureListWithEntryInGrid(assistList, m_creature, NPC_DRAKKARI_SCYTHECLAW, 30.0f);

            if (!assistList.empty())
            {
                Creature* target = *(assistList.begin());
                if (target && target->isAlive())
                    target->AI()->AttackStart(m_creature->getVictim());
            }

            m_bCalledForRaptorSpawn = false;
        }
        else
            m_uiCallForRaptorSpawnTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dred(Creature* pCreature)
{
    return new boss_dredAI(pCreature);
}

void AddSC_boss_dred()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_dred";
    pNewScript->GetAI = &GetAI_boss_dred;
    pNewScript->RegisterSelf();
}
