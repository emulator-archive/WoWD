#include "LogonStdAfx.h"
initialiseSingleton(AccountMgr);
initialiseSingleton(IPBanner);
initialiseSingleton(InformationCore);

void AccountMgr::ReloadAccounts(bool silent)
{
    if(!silent) sLog.outString("[AccountMgr] Reloading Accounts...");

    // Load *all* accounts.
    QueryResult * result = sLogonSQL->Query("SELECT acct, login, password, gm, flags, banned FROM accounts");
    Field * field;
    string AccountName;
    set<string> account_list;
    Account * acct;

    if(result)
    {
        do 
        {
            field = result->Fetch();
            AccountName = field[1].GetString();

            // transform to uppercase
            transform(AccountName.begin(), AccountName.end(), AccountName.begin(), towupper);

            acct = GetAccount(AccountName);
            if(acct == 0)
            {
                // New account.
                AddAccount(field);
            }
            else
            {
                // Update the account with possible changed details.
                UpdateAccount(acct, field);
            }

            // add to our "known" list
            account_list.insert(AccountName);

        } while(result->NextRow());

        delete result;
    }

    // check for any purged/deleted accounts
#ifdef WIN32
    HM_NAMESPACE::hash_map<string, Account>::iterator itr = AccountDatabase.begin();
    HM_NAMESPACE::hash_map<string, Account>::iterator it2;
#else
    std::map<string, Account>::iterator itr = AccountDatabase.begin();
    std::map<string, Account>::iterator it2;
#endif

    for(; itr != AccountDatabase.end();)
    {
        it2 = itr;
        ++itr;

        if(account_list.find(it2->first) == account_list.end())
            AccountDatabase.erase(it2);
    }

    if(!silent) sLog.outString("[AccountMgr] Found %u accounts.", AccountDatabase.size());
    IPBanner::getSingleton().Reload();
}

void AccountMgr::AddAccount(Field* field)
{
    Account acct;
    acct.AccountId      = field[0].GetUInt32();
    acct.Username       = field[1].GetString();
    acct.Password       = field[2].GetString();
    acct.GMFlags        = field[3].GetString();
    acct.AccountFlags   = field[4].GetUInt32();
	acct.Banned			= field[5].GetUInt32();

    // Convert username/password to uppercase. this is needed ;)
    transform(acct.Username.begin(), acct.Username.end(), acct.Username.begin(), towupper);
    transform(acct.Password.begin(), acct.Password.end(), acct.Password.begin(), towupper);

    AccountDatabase[acct.Username] = acct;
}

void AccountMgr::UpdateAccount(Account * acct, Field * field)
{
    uint32 id = field[0].GetUInt32();
    if(id != acct->AccountId)
    {
        //printf("Account %u `%s` is a duplicate.\n", id, acct->Username.c_str());
        sLog.outColor(TYELLOW, " >> deleting duplicate account %u [%s]...", id, acct->Username.c_str());
        sLog.outColor(TNORMAL, "\n");
        sLogonSQL->Execute("DELETE FROM accounts WHERE acct=%u", id);
        return;
    }
    acct->AccountId      = field[0].GetUInt32();
    acct->Username       = field[1].GetString();
    acct->Password       = field[2].GetString();
    acct->GMFlags        = field[3].GetString();
    acct->AccountFlags   = field[4].GetUInt32();
    acct->Banned         = field[5].GetUInt32();

    // Convert username/password to uppercase. this is needed ;)
    transform(acct->Username.begin(), acct->Username.end(), acct->Username.begin(), towupper);
    transform(acct->Password.begin(), acct->Password.end(), acct->Password.begin(), towupper);
}

bool AccountMgr::LoadAccount(string Name)
{
    QueryResult * result = sLogonSQL->Query("SELECT acct, login, password, gm, flags, banned FROM account_database WHERE login='%s'", Name.c_str());
    if(result == 0)
        return false;

    AddAccount(result->Fetch());
    delete result;

    return true;
}

