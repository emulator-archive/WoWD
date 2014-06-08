#include "StdAfx.h"
initialiseSingleton(MailSystem);

void MailSystem::StartMailSystem()
{
    //Guard<Mutex> guard(MUTEX);
    sLog.outString("Starting Mail System...");
    sLog.outString("");
    
    LoadMessages();

    sLog.outString("  Loading mail configuration options...");
    config_flags = 0;
    if(Config.MainConfig.GetBoolDefault("Mail.DisablePostageCostsForGM", true))
        config_flags |= MAIL_FLAG_NO_COST_FOR_GM;

    if(Config.MainConfig.GetBoolDefault("Mail.DisablePostageCosts", false))
        config_flags |= MAIL_FLAG_DISABLE_POSTAGE_COSTS;

    if(Config.MainConfig.GetBoolDefault("Mail.DisablePostageDelayItems", true))
        config_flags |= MAIL_FLAG_DISABLE_HOUR_DELAY_FOR_ITEMS;

    if(Config.MainConfig.GetBoolDefault("Mail.DisableMessageExpiry", false))
        config_flags |= MAIL_FLAG_NO_EXPIRY;

    if(Config.MainConfig.GetBoolDefault("Mail.EnableInterfactionMail", true))
        config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION;

    if(Config.MainConfig.GetBoolDefault("Mail.EnableInterfactionForGM", true))
        config_flags |= MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION_GM;

    sLog.outString("  Enabling periodic mail database reload (if enabled)...");
    uint32 delay = Config.MainConfig.GetIntDefault("Mail.ReloadDelay", 0);
    if(delay)
        sEventMgr.AddEvent(this, &MailSystem::PeriodicMailRefresh, EVENT_MAILSYSTEM_RELOAD, delay, 0);

    sLog.outString("");
    sLog.outString("Mail System Ready!");
}

void MailSystem::ShutdownMailSystem()
{
    sLog.outString("Shutting down mail system...");
    sLog.outString("  Deleting all messages / mailboxes...");
    MailboxMap::iterator itr;
    for(itr = Mailboxes.begin(); itr != Mailboxes.end(); ++itr)
        delete itr->second;

    Mailboxes.clear();
    sLog.outString("Mail system shut down.");
}

Mailbox* MailSystem::CreateMailbox(uint64 ownerguid)
{
    Mailbox * box = new Mailbox(ownerguid);
    Mailboxes[ownerguid] = box;
    return box;
}

Mailbox* MailSystem::GetPlayersMailbox(uint64 player_guid, bool create)
{
    Guard guard(MUTEX);

    MailboxMap::iterator itr = Mailboxes.find(player_guid);
    if(itr == Mailboxes.end())
    {
        if(create)
            return CreateMailbox(player_guid);
        else
            return NULL;
    }
    return itr->second;
}

uint32 MailSystem::GenerateMessageID()
{
    ++message_high;
    return message_high;
}

MailError MailSystem::DeliverMessage(uint64 recipent, MailMessage* message)
{
    Mailbox * box = GetPlayersMailbox(recipent, true);
    assert(box);
    if(box->MessageCount() == 50)   // Full mailbox
        return MAIL_ERR_INTERNAL_ERROR;

    box->AddMessage(message);
    return MAIL_OK;
}

void Mailbox::AddMessage(MailMessage* Message)
{
    Messages[Message->message_id] = *Message;
}

void Mailbox::DeleteMessage(uint32 MessageId, bool sql)
{
    Messages.erase(MessageId);
    if(sql)
        sDatabase.Execute("DELETE FROM mailbox WHERE message_id = %u", MessageId);
}

WorldPacket * Mailbox::BuildMailboxListingPacket()
{
    WorldPacket * data = new WorldPacket(SMSG_MAIL_LIST_RESULT, 500);
    MessageMap::iterator itr;
    uint32 count = 0;
    uint32 t = time(NULL);
    *data << uint8(0);     // size placeholder

    for(itr = Messages.begin(); itr != Messages.end(); ++itr)
    {
        if(itr->second.expire_time && t > itr->second.expire_time)
            continue;       // expired mail -> skip it
        
        if(itr->second.AddMessageDataToPacket(*data))
            ++count;
        
        if(count == 50)
            break;
    }

    const_cast<uint8*>(data->contents())[0] = count;

    // do cleanup on request mail
    CleanupExpiredMessages();
    return data;
}

