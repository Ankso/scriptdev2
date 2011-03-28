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
SDName: Dragonblight
SD%Complete: 100
SDComment: Quest support: 12166, 12499/12500(end sequenze). Taxi paths Wyrmrest temple.
SDCategory: Dragonblight
EndScriptData */

/* ContentData
npc_afrasastrasz
npc_alexstrasza_wr_gate
npc_liquid_fire_of_elune
npc_tariolstrasz
npc_torastrasza
EndContentData */

#include "precompiled.h"

/*######
## npc_afrasastrasz
######*/

enum
{
    TAXI_PATH_ID_MIDDLE_DOWN            = 882,
    TAXI_PATH_ID_MIDDLE_TOP             = 881
};

#define GOSSIP_ITEM_TAXI_MIDDLE_DOWN    "I would like to take a flight to the ground, Lord Of Afrasastrasz."
#define GOSSIP_ITEM_TAXI_MIDDLE_TOP     "My Lord, I must go to the upper floor of the temple."

bool GossipHello_npc_afrasastrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_MIDDLE_DOWN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_MIDDLE_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_afrasastrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_MIDDLE_DOWN);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_MIDDLE_TOP);
    }
    return true;
}

/*######
## npc_alexstrasza_wr_gate
######*/

enum
{
    QUEST_RETURN_TO_AG_A    = 12499,
    QUEST_RETURN_TO_AG_H    = 12500,
    MOVIE_ID_GATES          = 14
};

#define GOSSIP_ITEM_WHAT_HAPPENED   "Alexstrasza, can you show me what happened here?"

bool GossipHello_npc_alexstrasza_wr_gate(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestRewardStatus(QUEST_RETURN_TO_AG_A) || pPlayer->GetQuestRewardStatus(QUEST_RETURN_TO_AG_H))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_HAPPENED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_alexstrasza_wr_gate(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->SendMovieStart(MOVIE_ID_GATES);
    }

    return true;
}

/*######
## npc_tariolstrasz
######*/

enum
{
    QUEST_INFORM_QUEEN_A                = 12123,            //need to check if quests are required before gossip available
    QUEST_INFORM_QUEEN_H                = 12124,
    TAXI_PATH_ID_BOTTOM_TOP             = 878,
    TAXI_PATH_ID_BOTTOM_MIDDLE          = 883
};

#define GOSSIP_ITEM_TAXI_BOTTOM_TOP     "My Lord, I must go to the upper floor of the temple."
#define GOSSIP_ITEM_TAXI_BOTTOM_MIDDLE  "Can you spare a drake to travel to Lord Of Afrasastrasz, in the middle of the temple?"

bool GossipHello_npc_tariolstrasz(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_BOTTOM_TOP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_BOTTOM_MIDDLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_tariolstrasz(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_BOTTOM_TOP);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_BOTTOM_MIDDLE);
    }
    return true;
}

/*######
## npc_torastrasza
######*/

enum
{
    TAXI_PATH_ID_TOP_MIDDLE             = 880,
    TAXI_PATH_ID_TOP_BOTTOM             = 879
};

#define GOSSIP_ITEM_TAXI_TOP_MIDDLE     "I would like to see Lord Of Afrasastrasz, in the middle of the temple."
#define GOSSIP_ITEM_TAXI_TOP_BOTTOM     "Yes, Please. I would like to return to the ground floor of the temple."

bool GossipHello_npc_torastrasza(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_TOP_MIDDLE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TAXI_TOP_BOTTOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_torastrasza(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_TOP_MIDDLE);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->ActivateTaxiPathTo(TAXI_PATH_ID_TOP_BOTTOM);
    }
    return true;
}

/*######
## npc_hourglass_of_eternity
######*/
/*Support for 'Future you' is currently missing*/ 
enum
{
    NPC_INFINITE_CHRONO_MAGUS    = 27898,
    NPC_INFINITE_ASSAILANT       = 27896,
    NPC_INFINITE_DESTROYER       = 27897,
    NPC_INFINITE_TIMERENDER      = 27900,
    QUEST_MYSTERY_OF_INFINITE    = 12470

};

struct MANGOS_DLL_DECL npc_hourglassAI : public ScriptedAI
{
    npc_hourglassAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }
    
    uint64 uiWaveTimer;
    uint32 uiWaveCounter;

     void Reset()
     {
         uiWaveTimer = 5000;
         uiWaveCounter = 0;
     }

     void JustSummoned(Creature* pSummoned)
     {
         pSummoned->AI()->AttackStart(m_creature);
     }  

     void JustDied(Unit* pKiller)
     {
        if(Player *pPlayer = m_creature->GetMap()->GetPlayer(m_creature->GetOwnerGuid()))
        {
            pPlayer->FailQuest(QUEST_MYSTERY_OF_INFINITE);
        }
     }

     void SummonWave()
     {
         switch(uiWaveCounter)
         {
            case 0: m_creature->SummonCreature(NPC_INFINITE_CHRONO_MAGUS, m_creature->GetPositionX()+5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_ASSAILANT, m_creature->GetPositionX()-5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    break;
            case 1: m_creature->SummonCreature(NPC_INFINITE_CHRONO_MAGUS, m_creature->GetPositionX()+5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_CHRONO_MAGUS, m_creature->GetPositionX()-5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    break;
            case 2: m_creature->SummonCreature(NPC_INFINITE_CHRONO_MAGUS, m_creature->GetPositionX()+5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_ASSAILANT, m_creature->GetPositionX()-5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_DESTROYER, m_creature->GetPositionX()+5,m_creature->GetPositionY()+5 ,m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    break;
            case 3: m_creature->SummonCreature(NPC_INFINITE_CHRONO_MAGUS, m_creature->GetPositionX()+5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_ASSAILANT, m_creature->GetPositionX()-5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    m_creature->SummonCreature(NPC_INFINITE_DESTROYER, m_creature->GetPositionX()+5,m_creature->GetPositionY()+5 ,m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    break;
            case 4: m_creature->SummonCreature(NPC_INFINITE_TIMERENDER, m_creature->GetPositionX()+5,m_creature->GetPositionY(),m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                    break;

         }
     }
     void UpdateAI(const uint32 uiDiff)
     {
            if (uiWaveTimer <= uiDiff)
            {
                if(uiWaveCounter<=4)
                {
                    SummonWave();
                    uiWaveTimer = 15000;
                    uiWaveCounter++;
                }
                else m_creature->ForcedDespawn();

            } else uiWaveTimer -= uiDiff;
     }
};

CreatureAI* GetAI_npc_hourglass(Creature* pCreature)
{
    return new npc_hourglassAI(pCreature);
}

void AddSC_dragonblight()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_afrasastrasz";
    newscript->pGossipHello = &GossipHello_npc_afrasastrasz;
    newscript->pGossipSelect = &GossipSelect_npc_afrasastrasz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_alexstrasza_wr_gate";
    newscript->pGossipHello = &GossipHello_npc_alexstrasza_wr_gate;
    newscript->pGossipSelect = &GossipSelect_npc_alexstrasza_wr_gate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tariolstrasz";
    newscript->pGossipHello = &GossipHello_npc_tariolstrasz;
    newscript->pGossipSelect = &GossipSelect_npc_tariolstrasz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_torastrasza";
    newscript->pGossipHello = &GossipHello_npc_torastrasza;
    newscript->pGossipSelect = &GossipSelect_npc_torastrasza;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hourglass";
    newscript->GetAI = &GetAI_npc_hourglass;
    newscript->RegisterSelf();
}
