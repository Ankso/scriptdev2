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
SDName: battlegroundSA
SD%Complete: 80%
SDComment: completely rewriten by Ankso
SDCategory: Battleground Strand of the Ancients - vehicles and Seaforium charges handling.
EndScriptData */

#include "precompiled.h"
#include "BattleGroundSA.h"
#include "Vehicle.h"
 
#define SPELL_SEFORIUM_DAMAGE   52408
#define SPELL_SEFORIUM_VISUAL_1 34602
#define SPELL_SEFORIUM_VISUAL_2 71495

struct MANGOS_DLL_DECL npc_sa_bombAI : public ScriptedAI
{
    npc_sa_bombAI(Creature* pCreature) : ScriptedAI(pCreature) { SetCombatMovement(false); Reset();	}
 	uint32 event_bomb;
 	float fx, fy, fz;
    void Reset() { m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE); event_bomb = 10000; }
    void Aggro(Unit* who){}
    void JustDied(Unit* Killer){ m_creature->ForcedDespawn(); }
    void KilledUnit(Unit *victim){}
    void UpdateAI(const uint32 diff)
    {
 		if (event_bomb < diff)
 		{
 			m_creature->GetPosition(fx, fy, fz);
 			m_creature->CastSpell(m_creature, SPELL_SEFORIUM_VISUAL_1, true);
 			m_creature->CastSpell(m_creature, SPELL_SEFORIUM_VISUAL_2, true);
 			m_creature->CastSpell(fx, fy, fz, SPELL_SEFORIUM_DAMAGE, true, 0, 0, m_creature->GetCharmerGuid());
 			m_creature->ForcedDespawn();
 		} else event_bomb -= diff;
 	}
};
 
CreatureAI* GetAI_npc_sa_bomb(Creature* pCreature)
{
    return new npc_sa_bombAI (pCreature);
}
 
#define GOSSIP_START_EVENT_1		 "Comenzar a construir el Demoledor."
#define GOSSIP_START_EVENT_2		 "No tienes nada que hacer ahora!"
#define GOSSIP_EVENT_STARTED         "Ya estoy trabajando en ello!"

#define NPC_DEMILISHER		28781

#define SA_MESSAGE_0		"Empieza a fabricarse el Demoledor"
#define SA_MESSAGE_1		"Tiempo restante: 1 minuto"
#define SA_MESSAGE_1_1		"Tiempo restante: 40 segundos"
#define SA_MESSAGE_1_2		"Tiempo restante: 35 segundos"
#define SA_MESSAGE_2		"Tiempo restante: 30 segundos"
#define SA_MESSAGE_3		"Tiempo restante: 25 segundos"
#define SA_MESSAGE_4		"Tiempo restante: 20 segundos"
#define SA_MESSAGE_5		"Tiempo restante: 15 segundos"
#define SA_MESSAGE_6		"Tiempo restante: 10 segundos"
#define SA_MESSAGE_7		"Tiempo restante: 5 segundos"
#define SA_MESSAGE_8		"Listo!"

static float SummonLocations[2][4]=
{
     {1371.06f, -317.046f, 35.001f, 1.916f},
     {1353.24f, 223.91f, 35.2535f, 4.366f},
};

bool build;

struct MANGOS_DLL_DECL npc_sa_vendorAI : public ScriptedAI
{
    npc_sa_vendorAI(Creature* pCreature) : ScriptedAI(pCreature){ Reset(); }
    uint32 build_time;
 	uint8 gydId;
    void Reset(){}
 	void StartEvent(Player* pPlayer, uint8 gyd)
    {
 		build_time = 60000;
 		gydId = gyd;
 		build = true;
 		m_creature->MonsterSay(SA_MESSAGE_0,LANG_UNIVERSAL,0);
 		m_creature->MonsterSay(SA_MESSAGE_1,LANG_UNIVERSAL,0);
    }
 
    void UpdateAI(const uint32 diff)
    {
        if (build)
 		{
 			if (build_time < diff)
 			{
 				m_creature->SummonCreature(NPC_DEMILISHER,SummonLocations[gydId][0],SummonLocations[gydId][1],SummonLocations[gydId][2],SummonLocations[gydId][3], TEMPSUMMON_DEAD_DESPAWN, 0);
 				m_creature->MonsterSay(SA_MESSAGE_8,LANG_UNIVERSAL,0);
 				build=false;
 			}else build_time -= diff;
 
 			switch(build_time/2)
 			{
 				case 15000: m_creature->MonsterSay(SA_MESSAGE_2,LANG_UNIVERSAL,0); break;
 				case 7500: m_creature->MonsterSay(SA_MESSAGE_5,LANG_UNIVERSAL,0); break;
 			}
        }
    }
};
 