void Mailbox::CleanupExpiredMessages()
{
    MessageMap::iterator itr, it2;
    uint32 curtime = time(NULL);

    for(itr = Messages.begin(); itr != Messages.end();)
    {
        it2 = itr++;
        if(it2->second.expire_time && it2->second.expire_time < curtime)
        {
            Messages.erase(it2);
        }
    }
}

bool MailMessage::AddMessageDataToPacket(WorldPacket& data)
{
    // add stuff
    if(deleted_flag)
        return false;

    data << message_id;
    data << uint8(message_type);
    if(message_type)
        data << uint32(sender_guid);
    else
        data << sender_guid;

    data << subject;
    data << message_id;      // itempageid
    data << message_id;
    
    /*if(message_type == 0)
        data << uint32(0x29);
    else
        data << uint32(0x3E);*/
    data << stationary;

    uint32 itementry = 0, itemcount = 0;
    uint32 charges = 0, durability = 0, maxdurability = 0;

    if(attached_item_guid)
    {
        stringstream ss;
        ss << "SELECT `entry`, `count`, `charges`, `durability` FROM playeritems WHERE guid='"
            << attached_item_guid << "'";
        
        QueryResult * result = sDatabase.Query(ss.str().c_str());
        if(result)
        {
            itementry = result->Fetch()[0].GetUInt32();
            itemcount = result->Fetch()[1].GetUInt32();
            charges = result->Fetch()[2].GetUInt32();
            durability = result->Fetch()[3].GetUInt32();
            ItemPrototype * it = objmgr.GetItemPrototype(itementry);
            maxdurability = it ? it->MaxDurability : durability;

            delete result;
        }
    }

    data << itementry;
    data << uint32(0);  // unk
    data << uint32(0);  // unk
    data << uint32(0);  // unk
    for(uint32 i = 0; i < 17; ++i)
        data << uint32(0);

    data << (itemcount ? uint8(itemcount) : uint8(1));
    data << charges;
    data << maxdurability;
    data << durability;
    data << money;
    data << cod;
    data << uint32(read_flag);

    if(expire_time)
        data << float(float(expire_time - time(NULL)) / 86400.0f);
    else
        data << float(0);

    data << uint32(0);

    return true;
}

void MailSystem::SaveMessageToSQL(MailMessage * message)
{
    stringstream ss;
    ss << "INSERT INTO mailbox VALUES("
        << message->message_id << ","
        << message->message_type << ","
        << message->player_guid << ","
        << message->sender_guid << ",\""
        << message->subject << "\",\""
        << message->body << "\","
        << message->money << ",'"
        << message->attached_item_guid << "',"
        << message->cod << ","
        << message->stationary << ","
        << message->expire_time << ","
        << message->delivery_time << ","
        << message->copy_made << ","
        << message->read_flag << ","
        << message->deleted_flag << ")";
    sDatabase.Execute(ss.str().c_str());
}

