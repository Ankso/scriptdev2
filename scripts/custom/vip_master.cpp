/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/*
by Nezz
*/

#include "precompiled.h"

#define HOW_TO_BECOME_VIP    "Wie kann ich VIP werden?"
#define QUESTLIST            "Ich möchte meine VIP Quest beginnen."
#define VENDORLIST           "Zeigt mir eure Waren."
#define NEWS                 "Erzähl mir die News."
#define STARTED_VIP_QUEST    "Danke, dass du gespendet hast."
#define TRAINLIST            "Ich möchte mich weiterbilden"

struct MANGOS_DLL_DECL vip_masterAI : public ScriptedAI
{
    vip_masterAI(Creature *pCreature) : ScriptedAI(pCreature) {Reset();}

    bool InCombat;

    void Reset()
    {
        InCombat = false;
    }

    void AttackStart(Unit *pWho)
    {
        return; 
    }

    void Aggro(Unit *pWho)
    {
        return;
    }

};


CreatureAI* GetAI_vip_master(Creature *pCreature)
{
    return new vip_masterAI (pCreature);
}

bool GossipHello_vip_master(Player *pPlayer, Creature *pCreature)
{
    ObjectGuid pguid = pPlayer->GetGUID();
    ObjectGuid cguid = pCreature->GetGUID();

    /*if (pCreature->isQuestGiver()) {
        pPlayer->PrepareQuestMenu( cguid );
        pPlayer->SendPreparedQuest( cguid );*/

    if(pPlayer->isVIP( pguid ) == true)
        {//sLog.outError("pPlayer VIP Status: %d", pPlayer->isVIP(pguid));
        pPlayer->ADD_GOSSIP_ITEM( 2, QUESTLIST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->ADD_GOSSIP_ITEM( 1, VENDORLIST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->ADD_GOSSIP_ITEM( 3, TRAINLIST, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);}
    else
        {//sLog.outError("pPlayer VIP Status: %d", pPlayer->isVIP(pguid));
        pPlayer->ADD_GOSSIP_ITEM( 2, HOW_TO_BECOME_VIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);}

    pPlayer->ADD_GOSSIP_ITEM( 0, NEWS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, cguid);

    return true;
}

bool GossipSelect_vip_master(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 action)
{
    ObjectGuid pguid = pPlayer->GetGUID();
    ObjectGuid cguid = pCreature->GetGUID();

    if (action == GOSSIP_ACTION_INFO_DEF + 1) // Gib VIPQuest
    {
        /*QEmote qe;
        qe._Delay = 0;
        qe._Emote = 0;
        std::string title = "";
        pPlayer->pPlayerTalkClass->SendQuestGiverQuestList( qe, title, cguid ); */
        if (pCreature->isQuestGiver()) {
            pPlayer->PrepareQuestMenu( cguid );
            pPlayer->SendPreparedQuest( cguid ); }
    }
    else
        pPlayer->SEND_GOSSIP_MENU(110002,cguid); // VIPQuest fertig
    
    if (action == GOSSIP_ACTION_INFO_DEF + 2) // Wie werde ich VIP?
        pPlayer->SEND_GOSSIP_MENU(110000,cguid);

    if(action == GOSSIP_ACTION_TRADE)
    {
        pPlayer->SEND_VENDORLIST( pCreature->GetGUID() );
    }

    if(action == GOSSIP_ACTION_TRAIN)
    {
        pPlayer->SEND_TRAINERLIST( pCreature->GetGUID() );
    }

    if (action == GOSSIP_ACTION_INFO_DEF + 4) // News
        pPlayer->SEND_GOSSIP_MENU(110001,cguid);

    return true;
}

uint32 NPCDialogStatus_vip_master(Player *pPlayer, Creature *pCreature )
{
    return pPlayer->QUEST_DIALOG_STATUS(pPlayer, pCreature, DIALOG_STATUS_LOW_LEVEL_AVAILABLE);
}

bool QuestAccept_vip_master(Player *pPlayer, Creature *pCreature, Quest const *quest)
{
    if(quest->GetQuestId() == 99000)
    {
        //((vip_masterAI*)creature->AI())->DoSay(STARTED_VIP_QUEST, LANG_UNIVERSAL, pPlayer);
        return true;
    }
    return false;
}

void AddSC_vip_master()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name="vip_master";
    newscript->pGossipHello          = &GossipHello_vip_master;
    newscript->pGossipSelect         = &GossipSelect_vip_master;
    newscript->pDialogStatusNPC      = &NPCDialogStatus_vip_master;
    newscript->pQuestAcceptNPC       = &QuestAccept_vip_master;
    newscript->GetAI = &GetAI_vip_master;
    newscript->RegisterSelf();
}
