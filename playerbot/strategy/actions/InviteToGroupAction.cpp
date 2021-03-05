#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "InviteToGroupAction.h"
#include "../../ServerFacade.h"


using namespace ai;

namespace ai
{
    bool InviteToGroupAction::Invite(Player* player)
    {
        if (!player)
            return false;

        WorldPacket p;
        uint32 roles_mask = 0;
        p << player->GetName();
        p << roles_mask;
        bot->GetSession()->HandleGroupInviteOpcode(p);

        return true;
    }

    bool InviteNearbyToGroupAction::Execute(Event event)
    {
        list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
        for (auto& i : nearGuids)
        {
            Player* player = sObjectMgr.GetPlayer(i);

            if (!player)
                continue;

            if (player->GetGroup())
                continue;

            PlayerbotAI* botAi = player->GetPlayerbotAI();

            if (!botAi) //Only invite bots. Maybe change later.
                continue;

            if (botAi->GetMaster())
                if (!botAi->GetMaster()->GetPlayerbotAI() || botAi->GetMaster()->GetPlayerbotAI()->isRealPlayer()) //Do not invite bots with a player master.
                    if (!botAi->isRealPlayer()) //Unless the bot is really a player
                        continue;

            if (player->getLevel() > bot->getLevel() + 2)
                continue;

            if (player->getLevel() < bot->getLevel() - 2)
                continue;

            if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
                continue;

            return Invite(player);
        }

        return false;
    }

    bool InviteNearbyToGroupAction::isUseful()
    {
        if (!sPlayerbotAIConfig.randomBotGroupNearby)
            return false;

        if (bot->GetGroup())
        {
            if (bot->GetGroup()->IsFull())
                return false;

            if (bot->GetGroup()->GetLeaderGuid() != bot->GetObjectGuid())
                return false;
        }

        if (ai->GetMaster())
            if (!ai->GetMaster()->GetPlayerbotAI() || ai->GetMaster()->GetPlayerbotAI()->isRealPlayer()) //Alts do not invite.
                if (!ai->isRealPlayer()) //Unless the bot is really a player
                    return false;

        return true;
    }
}