void WorldSession::HandleSendMail(WorldPacket & recv_data )
{
    MailMessage msg;
    uint64 gameobject;
    uint32 unk2;
    string recepient;
    uint32 err = MAIL_OK;

    recv_data >> gameobject >> recepient;
    recv_data >> msg.subject >> msg.body >> msg.stationary;
    recv_data >> unk2 >> msg.attached_item_guid >> msg.money >> msg.cod;
    
    // Search for the recipient
    PlayerInfo * player = ObjectMgr::getSingleton().GetPlayerInfoByName(recepient.c_str());
    if(player == 0)
    {
        SendMailError(MAIL_ERR_RECIPIENT_NOT_FOUND);
        return;
    }

    bool interfaction = false;
    if(sMailSystem.MailOption(MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION) ||
        (HasPermissions() && sMailSystem.MailOption(MAIL_FLAG_CAN_SEND_TO_OPPOSITE_FACTION_GM)))
    {
        interfaction = true;
    }

    // Check we're sending to the same faction (disable this for testing)
    if(player->team != _player->GetTeam() && !interfaction)
    {
        SendMailError(MAIL_ERR_NOT_YOUR_ALLIANCE);
        return;
    }

    // Instant delivery time by default.
    msg.delivery_time = time(NULL);

    // Set up the cost
    int32 cost = 0;
    if(!sMailSystem.MailOption(MAIL_FLAG_DISABLE_POSTAGE_COSTS) &&
        !(GetPermissionCount() && sMailSystem.MailOption(MAIL_FLAG_NO_COST_FOR_GM)))
    {
        cost = 30;
    }

    // Check for attached money
    if(msg.money > 0)
        cost += msg.money;

    // check that we have enough in our backpack
    if(_player->GetUInt32Value(PLAYER_FIELD_COINAGE) < cost)
    {
        SendMailError(MAIL_ERR_NOT_ENOUGH_MONEY);
        return;
    }

    // Check for the item, and required item.
    if(msg.attached_item_guid != 0)
    {
        Item * attached_item = _player->GetItemInterface()->SafeRemoveAndRetreiveItemByGuid(msg.attached_item_guid, true);
        if(attached_item == 0 || attached_item->IsSoulbound())
        {
            // could not find item
            SendMailError(MAIL_ERR_INTERNAL_ERROR);
            return;
        }

        // Save the item with no owner (to objectmgr)
        attached_item->SetOwner(NULL);
        attached_item->SaveToDB(INVENTORY_SLOT_NOT_SET, 0, true);

        // Cut out the high part of the attached item.
        msg.attached_item_guid = attached_item->GetGUIDLow();

        // Delete from memory now, its not needed anymore.
        delete attached_item;

        // items add an hour to delivery time
        if(!sMailSystem.MailOption(MAIL_FLAG_DISABLE_HOUR_DELAY_FOR_ITEMS))
            msg.delivery_time += 3600;  // 1hr

        // no need to update guid, it shouldn't change..
    }

    // take the money
    _player->ModUInt32Value(PLAYER_FIELD_COINAGE, -cost);

    // Fill in the rest of the info
    msg.player_guid = player->guid;
    msg.sender_guid = _player->GetGUID();
    
    // 30 day expiry time for unread mail mail
    if(!sMailSystem.MailOption(MAIL_FLAG_NO_EXPIRY))
        msg.expire_time = time(NULL) + (TIME_DAY * 30);
    else
        msg.expire_time = 0;

    msg.copy_made = false;
    msg.read_flag = false;
    msg.deleted_flag = false;
    msg.message_type = 0;

    // Make an ID for us
    msg.message_id = sMailSystem.GenerateMessageID();

    // Save the message to database
    sMailSystem.SaveMessageToSQL(&msg);

    // Great, all our info is filled in. Now we can add it to the other players mailbox.
    sMailSystem.DeliverMessage(player->guid, &msg);

    // Success packet :)
    SendMailError(MAIL_OK);

    // If the player is online, send SMSG_RECEIVED_MAIL to make the letter show up
    Player * plr = World::GetPlayer(player->guid);
    if(plr && plr->GetSession())
    {
        WorldPacket data(SMSG_RECEIVED_MAIL, 4);
        data << uint32(0);
        SendPacket(&data);
    }
}

void WorldSession::HandleMarkAsRead(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0) return;

    MailMessage * message = m->GetMessage(message_id);
    if(message == 0) return;

    // mark the message as read
    message->read_flag = 1;

    // mail now has a 3 day expiry time
    if(!sMailSystem.MailOption(MAIL_FLAG_NO_EXPIRY))
        message->expire_time = time(NULL) + (TIME_DAY * 3);

    // update it in sql
    sDatabase.Execute("UPDATE mailbox SET read_flag = 1, expiry_time = %u WHERE message_id = %u", message->message_id, message->expire_time);
}

