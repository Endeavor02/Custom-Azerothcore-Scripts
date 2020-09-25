#include "ScriptMgr.h"
#include "GridNotifiersImpl.h"

enum
{
	min_level = 60,//Defines the min level the player needs to be
	max_level = 70 //Defines the max level the player can reach
};

class tbc_level_token : public ItemScript
{
public:
	tbc_level_token() : ItemScript("tbc_level_token") { }

	bool OnUse(Player* player, Item* item, const SpellCastTargets&)
	{
		if (player->IsInCombat() || player->InArena() || player->InBattleground()) //Item is not usable in combat, arenas and battlegrounds. This can be modified to your taste.
		{
			player->GetSession()->SendNotification("You may not use this token whilst you are in combat or present in an arena or battleground.");
		}
		else if (player->getLevel() == max_level)
		{
			player->GetSession()->SendNotification("You are already level %i.", max_level);
		}
		else if (player->getLevel() >= max_level)
		{
			player->GetSession()->SendNotification("This item cannot be used above level %i.", max_level);
		}
		else if (player->getLevel() < min_level)
		{
			player->GetSession()->SendNotification("This item cannot be used below level %i.", min_level);
		}
		else
		{
			player->DestroyItemCount(item->GetEntry(), 1, true); //Item is destroyed on useage.
			player->GiveLevel((player->getLevel()+1)); //The player recieves one level.
//			player->GetSession()->SendNotification("Your leveling token has now been destroyed, and you are now level %i", new_level);
		}
		return true;
	}
};

void AddSC_tbc_level_token()
{
	new tbc_level_token();
}