void AccountMgr::UpdateAccountLastIP(uint32 accountId, const char * lastIp)
{
    //This allows us to keep track of last connection date and last ip  (can be useful)
    sLogonSQL->Execute("UPDATE accounts SET lastlogin=NOW(), lastip='%s' WHERE acct=%u;", lastIp, accountId);
}


void AccountMgr::ReloadAccountsCallback()
{
    ReloadAccounts(true);
    IPBanner::getSingleton().Reload();
}

#ifdef WIN32
BAN_STATUS IPBanner::CalculateBanStatus(uint32 ip_address)
{
    Guard guard(setBusy);

    uint8 b1 = ((uint8*)&ip_address)[0];
    uint8 b2 = ((uint8*)&ip_address)[1];
    uint8 b3 = ((uint8*)&ip_address)[2];
    uint8 b4 = ((uint8*)&ip_address)[3];
#else
BAN_STATUS IPBanner::CalculateBanStatus(const char* ip_address)
{
    Guard guard(setBusy);

    //uint8 b1 = static_cast<uint8>( atol(strtok((char*)ip_address, ".")) );
    //uint8 b2 = static_cast<uint8>( atol(strtok(NULL, ".")) );
    //uint8 b3 = static_cast<uint8>( atol(strtok(NULL, ".")) );
    //uint8 b4 = static_cast<uint8>( atol(strtok(NULL, ".")) );
    uint32 b1, b2, b3, b4;
    if(sscanf(ip_address, "%u.%u.%u.%u", &b1, &b2, &b3, &b4) != 4)
        return BAN_STATUS_NOT_BANNED;

#endif

    // loop storage array
    set<IPBan*>::iterator itr = banList.begin();
    uint32 bantime = 0;
    bool banned = false;

    for(; itr != banList.end(); ++itr)
    {
        // compare first byte
        if((*itr)->ip.full.b1 == b1 || (*itr)->ip.full.b1 == 0xFF)
        {
            // compare second byte if there was a first match
            if((*itr)->ip.full.b2 == b2 || (*itr)->ip.full.b2 == 0xFF)
            {
                // compare third byte if there was a second match
                if((*itr)->ip.full.b3 == b3 || (*itr)->ip.full.b3 == 0xFF)
                {
                    // compare last byte if there was a third match
                    if((*itr)->ip.full.b4 == b4 || (*itr)->ip.full.b4 == 0xFF)
                    {
                        // full IP match
                        banned = true;
                        bantime = (*itr)->ban_expire_time;
                        break;
                    }
                }
            }
        }
    }

    // calculate status
    if(!banned)
    {
        sLog.outDebug("[IPBanner] IP has no ban entry");
        return BAN_STATUS_NOT_BANNED;
    }

    if(bantime > time(NULL))
    {
        // temporary ban.
        sLog.outDebug("[IPBanner] IP temporary banned, %u seconds left", (bantime-time(NULL)));
        return BAN_STATUS_TIME_LEFT_ON_BAN;
    }
    else if(bantime != 0)
    {
        // ban has expired. erase it from the banlist and database
        sLog.outDebug("[IPBanner] Expired IP temporary ban has been removed");
        Remove(itr);
        return BAN_STATUS_NOT_BANNED;
    }
    else
    {
        // permanantly banned.
        sLog.outDebug("[IPBanner] IP permanantly banned");
        return BAN_STATUS_PERMANANT_BAN;
    }
}

void IPBanner::Load()
{
    QueryResult * result = sLogonSQL->Query("SELECT ip, expire FROM ipbans;");
    Field * fields;
    IPBan * ban;
    const char * ip_str;
    if(result)
    {
        do 
        {
            ban = new IPBan;
            fields = result->Fetch();

            ip_str = fields[0].GetString();
            ban->ip.full.b1 = static_cast<uint8>( atol(strtok((char*)ip_str, ".")) );
            ban->ip.full.b2 = static_cast<uint8>( atol(strtok(NULL, ".")) );
            ban->ip.full.b3 = static_cast<uint8>( atol(strtok(NULL, ".")) );
            ban->ip.full.b4 = static_cast<uint8>( atol(strtok(NULL, ".")) );

            ban->ban_expire_time = fields[1].GetUInt32();

            banList.insert( ban );
        }
        while(result->NextRow());

        delete result;
    }
}

void IPBanner::Reload()
{
    setBusy.Acquire();
    banList.clear();
    Load();
    setBusy.Release();
}

void IPBanner::Remove(set<IPBan*>::iterator ban)
{
    Guard guard(setBusy);

    banList.erase(ban);
    sLogonSQL->Execute("DELETE FROM ipbans WHERE ip='%s';", (*ban)->ip.full);

    sLog.outDebug("[IPBanner] Removed expired IPBan for ip '%s'", (*ban)->ip.full);
}

void InformationCore::AddRealm(uint32 realm_id, Realm * rlm)
{
    m_realms.insert( make_pair( realm_id, *rlm ) );
}

void InformationCore::RemoveRealm(uint32 realm_id)
{
    map<uint32, Realm>::iterator itr = m_realms.find(realm_id);
    if(itr == m_realms.end()) return;

    sLog.outString("Removing realm `%s` (%u) due to socket close.", itr->second.Name.c_str(), realm_id);
    m_realms.erase(itr);
}

void InformationCore::SendRealms(AuthSocket * Socket)
{
    realmLock.Acquire();

    // packet header
    ByteBuffer data;
    data << uint8(0x10);
    data << uint16(0);      // Size Placeholder

    // dunno what this is..
    data << uint32(0);

    sAuthLogonChallenge_C * client = Socket->GetChallenge();
    if(client->build < CLIENT_2_0_7)
        data << uint8(m_realms.size());
    else
        data << uint16(m_realms.size());
    
    // loop realms :/
    map<uint32, Realm>::iterator itr = m_realms.begin();
    for(; itr != m_realms.end(); ++itr)
    {
        data << uint8(itr->second.Colour);
        data << uint8(0);                   // Locked Flag
        data << uint8(itr->second.Icon);

        // This part is the same for all.
        data << itr->second.Name;
        data << itr->second.Address;
        data << itr->second.Population;
        data << uint8(0);                       // Character Count
        data << uint8(itr->second.TimeZone);   // time zone
        data << uint8(0);
    }
    realmLock.Release();

    data << uint8(0x15);
    data << uint8(0);

    // Re-calculate size.
    *(uint16*)&data.contents()[1] = data.size() - 3;

    // Send to the socket.
    Socket->SendPacket((const u8*)data.contents(), data.size());
}

BigNumber * InformationCore::GetSessionKey(uint32 account_id)
{
    map<uint32, BigNumber*>::iterator itr = m_sessionkeys.find(account_id);
    if(itr == m_sessionkeys.end())
        return 0;
    else
        return itr->second;
}

void InformationCore::DeleteSessionKey(uint32 account_id)
{
    map<uint32, BigNumber*>::iterator itr = m_sessionkeys.find(account_id);
    if(itr == m_sessionkeys.end())
        return;
    
    delete itr->second;
    m_sessionkeys.erase(itr);
}

void InformationCore::SetSessionKey(uint32 account_id, BigNumber * key)
{
    m_sessionkeys[account_id] = key;
}

void InformationCore::TimeoutSockets()
{
    uint32 t = time(NULL);
    // check the ping time
    set<LogonCommServerSocket*>::iterator itr, it2;
    LogonCommServerSocket * s;
    for(itr = m_serverSockets.begin(); itr != m_serverSockets.end();)
    {
        s = *itr;
        it2 = itr;
        ++itr;

        if(s->IsConnected() && !s->CheckConnection() || (usepings && (t - s->last_ping) > 20)) // ping timeout
        {
            s->removed = true;
            s->Disconnect();
#ifdef ASYNC_NET
            s->PostCompletion(IOShutdownRequest);
#endif
            if(s->my_id)
                RemoveRealm(s->my_id);
            m_serverSockets.erase(it2);
        }
    }
}
