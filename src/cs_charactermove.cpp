/*
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
#include "Language.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "ObjectAccessor.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "Player.h"

class mod_character_move_command : public CommandScript
{
public:
    mod_character_move_command() : CommandScript("mod_character_move_command") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
			{ "charactermove",         SEC_ADMINISTRATOR,  true,   &HandleMoveCharCommand,  "" },
        };

        return commandTable;
    }

    static bool HandleMoveCharCommand(ChatHandler* handler, const char* args)
    {
		char* player_name = strtok((char*)args, " ");
        char* account_name = strtok(nullptr, " ");
		
        if (!player_name || !account_name)
		{
			handler->PSendSysMessage("Usage : .charactermove Player_Name New_Account_Username");
            return true;
		}
		std::string accountName = account_name;
        if (!AccountMgr::normalizeString(accountName))
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 targetAccountId = AccountMgr::GetId(accountName);
        if (!targetAccountId)
        {
            handler->PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, accountName.c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }
		
		std::string playerName = player_name;
		normalizePlayerName(playerName);
		Player* player = ObjectAccessor::FindPlayerByName(playerName.c_str()); // get player by name
		if (!player){
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
		}else{
			
			// Kick if player is online
			WorldSession* s = player->GetSession();
			s->KickPlayer();                            // mark session to remove at next session list update
			s->LogoutPlayer(false);                     // logout player without waiting next session list update
			
			CharacterDatabase.PQuery("UPDATE characters SET account = '%u' WHERE guid = '%u'", targetAccountId, player->GetGUID());
			handler->PSendSysMessage("Character has been moved to new account.");
		}
        return true;
    }
};

void AddSC_mod_character_move_command()
{
    new mod_character_move_command();
}
