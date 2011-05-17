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
SDName: Boss General Bjarngrim
SD%Complete: 70%
SDComment: Waypoint needed, we expect boss to always have 2x Stormforged Lieutenant following
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "halls_of_lightning.h"

enum
{
    //Yell
    SAY_AGGRO                               = -1602000,
    SAY_SLAY_1                              = -1602001,
    SAY_SLAY_2                              = -1602002,
    SAY_SLAY_3                              = -1602003,
    SAY_DEATH                               = -1602004,
    SAY_BATTLE_STANCE                       = -1602005,
    EMOTE_BATTLE_STANCE                     = -1602006,
    SAY_BERSEKER_STANCE                     = -1602007,
    EMOTE_BERSEKER_STANCE                   = -1602008,
    SAY_DEFENSIVE_STANCE                    = -1602009,
    EMOTE_DEFENSIVE_STANCE                  = -1602010,

    SPELL_DEFENSIVE_STANCE                  = 53790,
    SPELL_SPELL_REFLECTION                  = 36096,
    SPELL_PUMMEL                            = 12555,
    SPELL_KNOCK_AWAY                        = 52029,
    SPELL_IRONFORM                          = 52022,

    SPELL_BERSEKER_STANCE                   = 53791,
    SPELL_INTERCEPT                         = 58769,
    SPELL_WHIRLWIND                         = 52027,
    SPELL_CLEAVE                            = 15284,

    SPELL_BATTLE_STANCE                     = 53792,
    SPELL_MORTAL_STRIKE                     = 16856,
    SPELL_SLAM                              = 52026,

    //OTHER SPELLS
    SPELL_CHARGE_UP                         = 52098,      // only used when starting walk from one platform to the other
    SPELL_TEMPORARY_ELECTRICAL_CHARGE       = 52092,      // triggered part of above

    SPELL_ARC_WELD                          = 59085,
    SPELL_RENEW_STEEL_N                     = 52774,
    SPELL_RENEW_STEEL_H                     = 59160,

    STANCE_DEFENSIVE                        = 0,
    STANCE_BERSERKER                        = 1,
    STANCE_BATTLE                           = 2
};

/*######
## boss_bjarngrim
######*/


struct Waypoint
{
    float fX, fY, fZ;
};

static const Waypoint Waypoints[]=
{
    //fX        fY        fZ
    {1262.256f, 99.495f,  33.5055f},
    {1262.208f, -26.910f, 33.5056f},
    {1332.181f, -26.836f, 40.1806f},

    {1395.152f, 35.566f,  50.038f},

    {1332.181f, -26.836f, 40.1806f},
    {1262.208f, -26.910f, 33.5056f},
    {1262.256f, 99.495f,  33.5055f}
};


struct MANGOS_DLL_DECL boss_bjarngrimAI : public ScriptedAI
{
    boss_bjarngrimAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiStance = STANCE_DEFENSIVE;
        memset(&m_uiStormforgedLieutenantGUID, 0, sizeof(m_uiStormforgedLieutenantGUID));
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsChangingStance;
    bool m_bWaypointReached;

    uint8 m_uiChargingStatus;
    uint8 m_uiStance;
    uint8 m_uiCurrentWaypoint;

    uint32 m_uiCharge_Timer;
    uint32 m_uiChangeStance_Timer;

    uint32 m_uiReflection_Timer;
    uint32 m_uiKnockAway_Timer;
    uint32 m_uiPummel_Timer;
    uint32 m_uiIronform_Timer;

    uint32 m_uiIntercept_Timer;
    uint32 m_uiWhirlwind_Timer;
    uint32 m_uiCleave_Timer;

    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiSlam_Timer;

    uint32 m_uiWaypointWait_Timer;

    uint64 m_uiStormforgedLieutenantGUID[2];

