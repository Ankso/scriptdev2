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
SDName: boss_forgemaster_gafrost
SD%Complete: 0%
SDComment: by /dev/rsa
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "def_pit.h"
enum
{
        //common
        SPELL_BERSERK				= 47008,
        //yells
	SAY_AGGRO				= -1658001,
	SAY_SLAY_1				= -1658002,
	SAY_SLAY_2				= -1658003,
	SAY_DEATH				= -1658004,
	SAY_AXE					= -1658005,
	SAY_DEEPFREZE				= -1658006,
	SAY_TYRANNUS_DEATH			= -1659007,
        //summons
        //Abilities
	SPELL_PERMAFROST                            = 70326,
	SPELL_PERMAFROST_TRIGGER                    = 68786,
	SPELL_THROW_SARONITE                        = 68788,
	SPELL_THUNDERING_STOMP                      = 68771,
	SPELL_CHILLING_WAVE                         = 68778,
	SPELL_CHILLING_WAVE_H                       = 70333,
	SPELL_DEEP_FREEZE                           = 70381,
	SPELL_DEEP_FREEZE_H                         = 72930,
	SPELL_FORGE_MACE                            = 68785,
	SPELL_FORGE_MACE_H                          = 70335,
	SPELL_FORGE_BLADE                           = 68774,
	SPELL_FORGE_BLADE_H                         = 70334,
	EQUIP_ID_SWORD                              = 49345,
	EQUIP_ID_MACE                               = 49344,
	ACHIEV_DOESNT_GO_TO_ELEVEN                  = 4524

};

struct MANGOS_DLL_DECL boss_forgemaster_gafrostAI : public ScriptedAI
{
    boss_forgemaster_gafrostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_GAFROST, NOT_STARTED);
    }

    void Aggro(Unit *who) 
    {
        if(pInstance) pInstance->SetData(TYPE_GAFROST, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_GAFROST, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_forgemaster_gafrost(Creature* pCreature)
{
    return new boss_forgemaster_gafrostAI(pCreature);
}


void AddSC_boss_forgemaster_gafrost()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_forgemaster_gafrost";
    newscript->GetAI = &GetAI_boss_forgemaster_gafrost;
    newscript->RegisterSelf();

}
