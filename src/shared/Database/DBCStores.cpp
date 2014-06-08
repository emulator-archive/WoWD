#include "DBCStores.h"
#include "DataStore.h"

// You need a line like this for every DBC store. If you use createDBCStore (no Indexed), the lines will be ordered the way they are in the file
// SpellEntry is the file struct entry (for Spell.dbc here).
implementIndexedDBCStore(ItemSetStore,ItemSetEntry)
implementIndexedDBCStore(LockStore,Lock)
implementIndexedDBCStore(SpellStore,SpellEntry)
implementIndexedDBCStore(DurationStore,SpellDuration)
implementIndexedDBCStore(RangeStore,SpellRange)
implementIndexedDBCStore(EmoteStore,emoteentry)
implementIndexedDBCStore(RadiusStore,SpellRadius)
implementIndexedDBCStore(CastTimeStore,SpellCastTime)
implementIndexedDBCStore(AreaStore,AreaTable)
implementIndexedDBCStore(FactionTmpStore,FactionTemplateDBC)
implementIndexedDBCStore(RandomPropStore,RandomProps)
implementIndexedDBCStore(FactionStore,FactionDBC)
implementIndexedDBCStore(EnchantStore,EnchantEntry)
implementIndexedDBCStore(WorldMapAreaStore,WorldMapArea)
implementDBCStore(WorldMapOverlayStore,WorldMapOverlay)
implementDBCStore(SkillStore,skilllinespell)
implementIndexedDBCStore(SkillLineStore,skilllineentry)
implementDBCStore(TaxiPathStore,DBCTaxiPath)
implementDBCStore(TaxiNodeStore,DBCTaxiNode)
implementDBCStore(TaxiPathNodeStore,DBCTaxiPathNode)
implementDBCStore(WorldSafeLocsStore,GraveyardEntry)
implementIndexedDBCStore(TransportAnimationStore,TransportAnimation);
implementDBCStore(NameGenStore, NameGenEntry);
implementIndexedDBCStore(AuctionHouseStore,AuctionHouseDBC);
implementDBCStore(TalentStore, TalentEntry);
implementIndexedDBCStore(CreatureSpellDataStore, CreatureSpellDataEntry);
implementIndexedDBCStore(CreatureFamilyStore, CreatureFamilyEntry);
implementIndexedDBCStore(CharClassStore, CharClassEntry);
implementIndexedDBCStore(CharRaceStore, CharRaceEntry);
implementIndexedDBCStore(MapStore, MapEntry);
implementIndexedDBCStore(ItemExtendedCostStore,ItemExtendedCostEntry);

// When loading this DBC, just write new SpellStore("spell.dbc");

// When loading this DBC, just write new SpellStore("spell.dbc");