void WorldSession::HandleMailDelete(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << message_id << uint32(MAIL_RES_DELETED);

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    MailMessage * message = m->GetMessage(message_id);
    if(message == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    if(message->copy_made)
    {
        // we have the message as a copy on the item. we can't delete it or this item
        // will no longer function.

        // deleted_flag prevents it from being shown in the mail list.
        message->deleted_flag = 1;

        // update in sql
        sDatabase.Execute("UPDATE mailbox SET deleted_flag = 1 WHERE message_id = %u", message_id);
    }
    else
    {
        // delete the message, there are no other references to it.
        m->DeleteMessage(message_id, true);
    }

    data << uint32(MAIL_OK);
    SendPacket(&data);
}

void WorldSession::HandleTakeItem(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << message_id << uint32(MAIL_RES_ITEM_TAKEN);
    
    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    MailMessage * message = m->GetMessage(message_id);
    if(message == 0 || !message->attached_item_guid)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    // check for cod credit
    if(message->cod > 0)
    {
        if(_player->GetUInt32Value(PLAYER_FIELD_COINAGE) < message->cod)
        {
            data << uint32(MAIL_ERR_NOT_ENOUGH_MONEY);
            SendPacket(&data);
            return;
        }
    }

    // grab the item
    Item * item = 0;

    // try cached auctionhouse items first,
    // this will stop crashes later on :p

    if(message->message_type == AUCTION)
        item = objmgr.GetAItem(message->attached_item_guid);

    if(item == 0)
        item = objmgr.LoadItem(message->attached_item_guid);

    if(item == 0)
    {
        // doesn't exist

        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);
        
        return;
    }

    // find a free bag slot
    SlotResult result = _player->GetItemInterface()->FindFreeInventorySlot(item->GetProto());
    if(result.Result == 0)
    {
        // no free slots left!
        data << uint32(MAIL_ERR_BAG_FULL);
        SendPacket(&data);

        delete item;
        return;
    }

    // all is good
    // delete the item (so when its resaved it'll have an association)
    item->DeleteFromDB();

    // add the item to their backpack
    _player->GetItemInterface()->AddItemToFreeSlot(item);

    // message no longer has an item association
    message->attached_item_guid = 0;
    
    // update in sql!
    sDatabase.Execute("UPDATE mailbox SET attached_item_guid = 0, cod = 0 WHERE message_id = %u", message->message_id);

    // send complete packet
    data << uint32(MAIL_OK);
    SendPacket(&data);
    
    // COD really makes this hell here. it means if the player isn't online, we gotta load them,
    // change the coinage value and save them :(
    if(message->cod > 0)
    {
        // take the cod charges from us
        _player->ModUInt32Value(PLAYER_FIELD_COINAGE, -int32(message->cod));
        Player * sender = World::GetPlayer(message->sender_guid);
        if(sender != 0)
        {
            // player is inworld... this is easy..
            sender->ModUInt32Value(PLAYER_FIELD_COINAGE, message->cod);

            // might as well send them a message
            ChatHandler::getSingleton().SystemMessageToPlr(sender, "%s pays you %u copper for your %s (via cash on delivery).", 
                _player->GetName(), message->cod, item->GetProto()->Name1.c_str());
        }
        else
        {
            // this is the hard part, the player is offline so we need to load them, 
            // mod, and resave.
            QueryResult * result = sDatabase.Query("SELECT data FROM characters WHERE guid=%u", message->sender_guid);
            if(result)
            {
                Player * plr = sObjHolder.Create<Player>((uint32)message->sender_guid);
                plr->LoadValues(result->Fetch()[0].GetString());
                plr->ModUInt32Value(PLAYER_FIELD_COINAGE, message->cod);
                plr->UpdateOfflinePlayerData();
                sObjHolder.Delete<Player>(plr);
                delete result;
            }
        }
    }

    // prolly need to send an item push here
}

void WorldSession::HandleTakeMoney(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << message_id << uint32(MAIL_RES_MONEY_TAKEN);

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    MailMessage * message = m->GetMessage(message_id);
    if(message == 0 || !message->money)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    // add the money to the player
    _player->ModUInt32Value(PLAYER_FIELD_COINAGE, message->money);

    // message no longer has any money
    message->money = 0;

    // update in sql!
    sDatabase.Execute("UPDATE mailbox SET money = 0 WHERE message_id = %u", message->message_id);

    // send result
    data << uint32(MAIL_OK);
    SendPacket(&data);
}