    void Reset()
    {
        m_bIsChangingStance = false;
        m_bWaypointReached = false;

        m_uiChargingStatus = 0;
        m_uiCharge_Timer = 1000;

        m_uiCurrentWaypoint = 0;

        m_uiChangeStance_Timer = urand(20000, 25000);

        m_uiReflection_Timer = 8000;
        m_uiKnockAway_Timer = 20000;
        m_uiPummel_Timer = 10000;
        m_uiIronform_Timer = 25000;

        m_uiIntercept_Timer = 5000;
        m_uiWhirlwind_Timer = 10000;
        m_uiCleave_Timer = 8000;

        m_uiMortalStrike_Timer = 8000;
        m_uiSlam_Timer = 10000;

        m_uiStormforgedLieutenantGUID[0] = m_pInstance->GetData64(DATA_STORMFORGED_LIEUTENANT_0);
        m_uiStormforgedLieutenantGUID[1] = m_pInstance->GetData64(DATA_STORMFORGED_LIEUTENANT_1);

        for(uint8 i = 0; i < 2; ++i)
        {
            if (Creature* pStormforgedLieutenant = m_creature->GetMap()->GetCreature(m_uiStormforgedLieutenantGUID[i]))
            {
                if (!pStormforgedLieutenant->isAlive())
                    pStormforgedLieutenant->Respawn();
                pStormforgedLieutenant->GetMotionMaster()->MoveFollow(m_creature, 1.0f, M_PI_F/2*((i==0)?1:3));
            }
        }

        if (m_uiStance != STANCE_DEFENSIVE)
        {
            DoCastSpellIfCan(m_creature, SPELL_DEFENSIVE_STANCE);
            m_uiStance = STANCE_DEFENSIVE;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BJARNGRIM, NOT_STARTED);
        
        m_creature->GetMotionMaster()->MovePoint(m_uiCurrentWaypoint,Waypoints[m_uiCurrentWaypoint].fX,Waypoints[m_uiCurrentWaypoint].fY,Waypoints[m_uiCurrentWaypoint].fZ);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if (uiMoveType != POINT_MOTION_TYPE)
            return;
        

        m_bWaypointReached = true;
        switch(uiPointId)
        {
            case 1:
                m_uiWaypointWait_Timer = 15000;
                DoCastSpellIfCan(m_creature,SPELL_CHARGE_UP);
                break;
            case 2:
                m_uiWaypointWait_Timer = 15000;
                break;
            case 3:
                m_uiWaypointWait_Timer = 15000;
                m_creature->RemoveAurasDueToSpell(SPELL_CHARGE_UP);
                m_creature->RemoveAurasDueToSpell(52092);
                break;
            case 4:
                m_uiWaypointWait_Timer = 15000;
                DoCastSpellIfCan(m_creature,SPELL_CHARGE_UP);
                break;
            case 5:
                m_uiWaypointWait_Timer = 15000;
                break;
            case 0:
            case 6:
                m_uiWaypointWait_Timer = 15000;
                m_uiCurrentWaypoint = 0;
                m_creature->RemoveAurasDueToSpell(SPELL_CHARGE_UP);
                m_creature->RemoveAurasDueToSpell(52092);
                break;
        }

        m_uiCurrentWaypoint++;

        
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        //must get both lieutenants here and make sure they are with him
        m_creature->CallForHelp(30.0f);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BJARNGRIM, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BJARNGRIM, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bWaypointReached)
            if(m_uiWaypointWait_Timer < uiDiff)
            {
                m_bWaypointReached = false;
                m_creature->GetMotionMaster()->MovePoint(m_uiCurrentWaypoint,Waypoints[m_uiCurrentWaypoint].fX,Waypoints[m_uiCurrentWaypoint].fY,Waypoints[m_uiCurrentWaypoint].fZ);
            }
            else
                m_uiWaypointWait_Timer -= uiDiff;

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Change stance
        if (m_uiChangeStance_Timer < uiDiff)
        {
            //wait for current spell to finish before change stance
            if (m_creature->IsNonMeleeSpellCasted(false))
                return;

            int uiTempStance = rand()%(3-1);

            if (uiTempStance >= m_uiStance)
                ++uiTempStance;

            m_uiStance = uiTempStance;

            switch(m_uiStance)
            {
                case STANCE_DEFENSIVE:
                    DoScriptText(SAY_DEFENSIVE_STANCE, m_creature);
                    DoScriptText(EMOTE_DEFENSIVE_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_DEFENSIVE_STANCE);
                    break;
                case STANCE_BERSERKER:
                    DoScriptText(SAY_BERSEKER_STANCE, m_creature);
                    DoScriptText(EMOTE_BERSEKER_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_BERSEKER_STANCE);
                    break;
                case STANCE_BATTLE:
                    DoScriptText(SAY_BATTLE_STANCE, m_creature);
                    DoScriptText(EMOTE_BATTLE_STANCE, m_creature);
                    DoCastSpellIfCan(m_creature, SPELL_BATTLE_STANCE);
                    break;
            }

            m_uiChangeStance_Timer = urand(20000, 25000);
            return;
        }
        else
            m_uiChangeStance_Timer -= uiDiff;

