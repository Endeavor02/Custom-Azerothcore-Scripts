// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //
//                                                                                           //
//    ____                _           _   _             __  __      ____        _ _     _    //
//   / ___|_ __ ___  __ _| |_ ___  __| | | |__  _   _  |  \/  |_ __/ ___|  ___ | (_) __| |   //
//  | |   | '__/ _ \/ _` | __/ _ \/ _` | | '_ \| | | | | |\/| | '__\___ \ / _ \| | |/ _` |   //
//  | |___| | |  __/ (_| | ||  __/ (_| | | |_) | |_| | | |  | | |   ___) | (_) | | | (_| |   //
//   \____|_|  \___|\__,_|\__\___|\__,_| |_.__/ \__, | |_|  |_|_|  |____/ \___/|_|_|\__,_|   //
//                                              |___/                                        //
//                                                                                           //
// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / //

//Created by MrSolid, 2017. Exclusively for the ACWeb Community.

#include "ScriptMgr.h"
#include "GridNotifiersImpl.h"

enum
{
	min_level = 55,
	max_level = 60 //Defines the max level the player can reach
//	new_level = 56 //Defines the level which the player will reach after using this token.
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
			player->GiveLevel((player->getLevel()+1)); //The player recieves levels needed to reach new_level. E.g. 255-254=1; 1+254=255.
//			player->GetSession()->SendNotification("Your leveling token has now been destroyed, and you are now level %i", new_level);
		}
		return true;
	}
};

void AddSC_level_token() // Add to scriptloader normally
{
	new level_token();
}
