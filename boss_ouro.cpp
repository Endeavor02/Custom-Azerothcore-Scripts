/*
 * Copyright (C) 2016+     AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-GPL2
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 */

/* ScriptData
SDName: Boss_Ouro
SD%Complete: 85
SDComment: No model for submerging. Currently just invisible.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "temple_of_ahnqiraj.h"

enum Spells
{
    SPELL_SWEEP                 = 26103,
    SPELL_SANDBLAST             = 26102,
    SPELL_GROUND_RUPTURE        = 26100,
    SPELL_BIRTH                 = 26262, // The Birth Animation
    SPELL_DIRTMOUND_PASSIVE     = 26093
};

class boss_ouro : public CreatureScript
{
public:
    boss_ouro() : CreatureScript("boss_ouro") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ouroAI(creature);
    }

    struct boss_ouroAI : public ScriptedAI
    {
        boss_ouroAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 Sweep_Timer;
        uint32 SandBlast_Timer;
        uint32 Submerge_Timer;
        uint32 Back_Timer;
        uint32 ChangeTarget_Timer;
        uint32 Spawn_Timer;
        uint32 changeArea_Timer;
        Creature* myTrigger;
        uint32 p2_timer;
        bool p2 = false;
        Creature* Quakers[10];

        bool Enrage;
        bool Submerged;
        bool wasInCB = false;

        Creature* getTrigger()
        {
            std::list<Creature*> assistList;
            me->GetCreatureListWithEntryInGrid(assistList, 15717, 500.0f);
            Creature* Trigger;
            for (std::list<Creature*>::const_iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
            {
                for (Creature* const& i : assistList)
                {
                    Trigger = i;
                }
            }
//            std::cout << "Got trigger with GUID of " << Trigger->GetGUID(); was used for debugging
            return Trigger;
        }

        void getQuakeArray()
        {
            int j = 0;
            std::list<Creature*> assistList;
            me->GetCreatureListWithEntryInGrid(assistList, 9015717, 500.0f);
            Creature* Trigger;
            for (std::list<Creature*>::const_iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
            {
                for (Creature* const& i : assistList)
                {
                    if (j>=10)
                    {
                        j = 0;
                    }
                    Quakers[j] = i;
                    std::cout << "Got entry " << Quakers[j]->GetGUID() << "\n";
                    ++j;
                }
            }
        }

        float getRandX()
        {
            return (-9273.0) + (urand(0.0,273.0));
        }
        float getRandY()
        {
            return 2000.0 + urand(0.0,162.0);
        }
        float getRandZ()
        {
            return -62.0;
        }

        void Reset()
        {
            Sweep_Timer = urand(5000, 10000);
            SandBlast_Timer = urand(20000, 35000);
            Submerge_Timer = urand(90000, 150000);
            Back_Timer = urand(30000, 45000);
            p2_timer = 99999999999;
            ChangeTarget_Timer = urand(5000, 8000);
            Spawn_Timer = urand(10000, 20000);
            changeArea_Timer = urand(4000, 6000);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetObjectScale(1);
            Enrage = false;
            Submerged = false;
            p2 = false;
            if (wasInCB) {
                me->MonsterMoveWithSpeed(-9173.0, 2107.11, -65.0, 10.0);
                myTrigger->MonsterMoveWithSpeed(-9173.0, 2107.11, -65.0, 10.0);
                for (int i = 0; i < 10; ++i)
                {
                    Quakers[i]->MonsterMoveWithSpeed(-9181, 2140, -87, 200);
                }
            }
            wasInCB = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            getQuakeArray();
            myTrigger = getTrigger();
            wasInCB = true;
            DoCastVictim(SPELL_BIRTH);
            me->MonsterMoveWithSpeed(-9173.0,2107.11,-65.0,100.0);
        }

        void UpdateAI(uint32 diff)
        {


            if (me->GetHealthPct() <= 20 && !p2)
            {
                Submerge_Timer = 86400000; //It should be impossible for him to burrow in P2
                p2 = true;
                p2_timer = 2000;
                DoCast(me, 26615);
            }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            //CUSTOM TIMER SECTION

            if (Submerged && changeArea_Timer <= diff)
            {
                float x, y, z;
                x = -9170;
                y = 2125;
                z = -65;
                //Initial values in case it cannot select a target!
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0);
//                Unit* temp = me->GetNextRandomRaidMemberOrPet(75);
                x = target->GetPositionX();
                y = target->GetPositionY();
                z = target->GetPositionZ();
                std::cout << "Moving to " << x <<" x, " << y << " y, " << z << " z. \n";
                me->MonsterMoveWithSpeed(x,y,z, 10.0);
                myTrigger->MonsterMoveWithSpeed(x,y,z, 10.0);
                DoCast(me, SPELL_DIRTMOUND_PASSIVE);
                changeArea_Timer = urand(4000, 6000);
                for (int i = 0; i < 10; ++i)
                {
                    float x = getRandX();
                    float y = getRandY();
                    float z = getRandZ();
                    cout << "Sending Quaker to " << x << " x, " <<y<< " y, " << z << " z,\n";
                    Quakers[i]->MonsterMoveWithSpeed(x, y, z, 200);
                }
            }
            else changeArea_Timer -= diff;


            //END CUSTOM TIMER SECTION
            //Sweep_Timer
            if (!Submerged && Sweep_Timer <= diff)
            {
                DoCastVictim(SPELL_SWEEP);
                Sweep_Timer = urand(15000, 30000);
            } else Sweep_Timer -= diff;

            //SandBlast_Timer
            if (!Submerged && SandBlast_Timer <= diff)
            {
                DoCastVictim(SPELL_SANDBLAST);
                SandBlast_Timer = urand(20000, 35000);
            } else SandBlast_Timer -= diff;

            //Submerge_Timer
            if (!Submerged && Submerge_Timer <= diff)
            {
                //Cast
                std::cout << "Ouru is Submerging!\n";
                me->HandleEmoteCommand(EMOTE_ONESHOT_SUBMERGE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
//                me->setFaction(35); I think this is causing him to bug out and get stuck in this phase
                DoCast(me, SPELL_DIRTMOUND_PASSIVE);
                me->SetObjectScale(-1);
//                me->MonsterMoveWithSpeed(-9173.0, 2107.11, -85.0, 100.0);

                Submerged = true;
                Back_Timer = urand(30000, 45000);
                changeArea_Timer = urand(5000, 6000);
            } else Submerge_Timer -= diff;

            //ChangeTarget_Timer
            if (Submerged && ChangeTarget_Timer <= diff)
            {
                Unit* target = nullptr;
                target = SelectTarget(SELECT_TARGET_RANDOM, 0);

                if (target)
                    me->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), me->GetOrientation());
                ChangeTarget_Timer = urand(10000, 20000);
            } else ChangeTarget_Timer -= diff;

            //Back_Timer
            if (Submerged && Back_Timer <= diff)
            {
                std::cout << "Ouru should be back now \n";
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
//                me->setFaction(14);
//                me->MonsterMoveWithSpeed(-9173.0, 2107.11, -65.0, 500.0);
//                myTrigger->MonsterMoveWithSpeed(-9173.0, 2107.11, -65.0, 500.0);
                me->SetObjectScale(1);

                if (me->SelectNearestTarget(20, true)) {
                    DoCast(me->SelectNearestTarget(20, true),SPELL_GROUND_RUPTURE);
                }
                
                DoCastVictim(26060);

                Submerged = false;
                Submerge_Timer = urand(60000, 120000);
                for (int i = 0; i < 10; ++i)
                {
                    Quakers[i]->MonsterMoveWithSpeed(-9181, 2140, -87, 200);
                }
            } else Back_Timer -= diff;

            if (p2 && p2_timer <= diff)
            {
                DoCastVictim(26060);
                for (int i = 0; i < 10; ++i)
                {
                    float x = getRandX();
                    float y = getRandY();
                    float z = getRandZ();
                    Quakers[i]->MonsterMoveWithSpeed(x, y, z, 200);
                }
                p2_timer = 20000;
            }
            else p2_timer -= diff;
            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_ouro()
{
    new boss_ouro();
}
