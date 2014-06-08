#include "StdAfx.h"

/************************************************************************/
/* SimpleCreatureAI.cpp Script                                          */
/************************************************************************/

extern "C" WOWD_SCRIPT_DECL CreatureAIScript* CreateCreatureAIClass(uint32 uEntryId, Creature* creature);
extern "C" WOWD_SCRIPT_DECL void ScriptInitialize(ScriptModule *mod);

// Build version info function
BUILD_VERSIONINFO_DATA(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);

/* This script covers npc's that just have random say text when they enter
   combat. Texts were taken from the old lua scripts. 
*/

struct SimpleCreatureText
{
    uint32 EntryID;
    uint32 TextCount;
    string Texts[4];
};

static SimpleCreatureText DefiasTexts[] =
{
    { 38   , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 116  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 94   , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 909  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 910  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1727 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 594  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1725 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 589  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 590  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 619  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1726 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 6927 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 6829 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1707 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1711 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1715 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1706 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 1708 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 634  , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 4417 , 2, "I see those fools at the Abbey sent some fresh meat for us.", "This land belongs to the Defias Brotherhood now!", "", "" },
    { 550  , 3, "Who dares interfere with the business of the Defias Brotherhood? Die $C!", "Die in the name of Edwin Van Cleef!", "Here's a singing telegram for $N: Roses are red, violets are blue, I will kill any $R I see, including you!", "" },
    { 0    , 0, "","","","" },
};

class SimpleCreature : public CreatureAIScript
{
public:
    SimpleCreature(Creature* pCreature) : CreatureAIScript(pCreature)
    {
        Entry = pCreature->GetEntry();
        MyText = 0;
        
        // set my text
        switch(Entry)
        {
        case 38:
        case 116:
        case 94:
        case 909:
        case 910:
        case 1727:
        case 594:
        case 1725:
        case 589:
        case 590:
        case 619:
        case 1726:
        case 6927:
        case 2829:
        case 1707:
        case 1711:
        case 1715:
        case 1706:
        case 1708:
        case 634:
        case 4417:
        case 550:
            for(uint32 i = 0; DefiasTexts[i].EntryID != 0; ++i)
            {
                if(DefiasTexts[i].EntryID == Entry)
                {
                    MyText = &DefiasTexts[i];
                    return;
                }
            }break;
        }
    }
    
    void OnCombatStart(Unit* mTarget)
    {
        // 50% chance to send combat message.
        if(!MyText || !Rand(50.0f))
            return;
        uint32 TextID = rand() % MyText->TextCount;
        if(TextID > MyText->TextCount)
            TextID = 0;

        _unit->SendChatMessage(11, 0, MyText->Texts[TextID].c_str());
    }
    inline bool HasText() { return (Entry != 0) ? true : false; }

protected:
    uint32 Entry;
    SimpleCreatureText * MyText;
};

CreatureAIScript* CreateCreatureAIClass(uint32 uEntryId, Creature* creature)
{
    SimpleCreature * pc = new SimpleCreature(creature);
    if(!pc->HasText())
    {
        delete pc;
        return 0;
    }

    return pc;
}

void ScriptInitialize(ScriptModule *mod)
{
    // Defias creatures
    sScriptMgr.ScriptAddCreatureModule(38, mod);
    sScriptMgr.ScriptAddCreatureModule(116, mod);
    sScriptMgr.ScriptAddCreatureModule(94, mod);
    sScriptMgr.ScriptAddCreatureModule(909, mod);
    sScriptMgr.ScriptAddCreatureModule(910, mod);
    sScriptMgr.ScriptAddCreatureModule(1727, mod);
    sScriptMgr.ScriptAddCreatureModule(594, mod);
    sScriptMgr.ScriptAddCreatureModule(1725, mod);
    sScriptMgr.ScriptAddCreatureModule(589, mod);
    sScriptMgr.ScriptAddCreatureModule(590, mod);
    sScriptMgr.ScriptAddCreatureModule(619, mod);
    sScriptMgr.ScriptAddCreatureModule(1726, mod);
    sScriptMgr.ScriptAddCreatureModule(6927, mod);
    sScriptMgr.ScriptAddCreatureModule(2829, mod);
    sScriptMgr.ScriptAddCreatureModule(1707, mod);
    sScriptMgr.ScriptAddCreatureModule(1711, mod);
    sScriptMgr.ScriptAddCreatureModule(1715, mod);
    sScriptMgr.ScriptAddCreatureModule(1706, mod);
    sScriptMgr.ScriptAddCreatureModule(1708, mod);
    sScriptMgr.ScriptAddCreatureModule(634, mod);
    sScriptMgr.ScriptAddCreatureModule(4417, mod);
    sScriptMgr.ScriptAddCreatureModule(550, mod);
}
