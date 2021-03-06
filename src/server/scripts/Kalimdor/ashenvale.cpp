/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Ashenvale
SD%Complete: 70
SDComment: Quest support: 6544, 6482
SDCategory: Ashenvale Forest
EndScriptData */

/* ContentData
npc_torek
npc_ruul_snowhoof
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*####
# npc_torek
####*/

#define SAY_READY                   -1000106
#define SAY_MOVE                    -1000107
#define SAY_PREPARE                 -1000108
#define SAY_WIN                     -1000109
#define SAY_END                     -1000110

#define SPELL_REND                  11977
#define SPELL_THUNDERCLAP           8078

#define QUEST_TOREK_ASSULT          6544

#define ENTRY_SPLINTERTREE_RAIDER   12859
#define ENTRY_DURIEL                12860
#define ENTRY_SILVERWING_SENTINEL   12896
#define ENTRY_SILVERWING_WARRIOR    12897

class npc_torek : public CreatureScript
{
    public:

        npc_torek()
            : CreatureScript("npc_torek")
        {
        }

        struct npc_torekAI : public npc_escortAI
        {
            npc_torekAI(Creature* c) : npc_escortAI(c) {}

            uint32 Rend_Timer;
            uint32 Thunderclap_Timer;
            bool Completed;

            void WaypointReached(uint32 i)
            {
                Player* player = GetPlayerForEscort();

                if (!player)
                    return;

                switch (i)
                {
                case 1:
                    DoScriptText(SAY_MOVE, me, player);
                    break;
                case 8:
                    DoScriptText(SAY_PREPARE, me, player);
                    break;
                case 19:
                    //TODO: verify location and creatures amount.
                    me->SummonCreature(ENTRY_DURIEL, 1776.73f, -2049.06f, 109.83f, 1.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(ENTRY_SILVERWING_SENTINEL, 1774.64f, -2049.41f, 109.83f, 1.40f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(ENTRY_SILVERWING_WARRIOR, 1778.73f, -2049.50f, 109.83f, 1.67f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                case 20:
                    DoScriptText(SAY_WIN, me, player);
                    Completed = true;
                    if (player)
                        player->GroupEventHappens(QUEST_TOREK_ASSULT, me);
                    break;
                case 21:
                    DoScriptText(SAY_END, me, player);
                    break;
                }
            }

            void Reset()
            {
                Rend_Timer = 5000;
                Thunderclap_Timer = 8000;
                Completed = false;
            }

            void EnterCombat(Unit* /*who*/)
            {
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void UpdateAI(const uint32 diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;

                if (Rend_Timer <= diff)
                {
                    DoCast(me->getVictim(), SPELL_REND);
                    Rend_Timer = 20000;
                } else Rend_Timer -= diff;

                if (Thunderclap_Timer <= diff)
                {
                    DoCast(me, SPELL_THUNDERCLAP);
                    Thunderclap_Timer = 30000;
                } else Thunderclap_Timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_torekAI(creature);
        }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_TOREK_ASSULT)
            {
                //TODO: find companions, make them follow Torek, at any time (possibly done by core/database in future?)
                DoScriptText(SAY_READY, creature, player);
                creature->setFaction(113);

                if (npc_escortAI* pEscortAI = CAST_AI(npc_torekAI, creature->AI()))
                    pEscortAI->Start(true, true, player->GetGUID());
            }

            return true;
        }
};

/*####
# npc_ruul_snowhoof
####*/

#define QUEST_FREEDOM_TO_RUUL    6482
#define GO_CAGE                  178147

class npc_ruul_snowhoof : public CreatureScript
{
    public:

        npc_ruul_snowhoof()
            : CreatureScript("npc_ruul_snowhoof")
        {
        }

        struct npc_ruul_snowhoofAI : public npc_escortAI
        {
            npc_ruul_snowhoofAI(Creature* c) : npc_escortAI(c) {}

            void WaypointReached(uint32 i)
            {
                Player* player = GetPlayerForEscort();

                if (!player)
                    return;

                switch (i)
                {
                case 0:    {
                        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                        GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20);
                        if (Cage)
                            Cage->SetGoState(GO_STATE_ACTIVE);
                        break;}
                case 13:
                        me->SummonCreature(3922, 3449.218018f, -587.825073f, 174.978867f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        me->SummonCreature(3921, 3446.384521f, -587.830872f, 175.186279f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        me->SummonCreature(3926, 3444.218994f, -587.835327f, 175.380600f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        break;
                case 19:
                        me->SummonCreature(3922, 3508.344482f, -492.024261f, 186.929031f, 4.145029f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        me->SummonCreature(3921, 3506.265625f, -490.531006f, 186.740128f, 4.239277f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        me->SummonCreature(3926, 3503.682373f, -489.393799f, 186.629684f, 4.349232f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                        break;

                case 21:{
                        if (player)
                            player->GroupEventHappens(QUEST_FREEDOM_TO_RUUL, me);

                        break;  }
                }
            }

            void EnterCombat(Unit* /*who*/) {}

            void Reset()
            {
                GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20);
                if (Cage)
                    Cage->SetGoState(GO_STATE_READY);
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void UpdateAI(const uint32 diff)
            {
                npc_escortAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ruul_snowhoofAI(creature);
        }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_FREEDOM_TO_RUUL)
            {
                creature->setFaction(113);

                if (npc_escortAI* pEscortAI = CAST_AI(npc_ruul_snowhoofAI, (creature->AI())))
                    pEscortAI->Start(true, false, player->GetGUID());
            }

            return true;
        }
};

enum eEnums
{
    SAY_MUG_START1          = -1800054,
    SAY_MUG_START2          = -1800055,
    SAY_MUG_BRAZIER         = -1800056,
    SAY_MUG_BRAZIER_WAIT    = -1800057,
    SAY_MUG_ON_GUARD        = -1800058,
    SAY_MUG_REST            = -1800059,
    SAY_MUG_DONE            = -1800060,
    SAY_MUG_GRATITUDE       = -1800061,
    SAY_MUG_PATROL          = -1800062,
    SAY_MUG_RETURN          = -1800063,

    QUEST_VORSHA            = 6641,

    GO_NAGA_BRAZIER         = 178247,

    NPC_WRATH_RIDER         = 3713,
    NPC_WRATH_SORCERESS     = 3717,
    NPC_WRATH_RAZORTAIL     = 3712,

    NPC_WRATH_PRIESTESS     = 3944,
    NPC_WRATH_MYRMIDON      = 3711,
    NPC_WRATH_SEAWITCH      = 3715,

    NPC_VORSHA              = 12940,
    NPC_MUGLASH             = 12717
};

static float m_afFirstNagaCoord[3][3]=
{
    {3603.504150f, 1122.631104f, 1.635f},                      // rider
    {3589.293945f, 1148.664063f, 5.565f},                      // sorceress
    {3609.925537f, 1168.759521f, -1.168f}                      // razortail
};

static float m_afSecondNagaCoord[3][3]=
{
    {3609.925537f, 1168.759521f, -1.168f},                     // witch
    {3645.652100f, 1139.425415f, 1.322f},                      // priest
    {3583.602051f, 1128.405762f, 2.347f}                       // myrmidon
};

static float m_fVorshaCoord[]={3633.056885f, 1172.924072f, -5.388f};

class npc_muglash : public CreatureScript
{
    public:
        npc_muglash() : CreatureScript("npc_muglash") { }

        struct npc_muglashAI : public npc_escortAI
        {
            npc_muglashAI(Creature* creature) : npc_escortAI(creature) { }

            uint32 m_uiWaveId;
            uint32 m_uiEventTimer;
            bool m_bIsBrazierExtinguished;

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void WaypointReached(uint32 i)
            {
                Player* player = GetPlayerForEscort();

                switch (i)
                {
                    case 0:
                        if (player)
                            DoScriptText(SAY_MUG_START2, me, player);
                        break;
                    case 24:
                        if (player)
                            DoScriptText(SAY_MUG_BRAZIER, me, player);

                        if (GameObject* go = GetClosestGameObjectWithEntry(me, GO_NAGA_BRAZIER, INTERACTION_DISTANCE*2))
                        {
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                            SetEscortPaused(true);
                        }
                        break;
                    case 25:
                        DoScriptText(SAY_MUG_GRATITUDE, me);

                        if (player)
                            player->GroupEventHappens(QUEST_VORSHA, me);
                        break;
                    case 26:
                        DoScriptText(SAY_MUG_PATROL, me);
                        break;
                    case 27:
                        DoScriptText(SAY_MUG_RETURN, me);
                        break;
                }
            }

            void EnterCombat(Unit* /*who*/)
            {
                if (Player* player = GetPlayerForEscort())
                    if (HasEscortState(STATE_ESCORT_PAUSED))
                    {
                        if (urand(0, 1))
                            DoScriptText(SAY_MUG_ON_GUARD, me, player);
                        return;
                    }
            }

            void Reset()
            {
                m_uiEventTimer = 10000;
                m_uiWaveId = 0;
                m_bIsBrazierExtinguished = false;
            }

            void JustDied(Unit* /*killer*/)
            {
                Player* player = GetPlayerForEscort();
                if (HasEscortState(STATE_ESCORT_ESCORTING))
                {
                    if (player)
                    {
                        player->FailQuest(QUEST_VORSHA);
                    }
                }
            }

            void DoWaveSummon()
            {
                switch (m_uiWaveId)
                {
                    case 1:
                        me->SummonCreature(NPC_WRATH_RIDER,     m_afFirstNagaCoord[0][0], m_afFirstNagaCoord[0][1], m_afFirstNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_WRATH_SORCERESS, m_afFirstNagaCoord[1][0], m_afFirstNagaCoord[1][1], m_afFirstNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_WRATH_RAZORTAIL, m_afFirstNagaCoord[2][0], m_afFirstNagaCoord[2][1], m_afFirstNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        break;
                    case 2:
                        me->SummonCreature(NPC_WRATH_PRIESTESS, m_afSecondNagaCoord[0][0], m_afSecondNagaCoord[0][1], m_afSecondNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_WRATH_MYRMIDON,  m_afSecondNagaCoord[1][0], m_afSecondNagaCoord[1][1], m_afSecondNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_WRATH_SEAWITCH,  m_afSecondNagaCoord[2][0], m_afSecondNagaCoord[2][1], m_afSecondNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        break;
                    case 3:
                        me->SummonCreature(NPC_VORSHA, m_fVorshaCoord[0], m_fVorshaCoord[1], m_fVorshaCoord[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        break;
                    case 4:
                        SetEscortPaused(false);
                        DoScriptText(SAY_MUG_DONE, me);
                        break;
                }
            }

            void UpdateAI(const uint32 uiDiff)
            {
                npc_escortAI::UpdateAI(uiDiff);

                if (!me->getVictim())
                {
                    if (HasEscortState(STATE_ESCORT_PAUSED) && m_bIsBrazierExtinguished)
                    {
                        if (m_uiEventTimer < uiDiff)
                        {
                            ++m_uiWaveId;
                            DoWaveSummon();
                            m_uiEventTimer = 10000;
                        }
                        else
                            m_uiEventTimer -= uiDiff;
                    }
                    return;
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_muglashAI(creature);
        }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_VORSHA)
            {
                if (npc_muglashAI* pEscortAI = CAST_AI(npc_muglashAI, creature->AI()))
                {
                    DoScriptText(SAY_MUG_START1, creature);
                    creature->setFaction(113);

                    pEscortAI->Start(true, true, player->GetGUID());
                }
            }
            return true;
        }
};

class go_naga_brazier : public GameObjectScript
{
    public:

        go_naga_brazier()
            : GameObjectScript("go_naga_brazier")
        {
        }

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            if (Creature* creature = GetClosestCreatureWithEntry(go, NPC_MUGLASH, INTERACTION_DISTANCE*2))
            {
                if (npc_muglash::npc_muglashAI* pEscortAI = CAST_AI(npc_muglash::npc_muglashAI, creature->AI()))
                {
                    DoScriptText(SAY_MUG_BRAZIER_WAIT, creature);

                    pEscortAI->m_bIsBrazierExtinguished = true;
                    return false;
                }
            }

            return true;
        }
};

/*######
# npc_spirit_gorat
######*/

enum eGorat
{
    SAY_GORAT_1 = -1934658,
    SAY_GORAT_2 = -1934659,
    SAY_GORAT_3 = -1934660,
    SAY_GORAT_4 = -1934661,
    SAY_GORAT_5 = -1934662,
    SAY_GORAT_6 = -1934663,
    SAY_GORAT_7 = -1934664,
    SAY_ELENDILAD_1 = -1934665,
    SAY_ELENDILAD_2 = -1934666,

    QUEST_GORAT_VENGEANCE = 13621,
    SPELL_GORAT_BOW = 62792,
    SPELL_SUMMON_GORAT_SPIRIT = 62772,
    NPC_CAPTAIN_ELENDILAD = 33302
};

class npc_spirit_gorat : public CreatureScript
{

public:

    npc_spirit_gorat() : CreatureScript("npc_spirit_gorat") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_spirit_goratAI(pCreature);
    }

    struct npc_spirit_goratAI : public ScriptedAI
    {
        npc_spirit_goratAI(Creature* pCreature) : ScriptedAI(pCreature){}

        uint8 Phase;
        uint32 MoveTimer;
        uint64 PlayerGUID;

        void Reset()
        {
            DoScriptText(SAY_GORAT_1,me,0);
            MoveTimer = 4000;
            Phase = 0;
            PlayerGUID = 0;
        }

        void UpdateAI(uint32 const diff)
        {
                if(Player* pPlayer = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if(MoveTimer <= diff)
                    {
                        switch(Phase)
                        {
                            case 0: DoScriptText(SAY_GORAT_2,me,pPlayer); MoveTimer = 3000; Phase++; break;
                            case 1: DoScriptText(SAY_GORAT_3,me,pPlayer); MoveTimer = 3000; Phase++; break;
                            case 2: DoScriptText(SAY_GORAT_4,me,pPlayer); MoveTimer = 2000; Phase++; break;
                            case 3: me->GetMotionMaster()->MovePoint(0, 1433.32f, -2016.22f, 93.61f); MoveTimer = 4000; Phase++; break;
                            case 4: me->GetMotionMaster()->MovePoint(1, 1465.11f, -2049.95f, 93.25f); DoScriptText(SAY_GORAT_5,me,pPlayer); MoveTimer = 6000; Phase++; break;
                            case 5: me->GetMotionMaster()->MovePoint(2, 1476.53f, -2074.07f, 92.86f); DoScriptText(SAY_GORAT_6,me,pPlayer); MoveTimer = 5000; Phase++; break;
                            case 6: me->GetMotionMaster()->MovePoint(3, 1504.26f, -2087.99f, 90.63f); DoScriptText(SAY_GORAT_7,me,pPlayer); MoveTimer = 6000; Phase++; break;
                            case 7: me->SetFacing(4.90f,0); MoveTimer = 1000; Phase++; break;
                            case 8: me->CastSpell(me,SPELL_GORAT_BOW,false); MoveTimer = 5000; Phase++; break;
                            case 9: me->SummonCreature(NPC_CAPTAIN_ELENDILAD, 1514.61f, -2144.20f, 88.52f, 1.69f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000); MoveTimer = 60000; Phase++; break;
                            case 10: me->ForcedDespawn();
                            default: break;
                        }
                    }else MoveTimer -= diff;
                }  
                DoMeleeAttackIfReady();
        }

        void JustSummoned(Creature* pSummoned)
        {
            DoScriptText(SAY_ELENDILAD_1,pSummoned,me);
            me->Attack(pSummoned,true);
            me->CombatStart(pSummoned,true);
            pSummoned->CastSpell(pSummoned,78823,false);

            if(!pSummoned->isAlive())
            {
                DoScriptText(SAY_ELENDILAD_2,pSummoned,me);
            }
        }
    };
};

enum eBrutusk
{
    NPC_BRUTUSK         = 33386,
    NPC_AMBUSHER        = 33394,
    SPELL_RIDE_BRUTUSK  = 62885,
    SPELL_STOMP         = 62944,
    SPELL_KILL_CREDIT   = 62890,
    VEHICLE_ID          = 209,
};

class npc_brutusk : public CreatureScript
{
public:
    npc_brutusk() : CreatureScript("npc_brutusk"){}

    struct npc_brutuskAI : public ScriptedAI
    {
        npc_brutuskAI(Creature* creature) : ScriptedAI(creature) {}

        bool bSummoned;
        bool itemCheck;
        uint8 Phase;
        uint32 BrutuskTimer;
        uint64 PlayerGUID;
        uint64 AmbusherGUID;
        uint64 Ambusher1GUID;
        uint64 Ambusher2GUID;
        uint64 Ambusher3GUID;

        void Reset()
        {
            itemCheck = false;
            bSummoned = false;
            Phase = 0;
            BrutuskTimer = 1500;
            PlayerGUID = 0;
            AmbusherGUID = 0;
            Ambusher1GUID = 0;
            Ambusher2GUID = 0;
            Ambusher3GUID = 0;
        }

        void Ambush()
        {
            if(!bSummoned)
            {
                if(Creature* Ambusher = me->SummonCreature(NPC_AMBUSHER, 1383.12f, -2257.68f, 89.91f, 3.23f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                if(Creature* Ambusher1 = me->SummonCreature(NPC_AMBUSHER, 1392.16f, -2256.95f, 89.92f, 3.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                if(Creature* Ambusher2 = me->SummonCreature(NPC_AMBUSHER, 1387.01f, -2251.47f, 90.11f, 3.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))
                if(Creature* Ambusher3 = me->SummonCreature(NPC_AMBUSHER, 1387.86f, -2265.04f, 90.43f, 3.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000))   
                {
                    AmbusherGUID = Ambusher->GetGUID();
                    Ambusher1GUID = Ambusher1->GetGUID();
                    Ambusher2GUID = Ambusher2->GetGUID();
                    Ambusher3GUID = Ambusher3->GetGUID();
                    bSummoned = true;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK))
            {
                if(BrutuskTimer <= diff)
                {
                    switch(Phase)
                    {
                        case 0: me->GetMotionMaster()->MovePoint(0, 1262.94f, -2221.60f, 91.97f); BrutuskTimer = 9740; Phase++; break;
                        case 1: me->GetMotionMaster()->MovePoint(1, 1313.53f, -2240.48f, 91.73f); BrutuskTimer = 7000; Phase++; break;
                        case 2: me->GetMotionMaster()->MovePoint(2, 1329.94f, -2257.60f, 91.07f); BrutuskTimer = 2340; Phase++; break;
                        case 3: me->GetMotionMaster()->MovePoint(3, 1360.79f, -2257.60f, 89.96f); Ambush(); BrutuskTimer = 7000; Phase++; break;
                        case 4:
                        {
                            if(Creature* pAmbusher = me->FindNearestCreature(NPC_AMBUSHER,25.0f,true))
                            {
                                me->CastSpell(pAmbusher,82101,true);
                                BrutuskTimer = 2000;
                                Phase++;
                            }
                        }break;
                        case 5:
                        {
                            std::list<Creature*> ambushers;
                            me->GetCreatureListWithEntryInGrid(ambushers, 33394, 25.0f);
                            ambushers.sort(Trinity::ObjectDistanceOrderPred(me));
                            for(std::list<Creature*>::iterator itr = ambushers.begin(); itr != ambushers.end(); itr++)
                            {
                                if((*itr)->GetTypeId() == TYPEID_UNIT)
                                {
                                    (*itr)->CastSpell((*itr),SPELL_STOMP,true);
                                }
                            }
                            BrutuskTimer = 2000;
                            Phase++;
                        }break;
                        case 6: me->GetMotionMaster()->MovePoint(4, 1419.55f, -2259.52f, 90.16f); BrutuskTimer = 5800; Phase++; break;
                        case 7: me->GetMotionMaster()->MovePoint(5, 1496.33f, -2373.28f, 93.76f); BrutuskTimer = 14900; Phase++; break;
                        case 8: me->GetMotionMaster()->MovePoint(6, 1579.60f, -2449.60f, 97.99f); BrutuskTimer = 15820; Phase++; break;
                        case 9: me->GetMotionMaster()->MovePoint(7, 1576.38f, -2485.07f, 98.00f); BrutuskTimer = 5030; Phase++; break;
                        case 10: me->ForcedDespawn(); break;
                        default: break;
                    }
                }else BrutuskTimer -= diff;
            } 
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_brutuskAI(pCreature);
    }
};

/*######
## npc_kadrakk
######*/

enum eKadrakk
{
    SAY_KADRAKK = -1934667 
};
#define GOSSIP_KADRAKK "Truum is in need of lumber from the Warsong Camp. I need to use Brutusk to haul it."


class npc_kadrakk : public CreatureScript
{
public:
    npc_kadrakk() : CreatureScript("npc_kadrakk") { }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        switch(uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF: DoScriptText(SAY_KADRAKK, pCreature, pPlayer); pPlayer->AddItem(45051,1); pPlayer->CLOSE_GOSSIP_MENU(); break;
        }
        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pCreature->isQuestGiver())
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());

        if (pPlayer->GetQuestStatus(13628) == QUEST_STATUS_INCOMPLETE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_KADRAKK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;       
    }
};

enum ePeon
{
    GO_WOOD                 = 194349,
    SPELL_SUMMON_WOOD       = 63043,
    NPC_DEMORALIZED_PEON    = 33440,
    EMOTE_DIGGING           = 173,
    NPC_PEON_WOOD_BUNNY     = 33446,

    SAY_CHOP_1              = -1934668,
    SAY_CHOP_2              = -1934669,
    SAY_CHOP_3              = -1934670
};

#define GOSSIP_DEMORALIZED_PEON "Come now, friend. You chop and I will protect you from the elves, ok?"


class npc_dem_peon : public CreatureScript
{
public:
    npc_dem_peon() : CreatureScript("npc_dem_peon"){}

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
    {
        pPlayer->PlayerTalkClass->ClearMenus();       
        if(uiAction == GOSSIP_ACTION_INFO_DEF && pPlayer->GetQuestStatus(13640) == QUEST_STATUS_INCOMPLETE)
        {
                DoScriptText(SAY_CHOP_1, pCreature, pPlayer); 
                CAST_AI(npc_dem_peon::npc_dem_peonAI, pCreature->AI())->bChop = true;
                CAST_AI(npc_dem_peon::npc_dem_peonAI, pCreature->AI())->PlayerGUID = pPlayer->GetGUID(); 
                pPlayer->CLOSE_GOSSIP_MENU(); 
        }
        return true;
    }

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        if (pPlayer->GetQuestStatus(13640) == QUEST_STATUS_INCOMPLETE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DEMORALIZED_PEON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        }
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
        return true;       
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_dem_peonAI (pCreature);
    }

    struct npc_dem_peonAI : public ScriptedAI
    {
        npc_dem_peonAI(Creature* creature) : ScriptedAI(creature) {}

        bool bChop;
        uint8 Phase;
        uint32 ChopTimer;
        uint64 PlayerGUID;

        void Reset()
        {
            bChop = false;
            ChopTimer = 3000;
            PlayerGUID = 0;
            Phase = 0;
            me->HandleEmoteCommand(431);
            me->SetRespawnTime(30);
        }

        void UpdateAI(uint32 const diff)
        {
            if(bChop)
            {
                if(ChopTimer <= diff)
                {
                    if(Player* pPlayer = me->GetPlayer(*me, PlayerGUID))
                    {
                        switch(Phase)
                        {
                            case 0:
                            {
                                if(Creature * pBunny = me->FindNearestCreature(NPC_PEON_WOOD_BUNNY,25.0f))
                                {
                                    me->GetMotionMaster()->MovePoint(1, pBunny->GetPositionX()-1, pBunny->GetPositionY()-1, pBunny->GetPositionZ());
                                    me->HandleEmoteCommand(0);
                                    ChopTimer = 3500;
                                    Phase++;
                                }
                            }break;
                            case 1: DoScriptText(SAY_CHOP_2, me, pPlayer); ChopTimer = 2500; Phase++; break;
                            case 2: me->HandleEmoteCommand(EMOTE_DIGGING); ChopTimer = 3000; Phase++; break;
                            case 3: DoScriptText(SAY_CHOP_3, me, pPlayer);  me->CastSpell(me, SPELL_SUMMON_WOOD, true); ChopTimer = 3500; Phase++; break;
                            case 4: me->ForcedDespawn(1000); break;
                            default: break;

                        }
                    }             
                }else ChopTimer -= diff;
            }
        }
    };
};



void AddSC_ashenvale()
{
    new npc_torek();
    new npc_ruul_snowhoof();
    new npc_muglash();
    new go_naga_brazier();
    new npc_spirit_gorat();
    new npc_brutusk();
    new npc_kadrakk();
    new npc_dem_peon();
}