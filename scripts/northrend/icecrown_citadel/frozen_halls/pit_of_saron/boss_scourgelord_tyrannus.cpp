/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_scourgelord_tyrannus
SD%Complete: 0%
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum Spells
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
        //summons
        //Abilities
        SPELL_FEAR                              = 68950,
        SPELL_FORCEFUL_SMASH                    = 69155,
        SPELL_FORCEFUL_SMASH_H                  = 69627,
        SPELL_OVERLORDS_BRAND                   = 69172,
        SPELL_UNHOLY_POWER                      = 69167,
        SPELL_UNHOLY_POWER_H                    = 69629,
};

enum
{
    SAY_PREFIGHT_1                      = -1658050,
    SAY_GENERAL_TRASH                   = -1658051,
    SAY_PREFIGHT_2                      = -1658052,
    SAY_AGGRO                           = -1658053,
    SAY_SLAY_1                          = -1658054,
    SAY_SLAY_2                          = -1658055,
    SAY_DEATH                           = -1658056,
    SAY_MARK                            = -1658057,
    SAY_SMASH                           = -1658058,

    EMOTE_RIMEFANG_ICEBOLT              = -1658059,
    EMOTE_SMASH                         = -1658060,
};

enum phases
{
    PHASE_STAND_BY  = 0,
    PHASE_DIALOGUE  = 1,
    PHASE_IN_COMBAT = 2,
};

enum timers
{
    TIMER_DIALOGUE = 5*IN_MILLISECONDS,
    TIMER_SPELL_SMASH = 30*IN_MILLISECONDS,
    TIMER_SPELL_OVERLORDS_BRAND = 30*IN_MILLISECONDS,
};

struct MANGOS_DLL_DECL boss_scourgelord_tyrannusAI : public ScriptedAI
{
    boss_scourgelord_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint8 phase;
    uint32 dialogueTimer;
    uint32 smashTimer;
    uint32 overlordsBrandTimer;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, NOT_STARTED);
        // m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        phase = PHASE_STAND_BY;
        // Timers
        uint32 dialogueTimer = 0;
        uint32 smashTimer = TIMER_SPELL_SMASH;
        uint32 overlordsBrandTimer = TIMER_SPELL_OVERLORDS_BRAND + 15*IN_MILLISECONDS;
    }

    void Aggro(Unit *who)
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);
        DoScriptText(SAY_AGGRO, m_creature);
        phase = PHASE_DIALOGUE;
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, DONE);
        m_creature->SetRespawnDelay(7*DAY);
        phase = PHASE_STAND_BY;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1)) 
        {
            case 0:
               DoScriptText(SAY_SLAY_1, m_creature, pVictim);
               break;
            case 1:
               DoScriptText(SAY_SLAY_2, m_creature, pVictim);
               break;
        };
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (phase)
        {
            case PHASE_STAND_BY:
                return;
            case PHASE_DIALOGUE:
            {
                /*m_creature->GetMotionMaster()->MovementExpired();
                if (dialogueTimer == 0)
                {
                    DoScriptText(SAY_PREFIGHT_1, m_creature);
                    dialogueTimer = TIMER_DIALOGUE;
                    return;
                }
                else if (dialogueTimer < diff)
                {
                    DoScriptText(SAY_PREFIGHT_2, m_creature);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    phase = PHASE_IN_COMBAT;
                }
                else
                {
                    dialogueTimer -= diff;
                    return;
                }*/
                phase = PHASE_IN_COMBAT;
                break;
            }
            case PHASE_IN_COMBAT:
            {
                if (smashTimer < diff)
                {
                    DoScriptText(SAY_SMASH, m_creature);
                    DoScriptText(EMOTE_SMASH, m_creature);
                    DoCast(m_creature->getVictim(), pInstance->instance->GetDifficulty() == DUNGEON_DIFFICULTY_NORMAL ? SPELL_FORCEFUL_SMASH : SPELL_FORCEFUL_SMASH_H);
                    DoCast(m_creature, pInstance->instance->GetDifficulty() == DUNGEON_DIFFICULTY_NORMAL ? SPELL_UNHOLY_POWER : SPELL_UNHOLY_POWER_H);
                    smashTimer = TIMER_SPELL_SMASH;
                }
                else
                    smashTimer -= diff;

                if (overlordsBrandTimer < diff)
                {
                    DoCast(m_creature->getVictim(), SPELL_OVERLORDS_BRAND);
                    overlordsBrandTimer = TIMER_SPELL_OVERLORDS_BRAND;
                }
                else
                    overlordsBrandTimer -= diff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_rimefang_posAI : public ScriptedAI
{
    mob_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_OFF);
    }

    void Aggro(Unit *who) 
    {
        return;
    }

    void JustDied(Unit *killer)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_scourgelord_tyrannus(Creature* pCreature)
{
    return new boss_scourgelord_tyrannusAI(pCreature);
}

CreatureAI* GetAI_mob_rimefang_pos(Creature* pCreature)
{
    return new mob_rimefang_posAI(pCreature);
}


void AddSC_boss_tyrannus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_scourgelord_tyrannus";
    newscript->GetAI = &GetAI_boss_scourgelord_tyrannus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_rimefang_pos";
    newscript->GetAI = &GetAI_mob_rimefang_pos;
    newscript->RegisterSelf();
}