void WorldSession::HandleReturnToSender(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << message_id << uint32(MAIL_RES_RETURNED_TO_SENDER);

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    MailMessage * msg = m->GetMessage(message_id);
    if(msg == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }
    
    // copy into a new struct
    MailMessage message = *msg;

    // remove the old message
    m->DeleteMessage(message_id, true);

    // re-assign the owner/sender
    message.player_guid = message.sender_guid;
    message.sender_guid = _player->GetGUID();

    // turn off the read flag
    message.read_flag = false;
    message.deleted_flag = false;
    message.copy_made = false;

    // assign a new id
    message.message_id = sMailSystem.GenerateMessageID();

    // null out the cod charges. (the sender doesnt want to have to pay for his own item
    // that he got nothing for.. :p)
    message.cod = 0;

    // assign new delivery time
    message.delivery_time = message.attached_item_guid ? time(NULL) + 3600 : time(NULL);

    // add to the senders mailbox
    sMailSystem.DeliverMessage(message.player_guid, &message);

    // add to sql
    sMailSystem.SaveMessageToSQL(&message);    

    // finish the packet
    data << uint32(MAIL_OK);
    SendPacket(&data);
}

void WorldSession::HandleMailCreateTextItem(WorldPacket & recv_data )
{
    uint64 mailbox;
    uint32 message_id;
    recv_data >> mailbox >> message_id;

    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << message_id << uint32(MAIL_RES_MADE_PERMANENT);

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    ItemPrototype * proto = objmgr.GetItemPrototype(8383);
    MailMessage * message = m->GetMessage(message_id);
    if(message == 0 || !proto)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    SlotResult result = _player->GetItemInterface()->FindFreeInventorySlot(proto);
    if(result.Result == 0)
    {
        data << uint32(MAIL_ERR_INTERNAL_ERROR);
        SendPacket(&data);

        return;
    }

    Item * pItem = objmgr.CreateItem(8383, _player);
    pItem->SetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID, message_id);
    _player->GetItemInterface()->AddItemToFreeSlot(pItem);

    // mail now has an item after it
    message->copy_made = true;

    // update in sql
    sDatabase.Execute("UPDATE mailbox SET copy_made = 1 WHERE message_id = %u", message_id);

    data << uint32(MAIL_OK);
    SendPacket(&data);
}

void WorldSession::HandleItemTextQuery(WorldPacket & recv_data)
{
    uint32 message_id;
    recv_data >> message_id;

    string body = "Internal Error";

    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(m != 0)
    {
        MailMessage * message = m->GetMessage(message_id);
        if(message != 0)
            body = message->body;
    }

    WorldPacket data(SMSG_ITEM_TEXT_QUERY_RESPONSE, body.length() + 5);
    data << message_id << body;
    SendPacket(&data);
}

bool Mailbox::HasUnreadMessages()
{
    MessageMap::iterator iter = Messages.begin();
    for(; iter != Messages.end(); ++iter)
    {
        if(iter->second.read_flag == 0)
            return true;
    }

    return false;
}

void WorldSession::HandleMailTime(WorldPacket & recv_data)
{
    WorldPacket data(MSG_QUERY_NEXT_MAIL_TIME, 4);
    Mailbox * m = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    
    if(m && m->HasUnreadMessages())
        data << uint32(0);
    else
        data << uint32(0xc7a8c000);
    
    SendPacket(&data);
}

void WorldSession::SendMailError(uint32 error)
{
    WorldPacket data(SMSG_SEND_MAIL_RESULT, 12);
    data << uint32(0);
    data << uint32(MAIL_RES_MAIL_SENT);
    data << error;
    SendPacket(&data);
}

void WorldSession::HandleGetMail(WorldPacket & recv_data )
{
    Mailbox * box = sMailSystem.GetPlayersMailbox(_player->GetGUID(), false);
    if(box == 0)
    {
        OutPacket(SMSG_MAIL_LIST_RESULT, 1, "\x00");
        return;
    }

    WorldPacket * data = box->BuildMailboxListingPacket();
    SendPacket(data);
    delete data;
}