CreatureAI* GetAI_npc_sa_vendor(Creature* pCreature)
{
    return new npc_sa_vendorAI(pCreature);
}
 
bool GossipHello_npc_sa_vendor(Player* pPlayer, Creature* pCreature)
{
 	uint8 gyd = NULL;
 	if (pCreature->GetEntry() == 29260) 	
        gyd = 0;	
 	if (pCreature->GetEntry() == 29262) 
        gyd = 1;
    if (!build)
    {
 	    if (pPlayer->GetMapId() == 607)
            if (BattleGround *bg = pPlayer->GetBattleGround())
                if (bg->GetController() != pPlayer->GetTeam())
 		            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
 	    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
    }
    else
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    
    return true;
}
 
bool GossipSelect_npc_sa_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        uint8 gyd = NULL;
 		if (pCreature->GetEntry() == 29260)
            gyd = 0;
 		if (pCreature->GetEntry() == 29262)
            gyd = 1;
         pPlayer->CLOSE_GOSSIP_MENU();
 		((npc_sa_vendorAI*)pCreature->AI())->StartEvent(pPlayer, gyd);
     }
     if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
         pPlayer->CLOSE_GOSSIP_MENU();
     return true;
}
 
/*######
## go_wintergrasp_def_portal
######*/
 
static float SpawnLocation[7][3]=
{
    {1468.380005f, -225.798996f, 30.896200f}, //blue
    {1394.270020f, 72.551399f, 31.054300f},   //green
    {1216.069946f, 47.904301f, 54.278198f},   //purple
    {1255.569946f, -233.548996f, 56.43699f},  //red
    {1065.260010f, -89.79501f, 81.073402f},   //yellow
    {880.162f, -95.979f, 109.835f},
    {880.68f, -120.799f, 109.835f},
};
 
static float TeleLocation[7][3]=
{
    {1451.72f, -224.863f, 41.9564f},
    {1401.6f, 88.6693f, 41.1833f},
    {1210.68f, 60.3558f, 64.7388f},
    {1245.03f, -226.439f, 66.7201f},
    {1062.83f, -87.1955f, 93.8061f},
    {808.447f, -109.192f, 109.835f},
    {808.447f, -109.192f, 109.835f},
};
 
#define SPELL_JUST_TELEPORTED 54640

bool GOUse_go_wintergrasp_def_portal(Player* pPlayer, GameObject* pGo)
{
    float x, y, z;
    pGo->GetPosition(x, y, z);

    if (!pPlayer)
        return false;

    if (pPlayer->HasAura(54643))
        return false;

    if (pPlayer->GetMapId() == 607)
 	{
 		if (BattleGround *bg = pPlayer->GetBattleGround())
 		{
 			if (pPlayer->GetTeam() == ((BattleGroundSA*)bg)->GetController() || pPlayer->isGameMaster())
 			{
 				for (uint8 i = 0; i < 7; ++i)
 				{
 					if ((int32(x) == int32(SpawnLocation[i][0])) && 
 						(int32(y) == int32(SpawnLocation[i][1])) &&
 						(int32(z) == int32(SpawnLocation[i][2])))
 					{
 						pPlayer->TeleportTo(bg->GetMapId(), TeleLocation[i][0], TeleLocation[i][1], TeleLocation[i][2], 0);
                        pPlayer->CastSpell(pPlayer, SPELL_JUST_TELEPORTED, true);
 						return true;
 					}
 				}
 			} 
            else 
                pPlayer->MonsterSay("No puedo usar eso!", LANG_UNIVERSAL, pPlayer);
 		}
 	}
 	return false;
}
 
void AddSC_battlegroundSA()
{
    Script *newscript;
 
 	newscript = new Script;
    newscript->Name="npc_sa_bomb";
    newscript->GetAI = &GetAI_npc_sa_bomb;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "npc_sa_vendor";
    newscript->GetAI = &GetAI_npc_sa_vendor;
    newscript->pGossipHello = &GossipHello_npc_sa_vendor;
    newscript->pGossipSelect = &GossipSelect_npc_sa_vendor;
    newscript->RegisterSelf();
 
    newscript = new Script;
    newscript->Name = "go_sa_def_portal";
    newscript->pGOUse = &GOUse_go_wintergrasp_def_portal;
    newscript->RegisterSelf();
}