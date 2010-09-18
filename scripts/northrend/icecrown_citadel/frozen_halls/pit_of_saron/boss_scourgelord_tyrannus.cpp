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
#include "def_pit.h"

enum
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
	SAY_AMBUSH_1                                = -1658050,
	SAY_AMBUSH_2                                = -1658051,
	SAY_GAUNTLET_START                          = -1658052,
	SAY_INTRO_1                                 = -1658053,
	SAY_INTRO_2                                 = -1658054,
	SAY_AGGRO                                   = -1658055,
	SAY_SLAY_1                                  = -1658056,
	SAY_SLAY_2                                  = -1658057,
	SAY_DEATH                                   = -1658058,
	SAY_MARK_RIMEFANG_1                         = -1658059,
	SAY_MARK_RIMEFANG_2                         = -1658060,
	SAY_DARK_MIGHT_1                            = -1658061,
	SAY_DARK_MIGHT_2                            = -1658062,
	SAY_GORKUN_OUTRO_1                          = -1658063,
	SAY_GORKUN_OUTRO_2                          = -1658064,
	SAY_JAYNA_OUTRO_3                           = -1658065,
	SAY_SYLVANAS_OUTRO_3                        = -1658066,
	SAY_JAYNA_OUTRO_4                           = -1658067,
	SAY_SYLVANAS_OUTRO_4                        = -1658068,
	SAY_JAYNA_OUTRO_5                           = -1658069,
        //summons
        //Abilities
	SPELL_FORCEFUL_SMASH                        = 69155,
	SPELL_FORCEFUL_SMASH_H                      = 69627,
	SPELL_OVERLORDS_BRAND                       = 69172,
	SPELL_DARK_MIGHT                            = 69167,
	SPELL_DARK_MIGHT_H                          = 69629,
	SPELL_HOARFROST                             = 69246,
	SPELL_MARK_OF_RIMEFANG                      = 69275,
	SPELL_ICY_BLAST                             = 69233,
	SPELL_ICY_BLAST_H                           = 69646,
	SPELL_ICY_BLAST_2                           = 69238,
	SPELL_ICY_BLAST_2_H                         = 69628
};

struct MANGOS_DLL_DECL boss_scourgelord_tyrannusAI : public ScriptedAI
{
    boss_scourgelord_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_TYRANNUS, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

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
    }

    void Aggro(Unit *who) 
    {
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


void AddSC_boss_scourgelord_tyrannus()
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