        switch(m_uiStance)
        {
            case STANCE_DEFENSIVE:
            {
                if (m_uiReflection_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_SPELL_REFLECTION);
                    m_uiReflection_Timer = urand(8000, 9000);
                }
                else
                    m_uiReflection_Timer -= uiDiff;

                if (m_uiKnockAway_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_KNOCK_AWAY);
                    m_uiKnockAway_Timer = urand(20000, 21000);
                }
                else
                    m_uiKnockAway_Timer -= uiDiff;

                if (m_uiPummel_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_PUMMEL);
                    m_uiPummel_Timer = urand(10000, 11000);
                }
                else
                    m_uiPummel_Timer -= uiDiff;

                if (m_uiIronform_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_IRONFORM);
                    m_uiIronform_Timer = urand(25000, 26000);
                }
                else
                    m_uiIronform_Timer -= uiDiff;

                break;
            }
            case STANCE_BERSERKER:
            {
                if (m_uiIntercept_Timer < uiDiff)
                {
                    //not much point is this, better random target and more often?
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_INTERCEPT);
                    m_uiIntercept_Timer = urand(45000, 46000);
                }
                else
                    m_uiIntercept_Timer -= uiDiff;

                if (m_uiWhirlwind_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND);
                    m_uiWhirlwind_Timer = urand(10000, 11000);
                }
                else
                    m_uiWhirlwind_Timer -= uiDiff;

                if (m_uiCleave_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
                    m_uiCleave_Timer = urand(8000, 9000);
                }
                else
                    m_uiCleave_Timer -= uiDiff;

                break;
            }
            case STANCE_BATTLE:
            {
                if (m_uiMortalStrike_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                    m_uiMortalStrike_Timer = urand(20000, 21000);
                }
                else
                    m_uiMortalStrike_Timer -= uiDiff;

                if (m_uiSlam_Timer < uiDiff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLAM);
                    m_uiSlam_Timer = urand(15000, 16000);
                }
                else
                    m_uiSlam_Timer -= uiDiff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## mob_stormforged_lieutenant
######*/

struct MANGOS_DLL_DECL mob_stormforged_lieutenantAI : public ScriptedAI
{
    mob_stormforged_lieutenantAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcWeld_Timer;
    uint32 m_uiRenewSteel_Timer;

    void Reset()
    {
        m_uiArcWeld_Timer = urand(20000, 21000);
        m_uiRenewSteel_Timer = urand(10000, 11000);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (Creature* pBjarngrim = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_BJARNGRIM)))
            {
                if (pBjarngrim->isAlive() && !pBjarngrim->getVictim())
                    pBjarngrim->AI()->AttackStart(pWho);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcWeld_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARC_WELD);
            m_uiArcWeld_Timer = urand(20000, 21000);
        }
        else
            m_uiArcWeld_Timer -= uiDiff;

        if (m_uiRenewSteel_Timer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pBjarngrim = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_BJARNGRIM)))
                {
                    if (pBjarngrim->isAlive())
                        DoCastSpellIfCan(pBjarngrim, m_bIsRegularMode ? SPELL_RENEW_STEEL_N : SPELL_RENEW_STEEL_H);
                }
            }
            m_uiRenewSteel_Timer = urand(10000, 14000);
        }
        else
            m_uiRenewSteel_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bjarngrim(Creature* pCreature)
{
    return new boss_bjarngrimAI(pCreature);
}

CreatureAI* GetAI_mob_stormforged_lieutenant(Creature* pCreature)
{
    return new mob_stormforged_lieutenantAI(pCreature);
}

void AddSC_boss_bjarngrim()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_bjarngrim";
    newscript->GetAI = &GetAI_boss_bjarngrim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_stormforged_lieutenant";
    newscript->GetAI = &GetAI_mob_stormforged_lieutenant;
    newscript->RegisterSelf();
}
