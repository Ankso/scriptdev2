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
SD%Complete: 90%
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

static float FlyLocations[9][3]=
{
	{1023.99, 129.12, 674.573},
	{1002.09, 132.681, 674.232},
	{976.516, 145.066, 670.752},
	{982.668, 168.784, 672.454},
	{1011.71, 179.231, 673.174},
	{1035.29, 172.959, 670.7},
	{1056.73, 155.03, 671.419},
	{1051.14, 129.016, 672.91},
	{892.534, 166.436, 660.396},
};

struct MANGOS_DLL_DECL boss_scourgelord_tyrannusAI : public ScriptedAI
{
    boss_scourgelord_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
    }
	
	ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;

	uint32 m_uiForcefulSmash_Timer;
	uint32 m_uiOverlordsBrand_Timer;
	uint32 m_uiDarkMight_Timer;
	uint8  m_uiPhase;
	uint32 m_uiPhase_Timer;

    void Reset()

    {
 		m_uiForcefulSmash_Timer = 10000;
		m_uiOverlordsBrand_Timer = 35000;
		m_uiDarkMight_Timer = 40000;
		m_uiPhase = 1;
		m_uiPhase_Timer = 7500;
		if (m_pInstance)
            	m_pInstance->SetData(TYPE_TYRANNUS, NOT_STARTED);
		m_creature->Mount(27982);
    }

    void Aggro(Unit *who) 
    {
		DoScriptText(SAY_AGGRO, m_creature);
		if (m_pInstance)
            	m_pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);	
		m_creature->Unmount();
		m_creature->SummonCreature(NPC_RIMEFANG , m_creature->GetPositionX(), m_creature->GetPositionY(), 			m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 900000);
		m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void JustDied(Unit *killer)
    {
		DoScriptText(SAY_DEATH, m_creature);
		if (m_pInstance)
            m_pInstance->SetData(TYPE_TYRANNUS, DONE);
			
		Map* pMap = m_creature->GetMap();

		if (pMap && pMap->IsDungeon())
		{
			Map::PlayerList const &players = pMap->GetPlayers();
			for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
		
			if (itr->getSource()->GetTeam() == ALLIANCE)
				m_creature->SummonCreature(NPC_JAINA_PART2, 1184.49, 313.353, 627.621, 2.46317, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
			else
				m_creature->SummonCreature(NPC_SYLVANAS_PART2, 1184.49, 313.353, 627.621, 2.46317, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
		}
    }

	void KilledUnit(Unit *victim)
    {
        switch (urand(0,1))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (m_uiForcefulSmash_Timer < diff)
		{
			DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FORCEFUL_SMASH : SPELL_FORCEFUL_SMASH_H);
			m_uiForcefulSmash_Timer = 10000;
		}
		else 
			m_uiForcefulSmash_Timer -= diff;
		
		if (m_uiOverlordsBrand_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_OVERLORDS_BRAND);
			m_uiOverlordsBrand_Timer = 45000;
		}
		else 
			m_uiOverlordsBrand_Timer -= diff;
			
		if (m_uiDarkMight_Timer < diff)
		{
			//DoScriptText(SAY_DARK_MIGHT_1, m_creature);
			DoCast(m_creature, m_bIsRegularMode ? SPELL_DARK_MIGHT : SPELL_DARK_MIGHT_H);
			m_uiDarkMight_Timer = 60000;
		}
		else 
			m_uiDarkMight_Timer -= diff;
 
		DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_rimefang_posAI : public ScriptedAI
{
    mob_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
	m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0x3);
	m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
	
	uint32 m_uiMarkRimefang_Timer;
	uint32 m_uiHoarfrost_Timer;
	uint32 m_uiIcyBlast_Timer;
	uint32 m_uiIcyBlast2_Timer;
	uint32 m_uiMovement_Timer;

    void Reset()
    {
		m_uiMarkRimefang_Timer = 25000;
		m_uiHoarfrost_Timer = 999999;
		m_uiIcyBlast_Timer = 35000;
		m_uiIcyBlast2_Timer = 999999;
		m_uiMovement_Timer = 1000;
    }

    void Aggro(Unit *who) 
    {
 		SetCombatMovement(false);
		m_creature->GetMotionMaster()->Clear(false);
        	m_creature->GetMotionMaster()->MoveIdle();
		m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0x3);
		m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
		if (!m_creature->isHover())
        {
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            m_creature->SetHover(true);
        }
		if (!m_creature->GetMotionMaster()->empty() && (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE))
            m_creature->GetMotionMaster()->Clear(false);
	}

    void UpdateAI(const uint32 diff)
    {
	if (m_pInstance && m_pInstance->GetData(TYPE_TYRANNUS) != IN_PROGRESS)
		m_creature->ForcedDespawn();		
		
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
			
		if (m_uiMovement_Timer < diff)
        {			
			uint32 random = rand()%8;

			m_creature->GetMap()->CreatureRelocation(m_creature, FlyLocations[random][0], FlyLocations[random][1], FlyLocations[random][2], 0.0f);
			m_creature->SendMonsterMove(FlyLocations[random][0], FlyLocations[random][1], FlyLocations[random][2], SPLINETYPE_NORMAL, SPLINEFLAG_UNKNOWN7, 1);

			m_uiMovement_Timer = 13000;
        }


		if (m_uiMarkRimefang_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_MARK_OF_RIMEFANG);
			m_uiMarkRimefang_Timer = 20000;
			m_uiHoarfrost_Timer = 5000;
		}
		else 
			m_uiMarkRimefang_Timer -= diff;
			
		if (m_uiHoarfrost_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_HOARFROST);
			m_uiMarkRimefang_Timer = 22000;
			m_uiHoarfrost_Timer = 99999;
		}
		else 
			m_uiHoarfrost_Timer -= diff;
			
		if (m_uiIcyBlast_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, m_bIsRegularMode ? SPELL_ICY_BLAST : SPELL_ICY_BLAST_H);
			m_uiIcyBlast_Timer = 29000;
			m_uiIcyBlast2_Timer = 5000;
		}
		else 
			m_uiIcyBlast_Timer -= diff;
		
		if (m_uiIcyBlast2_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, m_bIsRegularMode ? SPELL_ICY_BLAST_2 : SPELL_ICY_BLAST_2_H);
			m_uiIcyBlast_Timer = 30500;
			m_uiIcyBlast2_Timer = 99999;
		}
		else 
			m_uiIcyBlast2_Timer -= diff;
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
