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
SDName: boss_ick && boss_krick
SD%Complete: 60%
SDComment: 
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "def_pit.h"
enum
{
    //common
    SPELL_BERSERK                         = 47008,
    //yells
    SAY_KRICK_AGGRO                             = -1658010,
    SAY_KRICK_SLAY_1                            = -1658011,
    SAY_KRICK_SLAY_2                            = -1658012,
    SAY_KRICK_BARRAGE_1                         = -1658013,
    SAY_KRICK_BARRAGE_2                         = -1658014,
    SAY_KRICK_POISON_NOVA                       = -1658015,
    SAY_KRICK_CHASE_1                           = -1658016,
    SAY_KRICK_CHASE_2                           = -1658017,
    SAY_KRICK_CHASE_3                           = -1658018,
    SAY_ICK_POISON_NOVA                         = -1658020,
    SAY_ICK_CHASE_1                             = -1658021,
    SAY_KRICK_OUTRO_1                           = -1658030,
    SAY_JAYNA_OUTRO_2                           = -1658031,
    SAY_SYLVANAS_OUTRO_2                        = -1658032,
    SAY_KRICK_OUTRO_3                           = -1658033,
    SAY_JAYNA_OUTRO_4                           = -1658034,
    SAY_SYLVANAS_OUTRO_4                        = -1658035,
    SAY_KRICK_OUTRO_5                           = -1658036,
    SAY_TYRANNUS_OUTRO_7                        = -1658037,
    SAY_KRICK_OUTRO_8                           = -1658038,
    SAY_TYRANNUS_OUTRO_9                        = -1658039,
    SAY_JAYNA_OUTRO_10                          = -1658040,
    SAY_SYLVANAS_OUTRO_10                       = -1658041,
    //summons
    NPC_EXPLODING_ORB                     = 36610,
    //Abilities
    SPELL_FEAR                            = 68950,
    SPELL_EXPLOSIVE_ORB                   = 69019,
    SPELL_EXPLOSIVE                       = 69012,
    SPELL_SHADOWBOLT                      = 69028,
    SPELL_STRANGULATE                     = 69413,
    SPELL_TOXIC                           = 69024,
    SPELL_TOXIC_H                         = 70436,
    SPELL_KICK                            = 69021,
    SPELL_POISON                          = 68989,
    SPELL_POISON_H                        = 70434,
    SPELL_PURSUIT                         = 68987,
    SPELL_PUSTULANT                       = 69581,
    SPELL_CONFUSION                       = 69029,
    SPELL_EXPLOSIVE_BARRAGE               = 69263
};

bool m_bIsOutro;

struct MANGOS_DLL_DECL boss_ickAI : public ScriptedAI
{
    boss_ickAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	Creature *Krick;

    bool m_bIsRegularMode;
	
	uint32 m_uiToxicWast_Timer;
	uint32 m_uiPoisonNova_Timer;
	uint32 m_uiShadowBolt_Timer;
	uint32 m_uiMightyKick_Timer;
	uint32 m_uiExplosiveBarrage_Timer;
	uint32 m_uiPursued_Timer;

    void Reset()
    {
		m_uiToxicWast_Timer = 5000;
		m_uiPoisonNova_Timer = 30000;
		m_uiShadowBolt_Timer = 15000;
		m_uiMightyKick_Timer = 20000;
		m_uiExplosiveBarrage_Timer = 35000;
		m_uiPursued_Timer = 25000;
		m_bIsOutro = false;
	if(!m_pInstance) return;
            m_pInstance->SetData(TYPE_KRICK, NOT_STARTED);
		Krick = GetClosestCreatureWithEntry(m_creature, NPC_KRICK, 300.0f);

    }

