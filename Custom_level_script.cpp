#include "ScriptMgr.h"
#include "GridNotifiersImpl.h"

enum
{
	min_level = 55,//Defines the minimum level the player can reach
	max_level = 60 //Defines the max level the player can reach
};

class level_token : public ItemScript
{
public:
	level_token() : ItemScript("level_token") { }

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
//			player->GetSession()->SendNotification("Your leveling token has now been destroyed, and you are now level %i", new_level); //I don't feel this is really needed but can be used
		}
		return true;
	}
};

void AddSC_level_token()
{
	new level_token();
}