void MailSystem::RemoveMessageIfDeleted(uint32 message_id, uint64 player_guid)
{
    Mailbox * box = GetPlayersMailbox(player_guid, false);
    if(box == 0) return;
    
    MailMessage * msg = box->GetMessage(message_id);
    if(msg == 0) return;

    if(msg->deleted_flag)   // we've deleted from inbox
        box->DeleteMessage(message_id, true);   // wipe the message
}

void MailSystem::SendAutomatedMessage(uint32 type, uint64 sender, uint64 receiver, string subject, string body,
                                      uint32 money, uint32 cod, uint64 item_guid, uint32 stationary)
{
    // This is for sending automated messages, for example from an auction house.
    MailMessage msg;
    msg.message_type = type;
    msg.sender_guid = sender;
    msg.player_guid = receiver;
    msg.subject = subject;
    msg.body = body;
    msg.money = money;
    msg.cod = cod;
    msg.attached_item_guid = item_guid;
    msg.stationary = stationary;
    msg.delivery_time = time(NULL);
    msg.expire_time = 0;
    msg.read_flag = false;
    msg.copy_made = false;
    msg.message_id = GenerateMessageID();
    msg.deleted_flag = false;

    // Send the message.
    DeliverMessage(receiver, &msg);

    // Save it in sql (important:P)
    SaveMessageToSQL(&msg);
}

void MailSystem::PeriodicMailRefresh()
{
    // this funtion will reload the mail from the database. use it if you've set up some external
    // source of getting mail (e.g. a web-based interface)
    Guard guard(MUTEX);

    // clear all mailboxes
    MailboxMap::iterator iter = Mailboxes.begin();
    for(; iter != Mailboxes.end(); ++iter)
        delete iter->second;
    Mailboxes.clear();

    // create/load from the database
    LoadMessages();
}

void MailSystem::LoadMessages()
{
    sLog.outString("  Creating/Loading Mailboxes...");

    uint32 high = 0;
    QueryResult * result = sDatabase.Query("SELECT * FROM mailbox");
    if(result)
    {
        Field * fields;
        uint32 message_id;
        MailMessage msg;
        Mailbox * box;
        uint64 owner;
        uint32 i;

        do 
        {
            fields = result->Fetch();
            owner = fields[2].GetUInt64();
            box = GetPlayersMailbox(owner, true);
            assert(box);

            // check message id
            message_id = fields[0].GetUInt32();
            if(message_id > high)
                high = message_id;

            // Create message struct
            i = 0;
            msg.message_id = fields[i++].GetUInt32();
            msg.message_type = fields[i++].GetUInt32();
            msg.player_guid = fields[i++].GetUInt32();
            msg.sender_guid = fields[i++].GetUInt32();
            msg.subject = fields[i++].GetString();
            msg.body = fields[i++].GetString();
            msg.money = fields[i++].GetUInt32();
            msg.attached_item_guid = fields[i++].GetUInt64();
            msg.cod = fields[i++].GetUInt32();
            msg.stationary = fields[i++].GetUInt32();
            msg.expire_time = fields[i++].GetUInt32();
            msg.delivery_time = fields[i++].GetUInt32();
            msg.copy_made = fields[i++].GetBool();
            msg.read_flag = fields[i++].GetBool();
            msg.deleted_flag = fields[i++].GetBool();

            // Add to the mailbox
            box->AddMessage(&msg);

        } while(result->NextRow());
        delete result;
    }
    message_high = high;

    sLog.outString("  Removing expired messages...");
    MailboxMap::iterator iter;
    for(iter = Mailboxes.begin(); iter != Mailboxes.end(); ++iter)
    {
        iter->second->CleanupExpiredMessages();
    }

    sLog.outString("  Deleting expired mailboxes...");
    Mailbox * dbox;
    for(iter = Mailboxes.begin(); iter != Mailboxes.end();)
    {
        dbox = iter->second;
        ++iter;
        if(dbox->MessageCount() == 0)
        {
            Mailboxes.erase(dbox->GetOwner());
            delete dbox;
        }
    }
}