    void Aggro(Unit *who) 
    {
        DoScriptText(SAY_KRICK_AGGRO, Krick);
	if(!m_pInstance) return;
            m_pInstance->SetData(TYPE_KRICK, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
	if(!m_pInstance) return;
            m_pInstance->SetData(TYPE_KRICK, DONE); 
		m_bIsOutro = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
				
		if (m_uiToxicWast_Timer < diff)
		{
			DoCast(m_creature, m_bIsRegularMode ? SPELL_TOXIC : SPELL_TOXIC_H);
			m_uiToxicWast_Timer = 5000;
        }
		else 
			m_uiToxicWast_Timer -= diff;
			
		if (m_uiPoisonNova_Timer < diff)
		{
			DoCast(m_creature, m_bIsRegularMode ? SPELL_POISON : SPELL_POISON_H);
			m_uiPoisonNova_Timer = 30000;
        }
		else 
			m_uiPoisonNova_Timer -= diff;
			
		if (m_uiShadowBolt_Timer < diff)
		{
			if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(Target, SPELL_SHADOWBOLT);
			m_uiShadowBolt_Timer = 15000;
        }
		else 
			m_uiShadowBolt_Timer -= diff;
			
		if (m_uiMightyKick_Timer < diff)
		{
			Map *map = m_creature->GetMap();
			Map::PlayerList const &PlayerList = map->GetPlayers();
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
			{
				if (m_creature->IsWithinDistInMap(i->getSource(), 5.0f))
					DoCast(i->getSource(), SPELL_KICK);
			}
			m_uiMightyKick_Timer = 22000;
        }
		else 
			m_uiMightyKick_Timer -= diff;
			
		if (m_uiExplosiveBarrage_Timer < diff)
		{
			if (Krick)
			{
				DoCast(Krick, SPELL_EXPLOSIVE_BARRAGE);
				Krick->SummonCreature(36879 , 831.567, 177.040, 509.52, 0.125028, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
				DoScriptText(SAY_KRICK_BARRAGE_1, Krick);
			}
			m_uiExplosiveBarrage_Timer = 35000;
        }
		else 
			m_uiExplosiveBarrage_Timer -= diff;
		
		if (m_uiPursued_Timer < diff)
		{
			DoCast(m_creature->getVictim(), SPELL_PURSUIT);
			DoCast(m_creature->getVictim(), SPELL_CONFUSION);
			m_uiPursued_Timer = (m_bIsRegularMode ? 22000 : 18000);
        }
		else 
			m_uiPursued_Timer -= diff;
 
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_krickAI : public ScriptedAI
{
    boss_krickAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
    }
	
	ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;

    void Reset()
    {
		m_creature->setFaction(35);
		m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* who)
    {
        
    }

    void JustDied(Unit* killer)
    {

    }

    void UpdateAI(const uint32 diff)
    {		
		if (m_bIsOutro)
		{
			m_bIsOutro = false;
			DoScriptText(SAY_KRICK_OUTRO_1, m_creature);
				
			Map* pMap = m_creature->GetMap();

			if (pMap && pMap->IsDungeon())
			{
				Map::PlayerList const &players = pMap->GetPlayers();
				for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
		
				if (i->getSource()->GetTeam() == ALLIANCE)
					m_creature->SummonCreature(NPC_VICTUS, 801.455, 113.913, 509.002, 0.125028, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
				else
					m_creature->SummonCreature(NPC_GORKUN, 801.455, 113.913, 509.002, 0.125028, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
			}
		}
    }
};

CreatureAI* GetAI_boss_ick(Creature* pCreature)
{
    return new boss_ickAI(pCreature);
}

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI(pCreature);
}

struct MANGOS_DLL_DECL mob_exploding_orbAI : public ScriptedAI
{
   mob_exploding_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
   }

    ScriptedInstance* m_pInstance;
   uint32 ExplodeTimer;

    void Reset()
    {
      ExplodeTimer = 18000;
    }

    void AttackStart(Unit* who)
    {
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_pInstance) return;

        if (ExplodeTimer < diff)
        {
           DoCast(m_creature, SPELL_EXPLOSIVE_ORB);
           m_creature->ForcedDespawn();
        } else ExplodeTimer -= diff;
        return;
    }

};

CreatureAI* GetAI_mob_exploding_orb(Creature* pCreature)
{
    return new mob_exploding_orbAI(pCreature);
}


void AddSC_boss_krick()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_krick";
    newscript->GetAI = &GetAI_boss_krick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_ick";
    newscript->GetAI = &GetAI_boss_ick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_exploding_orb";
    newscript->GetAI = &GetAI_mob_exploding_orb;
    newscript->RegisterSelf();
}
