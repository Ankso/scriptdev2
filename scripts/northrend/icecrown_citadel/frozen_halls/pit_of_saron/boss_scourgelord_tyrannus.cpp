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

// Scripted by Tacx/Chris - http://www.blood-wow.com (if you use this script, do not remove this seal, no matter what other modification you apply to script).
// Need to make the fly moves for Rimefang
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
		RegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
    }
	
	ScriptedInstance* m_pInstance;

    bool RegularMode;

	uint32 ForcefulSmashTimer;
	uint32 OverlordsBrandTimer;
	uint32 DarkMighttimer;
	uint8 Phase;
	uint32 PhaseTimer;

    void Reset()

    {
 		ForcefulSmashTimer = 10000;
		OverlordsBrandTimer = 35000;
		DarkMighttimer = 40000;
		Phase = 1;
		PhaseTimer = 7500;
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

		if (ForcefulSmashTimer < diff)
		{
			DoCast(m_creature->getVictim(), RegularMode ? SPELL_FORCEFUL_SMASH : SPELL_FORCEFUL_SMASH_H);
			ForcefulSmashTimer = 10000;
		}
		else 
			ForcefulSmashTimer -= diff;
		
		if (OverlordsBrandTimer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_OVERLORDS_BRAND);
			OverlordsBrandTimer = 45000;
		}
		else 
			OverlordsBrandTimer -= diff;
			
		if (DarkMighttimer < diff)
		{
			//DoScriptText(SAY_DARK_MIGHT_1, m_creature);
			DoCast(m_creature, RegularMode ? SPELL_DARK_MIGHT : SPELL_DARK_MIGHT_H);
			DarkMighttimer = 60000;
		}
		else 
			DarkMighttimer -= diff;
 
		DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_rimefang_posAI : public ScriptedAI
{
    mob_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
	RegularMode = pCreature->GetMap()->IsRegularDifficulty();
	m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0x3);
	m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool RegularMode;
	
	uint32 MarkRimefangTimer;
	uint32 HoarfrostTimer;
	uint32 IcyBlastTimer;
	uint32 IcyBlast2Timer;
	uint32 MovementTimer;

    void Reset()
    {
		MarkRimefangTimer = 25000;
		HoarfrostTimer = 999999;
		IcyBlastTimer = 35000;
		IcyBlast2Timer = 999999;
		MovementTimer = 1000;
    }

    void Aggro(Unit *who) 
    {
 		SetCombatMovement(false);
		m_creature->GetMotionMaster()->Clear(false);
        	m_creature->GetMotionMaster()->MoveIdle();

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
			
		if (MarkRimefangTimer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_MARK_OF_RIMEFANG);
			MarkRimefangTimer = 20000;
			HoarfrostTimer = 5000;
		}
		else 
			MarkRimefangTimer -= diff;
			
		if (HoarfrostTimer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_HOARFROST);
			MarkRimefangTimer = 22000;
			HoarfrostTimer = 99999;
		}
		else 
			HoarfrostTimer -= diff;
			
		if (IcyBlastTimer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, RegularMode ? SPELL_ICY_BLAST : SPELL_ICY_BLAST_H);
			IcyBlastTimer = 29000;
			IcyBlast2Timer = 5000;
		}
		else 
			IcyBlastTimer -= diff;
		
		if (IcyBlast2Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, RegularMode ? SPELL_ICY_BLAST_2 : SPELL_ICY_BLAST_2_H);
			IcyBlastTimer = 30500;
			IcyBlast2Timer = 99999;
		}
		else 
			IcyBlast2Timer -= diff;
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
