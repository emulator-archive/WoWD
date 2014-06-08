// Copyright (C) 2004 WoW Daemon
// Copyright (C) 2005 Oxide
// Copyright (C) 2005 DK
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include "StdAfx.h"

pSpellAura SpellAuraHandler[TOTAL_SPELL_AURAS]={
        &Aura::SpellAuraNULL,//SPELL_AURA_NONE = 0
        &Aura::SpellAuraBindSight,//SPELL_AURA_BIND_SIGHT = 1
        &Aura::SpellAuraModPossess,//SPELL_AURA_MOD_POSSESS = 2,
        &Aura::SpellAuraPeriodicDamage,//SPELL_AURA_PERIODIC_DAMAGE = 3,
        &Aura::SpellAuraDummy,//SPELL_AURA_DUMMY = 4,
        &Aura::SpellAuraModConfuse,//SPELL_AURA_MOD_CONFUSE = 5,
        &Aura::SpellAuraModCharm,//SPELL_AURA_MOD_CHARM = 6,
        &Aura::SpellAuraModFear,//SPELL_AURA_MOD_FEAR = 7,
        &Aura::SpellAuraPeriodicHeal,//SPELL_AURA_PERIODIC_HEAL = 8,
        &Aura::SpellAuraModAttackSpeed,//SPELL_AURA_MOD_ATTACKSPEED = 9,
        &Aura::SpellAuraModThreatGenerated,//SPELL_AURA_MOD_THREAT = 10,
        &Aura::SpellAuraModTaunt,//SPELL_AURA_MOD_TAUNT = 11,
        &Aura::SpellAuraModStun,//SPELL_AURA_MOD_STUN = 12,
        &Aura::SpellAuraModDamageDone,//SPELL_AURA_MOD_DAMAGE_DONE = 13,
        &Aura::SpellAuraModDamageTaken,//SPELL_AURA_MOD_DAMAGE_TAKEN = 14,
        &Aura::SpellAuraDamageShield,//SPELL_AURA_DAMAGE_SHIELD = 15,
        &Aura::SpellAuraModStealth,//SPELL_AURA_MOD_STEALTH = 16,
        &Aura::SpellAuraModDetect,//SPELL_AURA_MOD_DETECT = 17,
        &Aura::SpellAuraModInvisibility,//SPELL_AURA_MOD_INVISIBILITY = 18,
        &Aura::SpellAuraModInvisibilityDetection,//SPELL_AURA_MOD_INVISIBILITY_DETECTION = 19,
        &Aura::SpellAuraModTotalHealthRegenPct,// SPELL_AURA_MOD_TOTAL_HEALTH_REGEN_PCT = 20
        &Aura::SpellAuraModTotalManaRegenPct,// SPELL_AURA_MOD_TOTAL_MANA_REGEN_PCT = 21
        &Aura::SpellAuraModResistance,//SPELL_AURA_MOD_RESISTANCE = 22,
        &Aura::SpellAuraPeriodicTriggerSpell,//SPELL_AURA_PERIODIC_TRIGGER_SPELL = 23,
        &Aura::SpellAuraPeriodicEnergize,//SPELL_AURA_PERIODIC_ENERGIZE = 24,
        &Aura::SpellAuraModPacify,//SPELL_AURA_MOD_PACIFY = 25,
        &Aura::SpellAuraModRoot,//SPELL_AURA_MOD_ROOT = 26,
        &Aura::SpellAuraModSilence,//SPELL_AURA_MOD_SILENCE = 27,
        &Aura::SpellAuraReflectSpells,//SPELL_AURA_REFLECT_SPELLS = 28,
        &Aura::SpellAuraModStat,//SPELL_AURA_MOD_STAT = 29,
        &Aura::SpellAuraModSkill,//SPELL_AURA_MOD_SKILL = 30,
        &Aura::SpellAuraModIncreaseSpeed,//SPELL_AURA_MOD_INCREASE_SPEED = 31,
        &Aura::SpellAuraModIncreaseMountedSpeed,//SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED = 32,
        &Aura::SpellAuraModDecreaseSpeed,//SPELL_AURA_MOD_DECREASE_SPEED = 33,
        &Aura::SpellAuraModIncreaseHealth,//SPELL_AURA_MOD_INCREASE_HEALTH = 34,
        &Aura::SpellAuraModIncreaseEnergy,//SPELL_AURA_MOD_INCREASE_ENERGY = 35,
        &Aura::SpellAuraModShapeshift,//SPELL_AURA_MOD_SHAPESHIFT = 36,
        &Aura::SpellAuraModEffectImmunity,//SPELL_AURA_EFFECT_IMMUNITY = 37,
        &Aura::SpellAuraModStateImmunity,//SPELL_AURA_STATE_IMMUNITY = 38,
        &Aura::SpellAuraModSchoolImmunity,//SPELL_AURA_SCHOOL_IMMUNITY = 39,
        &Aura::SpellAuraModDmgImmunity,//SPELL_AURA_DAMAGE_IMMUNITY = 40,
        &Aura::SpellAuraModDispelImmunity,//SPELL_AURA_DISPEL_IMMUNITY = 41,
        &Aura::SpellAuraProcTriggerSpell,//SPELL_AURA_PROC_TRIGGER_SPELL = 42,
        &Aura::SpellAuraProcTriggerDamage,//SPELL_AURA_PROC_TRIGGER_DAMAGE = 43,
        &Aura::SpellAuraTrackCreatures,//SPELL_AURA_TRACK_CREATURES = 44,
        &Aura::SpellAuraTrackResources,//SPELL_AURA_TRACK_RESOURCES = 45,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_PARRY_SKILL = 46, obsolete? not used in 1.12.1 spell.dbc
        &Aura::SpellAuraModParryPerc,//SPELL_AURA_MOD_PARRY_PERCENT = 47,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_DODGE_SKILL = 48, obsolete?
        &Aura::SpellAuraModDodgePerc,//SPELL_AURA_MOD_DODGE_PERCENT = 49,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_BLOCK_SKILL = 50, obsolete?,
        &Aura::SpellAuraModBlockPerc,//SPELL_AURA_MOD_BLOCK_PERCENT = 51,
        &Aura::SpellAuraModCritPerc,//SPELL_AURA_MOD_CRIT_PERCENT = 52,
        &Aura::SpellAuraPeriodicLeech,//SPELL_AURA_PERIODIC_LEECH = 53,
        &Aura::SpellAuraModHitChance,//SPELL_AURA_MOD_HIT_CHANCE = 54,
        &Aura::SpellAuraModSpellHitChance,//SPELL_AURA_MOD_SPELL_HIT_CHANCE = 55,
        &Aura::SpellAuraTransform,//SPELL_AURA_TRANSFORM = 56,
        &Aura::SpellAuraModSpellCritChance,//SPELL_AURA_MOD_SPELL_CRIT_CHANCE = 57,
        &Aura::SpellAuraIncreaseSwimSpeed,//SPELL_AURA_MOD_INCREASE_SWIM_SPEED = 58,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_DAMAGE_DONE_CREATURE = 59,
        &Aura::SpellAuraPacifySilence,//SPELL_AURA_MOD_PACIFY_SILENCE = 60,
        &Aura::SpellAuraModScale,//SPELL_AURA_MOD_SCALE = 61,
        &Aura::SpellAuraPeriodicHealthFunnel,//SPELL_AURA_PERIODIC_HEALTH_FUNNEL = 62,
        &Aura::SpellAuraNULL,//SPELL_AURA_PERIODIC_MANA_FUNNEL = 63,
        &Aura::SpellAuraPeriodicManaLeech,//SPELL_AURA_PERIODIC_MANA_LEECH = 64,
        &Aura::SpellAuraModCastingSpeed,//SPELL_AURA_MOD_CASTING_SPEED = 65,
        &Aura::SpellAuraFeighDeath,//SPELL_AURA_FEIGN_DEATH = 66,
        &Aura::SpellAuraModDisarm,//SPELL_AURA_MOD_DISARM = 67,
        &Aura::SpellAuraModStalked,//SPELL_AURA_MOD_STALKED = 68,
        &Aura::SpellAuraSchoolAbsorb,//SPELL_AURA_SCHOOL_ABSORB = 69,
        &Aura::SpellAuraNULL,//SPELL_AURA_EXTRA_ATTACKS = 70,
        &Aura::SpellAuraModSpellCritChanceSchool,//SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL = 71,
        &Aura::SpellAuraModPowerCost,//SPELL_AURA_MOD_POWER_COST = 72,
        &Aura::SpellAuraModPowerCostSchool,//SPELL_AURA_MOD_POWER_COST_SCHOOL = 73,
        &Aura::SpellAuraReflectSpellsSchool,//SPELL_AURA_REFLECT_SPELLS_SCHOOL = 74,
        &Aura::SpellAuraModLanguage,//SPELL_AURA_MOD_LANGUAGE = 75,
        &Aura::SpellAuraAddFarSight,//SPELL_AURA_FAR_SIGHT = 76,
        &Aura::SpellAuraMechanicImmunity,//SPELL_AURA_MECHANIC_IMMUNITY = 77,
        &Aura::SpellAuraMounted,//SPELL_AURA_MOUNTED = 78,
        &Aura::SpellAuraModDamagePercDone,//SPELL_AURA_MOD_DAMAGE_PERCENT_DONE = 79,
        &Aura::SpellAuraModPercStat,//SPELL_AURA_MOD_PERCENT_STAT = 80,
        &Aura::SpellAuraSplitDamage,//SPELL_AURA_SPLIT_DAMAGE = 81,
        &Aura::SpellAuraWaterBreathing,//SPELL_AURA_WATER_BREATHING = 82,
        &Aura::SpellAuraModBaseResistance,//SPELL_AURA_MOD_BASE_RESISTANCE = 83,
        &Aura::SpellAuraModRegen,//SPELL_AURA_MOD_REGEN = 84,
        &Aura::SpellAuraModPowerRegen,//SPELL_AURA_MOD_POWER_REGEN = 85,
        &Aura::SpellAuraChannelDeathItem,//SPELL_AURA_CHANNEL_DEATH_ITEM = 86,
        &Aura::SpellAuraModDamagePercTaken,//SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN = 87,
        &Aura::SpellAuraModRegenPercent,//SPELL_AURA_MOD_PERCENT_REGEN = 88,
        &Aura::SpellAuraPeriodicDamagePercent,//SPELL_AURA_PERIODIC_DAMAGE_PERCENT = 89,
        &Aura::SpellAuraModResistChance,//SPELL_AURA_MOD_RESIST_CHANCE = 90,
        &Aura::SpellAuraModDetectRange,//SPELL_AURA_MOD_DETECT_RANGE = 91,
        &Aura::SpellAuraPreventsFleeing,//SPELL_AURA_PREVENTS_FLEEING = 92,
        &Aura::SpellAuraModUnattackable,//SPELL_AURA_MOD_UNATTACKABLE = 93,
        &Aura::SpellAuraInterruptRegen,//SPELL_AURA_INTERRUPT_REGEN = 94,
        &Aura::SpellAuraGhost,//SPELL_AURA_GHOST = 95,
        &Aura::SpellAuraMagnet,//SPELL_AURA_SPELL_MAGNET = 96,
        &Aura::SpellAuraManaShield,//SPELL_AURA_MANA_SHIELD = 97,
        &Aura::SpellAuraSkillTalent,//SPELL_AURA_MOD_SKILL_TALENT = 98,
        &Aura::SpellAuraModAttackPower,//SPELL_AURA_MOD_ATTACK_POWER = 99,
        &Aura::SpellAuraVisible,//SPELL_AURA_AURAS_VISIBLE = 100,
        &Aura::SpellAuraModResistancePCT,//SPELL_AURA_MOD_RESISTANCE_PCT = 101,
        &Aura::SpellAuraModCreatureAttackPower,//SPELL_AURA_MOD_CREATURE_ATTACK_POWER = 102,
        &Aura::SpellAuraModTotalThreat,//SPELL_AURA_MOD_TOTAL_THREAT = 103,
        &Aura::SpellAuraWaterWalk,//SPELL_AURA_WATER_WALK = 104,
        &Aura::SpellAuraFeatherFall,//SPELL_AURA_FEATHER_FALL = 105,
        &Aura::SpellAuraHover,//SPELL_AURA_HOVER = 106,
        &Aura::SpellAuraAddFlatModifier,//SPELL_AURA_ADD_FLAT_MODIFIER = 107,
        &Aura::SpellAuraAddPctMod,//SPELL_AURA_ADD_PCT_MODIFIER = 108,
        &Aura::SpellAuraAddTargetTrigger,//SPELL_AURA_ADD_TARGET_TRIGGER = 109,
        &Aura::SpellAuraModPowerRegPerc,//SPELL_AURA_MOD_POWER_REGEN_PERCENT = 110,
        &Aura::SpellAuraNULL,//SPELL_AURA_ADD_CASTER_HIT_TRIGGER = 111, //obsolete?
        &Aura::SpellAuraNULL,//SPELL_AURA_OVERRIDE_CLASS_SCRIPTS = 112, //obsolete?
        &Aura::SpellAuraModRangedDamageTaken,//SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN = 113,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT = 114,
        &Aura::SpellAuraModHealing,//SPELL_AURA_MOD_HEALING = 115,
        &Aura::SpellAuraNULL,//SPELL_AURA_IGNORE_REGEN_INTERRUPT = 116,//obselete
        &Aura::SpellAuraModMechanicResistance,//SPELL_AURA_MOD_MECHANIC_RESISTANCE = 117,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_HEALING_PCT = 118,
        &Aura::SpellAuraNULL,//SPELL_AURA_SHARE_PET_TRACKING = 119,//obselete
        &Aura::SpellAuraNULL,//SPELL_AURA_UNTRACKABLE = 120,
        &Aura::SpellAuraEmphaty,//SPELL_AURA_EMPATHY = 121,
        &Aura::SpellAuraModOffhandDamagePct,//SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT = 122,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_POWER_COST_PCT = 123,
        &Aura::SpellAuraModRangedAttackPower,//SPELL_AURA_MOD_RANGED_ATTACK_POWER = 124,
        &Aura::SpellAuraModMeleeDamageTaken,//SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN = 125,
        &Aura::SpellAuraModMeleeDamageTakenPct,//SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT = 126,
        &Aura::SpellAuraRAPAttackerBonus,//SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS = 127,
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_POSSESS_PET = 128,
        &Aura::SpellAuraModIncreaseSpeedAlways,//SPELL_AURA_MOD_INCREASE_SPEED_ALWAYS = 129,
        &Aura::SpellAuraModIncreaseMountedSpeed,//SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS = 130,
        &Aura::SpellAuraModCreatureRangedAttackPower,//SPELL_AURA_MOD_CREATURE_RANGED_ATTACK_POWER = 131,
        &Aura::SpellAuraModIncreaseEnergyPerc,//SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT = 132,
        &Aura::SpellAuraModIncreaseHealthPerc,//SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT = 133,
        &Aura::SpellAuraModManaRegInterrupt,//SPELL_AURA_MOD_MANA_REGEN_INTERRUPT = 134,
        &Aura::SpellAuraModHealingDone,//SPELL_AURA_MOD_HEALING_DONE = 135,
        &Aura::SpellAuraModHealingDonePct,//SPELL_AURA_MOD_HEALING_DONE_PERCENT = 136,
        &Aura::SpellAuraModTotalStatPerc,//SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE = 137,
        &Aura::SpellAuraModHaste,//SPELL_AURA_MOD_HASTE = 138,
        &Aura::SpellAuraForceReaction,//SPELL_AURA_FORCE_REACTION = 139,
        &Aura::SpellAuraModRangedHaste,//SPELL_AURA_MOD_RANGED_HASTE = 140,
        &Aura::SpellAuraModRangedAmmoHaste,//SPELL_AURA_MOD_RANGED_AMMO_HASTE = 141,
        &Aura::SpellAuraModBaseResistancePerc,//SPELL_AURA_MOD_BASE_RESISTANCE_PCT = 142,
        &Aura::SpellAuraModResistanceExclusive,//SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE = 143,
        &Aura::SpellAuraSafeFall,//SPELL_AURA_SAFE_FALL = 144,
        &Aura::SpellAuraNULL,//SPELL_AURA_CHARISMA = 145,
        &Aura::SpellAuraNULL,//SPELL_AURA_PERSUADED = 146,// obselete
        &Aura::SpellAuraNULL,//SPELL_AURA_ADD_CREATURE_IMMUNITY = 147,
        &Aura::SpellAuraRetainComboPoints,//SPELL_AURA_RETAIN_COMBO_POINTS = 148,
        &Aura::SpellAuraNULL,//SPELL_AURA_RESIST_PUSHBACK = 149,//    Resist Pushback
        &Aura::SpellAuraModShieldBlockPCT,//SPELL_AURA_MOD_SHIELD_BLOCK_PCT = 150,//    Mod Shield Block %
        &Aura::SpellAuraTrackStealthed,//SPELL_AURA_TRACK_STEALTHED = 151,//    Track Stealthed
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_DETECTED_RANGE = 152,//    Mod Detected Range
        &Aura::SpellAuraNULL,//SPELL_AURA_SPLIT_DAMAGE_FLAT= 153,//    Split Damage Flat
        &Aura::SpellAuraModStealthLevel,//SPELL_AURA_MOD_STEALTH_LEVEL = 154,//    Stealth Level Modifier
        &Aura::SpellAuraNULL,//SPELL_AURA_MOD_WATER_BREATHING = 155,//    Mod Water Breathing
        &Aura::SpellAuraModReputationAdjust,//SPELL_AURA_MOD_REPUTATION_ADJUST = 156,//    Mod Reputation Gain
        &Aura::SpellAuraNULL,//SPELL_AURA_PET_DAMAGE_MULTI = 157,//    Mod Pet Damage
        &Aura::SpellAuraNULL,//missing = 158 //used Apply Aura: Mod Shield Block //http://www.thottbot.com/?sp=25036
        &Aura::SpellAuraNULL,//missing = 159 //used Apply Aura: No PVP Credit http://www.thottbot.com/?sp=2479
        &Aura::SpellAuraNULL,//missing = 160 //Apply Aura: Mod Side/Rear PBAE Damage Taken %//used http://www.thottbot.com/?sp=23198
        &Aura::SpellAuraModHealthRegInCombat,//SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT = 161,
        &Aura::SpellAuraNULL,//missing = 162 //used //Apply Aura: Power Burn (Mana) //http://www.thottbot.com/?sp=19659
        &Aura::SpellAuraNULL,//missing = 163 //Apply Aura: Mod Crit Damage Bonus (Physical)
        &Aura::SpellAuraNULL,//missing = 164 //used //test spell
        &Aura::SpellAuraNULL,//missing = 165
        &Aura::SpellAuraNULL,//missing = 166 //used //Apply Aura: Mod Attack Power % // http://www.thottbot.com/?sp=30803
        &Aura::SpellAuraNULL,//missing = 167
        &Aura::SpellAuraNULL,//missing = 168 //used //Apply Aura: Increase Damage % *type* //http://www.thottbot.com/?sp=24991
        &Aura::SpellAuraNULL,//missing = 169 //used //Apply Aura: Increase Critical % *type* //http://www.thottbot.com/?sp=24293
        &Aura::SpellAuraNULL,//missing = 170 //used //Apply Aura: Detect Amore //http://www.thottbot.com/?sp=26802
        &Aura::SpellAuraNULL,//missing = 171
        &Aura::SpellAuraNULL,//missing = 172 //used //Apply Aura: Increase Movement and Mounted Speed (Non-Stacking) //http://www.thottbot.com/?sp=26022 2e effect
        &Aura::SpellAuraNULL,//missing = 173 // Apply Aura: Allow Champion Spells
        &Aura::SpellAuraNULL,//missing = 174 //used //Apply Aura: Increase Spell Damage by % Spirit (Spells) //http://www.thottbot.com/?sp=15031
        &Aura::SpellAuraNULL,//missing = 175 //used //Apply Aura: Increase Spell Healing by % Spirit //http://www.thottbot.com/?sp=15031
        &Aura::SpellAuraNULL,//missing = 176 //used // not known
        &Aura::SpellAuraNULL,//missing = 177 //used //Apply Aura: Area Charm // http://www.thottbot.com/?sp=26740
        &Aura::SpellAuraNULL,//missing = 178 //Apply Aura: Increase Debuff Resistance 
        &Aura::SpellAuraNULL,//missing = 179 //used //Apply Aura: Increase Attacker Spell Crit % *type* //http://www.thottbot.com/?sp=12579
        &Aura::SpellAuraNULL,//missing = 180 //used //Apply Aura: Increase Spell Damage *type* //http://www.thottbot.com/?sp=29113
        &Aura::SpellAuraNULL,//missing = 181
        &Aura::SpellAuraIncreaseArmorByPctInt,//missing = 182 //used //Apply Aura: Increase Armor by % of Intellect //http://www.thottbot.com/?sp=28574  SPELL_AURA_INC_ARMOR_BY_PCT_INT
        &Aura::SpellAuraNULL,//missing = 183 //used //Apply Aura: Decrease Critical Threat by % (Spells) //http://www.thottbot.com/?sp=28746
        &Aura::SpellAuraNULL,//missing = 184
        &Aura::SpellAuraNULL,//missing = 185 //used //Apply Aura: Reduces Attacker Chance to Hit with Ranged //http://www.thottbot.com/?sp=30895
        &Aura::SpellAuraNULL,//missing = 186 //used //Apply Aura: Reduces Attacker Chance to Hit with Spells (Spells) //http://www.thottbot.com/?sp=30895
        &Aura::SpellAuraNULL,//missing = 187 //used //Apply Aura: Reduces Attacker Chance to Crit with Melee (Ranged?) //http://www.thottbot.com/?sp=30893
        &Aura::SpellAuraNULL,//missing = 188 //used //Apply Aura: Reduces Attacker Chance to Crit with Ranged (Melee?) //http://www.thottbot.com/?sp=30893
        &Aura::SpellAuraNULL,//missing = 189 //Apply Aura: Increases Rating
        &Aura::SpellAuraNULL,//missing = 190 //used // Apply Aura: Increases Reputation Gained by % //http://www.thottbot.com/?sp=30754
        &Aura::SpellAuraNULL,//missing = 191 //used // noname //http://www.thottbot.com/?sp=29894
        &Aura::SpellAuraNULL,//192 Apply Aura: Melee Slow %
        &Aura::SpellAuraIncreaseTimeBetweenAttacksPCT,//193 Apply Aura: Increase Time Between Attacks (Melee, Ranged and Spell) by %
        &Aura::SpellAuraIncreaseSpellDamagebyInt,//194 Apply Aura: Increase Spell Damage by % of Intellect (All)
        &Aura::SpellAuraIncreaseHpbyInt,//195 Apply Aura: Increase Healing by % of Intellect
        &Aura::SpellAuraNULL,//196 Apply Aura: Mod All Weapon Skills (6)
        &Aura::SpellAuraNULL,//197 Apply Aura: Reduce Attacker Critical Hit Chance by %
        &Aura::SpellAuraNULL,//198
        &Aura::SpellAuraNULL,//199 Apply Aura: Increases Spell % To Hit (Fire, Nature, Frost)
        &Aura::SpellAuraNULL,//200
        &Aura::SpellAuraNULL,//201 Apply Aura: Cannot be Dodged
        &Aura::SpellAuraNULL,//202
        &Aura::SpellAuraNULL,//203 Apply Aura: Reduces Attacker Critical Hit Damage with Melee by %
        &Aura::SpellAuraNULL,//204 Apply Aura: Reduces Attacker Critical Hit Damage with Ranged by %
        &Aura::SpellAuraNULL,//205
        &Aura::SpellAuraNULL,//206
        &Aura::SpellAuraEnableFlight,//207 set fly
        &Aura::SpellAuraNULL,//208
        &Aura::SpellAuraNULL,//209
        &Aura::SpellAuraNULL,//210
        &Aura::SpellAuraNULL,//211
        &Aura::SpellAuraNULL,//212 Apply Aura: Increase Ranged Atk Power by % of Intellect
        &Aura::SpellAuraIncreaseRageFromDamageDealtPCT //213 Apply Aura: Increase Rage from Damage Dealt by %
};

inline void ApplyFloatSM(float ** m,float v,uint32 mask, float def)
{
    if(*m == 0)
    {
        *m = new float[SPELL_GROUPS];

        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            (*m)[x] = def;
            if((1<<x) & mask)
                (*m)[x]+=v;
        }
    }
    else
    {
        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            if((1<<x) & mask)
                (*m)[x]+=v;
        }
    }
}

inline void ApplyFloatPSM(float ** m,int32 v,uint32 mask, float def)
{
    if(*m == 0)
    {
        *m = new float[SPELL_GROUPS];

        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            (*m)[x] = def;
            if((1<<x) & mask)
                (*m)[x]+=((float)v)/100.0f;
        }
    }
    else
    {
        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            if((1<<x) & mask)
                (*m)[x]+=((float)v)/100.0f;
        }
    }
}

inline void ApplyIntSM(int32 ** m,int v,uint32 mask, int32 def)
{
    if(*m == 0)
    {
        *m = new int32[SPELL_GROUPS];

        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            (*m)[x] = def;
            if((1<<x) & mask)
                (*m)[x]+=v;
        }
    }
    else
    {
        for(uint32 x=0;x<SPELL_GROUPS;x++)
            if((1<<x) & mask)
                (*m)[x]+=v;
    }
}

Unit * Aura::GetUnitCaster()
{
    return objmgr.GetUnit(m_casterGuid);
}

Object * Aura::GetCaster()
{
    //FIXME: add split for creature\playr
    /*Object*p=sObjHolder.GetObject<Creature>(m_casterGuid);
    if(p)return p;
    p= sObjHolder.GetObject<Player>(m_casterGuid);
    if(p)return p;
    return sObjHolder.GetObject<GameObject>(m_casterGuid);*/
    Object *p = NULL;
    switch(UINT32_LOPART(GUID_HIPART(m_casterGuid)))
    {
    case HIGHGUID_PLAYER:
        p = sObjHolder.GetObject<Player>(m_casterGuid);
        break;
    case HIGHGUID_UNIT:
        p = sObjHolder.GetObject<Creature>(m_casterGuid);
        break;
    case HIGHGUID_PET:
        p = sObjHolder.GetObject<Pet>(m_casterGuid);
        break;
    case HIGHGUID_GAMEOBJECT:
        p = sObjHolder.GetObject<GameObject>(m_casterGuid);
        break;
    }
    return p;  
}

Aura::Aura(SpellEntry *proto, int32 duration,Object* caster, Unit *target)
{
    m_spellProto = proto;
    m_duration   = duration;
    
    m_casterGuid = caster->GetGUID();
    m_target = target;

    //SetCasterFaction(caster->_getFaction());

//    m_auraSlot = 0;
    m_modcount = 0;
    m_dynamicValue = 0;
    m_areaAura = false;

    if(caster->IsUnit())
    {
        if(isAttackable((Unit*)caster,target))
        {        
            SetNegative();
            /*if(caster->IsPlayer())
            {
                ((Player*)caster)->CombatModeDelay = COMBAT_DECAY_TIME;
            }
            else if(caster->IsPet())
            {
                Player*p=((Pet*)caster)->GetPetOwner();
                if(p)
                    p->CombatModeDelay = COMBAT_DECAY_TIME;
            }*/
        }
        else 
            SetPositive();
    }

    if(!IsPassive())
    {
        timeleft=time(NULL);
    }

    m_castedItemId = 0;
    m_visualSlot = 0xFF;
}

void Aura::Remove()
{
    sEventMgr.RemoveEvents(this);
    if(!IsPassive())
        RemoveAuraVisual();
    ApplyModifiers(false);

    // reset diminishing return timer if needed
    ::UnapplyDiminishingReturnTimer(m_target, m_spellProto);
    
    for(uint32 x=0;x<3;x++)
    {
        if(!m_spellProto->Effect[x])
            break;
        if(m_spellProto->Effect[x]==SPELL_EFFECT_TRIGGER_SPELL)
        {
            //if(GetSpellProto()->EffectTriggerSpell[x]!=GetSpellId())
            m_target->RemoveAura(GetSpellProto()->EffectTriggerSpell[x]);
        }
        else if(m_spellProto->Effect[x] == SPELL_EFFECT_APPLY_AREA_AURA 
            && m_casterGuid == m_target->GetGUID())
        {
            RemoveAA();
        }
    }
	std::map<uint32,struct SpellCharge>::iterator iter;
	iter = m_target->m_chargeSpells.find(GetSpellId());
	if(iter!=m_target->m_chargeSpells.end())
	{
		if(!(iter->second).FromProc)
		m_target->m_chargeSpells.erase(iter);
	}
    m_target->m_auras[m_auraSlot] = NULL;

    if(GetSpellProto()->SpellGroupType && m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 speedmod=0;
        SM_FIValue(m_target->SM_FSpeedMod,&speedmod,m_spellProto->SpellGroupType);
        if(speedmod)
        {
            m_target->m_speedModifier-=speedmod;
            m_target->UpdateSpeed();
        }
    }

    // remove attacker
    Unit * caster = GetUnitCaster();
    if(caster)
        m_target->removeAttacker(caster);
        


    /***Capt:*******************Cooldown**************************
    /* this is only needed for some spells
    /* for now only spells that have:
    /* (m_spellInfo->Attributes == 0x2050000) && !(m_spellInfo->AttributesEx)
    /* are handled. Its possible there are more spells like this
    *************************************************************/
    if ( caster && caster->IsPlayer() && caster->IsInWorld() && (m_spellProto->Attributes&ATTRIBUTES_UNK27) && !m_spellProto->AttributesEx)
    {
        WorldPacket data(12);
        data.SetOpcode(SMSG_COOLDOWN_EVENT);
        data << m_spellProto->Id << caster->GetGUID();
        caster->SendMessageToSet(&data, true);
    }
        
    delete this; // suicide xD    leaking this shit out
}

void Aura::AddMod(uint32 t, int32 a,uint32 miscValue,uint32 i)
{
    //this is fix, when u have the same unit in target list several times
    //for(uint32 x=0;x<m_modcount;x++)
    //    if(m_modList[x].i==i)return;

    /*if(m_modList[0].m_type == t ||
        m_modList[1].m_type == t ||
        m_modList[2].m_type == t)
        return; // dont duplicate mods // some spells apply duplicate mods, like some seals*/ 

    if(m_modcount >= 3)
    {
        sLog.outString("Tried to add >3 (%u) mods to spellid %u [%u:%u, %u:%u, %u:%u]", m_modcount+1, this->m_spellProto->Id, m_modList[0].m_type, m_modList[0].m_amount, m_modList[1].m_type, m_modList[1].m_amount, m_modList[2].m_type, m_modList[2].m_amount);
        return;
    }
    m_modList[m_modcount].m_type = t;
    m_modList[m_modcount].m_amount = a;
    m_modList[m_modcount].m_miscValue = miscValue;
    m_modList[m_modcount].i = i;
    m_modcount++;
    //ASSERT(m_modcount<=3);
}

void Aura::ApplyModifiers(bool apply)
{ 
    
    for (uint32 x=0;x<m_modcount; x++)
    {
        mod = &m_modList[x];
        sLog.outDebug( "WORLD: target = %u , Spell Aura id = %u, SpellId  = %u, i = %u, apply = %s",
            m_target->GetGUIDLow(),mod->m_type, m_spellProto->Id, mod->i, apply ? "true" : "false"); 

        if(m_target->SchoolImmunityList[m_spellProto->School] &&
            m_target->GetGUID() != m_casterGuid)    // make sure that we dont block self spells
        {
            // hack fix for bubble :p
            switch(mod->m_type)
            {
            case SPELL_AURA_MOD_STUN:
            case SPELL_AURA_MOD_FEAR:
            case SPELL_AURA_MOD_ROOT:
            case SPELL_AURA_MOD_CHARM:
            case SPELL_AURA_MOD_CONFUSE:
                continue;
                break;
            }
        }

        if(mod->m_type<TOTAL_SPELL_AURAS)
              (*this.*SpellAuraHandler[mod->m_type])(apply);
        else
            sLog.outError("Unknown Aura id %d", (uint32)mod->m_type);
    }
    
    if(GetSpellProto()->procFlags)
    {
        for(uint32 x=0; x<3; x++)
            if(GetSpellProto()->EffectApplyAuraName[x] == SPELL_AURA_PROC_TRIGGER_SPELL||GetSpellId()==974||GetSpellId()==32593||GetSpellId()==32594)
                return;//already have proc for this aura

        if(apply)
        {
            ProcTriggerSpell pts;
            pts.origId = GetSpellId();
            pts.caster = m_casterGuid;
            pts.spellId = 0;
            for(uint32 x=0; x<3; x++)
            {
                if(GetSpellProto()->EffectTriggerSpell[x] != 0)
                {
                    pts.spellId = GetSpellProto()->EffectTriggerSpell[x];
                    break;
                }
            }
            if(!pts.spellId)
                return;
        
            pts.procChance = GetSpellProto()->procChance;
            pts.procFlags = GetSpellProto()->procFlags;
            pts.procCharges = GetSpellProto()->procCharges;
            m_target->m_procSpells.push_front(pts);            
        }
        else
        {
            for(std::list<struct ProcTriggerSpell>::iterator itr = m_target->m_procSpells.begin();itr != m_target->m_procSpells.end();itr++)
            {
                if(itr->origId == GetSpellId() && itr->caster == m_casterGuid)
                {
                    m_target->m_procSpells.erase(itr);
                    break;
                }
            }        
        }
    }
}

void Aura::AddAuraVisual()
{
    uint8 slot, i;
    slot = 0xFF;

    if (m_positive)
    {
        for (i = 0; i < MAX_POSITIVE_AURAS; i++)
        {
            //if (m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURA + i)) == 0)
            if(m_target->m_auras[i] == 0)
            {
                slot = i;
                break;
            }
        }
    }
    else
    {
        for (i = MAX_POSITIVE_AURAS; i < MAX_AURAS; i++)
        {
            //if (m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURA + i)) == 0)
            if(m_target->m_auras[i] == 0)
            {
                slot = i;
                break;
            }
        }
    }

    if (slot == 0xFF)
    {
        return;
    }
    m_visualSlot = m_target->AddAuraVisual(m_spellProto->Id, 1, IsPositive());

    /*m_target->SetUInt32Value(UNIT_FIELD_AURA + slot, m_spellProto->Id);

    uint8 flagslot = slot >> 3;

    uint32 value = m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));

    uint8 aurapos = (slot & 7) << 2;
    uint32 value1 = (uint32)AFLAG_SET << aurapos;
    value |= value1;

    m_target->SetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot), value);
    
    uint32 index = (slot / 8);
    uint32 byte  = (slot % 8);

    uint32 x = 0, y = 0;
    uint32 val = m_target->GetUInt32Value(UNIT_FIELD_AURALEVELS + index);
    if(x != 0)
    {
        val |= (x << (byte * 8));
        m_target->SetUInt32Value(UNIT_FIELD_AURALEVELS + index, val);
    }

    val = m_target->GetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS + index);
    if(y != 0)
    {
        val |= (y << (byte * 8));
        m_target->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS + index, val);
    }
    sLog.outDebug("Adding Aura Visual - target: %d , slot: %d , flagslot: %d , flagsvalue: 0x%.4X",m_target->GetGUID(),slot,flagslot,value);*/
   
    //  0000 0000 original
    //  0000 1001 AFLAG_SET
    //  1111 1111 0xFF

    //uint8 appslot = slot >> 1;

    if( m_target->IsPlayer())
    {
        WorldPacket data(SMSG_UPDATE_AURA_DURATION, 5);
        data << m_visualSlot << (uint32)m_duration;
        ((Player*)m_target)->GetSession()->SendPacket(&data);
    }

    m_auraSlot = slot;
}

void Aura::RemoveAuraVisual()
{
    m_target->RemoveAuraVisual(m_spellProto->Id, 1);

    //UNIT_FIELD_AURAFLAGS 0-7;UNIT_FIELD_AURAFLAGS+1 8-15;UNIT_FIELD_AURAFLAGS+2 16-23 ... For each Aura 1 Byte

    /*if(m_auraSlot>=MAX_AURAS)
        return;
    uint8 slot = m_auraSlot;

    m_target->SetUInt32Value((uint16)(UNIT_FIELD_AURA + slot), 0);

    uint8 flagslot = slot >> 3;

    uint32 value = m_target->GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));
    
    uint8 aurapos = (slot & 7) << 2;
     
    uint32 value1 = ~( (uint32)AFLAG_SET << aurapos );
    
    value &= value1;
   
    m_target->SetUInt32Value(UNIT_FIELD_AURAFLAGS + flagslot,value);
    sLog.outDebug("Removing Aura Visual - target: %d , slot: %d , flagslot: %d , flagsvalue: 0x%.4X",m_target->GetGUID(),slot,flagslot,value); */
}

void Aura::EventUpdateAA(float r)
{
    Unit * u_caster = GetUnitCaster();

    // if the caster is no longer valid->remove the aura
    if(u_caster == 0)
        Remove();

    Player * plr = 0;

    if(u_caster->GetTypeId() == TYPEID_PLAYER)
        plr = static_cast<Player*>(u_caster);
    else if(u_caster->GetTypeId() == TYPEID_UNIT)
        plr = static_cast<Player*>(static_cast<Creature*>(u_caster)->GetTotemOwner());

    if(plr == 0 || plr->GetTypeId() != TYPEID_PLAYER)    // No player involved...
        return;

    vector<uint64> NewTargets;

    // Add the aura to the caster, if he's in range of course.
    if(plr->GetDistanceSq(u_caster) < r)
    {
        if(!plr->HasActiveAura(m_spellProto->Id))
        {
            Aura * aura = new Aura(m_spellProto, -1, u_caster, plr);
            aura->m_areaAura = true;
            aura->AddMod(mod->m_type, mod->m_amount, mod->m_miscValue, mod->i);
            plr->AddAura(aura);
            NewTargets.push_back(plr->GetGUID());
        }
    }

    SubGroup * group = plr->GetSubGroup();
    if(group)
    {
        GroupMembersSet::iterator itr = group->GetGroupMembersBegin();
        for(; itr != group->GetGroupMembersEnd(); ++itr)
        {
            if((*itr) != plr && (*itr)->GetDistanceSq(u_caster) <= r)
            {
                // Add the aura if they don't have it.
                if(!(*itr)->HasActiveAura(m_spellProto->Id))
                {
                    Aura * aura = new Aura(m_spellProto, -1, u_caster, (*itr));
                    aura->m_areaAura = true;
                    aura->AddMod(mod->m_type, mod->m_amount, mod->m_miscValue, mod->i);
                    (*itr)->AddAura(aura);
                    NewTargets.push_back((*itr)->GetGUID());
                }
            }
        }
    }

    // Update the existing players in the target set.
    AreaAuraList::iterator itr, it2;

    for(itr = targets.begin(); itr != targets.end(); )
    {
        it2 = itr;
        ++itr;

        // Check if the target is 'valid'.
        Player * iplr = World::GetPlayer((*it2));
        if(!iplr || iplr->GetDistanceSq(u_caster) > r)
        {
            targets.erase(it2);
            if(iplr)
                iplr->RemoveAura(m_spellProto->Id);
            
            continue;
        }

        // Is the player outside of the group now?
        // (doesn't apply to the caster)
        if(plr == iplr) continue;

        if(!group || !group->HasMember(iplr->GetGUID()))
        {
            iplr->RemoveAura(m_spellProto->Id);
            targets.erase(it2);
        }
    }

    // Push new targets into the set.
    for(vector<uint64>::iterator vtr = NewTargets.begin(); vtr != NewTargets.end(); ++vtr)
        targets.insert((*vtr));

    NewTargets.clear();
}

void Aura::RemoveAA()
{
    AreaAuraList::iterator itr;
    Unit * caster = GetUnitCaster();

    for(itr = targets.begin(); itr != targets.end(); ++itr)
    {
        // Check if the target is 'valid'.
        Player * iplr = World::GetPlayer((*itr));
        if(!iplr || iplr == caster)
            continue;
        
        iplr->RemoveAura(m_spellProto->Id);
    }
    targets.clear();
}

//------------------------- Aura Effects -----------------------------

void Aura::SpellAuraModBaseResistance(bool apply)
{
    SpellAuraModResistance(apply);
    //both add/decrease some resistance difference is unknown
}

void Aura::SpellAuraModBaseResistancePerc(bool apply)
{
    uint32 Flag = mod->m_miscValue; 
    int32 amt;
    if(apply)
    {
        amt = mod->m_amount;
        if(amt > 0)
            SetPositive();
        else 
            SetNegative();
    }
    else
        amt =- mod->m_amount;

    for(uint32 x=0;x<7;x++)
    {
        if(Flag & (((uint32)1)<< x))
        {
            if(m_target->GetTypeId()==TYPEID_PLAYER)
            {
                if(mod->m_amount>0)
                {
                    ((Player*)m_target)->BaseResistanceModPctPos[x]+=amt;
                }
                else
                {
                    ((Player*)m_target)->BaseResistanceModPctNeg[x]-=amt;
                }
                ((Player*)m_target)->CalcResistance(x);
              
            }else
            {
                ((Creature*)m_target)->BaseResistanceModPct[x]+=amt;
                ((Creature*)m_target)->CalcResistance(x);
            }
        }
    }
}

void Aura::SpellAuraNULL(bool apply)
{
}

void Aura::SpellAuraBindSight(bool apply)
{
    SetPositive();
    // MindVision
    Unit *caster = GetUnitCaster();
    if(!caster || !m_target) return;
    
    caster->SetUInt64Value(PLAYER_FARSIGHT, (apply ? m_target->GetGUID() : 0));
}

void Aura::SpellAuraModPossess(bool apply)
{
    Unit *caster = GetUnitCaster();
    if(!caster)
        return;

    if(caster->GetTypeId() != TYPEID_PLAYER) 
        return;

    SetPositive();
    Creature *target = static_cast<Creature*>(m_target);
    
    // check for RC spell
    if(m_spellProto->Id == 24937)
    {
        if(apply)
        {
            // spawn rc
            target = sObjHolder.Create<Creature>();
            target->SetInstanceID(caster->GetInstanceID());
            target->Create("RC Tank", caster->GetMapId(), caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), caster->GetOrientation());
            target->SetUInt32Value(OBJECT_FIELD_ENTRY, 15328);
            target->SetUInt32Value(OBJECT_FIELD_TYPE,  9);
            target->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
            target->SetUInt32Value(UNIT_FIELD_HEALTH, 100);
            target->SetUInt32Value(UNIT_FIELD_POWER1, 2434);
            target->SetUInt32Value(UNIT_FIELD_POWER3, 100);
            target->SetUInt32Value(UNIT_FIELD_POWER5, 1000000);
            target->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 100);
            target->SetUInt32Value(UNIT_FIELD_MAXPOWER1, 2134);
            target->SetUInt32Value(UNIT_FIELD_MAXPOWER3, 100);
            target->SetUInt32Value(UNIT_FIELD_MAXPOWER5, 1000000);
            target->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
            target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 1);      // 1616
            target->SetUInt32Value(UNIT_FIELD_BYTES_0, 0 | ((0x02 << 8) | (0x02 << 16)));
            target->SetUInt32Value(UNIT_FIELD_FLAGS, 0x01080008);
            target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
            target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
            target->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.27f);
            target->SetFloatValue(UNIT_FIELD_COMBATREACH, 5.0f);
            target->SetUInt32Value(UNIT_FIELD_DISPLAYID, 15381);
            target->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 15381);
            // bytes_2: 01 00 00 00

            target->m_faction = caster->m_faction;
            target->m_factionDBC = caster->m_factionDBC;

            target->AddToWorld();
        } 
        else 
        {
            target = sObjHolder.GetObject<Creature>(caster->GetUInt64Value(UNIT_FIELD_CHARM));
            if(!target) return;
        }        
    }
    WorldPacket data1;
    target->GetAIInterface()->StopMovement(5000);
    target->m_useAI = !apply;
    //target->GetAIInterface()->m_canMove = !apply;

    data1.Initialize(SMSG_DEATH_NOTIFY_OBSOLETE);
    data1 << target->GetNewGUID() << uint8(0x01);
    ((Player*)caster)->GetSession()->SendPacket(&data1);

    if(apply)
    {
        caster->SetCharmTempVal(caster->m_noInterrupt);
        ((Player*)caster)->m_CurrentCharm = target;
        caster->m_noInterrupt = 1;
        caster->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER);

        caster->SetUInt64Value(UNIT_FIELD_CHARM, target->GetGUID());
        caster->SetUInt64Value(PLAYER_FARSIGHT, target->GetGUID());

        target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, caster->GetGUID());
        target->SetCharmTempVal(target->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
        target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, caster->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
        target->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PLAYER_CONTROLLED_CREATURE);

        if(target->GetTypeId() == TYPEID_PLAYER)
        {
            uint32 fl = target->GetUInt32Value(UNIT_FIELD_FLAGS);
            fl |= U_FIELD_FLAG_PLAYER_CONTROLLED_CREATURE;

            target->BuildFieldUpdatePacket(((Player*)(target)), UNIT_FIELD_FLAGS, fl);
        }

        target->_setFaction();  // update faction pointers
        target->UpdateOppFactionSet();

        WorldPacket data;
        data.SetOpcode(SMSG_PET_SPELLS);
        data << uint64(target->GetGUID());
        data << uint32(0) << uint32(0x0101);
        data << uint32(0x07000002) << uint64(0);
        data << uint32(0x02006165) << uint32(0x030061AB);
        data << uint32(0x040061C3) << uint32(0x05006C62);
        data << uint64(0) << uint32(0) << uint32(0x6C650200) << uint32(0);
        data << uint32(0) << uint32(0x61680000) << uint64(0) << uint16(0);
        ((Player*)caster)->GetSession()->SendPacket(&data);
    } 
    else 
    {
        caster->SetUInt64Value(PLAYER_FARSIGHT, 0);
        caster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
        ((Player*)caster)->m_CurrentCharm = NULL;
        sEventMgr.RemoveEvents(caster, EVENT_PLAYER_CHARM_ATTACK);

        caster->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
        caster->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER);
        target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);

        target->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PLAYER_CONTROLLED_CREATURE);
        target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, target->GetCharmTempVal());
        caster->m_noInterrupt = caster->GetCharmTempVal();

        if(target->GetTypeId() == TYPEID_PLAYER)
        {
            target->BuildFieldUpdatePacket(((Player*)target), UNIT_FIELD_FLAGS, target->GetUInt32Value(UNIT_FIELD_FLAGS));
        }

        caster->SetCharmTempVal(0);
        target->_setFaction();  // update faction pointers

        WorldPacket data;
        data.SetOpcode(SMSG_PET_SPELLS);
        data << uint64(0) << uint64(0);
        ((Player*)caster)->GetSession()->SendPacket(&data);

        //AI job
        if(target->GetAIInterface()->GetNextTarget())
        {
            target->GetAIInterface()->GetNextTarget()->WipeTargetList();
            target->GetAIInterface()->GetNextTarget()->GetAIInterface()->AttackReaction(caster, 1000, 0);
        }
        target->GetAIInterface()->WipeTargetList();
        target->GetAIInterface()->AttackReaction(caster, 1000, 0);
    }

    if(m_spellProto->Id == 24937 && !apply)
    {
        target->RemoveFromWorld();
        sEventMgr.AddEvent(((Creature*)target), &Creature::SafeDelete, EVENT_CREATURE_SAFE_DELETE, 1, 1);
    }
}

void Aura::SpellAuraPeriodicDamage(bool apply)
{
    if(apply)
    {
        switch(m_spellProto->Id)
        {
        case 703:
        case 8631:
        case 8632:
        case 8633:
        case 8818:
        case 11289:
        case 11290:
            Unit *c = GetUnitCaster();
            if(c)
                c->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);  // remove stealth
            break;
        };
        uint32 dmg = mod->m_amount;
        uint32 gr = GetSpellProto()->SpellGroupType;
        if(gr)
        {
            Unit*c=GetUnitCaster();
            if(c)
            {
                SM_FIValue(c->SM_FDOT,(int32*)&dmg,gr);
                SM_PIValue(c->SM_PDOT,(int32*)&dmg,gr);
            }        
        }

        sEventMgr.AddEvent(this, &Aura::EventPeriodicDamage,dmg, 
            EVENT_AURA_PERIODIC_DAMAGE,GetSpellProto()->EffectAmplitude[mod->i],0);
        SetNegative();
    }
}

void Aura::EventPeriodicDamage(uint32 amount)
{
    //DOT
    if(!m_target->isAlive())
        return;
    if(m_target->SchoolImmunityList[GetSpellProto()->School])
        return;

    uint32 damage = amount;

    uint32 school = GetSpellProto()->School;
    Unit * c = GetUnitCaster();
/*    if (c)
        SendPeriodicAuraLog(c, m_target, GetSpellProto()->Id, school, damage, FLAG_PERIODIC_DAMAGE);
    else
        SendPeriodicAuraLog(m_casterGuid, m_target, GetSpellProto()->Id, school, damage, FLAG_PERIODIC_DAMAGE);*/

    if(school == SHADOW_DAMAGE)
    {
        if(m_casterGuid == m_target->VampEmbCaster)
        {
            if(c)
                c->VampiricEmbrace(damage,m_target);
        }
    }

    if(c)
        c->SpellNonMeleeDamageLog(m_target, GetSpellProto()->Id, damage, true);
    else
        m_target->SpellNonMeleeDamageLog(m_target, GetSpellProto()->Id, damage, true);
}

void Aura::SpellAuraDummy(bool apply)
{
    bool dr = sScriptMgr.CallScriptedDummyAura(GetSpellId(), mod->i, this, apply);

    // for seal -> set judgement crap
    if(GetSpellProto()->buffType & SPELL_TYPE_SEAL && !dr && mod->i == 2)
    {
        Player*c=(Player*)GetUnitCaster();
        if(c == 0) return;
        
        if(apply)
        {
            if(!c->HasFlag(UNIT_FIELD_AURASTATE, 16))
                c->SetFlag(UNIT_FIELD_AURASTATE, 16);

            if(!c->judgespell)
                c->judgespell = mod->m_amount;
            if(!c->Seal)
                c->Seal = m_spellProto->Id;
        }
        else
        {
            if(c->HasFlag(UNIT_FIELD_AURASTATE, 16))
                c->RemoveFlag(UNIT_FIELD_AURASTATE, 16);

            if(c->judgespell)
                c->judgespell = 0;
            if(c->Seal)
                c->Seal = 0;
        }
    }

    switch(GetSpellProto()->Id)
    {
	case 974:
	case 32593:
	case 32594:
		{
			if(apply)
			{
			ProcTriggerSpell pts;
			pts.origId = GetSpellProto()->Id;
			pts.caster = m_casterGuid;
			pts.spellId=GetSpellProto()->Id;
			if(!pts.spellId)
				return;
			pts.procChance = GetSpellProto()->procChance;
			pts.procFlags = GetSpellProto()->procFlags;
			pts.procCharges = GetSpellProto()->procCharges;
			m_target->m_procSpells.push_front(pts);
			}
			else
			{
			for(std::list<struct ProcTriggerSpell>::iterator itr = m_target->m_procSpells.begin();itr != m_target->m_procSpells.end();itr++)
			{
				if(itr->origId == GetSpellId() && itr->caster == m_casterGuid)
				{
					m_target->m_procSpells.erase(itr);
					break;
				}
			}
			}
		}break;
    case 126: //Eye of Killrog
        {
            if(apply)
            {
                m_target->SetUInt64Value(PLAYER_FARSIGHT,m_target->GetUInt64Value(UNIT_FIELD_SUMMON));
                m_target->m_noInterrupt++;                        
            }
            else
            {
                m_target->SetUInt64Value(PLAYER_FARSIGHT,0);
                Creature *summon = objmgr.GetCreature(m_target->GetUInt64Value(UNIT_FIELD_SUMMON));
                if(summon)
                {
                    WorldPacket data;
                    data.Initialize(SMSG_DESTROY_OBJECT);
                    data << summon->GetGUID();
                    summon->SendMessageToSet (&data, false);

                    if(summon->IsInWorld())
                    {
                        summon->RemoveFromWorld();
                    }
                    sObjHolder.Delete(summon);
                }
                m_target->SetUInt64Value(UNIT_FIELD_SUMMON,0);
                m_target->m_noInterrupt--;
                if(m_target->m_noInterrupt < 0)
                    m_target->m_noInterrupt = 0;
            }
        }
    case 2096://MindVision
        {
        }break;
    case 6196://FarSight
        {
            if(apply)
            {
            }
            else
            {
                Creature *summon = objmgr.GetCreature(m_target->GetUInt64Value(PLAYER_FARSIGHT));
                if(summon)
                {
                    WorldPacket data;
                    data.Initialize(SMSG_DESTROY_OBJECT);
                    data << summon->GetGUID();
                    summon->SendMessageToSet (&data, false);

                    if(summon->IsInWorld())
                    {
                        summon->RemoveFromWorld();
                    }
                    sObjHolder.Delete(summon);
                }
                m_target->SetUInt64Value(PLAYER_FARSIGHT,0);
            }
        }break;
    case 15286://Vampiric Embrace
        {
            if(apply)
            {
                SetNegative();
                m_target->VampEmbCaster = this->GetUnitCaster()->GetGUID();
            }
            else
            {
                m_target->VampEmbCaster = 0;
            }
        }break;
    case 20707://Soulstone Resurrecton
        if(apply)
            ((Player*)m_target)->SoulStone = 3026;
        else if(m_target->isAlive())
            ((Player*)m_target)->SoulStone = 0;
        break;
    case 20762:
        if(apply)
            ((Player*)m_target)->SoulStone = 20758;
        else if(m_target->isAlive())
            ((Player*)m_target)->SoulStone = 0;
        break;
    case 20763:
        if(apply)
            ((Player*)m_target)->SoulStone = 20759;
        else if(m_target->isAlive())
            ((Player*)m_target)->SoulStone = 0;
        break;
    case 20764:
        if(apply)
            ((Player*)m_target)->SoulStone = 20760;
        else if(m_target->isAlive())
            ((Player*)m_target)->SoulStone = 0;
        break;
    case 20765:
        if(apply)
            ((Player*)m_target)->SoulStone = 20761;
        else if(m_target->isAlive())
            ((Player*)m_target)->SoulStone = 0;
        break;
    //case 20154://Soulstone Resurrecton
    //case 20287:
    //case 20288:
    //case 20289:
    //case 20290:
    //case 20291:
    //case 20292:
    //case 20293:
    /*case 20165:
    case 20347:
    case 20348:
    case 20349:
    case 20166:
    case 20356:
    case 20357:
    case 20164:
    case 20375:
    case 20915:
    case 20918:
    case 20919:
    case 20920:
    case 21082:
    case 20162:
    case 20305:
    case 20306:
    case 20307:
    case 20308:
        {
            if(mod->i == 2 || mod->i == 0)
            {
                Player*c=(Player*)GetUnitCaster();
                if(apply)
                {
                    c->RemoveAura(c->Seal);
                    c->Seal = GetSpellId();
                    c->judgespell = mod->m_amount;
                    c->SetFlag(UNIT_FIELD_AURASTATE,16);
                }
                else
                {
                    c->Seal = 0;
                    c->RemoveFlag(UNIT_FIELD_AURASTATE,16);
                }
            }
        }break;*/
    case 1002:  // Eyes of the beast
        {
            // Take control of pets vision

            // set charmed by and charm target
            Unit * Caster = GetUnitCaster() ;
            if(Caster == 0 || Caster->GetTypeId() != TYPEID_PLAYER) return;
            Player * pCaster = static_cast<Player*>(Caster);

            if(apply)
            {
                m_target->DisableAI();
                pCaster->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
                m_target->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, 0);
                pCaster->SetUInt64Value(UNIT_FIELD_CHARM, m_target->GetGUID());
                m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, pCaster->GetGUID());
                pCaster->SetUInt64Value(PLAYER_FARSIGHT, m_target->GetGUID());
                pCaster->m_CurrentCharm = ((Creature*)m_target);
                m_target->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PLAYER_CONTROLLED_CREATURE);
                pCaster->m_noInterrupt = 1;
                pCaster->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER);

                WorldPacket data(SMSG_DEATH_NOTIFY_OBSOLETE);
                data << m_target->GetNewGUID() << uint8(1);
                pCaster->GetSession()->SendPacket(&data);
            }
            else
            {
                Caster->EnableAI();
                pCaster->SetUInt64Value(UNIT_FIELD_SUMMON, m_target->GetGUID());
                m_target->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, pCaster->GetGUID());
                pCaster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
                m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);
                pCaster->SetUInt64Value(PLAYER_FARSIGHT, 0);
                pCaster->m_CurrentCharm = 0;
                m_target->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_PLAYER_CONTROLLED_CREATURE);
                pCaster->m_noInterrupt = 0;
                pCaster->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER);

                WorldPacket data(SMSG_DEATH_NOTIFY_OBSOLETE);
                data << pCaster->GetNewGUID() << uint8(1);
                pCaster->GetSession()->SendPacket(&data);
            }
        }break;
    case 570:   // far sight
    case 1345:
    case 6197:
    case 6198:  // eagle eye
    case 24125:
    case 21171:
        {
            if(!apply && m_target->GetTypeId() == TYPEID_PLAYER)
            {
                // reset players vision
                Player * plr = static_cast<Player*>(m_target);
                plr->GetMapMgr()->ChangeFarsightLocation(plr, NULL);

                Creature * farsight = World::GetCreature(plr->GetUInt64Value(PLAYER_FARSIGHT));
                plr->SetUInt64Value(PLAYER_FARSIGHT, 0);
                if(farsight)
                {
                    farsight->RemoveFromWorld();
                    sObjHolder.Delete<Creature>(farsight);
                }
            }
        }break;
    case 33763: // LifeBloom
         {
             if(!apply)
             {
                 uint32 newHealth = m_target->GetUInt32Value(UNIT_FIELD_HEALTH) + (uint32)mod->m_amount;
    
                if(newHealth <= m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH))
                    m_target->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
                else
                    m_target->SetUInt32Value(UNIT_FIELD_HEALTH, m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
             }
         }break;
    }    
}

void Aura::SpellAuraModConfuse(bool apply)
{   
    if(apply)
    {
        SetNegative();
        
        if(m_target->GetTypeId() == TYPEID_PLAYER)
        {
            //m_target->SetFlag(UNIT_FIELD_FLAGS, 0x00040000);
            m_target->SetFlag(UNIT_FIELD_FLAGS, 0x04);
            m_target->setAItoUse(true);
        }
        m_target->GetAIInterface()->HandleEvent(EVENT_WANDER, m_target, 0); 
    }
    else
    {
        if(m_target->GetTypeId() == TYPEID_PLAYER)
        {
            //m_target->RemoveFlag(UNIT_FIELD_FLAGS, 0x00040000);
            m_target->RemoveFlag(UNIT_FIELD_FLAGS, 0x04);
            m_target->GetAIInterface()->StopMovement(1);
            m_target->setAItoUse(false);
        }
        m_target->GetAIInterface()->HandleEvent(EVENT_UNWANDER, m_target, 0);
        Unit*m_caster = GetUnitCaster();
        if(!m_caster)
            return;
        if(m_caster->isAlive())
        {
            m_target->GetAIInterface()->AttackReaction(GetUnitCaster(), 1, 0);
            m_target->GetAIInterface()->HandleEvent(EVENT_ENTERCOMBAT, GetUnitCaster(), 0);
        }
    }
}

void Aura::SpellAuraModCharm(bool apply)
{
    if(m_target->IsPlayer())
        return;
    
    if(!m_target->GetCreatureName())
        return;
        
    Creature *_this = (Creature*)m_target;
    
    Player*caster = (Player*)GetUnitCaster();
  
    if(!caster)
    {
        _this->RemoveEnslave();
        return;
    }
   
//    uint32 enslaveCount = _this->GetEnslaveCount();
   
    //potential bug:
      //more u enslave->harder to do it. but:
        //1) FOR YOU NOT FOR EVERY1 !!!!
        //2) it's never cleared -> after 10 enslavements -> noone ever will be able to enslave it

 /*   if(apply)
    {
        if(caster->GetUInt64Value(UNIT_FIELD_CHARM) || mod->m_amount < m_target->getLevel() )
        {
            _this->RemoveEnslave();
            return;
        }
        if(Rand(enslaveCount*20))//You can enslave 5 times max
        {
            _this->RemoveEnslave();
            return;
        }

        //Player stuff
        m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, caster->GetGUID());
        m_target->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, caster->GetGUID());
        caster->SetUInt64Value(UNIT_FIELD_CHARM, m_target->GetGUID());
        caster->SetUInt64Value(UNIT_FIELD_SUMMON, m_target->GetGUID());
        m_target->SetIsPet(true);

        caster->SetPet(_this);
        caster->SetPetName(m_target->GetCreatureName()->Name);
        //Pet Spells
        WorldPacket data;
        data.Initialize(SMSG_PET_SPELLS);
        data << (uint64)m_target->GetGUID();
        data << uint32(0x00000000);
        data << uint32(0x00001000);
        data << uint32(PET_SPELL_ATTACK);
        data << uint32(PET_SPELL_FOLLOW);
        data << uint32(PET_SPELL_STAY);
        for(int i = 0; i < 4; i++)
            data << uint32(0);
        data << uint32(PET_SPELL_AGRESSIVE);
        data << uint32(PET_SPELL_DEFENSIVE);
        data << uint32(PET_SPELL_PASSIVE);
        caster->GetSession()->SendPacket(&data);

        //Enslave
        _this->SetEnslaveSpell(GetSpellProto()->Id);
        _this->SetEnslaveCount(enslaveCount+1);
        
        //Faction
        m_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, caster->getRace());
        m_target->_setFaction();
        m_target->UpdateOppFactionSet();

        //AI Init
        m_target->GetAIInterface()->Init(m_target,AITYPE_PET,MOVEMENTTYPE_NONE,caster);
        m_target->GetAIInterface()->WipeHateList();
        m_target->GetAIInterface()->WipeTargetList();
    }
    else
    {
        //Player stuff
        m_target->SetUInt64Value(UNIT_FIELD_CHARMEDBY, 0);
        m_target->SetUInt64Value(UNIT_FIELD_SUMMONEDBY, 0);
        caster->SetUInt64Value(UNIT_FIELD_CHARM, 0);
        caster->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
        m_target->SetIsPet(false);
        caster->SetPet(NULL);
        caster->SetPetName("");
        //Pet Spells
        WorldPacket data;
        data.Initialize(SMSG_PET_SPELLS);
        data << uint64(0);
        caster->GetSession()->SendPacket(&data);

        _this->SetEnslaveSpell(0);

        //Faction
        m_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, m_target->GetBaseUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
        m_target->_setFaction();
        m_target->UpdateOppFactionSet();

        m_target->GetAIInterface()->Init(m_target, AITYPE_AGRO, MOVEMENTTYPE_NONE);
    }*/
}

void Aura::SpellAuraModFear(bool apply)
{
    if(apply)
    {
        SetNegative();
        
        if(m_target->GetTypeId() == TYPEID_PLAYER)
        {
            m_target->SetFlag(UNIT_FIELD_FLAGS, 0x00040000);
            m_target->SetFlag(UNIT_FIELD_FLAGS, 0x04);
            m_target->setAItoUse(true);
        }
        m_target->GetAIInterface()->SetUnitToFear(GetUnitCaster());
        m_target->GetAIInterface()->HandleEvent(EVENT_FEAR, m_target, 0); 
    }
    else
    {
        if(m_target->GetTypeId() == TYPEID_PLAYER)
        {
            m_target->RemoveFlag(UNIT_FIELD_FLAGS, 0x00040000);
            m_target->RemoveFlag(UNIT_FIELD_FLAGS, 0x04);
            m_target->GetAIInterface()->StopMovement(1);
            m_target->setAItoUse(false);
        }
        m_target->GetAIInterface()->HandleEvent(EVENT_UNFEAR, m_target, 0);
        m_target->GetAIInterface()->SetUnitToFear(NULL);
        Unit*m_caster = GetUnitCaster();
        if(!m_caster)
            return;
        if(m_caster->isAlive())
        {
            m_target->GetAIInterface()->AttackReaction(GetUnitCaster(), 1, 0);
            m_target->GetAIInterface()->HandleEvent(EVENT_ENTERCOMBAT, GetUnitCaster(), 0);
        }
    }
}

void Aura::SpellAuraPeriodicHeal(bool apply)
{
    if(apply)
    {
        SetPositive();        
        sEventMgr.AddEvent(this, &Aura::EventPeriodicHeal,(uint32)mod->m_amount,
            EVENT_AURA_PERIODIC_HEAL,    GetSpellProto()->EffectAmplitude[mod->i],0);
    }
}

void Aura::EventPeriodicHeal(uint32 amount)
{
    if(!m_target->isAlive())
        return;

    int32 add= amount + m_target->HealTakenMod[GetSpellProto()->School];//that must include 3 more modifiers but let it be so for now
    if(add < 0) add = 0;
    uint32 newHealth = m_target->GetUInt32Value(UNIT_FIELD_HEALTH) + (uint32)add;
    
    if(newHealth <= m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH))
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
    else
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH, m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

    SendPeriodicAuraLog(m_casterGuid, m_target, m_spellProto->Id, m_spellProto->School, add, FLAG_PERIODIC_HEAL);

    if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
    {
        m_target->Emote(EMOTE_ONESHOT_EAT);
    }
}

void Aura::SpellAuraModAttackSpeed(bool apply)
{
    if(mod->m_amount>0)
        SetPositive();
    else 
        SetNegative();
 
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        if(apply)
        {
            ((Player*)m_target)->m_meleeattackspeedmod -= mod->m_amount;
            ((Player*)m_target)->m_rangedattackspeedmod -= mod->m_amount;

        }
        else
        {
            ((Player*)m_target)->m_meleeattackspeedmod += mod->m_amount;
            ((Player*)m_target)->m_rangedattackspeedmod += mod->m_amount;
        }
        uint32 speed;
        Item *weap = ((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
        if(weap)
            speed = weap->GetProto()->Delay;
        else
            speed = 2000;
        m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, (uint32)(speed*(1.0+(float)(((Player*)m_target)->m_meleeattackspeedmod)/100.0)));
 
        
        weap = ((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
        if(weap)
        {
            speed = weap->GetProto()->Delay;
            m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01,(uint32)(speed*(1.0+(float)(((Player*)m_target)->m_meleeattackspeedmod)/100.0)));
        }

        weap=((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
        if(weap)
        {
            speed=weap->GetProto()->Delay;
            m_target->SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME ,(uint32)(speed*(1.0+(float)(((Player*)m_target)->m_rangedattackspeedmod)/100.0)));
        }
        // Capt:
        // There used to be a comment here regarding eventattackstart.
        // The one that wrote that gimme a PM.
    }
    else
    {
        m_target->ModPUInt32Value(UNIT_FIELD_BASEATTACKTIME,mod->m_amount,!apply);
        m_target->ModPUInt32Value(UNIT_FIELD_BASEATTACKTIME_01,mod->m_amount,!apply);
        m_target->ModPUInt32Value(UNIT_FIELD_RANGEDATTACKTIME,mod->m_amount,!apply);
    }
     
}

void Aura::SpellAuraModThreatGenerated(bool apply)
{
    if(!m_target)
        return;

    if(m_target->GetGeneratedThreatModifyer() == mod->m_amount)
    {
        mod->m_amount < 0 ? SetPositive() : SetNegative();
        apply ? m_target->ModGeneratedThreatModifyer(mod->m_amount) : m_target->ModGeneratedThreatModifyer(-(mod->m_amount));
        return;
    }
    else // if we this aura is better then the old one, replace the effect.
    {
        if(apply)
        {
            if(m_target->GetGeneratedThreatModifyer() < mod->m_amount)
            {
                m_target->ModGeneratedThreatModifyer(0);
                m_target->ModGeneratedThreatModifyer(mod->m_amount);
            }
        }
    }
}

void Aura::SpellAuraModTaunt(bool apply)
{
    Unit* m_caster = GetUnitCaster();
    if(!m_caster || !m_caster->isAlive())
        return;

    SetNegative();
    
    if(apply)
    {
        m_target->GetAIInterface()->AttackReaction(m_caster, 1, 0);
        m_target->GetAIInterface()->taunt(m_caster, true);
    }
    else
    {
        if(m_target->GetAIInterface()->getTauntedBy() == m_caster)
        {
            m_target->GetAIInterface()->taunt(m_caster, false);
        }
    }
}

void Aura::SpellAuraModStun(bool apply)
{
    if(!m_target)
        return;

    SetNegative();
    if(apply)
    {
        if(m_target->m_rooted == 0)
            m_target->Root();

        m_target->m_rooted++;
        m_target->m_stunned++;

        if(m_target->GetTypeId() == TYPEID_UNIT)
        {
            // freeze rotation
            m_target->SetUInt64Value(UNIT_FIELD_TARGET, 0);
        }

        // remove the current spell (for channelers)
        if(m_target->m_currentSpell && m_target->GetGUID() != m_casterGuid && 
            m_target->m_currentSpell->getState() == SPELL_STATE_CASTING )
        {
            m_target->m_currentSpell->cancel();
            m_target->m_currentSpell = 0;
        }
    }
    else
    {
        m_target->m_stunned--;
        m_target->m_rooted--;

        if(!m_target->m_rooted)
            m_target->Unroot();

        // attack them back.. we seem to lose this sometimes for some reason
        if(m_target->GetTypeId() == TYPEID_UNIT)
        {
            Unit * target = GetUnitCaster();
            if(m_target->GetAIInterface()->GetNextTarget() != 0)
                target = m_target->GetAIInterface()->GetNextTarget();

            if(!target) return;
            m_target->GetAIInterface()->AttackReaction(target, 1, 1, 0);
        }        
    }


    if(apply)
    {
        switch(this->m_spellProto->Id)
        {
        case 652:
        case 2070:
        case 6770:
        case 6771:
        case 11297:
        case 11298:
            {
                // sap
                Unit *c = GetUnitCaster();
                if(c)
                    c->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);  // remove stealth
            }break;
        case 1776:
        case 1777:
        case 1780:
        case 1781:
        case 8629:
        case 8630:
        case 11285:
        case 11286:
        case 11287:
        case 11288:
        case 12540:
        case 13579:
        case 24698:
        case 28456:
            {
                // gouge
                Unit *c = GetUnitCaster();
                if(c && c->GetTypeId() == TYPEID_PLAYER)
                {
                    //((Player*)c)->CombatModeDelay = 10;
                    ((Player*)c)->EventAttackStop();
                    c->smsg_AttackStop(m_target);
                    c->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);  // remove stealth
                }
            }
        }
    }
}

void Aura::SpellAuraModDamageDone(bool apply)
{
    int32 val;
    if(m_target->IsPlayer())
    {
        uint32 index;
         
        if(mod->m_amount > 0)
        {
            if(apply)
            {
                SetPositive();
                val = mod->m_amount;
            }
            else
            {
                val =- mod->m_amount;
            }
            index = PLAYER_FIELD_MOD_DAMAGE_DONE_POS;
        
        }
        else
        {
            if(apply)
            {
                SetNegative();
                val =- mod->m_amount;
            }
            else
            {
                val = mod->m_amount;
            }
            index = PLAYER_FIELD_MOD_DAMAGE_DONE_NEG;
        }

        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->ModUInt32Value(index + x,val);
            }
        }    
    }
    else
    {    
        if(mod->m_amount > 0)
        {
            if(apply)
            {
                SetPositive();
                val = mod->m_amount;
            }
            else
            {
                val =- mod->m_amount;
            }
        
        }
        else
        {
            if(apply)
            {
                SetNegative();
                val = mod->m_amount;
            }
            else
            {
                val =- mod->m_amount;
            }
        }

        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                ((Creature*)m_target)->ModDamageDone[x]+=val;
            }
        }
    }
   
    if(mod->m_miscValue&1)
      m_target->CalcDamage();
}

void Aura::SpellAuraModDamageTaken(bool apply)
{
    int32 val = 0;
    if(apply)
    {
         val = mod->m_amount;
    /*     if(val>0)
         {
             Unit*m_caster=GetUnitCaster();
             if(m_caster && isAttackable(m_caster,m_target))
             SetNegative();
         }
         else SetPositive();*/
    }
    else
        val = -mod->m_amount;
    
    for(uint32 x=0;x<7;x++)
    {
        if (mod->m_miscValue & (((uint32)1)<<x) )
        {
            m_target->DamageTakenMod[x]+=val;
        }
    }
}

void Aura::SpellAuraDamageShield(bool apply)
{
    if(apply)
    {
        SetPositive();
        DamageShield ds;// = new DamageShield();
      //  ds.m_caster = m_casterGuid;
        ds.m_damage = mod->m_amount;
        ds.m_spellId = GetSpellProto()->Id;
        ds.m_school = GetSpellProto()->School;
        m_target->m_damageShields.push_back(ds);
    }
    else
    {
        for(std::list<struct DamageShield>::iterator i = m_target->m_damageShields.begin();i != m_target->m_damageShields.end();i++)
        {
            if(i->m_spellId == GetSpellId())
            {
                 m_target->m_damageShields.erase(i);
                 return;
            }
        }
    }
}

void Aura::SpellAuraModStealth(bool apply)
{                    
    if(apply)
    {
        SetPositive();
        m_target->SetStealth(GetSpellId());
        m_target->SetFlag(UNIT_FIELD_BYTES_1,0x02000000);//sneak anim
        m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_STEALTH);
        m_target->m_stealthLevel += mod->m_amount;
    }
    else 
    {
        m_target->SetStealth(0);
        m_target->m_stealthLevel -= mod->m_amount;
        m_target->RemoveFlag(UNIT_FIELD_BYTES_1,0x02000000);
        if( m_target->GetTypeId() == TYPEID_PLAYER )
        {
            WorldPacket data;
            data.SetOpcode(SMSG_COOLDOWN_EVENT);
            data << (uint32)GetSpellProto()->Id << m_target->GetGUID();
            ((Player*)m_target)->GetSession()->SendPacket (&data);
        }

        // hack fix for vanish stuff
        /*if(m_spellProto->NameHash == 0xa50106d7 && m_target->GetTypeId() == TYPEID_PLAYER)     // Vanish
        {
            // check for stealh spells
            Player * p_caster = (Player*)m_target;
            uint32 stealth_id = 0;
            SpellSet::iterator itr = p_caster->mSpells.begin();
            SpellSet::iterator end = p_caster->mSpells.end();
            for(; itr != end; ++itr)
            {
                if((*itr) == 1787 || (*itr) == 1786 || (*itr) == 1785 || (*itr) == 1784)
                {
                    stealth_id = *itr;
                    break;
                }
            }
            if(stealth_id)
                p_caster->CastSpell(p_caster, sSpellStore.LookupEntry(stealth_id), true);
        }*/
    }    
}

void Aura::SpellAuraModDetect(bool apply)
{
    if(apply)
    {
        //SetPositive();
        m_target->m_stealthDetectBonus += mod->m_amount;
    }
    else 
        m_target->m_stealthDetectBonus -= mod->m_amount;
}

void Aura::SpellAuraModInvisibility(bool apply)
{
    if(apply)
    {
        SetPositive();
        m_target->m_invisible = true;
    }
    else 
        m_target->m_invisible = false;
        
    ((Player*)m_target)->UpdateVisibility(true);    
}

void Aura::SpellAuraModInvisibilityDetection(bool apply)
{
    //Always Positive

    assert(mod->m_miscValue < INVISIBILTY_FLAG_TOTAL);
    if(apply)
    {
        m_target->InvisibilityDetectBonus[mod->m_miscValue] += mod->m_amount;
        SetPositive ();
    }
    else 
        m_target->InvisibilityDetectBonus[mod->m_miscValue] -= mod->m_amount;

    if(m_target->IsPlayer())
        ((Player*)m_target)->UpdateVisibility(true);
}

void Aura::SpellAuraModTotalHealthRegenPct(bool apply)
{
    if(apply)
    {
        SetPositive();        
        sEventMgr.AddEvent(this, &Aura::EventPeriodicHealPct,(float)mod->m_amount,
            EVENT_AURA_PERIODIC_HEAL,    GetSpellProto()->EffectAmplitude[mod->i],0);
    }
}

void Aura::EventPeriodicHealPct(float RegenPct)
{
    if(!m_target->isAlive())
        return;

    uint32 add = m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH) * (RegenPct / 100.0f);

    uint32 newHealth = m_target->GetUInt32Value(UNIT_FIELD_HEALTH) + add;

    if(newHealth <= m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH))
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
    else
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH, m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH));

    SendPeriodicAuraLog(m_casterGuid, m_target, m_spellProto->Id, m_spellProto->School, add, FLAG_PERIODIC_HEAL);

    if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
    {
        m_target->Emote(EMOTE_ONESHOT_EAT);
    }
}

void Aura::SpellAuraModTotalManaRegenPct(bool apply)
{
    if(apply)
    {
        SetPositive();        
        sEventMgr.AddEvent(this, &Aura::EventPeriodicManaPct,(float)mod->m_amount,
            EVENT_AURA_PERIOCIC_MANA,    GetSpellProto()->EffectAmplitude[mod->i],0);
    }
}

void Aura::EventPeriodicManaPct(float RegenPct)
{
    if(!m_target->isAlive())
        return;

    uint32 add = m_target->GetUInt32Value(UNIT_FIELD_MAXPOWER1) * (RegenPct / 100.0f);

    uint32 newHealth = m_target->GetUInt32Value(UNIT_FIELD_POWER1) + add;

    if(newHealth <= m_target->GetUInt32Value(UNIT_FIELD_MAXPOWER1))
        m_target->SetUInt32Value(UNIT_FIELD_POWER1, newHealth);
    else
        m_target->SetUInt32Value(UNIT_FIELD_POWER1, m_target->GetUInt32Value(UNIT_FIELD_MAXPOWER1));

    // CAPT 
    // TODO: sniff it or disasm wow.exe to find the mana flag
    //SendPeriodicAuraLog(m_target, m_casterGuid, GetSpellProto()->Id, FLAG_PERIODIC_HEAL, add,true);
    //SendPeriodicAuraLog(m_target, m_casterGuid, GetSpellProto()->Id, FLAG_PERIODIC_HEAL, add);

    if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
    {
        m_target->Emote(EMOTE_ONESHOT_EAT);
    }
}

void Aura::SpellAuraModResistance(bool apply)
{
    uint32 Flag = mod->m_miscValue; 
    int32 amt;
    if(apply)
    {
        amt = mod->m_amount;
        if(amt > 0)
            SetPositive();
        else 
            SetNegative();
    }
    else 
        amt=-mod->m_amount;  
    
    if(m_target->GetTypeId() == TYPEID_PLAYER)
    {
        for(uint32 x=0;x<7;x++)
        {
            if(Flag & (((uint32)1)<< x) )
            {
                if(mod->m_amount>0)
                        ((Player*)m_target)->FlatResistanceModifierPos[x]+=amt;
                else
                        ((Player*)m_target)->FlatResistanceModifierNeg[x]-=amt;
                ((Player*)m_target)->CalcResistance(x);
            }
        }        
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if(Flag & (((uint32)1)<<x))
            {
                ((Creature*)m_target)->FlatResistanceMod[x]+=amt;
                ((Creature*)m_target)->CalcResistance(x);
            }
        }
    }
}

void Aura::SpellAuraPeriodicTriggerSpell(bool apply)
{
    if(m_spellProto->EffectTriggerSpell[mod->i] == 0)
        return;

    if(IsPassive() && m_spellProto->dummy != 2010  && m_spellProto->dummy != 2020 && m_spellProto->dummy != 2255) //this spells are passive and are not done on the attack...
    {
        Unit * target = (m_target != 0) ? m_target : GetUnitCaster();
        if(target == 0) return;

        SpellEntry *proto = sSpellStore.LookupEntry( m_spellProto->EffectTriggerSpell[mod->i] );

        if(apply)
            target->AddOnStrikeSpell( proto, m_spellProto->EffectAmplitude[mod->i] );
        else
            target->RemoveOnStrikeSpell( proto );

        return;
    }
    
    if(apply)
    {
        //FIXME: positive or negative?
        uint32 sp = GetSpellProto()->EffectTriggerSpell[mod->i];
        SpellEntry *spe = sSpellStore.LookupEntry(sp);
        if(!sp || !spe)
        {
            //    sp=22845;
            return;//null spell
        }

        Unit *m_caster = GetUnitCaster();
        if(!m_caster)
            return;        

        sEventMgr.AddEvent(this, &Aura::EventPeriodicTriggerSpell, spe, m_caster->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT),
            EVENT_AURA_PERIODIC_TRIGGERSPELL,GetSpellProto()->EffectAmplitude[mod->i], 0);    
    }
}

void Aura::EventPeriodicTriggerSpell(SpellEntry* spellInfo, uint64 target)
{
    // Trigger Spell
    // check for spell id

    Unit *m_caster=GetUnitCaster();
    if(!m_caster)
        return;

    Unit *pTarget = World::GetUnit(target);
    int8 fail = -1;

    if(!pTarget || pTarget->isDead())
    {
        fail = SPELL_FAILED_TARGETS_DEAD;
    }
    else
    {
        if(pTarget != m_caster && !isAttackable(m_caster, pTarget))
            fail = SPELL_FAILED_BAD_TARGETS;
    }
    if(fail > 0)
    {
        WorldPacket data(14);
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            data.SetOpcode(SMSG_SPELL_FAILURE);
            data << m_caster->GetNewGUID() << GetSpellProto()->Id << fail;
            ((Player*)m_caster)->GetSession()->SendPacket(&data);

            data.Initialize(MSG_CHANNEL_UPDATE);
            data << uint32(0);
            ((Player*)m_caster)->GetSession()->SendPacket(&data);
        }
        data.Initialize(SMSG_SPELL_FAILED_OTHER);
        data << m_caster->GetNewGUID() << GetSpellProto()->Id << fail;
        m_caster->SendMessageToSet(&data, false);

        this->Remove();
        return;
    }

    if(spellInfo->dummy = 225 ) // this is arcane missles to avoid casting on self
        if(m_casterGuid == pTarget->GetGUID())
            return;

    Spell *spell = new Spell(m_caster, spellInfo, true, this);
    SpellCastTargets targets;

    targets.m_unitTarget = pTarget->GetGUID();
    targets.m_targetMask = TARGET_FLAG_UNIT;
    spell->prepare(&targets);    
}

void Aura::SpellAuraPeriodicEnergize(bool apply)
{
    if(apply)
    {
        SetPositive();
        sEventMgr.AddEvent(this, &Aura::EventPeriodicEnergize,(uint32)mod->m_amount,(uint32)mod->m_miscValue,
            EVENT_AURA_PERIODIC_ENERGIZE,GetSpellProto()->EffectAmplitude[mod->i],0);
    }
}

void Aura::EventPeriodicEnergize(uint32 amount,uint32 type)
{        
    /*Unit*m_caster=GetUnitCaster();
    if(!m_caster) return;
        
    if(m_target->isAlive() && m_caster->isAlive())
    {
            /*uint32 powerField;
            uint32 currentPower;
            switch(GetSpellProto()->powerType)
            {
                case POWER_TYPE_HEALTH:{
                        powerField = UNIT_FIELD_HEALTH;
                                       }break;
                case POWER_TYPE_MANA:{
                        powerField = UNIT_FIELD_POWER1;
                                     }break;
                case POWER_TYPE_RAGE:{
                        powerField = UNIT_FIELD_POWER2;
                                     }break;
                case POWER_TYPE_ENERGY:{
                        powerField = UNIT_FIELD_POWER4;
                                       }break;
                default:{
                        sLog.outDebug("unknown power type");
                        }break;
            };
            if((GetSpellProto()->powerType != type) && (GetSpellId() != 1539))//Feed Pet
            {
                
                currentPower = m_target->GetUInt32Value(powerField);
          
                if(currentPower < (GetSpellProto()->manaCost ))//wtf??
                    m_target->SetUInt32Value(powerField, 0);
                else
                    m_target->SetUInt32Value(powerField, currentPower - GetSpellProto()->manaCost );
            }*/
            //disabled by Phantomas
            //meaning of this is unknown....reenable it if's needed
    uint32 POWER_TYPE=UNIT_FIELD_POWER1+type;
    
    ASSERT(POWER_TYPE<=UNIT_FIELD_POWER5);

    uint32 curEnergy = m_target->GetUInt32Value(POWER_TYPE);
    uint32 maxEnergy = m_target->GetUInt32Value(POWER_TYPE+6);
    uint32 totalEnergy = curEnergy+amount;
    if(totalEnergy > maxEnergy)
        m_target->SetUInt32Value(POWER_TYPE,maxEnergy);
    else
        m_target->SetUInt32Value(POWER_TYPE,totalEnergy);
    
    SendPeriodicAuraLog( m_casterGuid, m_target, m_spellProto->Id, m_spellProto->School, amount, FLAG_PERIODIC_ENERGIZE);

    if((GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP) && type == 0)
    {
        m_target->Emote(EMOTE_ONESHOT_EAT);
    }
}

void Aura::SpellAuraModPacify(bool apply)
{
    // Cant Attack
    if(apply)
    {
        if(m_spellProto->Id == 24937)
            SetPositive();
        else
            SetNegative();     
        m_target->m_pacified++;
    }
    else
    {
        m_target->m_pacified--;
    }
}

void Aura::SpellAuraModRoot(bool apply)
{
    //if(m_spellProto->MechanicsType == DISPEL_MECHANIC_ROOT && m_spellProto->Attributes == 1073840128)
        //m_target->m_rooted = m_spellProto->Id;

    SetNegative();
    if(apply)
    {
        if(m_target->m_rooted == 0)
            m_target->Root();

        m_target->m_rooted++;
    }
    else
    {
        m_target->m_rooted--;

        if(!m_target->m_rooted)
            m_target->Unroot();

        if(m_target->GetTypeId() == TYPEID_UNIT)
        {
            Unit * target = GetUnitCaster();
            if(!target) return;

            m_target->GetAIInterface()->AttackReaction(target, 1, 1, 0);
        }
    }    
}

void Aura::SpellAuraModSilence(bool apply)
{
    if(apply)
        m_target->m_silenced++;
    else
        m_target->m_silenced--;    
}

void Aura::SpellAuraReflectSpells(bool apply)
{
    if(apply)
    {
        for(std::list<struct ReflectSpellSchool*>::iterator i = m_target->m_reflectSpellSchool.begin();i != m_target->m_reflectSpellSchool.end();i++)
        {
            if(GetSpellId() == (*i)->spellId)
            {
                m_target->m_reflectSpellSchool.erase(i);
            }
        }
        ReflectSpellSchool *rss = new ReflectSpellSchool;

        rss->chance = mod->m_amount;
        rss->spellId = GetSpellId();
        rss->school = -1;
        m_target->m_reflectSpellSchool.push_back(rss);
    }
    else
    {
        for(std::list<struct ReflectSpellSchool*>::iterator i = m_target->m_reflectSpellSchool.begin();i != m_target->m_reflectSpellSchool.end();i++)
        {
            if(GetSpellId() == (*i)->spellId)
            {
                delete *i;
                m_target->m_reflectSpellSchool.erase(i);
                break;
            }
        }
    }    
}

void Aura::SpellAuraModStat(bool apply)
{
    int32 val;

    if(apply)
    {
       val = mod->m_amount;
       if (val>0)
           SetPositive();
       else
           SetNegative();
    }
    else 
    {
        val = -mod->m_amount;
    }

    if (mod->m_miscValue == -1)//all stats
    {            
        if(m_target->IsPlayer())
        {            
            for(uint32 x=0;x<5;x++)
            {
                if(mod->m_amount>0)
                    ((Player*)m_target)->FlatStatModPos[x] += val; 
                else
                    ((Player*)m_target)->FlatStatModNeg[x] -= val;
        
                ((Player*)m_target)->CalcStat(x);    
            }

            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            for(uint32 x=0;x<5;x++)
            {
                ((Creature*)m_target)->FlatStatMod[x] += val;
                ((Creature*)m_target)->CalcStat(x);            
            }
        }
    } 
    else
    {
        ASSERT(mod->m_miscValue < 5);
        if(m_target->IsPlayer())
        {
            if(mod->m_amount>0)
                ((Player*)m_target)->FlatStatModPos[mod->m_miscValue] += val; 
            else 
                ((Player*)m_target)->FlatStatModNeg[mod->m_miscValue] -= val;
    
            ((Player*)m_target)->CalcStat(mod->m_miscValue);    
        
            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            ((Creature*)m_target)->FlatStatMod[mod->m_miscValue]+=val;
            ((Creature*)m_target)->CalcStat(mod->m_miscValue);
        }
    }
}

void Aura::SpellAuraModSkill(bool apply)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {   
        if(apply)
        {
            SetPositive();
            ((Player*)m_target)->ModSkillBonus(mod->m_miscValue, mod->m_amount); 
        }
        else
            ((Player*)m_target)->ModSkillBonus(mod->m_miscValue, -mod->m_amount); 
        
        ((Player*)m_target)->UpdateChances();
    }
}

void Aura::SpellAuraModIncreaseSpeed(bool apply)
{
    if(apply)
    {
        //if(m_target->isAlive() ) SetPositive(); 
        m_target->m_speedModifier += mod->m_amount;
    }
    else
        m_target->m_speedModifier -= mod->m_amount;
    m_target->UpdateSpeed();
}

void Aura::SpellAuraModIncreaseMountedSpeed(bool apply)
{
    if(apply)
    { 
        SetPositive();
        m_target->m_mountedspeedModifier += mod->m_amount;
    }
    else
        m_target->m_mountedspeedModifier -= mod->m_amount;
    m_target->UpdateSpeed();
}

void Aura::SpellAuraModCreatureRangedAttackPower(bool apply)
{
    if(apply)
    {
        for(uint32 x=0;x<7;x++)
            if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->CreatureRangedAttackPowerMod[x+1] += mod->m_amount;
        if(mod->m_amount > 0)
            SetPositive();
        else
            SetNegative();
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->CreatureRangedAttackPowerMod[x+1] -= mod->m_amount;
            }
        }
    }
}

void Aura::SpellAuraModDecreaseSpeed(bool apply)
{
    if(apply)
    {
        m_target->m_speedModifier += mod->m_amount;
    }
    else
        m_target->m_speedModifier -= mod->m_amount;
    m_target->UpdateSpeed();

}

void Aura::SpellAuraModIncreaseHealth(bool apply)
{
    int32 amt;
    if(apply)
    {
        SetPositive();
        amt = mod->m_amount;
    }
    else 
        amt =- mod->m_amount;
   
    if(m_target->IsPlayer())
    {    
        ((Player*)m_target)->SetHealthFromSpell(((Player*)m_target)->GetHealthFromSpell() + amt);
        ((Player*)m_target)->UpdateStats();
    }
    else
         m_target->ModUInt32Value(UNIT_FIELD_MAXHEALTH, amt);
}

void Aura::SpellAuraModIncreaseEnergy(bool apply)
{
    SetPositive();
    uint32 powerField,maxField ;
    uint8 powerType = m_target->GetPowerType();
    
    if(powerType == POWER_TYPE_MANA) // Mana
    {
        powerField = UNIT_FIELD_POWER1;
        maxField = UNIT_FIELD_MAXPOWER1;
    }
    else if(powerType == POWER_TYPE_RAGE) // Rage
    {
        powerField = UNIT_FIELD_POWER2;
        maxField = UNIT_FIELD_MAXPOWER2;
    }
    else if(powerType == POWER_TYPE_ENERGY) // Energy
    {
        powerField = UNIT_FIELD_POWER4;
        maxField = UNIT_FIELD_MAXPOWER4;
    }

    m_target->ModUInt32Value(powerField,apply?mod->m_amount:-mod->m_amount);
    m_target->ModUInt32Value(maxField,apply?mod->m_amount:-mod->m_amount);

    if(!powerType && m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 amt = apply?mod->m_amount:-mod->m_amount;
        ((Player*)m_target)->SetManaFromSpell(((Player*)m_target)->GetManaFromSpell() + amt);
    }
}

void Aura::SpellAuraModShapeshift(bool apply)
{
    if(!m_target->IsPlayer())
        return;
    
    uint32 spellId = 0;
    uint32 modelId = 0;
    bool recalc = false;
    bool freeMovements = false;
    static uint32 oldap;

    switch(mod->m_miscValue)
    {
    case FORM_CAT: 
        {//druid
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 3025;
            recalc = true;
            if(apply)
            {
                oldap = m_target->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME);
                m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME,1000); // cat form have speed 1.00
                uint32 tmp = m_target->GetUInt32Value(UNIT_FIELD_BYTES_0) & 0x00FFFFFF;
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0,tmp|0x03000000 );//Energy for cat form
                m_target->SetUInt32Value(UNIT_FIELD_MAXPOWER4,100);//0 Energy
                m_target->SetUInt32Value(UNIT_FIELD_POWER4,100);//0 Energy
                 if(m_target->getRace() == RACE_NIGHTELF)//NE
                    modelId = 892;
                else //TAUREN
                    modelId = 8571;
                
            }
            else
            {//turn back to mana
                m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME,oldap);
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0,m_target->GetUInt32Value(UNIT_FIELD_BYTES_0)&0x00FFFFFF);
                if(m_target->m_stealth)
                m_target->RemoveAura(m_target->m_stealth);//prowl
            }       
        } break;
    /*case FORM_TREE:{
        spellId = 3122;
        } break;*/
    case FORM_TRAVEL:
        {//druid
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 5419;
               modelId = 917;
        } break;
    case FORM_AQUA:
        {//druid aqua
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 5421;
               modelId = 2428;
        } break;
    case FORM_BEAR:
        {//druid only
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 1178;
            recalc = true;
            if(apply)
            {
                uint32 tmp = m_target->GetUInt32Value(UNIT_FIELD_BYTES_0) & 0x00FFFFFF;
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0,tmp | 0x01000000);//Rage for bear form
                m_target->SetUInt32Value(UNIT_FIELD_MAXPOWER2, 1000);
                m_target->SetUInt32Value(UNIT_FIELD_POWER2, 0);

                if(m_target->getRace() == 4)//NE
                    modelId = 2281;
                else //TAUREN
                    modelId = 2289;
            }
            else 
            {//reset back to mana
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0, m_target->GetUInt32Value(UNIT_FIELD_BYTES_0) & 0x00FFFFFF);
            }
        } break;    
    case FORM_DIREBEAR:
        {//druid only
            m_target->disarmed = true;
            freeMovements = true; 
            spellId = 9635;
            recalc = true;
            if(apply)
            {
                uint32 tmp = m_target->GetUInt32Value(UNIT_FIELD_BYTES_0) & 0x00FFFFFF;
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0,tmp | 0x01000000);//Rage for bear form
                m_target->SetUInt32Value(UNIT_FIELD_MAXPOWER2, 1000);
                m_target->SetUInt32Value(UNIT_FIELD_POWER2, 0);//0 rage
                if(m_target->getRace() == 4)//NE
                    modelId = 2281;
                else //TAUREN
                    modelId = 2289;
            }
            else //reset back to mana
                m_target->SetUInt32Value(UNIT_FIELD_BYTES_0, m_target->GetUInt32Value(UNIT_FIELD_BYTES_0) & 0x00FFFFFF);      
        } break;
  /* case FORM_CREATUREBEAR:{
        spellId = 2882;
        modelId = 9492;-----------------obsolete ?
                           } break;*/
    case FORM_GHOSTWOLF:
        {
            modelId = 1236;
        } break;  
    case FORM_DEFENSIVESTANCE:
        {
            spellId = 7376;               
        } break;
    case FORM_BERSERKERSTANCE:
        {
            spellId = 7381;   
        } break;
    case FORM_SHADOW:
        {
            if(apply)
            {
                WorldPacket data;
                data.SetOpcode(SMSG_COOLDOWN_EVENT);
                data << (uint32)GetSpellProto()->Id << m_target->GetGUID();
                static_cast<Player *>(m_target)->GetSession()->SendPacket(&data);
            }
        }break;
    case FORM_FLIGHT:
        {// druid
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 33948; //Flight Form Passive
            //FIXME: model?
        }break;
    case FORM_STEALTH:
        {// rogue        
            static_cast<Player *>(m_target)->UpdateVisibility(true);
        } break;
    case FORM_MOONKIN:
        {//druid
            m_target->disarmed = true;
            freeMovements = true;
            spellId = 24905;
            if(apply)
            {
                if(m_target->getRace() == RACE_NIGHTELF)
                    modelId = 15374;
                else 
                    modelId = 15375;
            }
        }break;
    }

    if (apply)
    {
        if(m_target->getClass() == WARRIOR)//REMOVE/FIX THAT WHEN BONUSES OF TALENTS ARE IMPLEMENTED
            m_target->SetUInt32Value(UNIT_FIELD_POWER2, 0);//0 rage

        if(spellId != GetSpellId())
        {
            if(static_cast<Player *>(m_target)->m_ShapeShifted)
                static_cast<Player *>(m_target)->RemoveAura(static_cast<Player *>(m_target)->m_ShapeShifted);

            static_cast<Player *>(m_target)->m_ShapeShifted = GetSpellId();
        }

        if(modelId)
            m_target->SetUInt32Value(UNIT_FIELD_DISPLAYID, modelId);
    
        m_target->SetUInt32Value(UNIT_FIELD_BYTES_1, (m_target->GetUInt32Value(UNIT_FIELD_BYTES_1) & 0xff00ffff) | (mod->m_miscValue << 16));        
    
        if(recalc)
            static_cast<Player *>(m_target)->UpdateStats();
                
        if(spellId == 0)
            return;
        // check for spell id
        SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );
        
        Spell *sp = new Spell(m_target, spellInfo, true, NULL);
        SpellCastTargets tgt;
        tgt.m_unitTarget = m_target->GetGUID();
        sp->prepare(&tgt);
        
        // remove the caster from imparing movements
        if(freeMovements)
        {
            for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
            {
                if(m_target->m_auras[x])
                {
                    if(m_target->m_auras[x]->GetSpellProto()->MechanicsType == 7 || m_target->m_auras[x]->GetSpellProto()->MechanicsType == 11) // Remove roots and slow spells
                        m_target->m_auras[x]->Remove();
                    else // if got immunity for slow, remove some that are not in the mechanics
                    {
                        for(int i=0;i<3;i++)
                            if(m_target->m_auras[x]->GetSpellProto()->EffectApplyAuraName[i] == SPELL_AURA_MOD_DECREASE_SPEED||m_target->m_auras[x]->GetSpellProto()->EffectApplyAuraName[i] == SPELL_AURA_MOD_ROOT)
                            {
                                m_target->m_auras[x]->Remove();
                                break;
                            }
                    }
               }
            }
        }
    }
    else 
    {
        m_target->disarmed = false;
        m_target->SetUInt32Value(UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));                
        if(spellId!=GetSpellId())
        {
            if(spellId)    
                m_target->RemoveAura(spellId);
        }
        static_cast<Player *>(m_target)->m_ShapeShifted=0;

        m_target->SetUInt32Value(UNIT_FIELD_BYTES_1,m_target->GetUInt32Value(UNIT_FIELD_BYTES_1) & 0xff00ffff);
        
        if(recalc)
            static_cast<Player *>(m_target)->UpdateStats();      
    } 
}

void Aura::SpellAuraModEffectImmunity(bool apply)
{
    if(m_spellProto->Id == 24937)
        SetPositive();
}

void Aura::SpellAuraModStateImmunity(bool apply)
{
    //%50 chance to dispel 1 magic effect on target
    //23922
}

void Aura::SpellAuraModSchoolImmunity(bool apply)
{
    if(apply)
    {
        SetPositive();
        for(uint32 i = 0; i < 7; i++)
        {
            if(mod->m_miscValue & (1<<i))
            {
                m_target->SchoolImmunityList[i] ++;
                m_target->RemoveAurasOfSchool(i, false);
            }
        }
    }
    else
    {
        for(int i = 0; i < 7; i++)
        {
            if(mod->m_miscValue & (1<<i))
                m_target->SchoolImmunityList[i]--;
        }
    }    
}

void Aura::SpellAuraModDmgImmunity(bool apply)
{
  
}

void Aura::SpellAuraModDispelImmunity(bool apply)
{
    assert(mod->m_miscValue < 10);
    if(apply)
        m_target->dispels[mod->m_miscValue]++;
    else 
        m_target->dispels[mod->m_miscValue]--;

    if(apply)
    {
        for(uint32 x=0;x<MAX_AURAS;x++)
        {
            if(m_target->m_auras[x])
                if(m_target->m_auras[x]->GetSpellProto()->DispelType==mod->m_miscValue)
                    m_target->m_auras[x]->Remove();
        }
    }
}

void Aura::SpellAuraProcTriggerSpell(bool apply)
{
    if(apply)
    {
        ProcTriggerSpell pts;
        pts.origId = GetSpellProto()->Id;
        pts.caster = m_casterGuid;
        if(GetSpellProto()->EffectTriggerSpell[mod->i])
            pts.spellId=GetSpellProto()->EffectTriggerSpell[mod->i];
/*        else 
            pts.spellId = GetSpellProto()->dummy;*/
        if(!pts.spellId)
            return;
        pts.procChance = GetSpellProto()->procChance;
        pts.procFlags = GetSpellProto()->procFlags;
        pts.procCharges = GetSpellProto()->procCharges;
        m_target->m_procSpells.push_front(pts);
    }
    else
    {
        for(std::list<struct ProcTriggerSpell>::iterator itr = m_target->m_procSpells.begin();itr != m_target->m_procSpells.end();itr++)
        {
            if(itr->origId == GetSpellId() && itr->caster == m_casterGuid)
            {
                m_target->m_procSpells.erase(itr);
                break;
            }
        }
    }
}

void Aura::SpellAuraProcTriggerDamage(bool apply)
{
    //tempor disabled by Phantomas
    /*
    if(apply)
    {
        DamageShield* ds = new DamageShield();
        ds->m_caster = m_casterGuid;
        ds->m_damage = mod->m_amount;
        ds->m_spellId = GetSpellProto()->Id;
        m_target->m_damageShields.push_back((*ds));
    }
    else
    {
        for(std::list<struct DamageShield>::iterator i = m_target->m_damageShields.begin();i != m_target->m_damageShields.end();i++)
        {
            if(i->m_spellId == GetSpellProto()->Id && i->m_caster == m_casterGuid)
            {
                m_target->m_damageShields.erase(i);
                break;
            }
        }
    }*/    
}

void Aura::SpellAuraTrackCreatures(bool apply)
{
    if(apply)
    {
        if(((Player*)m_target)->TrackingSpell)
            m_target->RemoveAura(((Player*)m_target)->TrackingSpell);

       m_target->SetUInt32Value(PLAYER_TRACK_CREATURES,(uint32)1<< (mod->m_miscValue-1));
       ((Player*)m_target)->TrackingSpell = GetSpellId();
    
    }
    else
    {
        ((Player*)m_target)->TrackingSpell = 0;
            m_target->SetUInt32Value(PLAYER_TRACK_CREATURES,0);

    }
}

void Aura::SpellAuraTrackResources(bool apply)
{
    if(apply)
    {
        if(((Player*)m_target)->TrackingSpell)
            m_target->RemoveAura(((Player*)m_target)->TrackingSpell);

       m_target->SetUInt32Value(PLAYER_TRACK_RESOURCES,(uint32)1<< (mod->m_miscValue-1));
       ((Player*)m_target)->TrackingSpell=GetSpellId();
    }
    else
    {
        ((Player*)m_target)->TrackingSpell = 0;
            m_target->SetUInt32Value(PLAYER_TRACK_RESOURCES,0);
    }
}

void Aura::SpellAuraModParryPerc(bool apply)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 amt;
        if(apply)
        {
            amt = mod->m_amount;
            if(amt>0)
                SetPositive();
            else
                SetNegative();

        }
        else
            amt = -mod->m_amount;

        ((Player*)m_target)->SetParryFromSpell(((Player*)m_target)->GetParryFromSpell() + amt );
        ((Player*)m_target)->UpdateChances();
    }
}

void Aura::SpellAuraModDodgePerc(bool apply)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 amt;
        if(apply)
        {
            amt = mod->m_amount;
            if(amt>0)
                SetPositive();
            else
                SetNegative();

        }
        else 
            amt = -mod->m_amount;
        ((Player*)m_target)->SetDodgeFromSpell(((Player*)m_target)->GetDodgeFromSpell() + amt );
        ((Player*)m_target)->UpdateChances();
    }
}

void Aura::SpellAuraModBlockPerc(bool apply)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 amt;
        if(apply)
        {
            amt = mod->m_amount;
            if(amt>0)
                SetPositive();
            else 
                SetNegative();
        }
        else 
            amt = -mod->m_amount;

        ((Player*)m_target)->SetBlockFromSpellPCT(((Player*)m_target)->GetBlockFromSpellPCT() + amt );
        ((Player*)m_target)->UpdateChances();
    }
}
void Aura::SpellAuraModCritPerc(bool apply)
{
    if (m_target->IsPlayer())
    {
        if(apply)
        {
            WeaponModifier md;
            md.value = mod->m_amount;
            md.wclass = GetSpellProto()->EquippedItemClass;
            md.subclass = GetSpellProto()->EquippedItemSubClass;
            ((Player*)m_target)->tocritchance.insert(make_pair(GetSpellId(), md));
        }
        else 
        {
            /*std::list<WeaponModifier>::iterator i = ((Player*)m_target)->tocritchance.begin();

            for(;i!=((Player*)m_target)->tocritchance.end();i++)
            {
                if((*i).spellid==GetSpellId())
                {
                    ((Player*)m_target)->tocritchance.erase(i);
                    break;
                }
            }*/
            ((Player*)m_target)->tocritchance.erase(GetSpellId());
        }
        ((Player*)m_target)->UpdateChances();
    }
}

void Aura::SpellAuraPeriodicLeech(bool apply)
{
    if(apply)
    {
        SetNegative();
        uint32 amt = mod->m_amount;
        /*if(GetSpellProto()->SpellGroupType)
        {
            Unit*c=GetUnitCaster();
            if(c)
            SM_PIValue(c->SM_PEffectBonus,(int32*)&amt,GetSpellProto()->SpellGroupType);
        }*/
        sEventMgr.AddEvent(this, &Aura::EventPeriodicLeech,amt,
            EVENT_AURA_PERIODIC_LEECH,    GetSpellProto()->EffectAmplitude[mod->i],0);
    }
}

void Aura::EventPeriodicLeech(uint32 amount)
{
    Unit*m_caster = GetUnitCaster();
    
    if(!m_caster)
        return;

    if(m_target->isAlive() && m_caster->isAlive())
    {
        if(m_target->SchoolImmunityList[GetSpellProto()->School])
            return;
    
        uint32 Amount = min(amount,m_target->GetUInt32Value(UNIT_FIELD_HEALTH));
        uint32 newHealth = m_caster->GetUInt32Value(UNIT_FIELD_HEALTH) + Amount ;
        
        uint32 mh = m_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
        if(newHealth <= mh)
            m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
        else
            m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, mh);
        
        SendPeriodicAuraLog(m_target, m_target, m_spellProto->Id, m_spellProto->School, Amount, FLAG_PERIODIC_LEECH);
        m_target->DealDamage(m_target, Amount, 0, 0, GetSpellProto()->Id);
    }    
}



void Aura::SpellAuraModHitChance(bool apply)
{
    if (m_target->IsPlayer())
    {
        if (apply)
        {
            ((Player*)m_target)->SetHitFromMeleeSpell(((Player*)m_target)->GetHitFromMeleeSpell() + mod->m_amount);
            if(mod->m_amount>0)
                SetPositive ();
            else 
                SetNegative();
        }
        else
        {
            ((Player*)m_target)->SetHitFromMeleeSpell(((Player*)m_target)->GetHitFromMeleeSpell() - mod->m_amount);
            if(((Player*)m_target)->GetHitFromMeleeSpell() < 0)
            {
                ((Player*)m_target)->SetHitFromMeleeSpell(0);
            }
        }
    }
}

void Aura::SpellAuraModSpellHitChance(bool apply)
{
    if (m_target->IsPlayer())
    {
        if (apply)
        {
            ((Player*)m_target)->SetHitFromSpell(((Player*)m_target)->GetHitFromSpell() + mod->m_amount);
            if(mod->m_amount>0)
                SetPositive ();
            else 
                SetNegative();
        }
        else
        {
            ((Player*)m_target)->SetHitFromSpell(((Player*)m_target)->GetHitFromSpell() - mod->m_amount);
            if(((Player*)m_target)->GetHitFromSpell() < 0)
            {
                ((Player*)m_target)->SetHitFromSpell(0);
            }
        }
    }
}

void Aura::SpellAuraTransform(bool apply)
{
   // SetPositive();
    switch(GetSpellProto()->Id)
    {
        case 20584://wisp
            m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, apply?10045:m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
        break;

        case 16739: // Orb of Deception
            break;
        
        case 118://polymorh
        case 851:
        case 5254:
        case 12824:
        case 12825:
        case 12826:
        case 13323:
        case 15534:
        case 22274:
        case 23603:
            {
                if (apply) 
                {
                    m_target->m_pacified++;
                    /*                           tmpAff = new Affect(spellInfo,m_duration,GetUnitCasterGUID(),GetUnitCaster());

                    uint32 value = 0;
                    uint32 type = 0;
                    uint32 damage = 0;

                    type = 1;

                    tmpAff->AddMod(SPELL_AURA_MOD_POWER_REGEN_PERCENT,2500,POWER_TYPE_MANA,type);
                    tmpAff->SetPositive();*/

                    //((Modifier *)mod)->SetValue1 (GetUInt32Value (UNIT_FIELD_DISPLAYID));
                    sEventMgr.AddEvent(this, &Aura::EventPeriodicHeal1,(uint32)1000,EVENT_AURA_PERIODIC_HEAL,1000,0);
             
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, 856);
                }
                else 
                {
                    m_target->m_pacified--;
                    if(m_target->m_pacified < 0) 
                    m_target->m_pacified = 0;
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                }
            }break;
        case 228:
            {
                if (apply) 
                {
                    m_target->m_pacified++;
                    //((Modifier *)mod)->SetValue1 (m_target->GetUInt32Value (UNIT_FIELD_DISPLAYID));
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, 304);
                }
                else 
                {
                    m_target->m_pacified--;
                    if(m_target->m_pacified < 0) 
                    m_target->m_pacified = 0;
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                }
            }break;
        case 4060:
            {
                if (apply)
                {
                    //((Modifier *)mod)->SetValue1 (m_target->GetUInt32Value (UNIT_FIELD_DISPLAYID));
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, 131);
                } 
                else
                {
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                }
            }break;
        case 17738:
            {
                if (apply)
                {
                    //((Modifier *)mod)->SetValue1 (m_target->GetUInt32Value (UNIT_FIELD_DISPLAYID));
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, 1141);
                }
                else
                {
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                }
            }break;

        case 19937:
            {
                if (apply)
                {
                    // TODO: Sniff the spell / item, we need to know the real displayID
                    // guessed this may not be correct
                    // human = 7820
                    // dwarf = 7819
                    // halfling = 7818
                    // maybe 7842 as its from a lesser npc
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, 7842);
                } 
                else
                {
                    m_target->SetUInt32Value (UNIT_FIELD_DISPLAYID, m_target->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID));
                }
            }break;
    };
}

void Aura::SpellAuraModSpellCritChance(bool apply)
{
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        int32 amt;
        if(apply)
        {
            amt = mod->m_amount;
            if(amt>0)
                SetPositive();
        }
        else 
            amt = -mod->m_amount;

        m_target->spellcritperc += amt;
        ((Player*)m_target)->SetSpellCritFromSpell(((Player*)m_target)->GetSpellCritFromSpell() + amt);
        //((Player*)m_target)->UpdateStats();
    }
}

void Aura::SpellAuraIncreaseSwimSpeed(bool apply)
{   
    if(apply)
    {
        if(m_target->isAlive())  SetPositive(); 
        m_target->m_swimSpeed = 0.04722222*(100+mod->m_amount);
    }
    else 
        m_target->m_swimSpeed = PLAYER_NORMAL_SWIM_SPEED;
    if(m_target->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(SMSG_FORCE_SWIM_SPEED_CHANGE, 17);
        data << m_target->GetNewGUID();
        data << (uint32)2;
        data << m_target->m_swimSpeed;
        ((Player*)m_target)->GetSession()->SendPacket(&data);
    }   
}

void Aura::SpellAuraPacifySilence(bool apply)
{
    // Cant Attack and cast spell
    if(apply)
    {
        if(m_spellProto->Id == 24937)
            SetPositive();
        else
            SetNegative();
        m_target->m_silenced++;
        m_target->m_pacified++;
    }
    else
    {
        m_target->m_pacified--;
        m_target->m_silenced--;
    }
}

void Aura::SpellAuraModScale(bool apply)
{
    float current = m_target->GetFloatValue(OBJECT_FIELD_SCALE_X);
    float delta = mod->m_amount/100.0;
    
    m_target->SetFloatValue(OBJECT_FIELD_SCALE_X, apply ? (current+current*delta) : current/(1.0+delta));
}

void Aura::SpellAuraPeriodicHealthFunnel(bool apply)
{
    if(apply)
    {    
        uint32 amt = mod->m_amount;
        /*if(GetSpellProto()->SpellGroupType)
        {
            Unit*c=GetUnitCaster();
            if(c)
            SM_PIValue(c->SM_PEffectBonus,(int32*)&amt,GetSpellProto()->SpellGroupType);
        }*/
        sEventMgr.AddEvent(this, &Aura::EventPeriodicHealthFunnel, amt,
            EVENT_AURA_PERIODIC_HEALTH_FUNNEL, GetSpellProto()->EffectAmplitude[mod->i], 0);
    }
}

void Aura::EventPeriodicHealthFunnel(uint32 amount)
{
    //Blood Siphon
    //Deals 200 damage per second.
    //Feeding Hakkar 1000 health per second.
    Unit* m_caster = GetUnitCaster();
    if(!m_caster)
        return;
    if(m_target->isAlive() && m_caster->isAlive())
    {
        m_caster->DealDamage(m_target, amount, 0, 0, 24322);
        uint32 newHealth = m_caster->GetUInt32Value(UNIT_FIELD_HEALTH) + 1000;
        
        uint32 mh = m_caster->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
        if(newHealth <= mh)
            m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, newHealth);
        else
            m_caster->SetUInt32Value(UNIT_FIELD_HEALTH, mh);
    
        SendPeriodicAuraLog(m_target, m_target, m_spellProto->Id, m_spellProto->School, 1000, FLAG_PERIODIC_LEECH);
    }
}

void Aura::SpellAuraPeriodicManaLeech(bool apply)
{
    if(apply)
    {    
        uint32 amt=mod->m_amount;
        /*if(GetSpellProto()->SpellGroupType)
        {
            Unit*c=GetUnitCaster();
            if(c)
            SM_PIValue(c->SM_PEffectBonus,(int32*)&amt,GetSpellProto()->SpellGroupType);
        }*/
        sEventMgr.AddEvent(this, &Aura::EventPeriodicManaLeech,amt,
            EVENT_AURA_PERIODIC_LEECH,     GetSpellProto()->EffectAmplitude[mod->i],0);
    }    
}

void Aura::EventPeriodicManaLeech(uint32 amount)
{
    Unit* m_caster = GetUnitCaster();
    if(!m_caster)
        return;
    if(m_target->isAlive() && m_caster->isAlive())
    {
    
        int32 amt = min(amount,m_target->GetUInt32Value(UNIT_FIELD_POWER1));
        uint32 cm = m_caster->GetUInt32Value(UNIT_FIELD_POWER1)+amt;
        uint32 mm = m_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
        if(cm <= mm)
            m_caster->SetUInt32Value(UNIT_FIELD_POWER1, cm);
        else 
            m_caster->SetUInt32Value(UNIT_FIELD_POWER1, mm);
        m_target->ModUInt32Value(UNIT_FIELD_POWER1, -amt);
    }
}

void Aura::SpellAuraModCastingSpeed(bool apply)
{
    float current = m_target->GetFloatValue(UNIT_MOD_CAST_SPEED);
    if(apply)
        current -= float(mod->m_amount / 100.0f);
    else
        current += float(mod->m_amount / 100.0f);
    m_target->SetFloatValue(UNIT_MOD_CAST_SPEED, current);
}

void Aura::SpellAuraFeighDeath(bool apply)
{
    //Not sure
    apply ? m_target->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_DEAD) : m_target->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_DEAD);
}

void Aura::SpellAuraModDisarm(bool apply)
{
    //U_FIELD_FLAG_WEAPON_OFF
    apply ? m_target->SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_WEAPON_OFF) : m_target->RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_WEAPON_OFF);
    apply ? m_target->disarmed = true : m_target->disarmed = false;
}

void Aura::SpellAuraModStalked(bool apply)
{
    if(apply)
    {
        //SetNegative();
        m_target->stalkedby = m_casterGuid;
    }
    else 
        m_target->stalkedby = 0;
}

void Aura::SpellAuraSchoolAbsorb(bool apply)
{
    Absorb *ab;
    if(apply)
    {
        SetPositive();
        ab = new Absorb;
        ab->amt = mod->m_amount;
        ab->spellid = GetSpellId();
        ab->caster = m_casterGuid;
        for(uint32 x=0;x<7;x++)
            if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->Absorbs[x].push_back(ab);
    }
    else
    {
        ab = NULL;
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                for(SchoolAbsorb::iterator i = m_target->Absorbs[x].begin();i!=m_target->Absorbs[x].end();i++)
                {
                    if( (*i)->spellid == GetSpellId() && (*i)->caster==m_casterGuid )
                    {
                        ab=(*i);
                        m_target->Absorbs[x].erase(i);
                        break;
                    }    
                }

                /*if(ab)
                    delete ab;//should not be null, but just in case...*/
            }
        }
    }    
}

void Aura::SpellAuraModSpellCritChanceSchool(bool apply)
{
    if(apply)
    {
        for(uint32 x=0;x<7;x++)
            if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->SpellCritChanceSchool[x] += mod->m_amount;
        if(mod->m_amount > 0)
            SetPositive();
        else
            SetNegative();
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->SpellCritChanceSchool[x] -= mod->m_amount;
                /*if(m_target->SpellCritChanceSchool[x] < 0)
                    m_target->SpellCritChanceSchool[x] = 0;*/
            }
        }
    }
    if(m_target->IsPlayer())
        ((Player*)m_target)->UpdateChanceFields();
}

void Aura::SpellAuraModPowerCost(bool apply)
{
    if(apply)
    {
        for(uint32 x=0;x<7;x++)
        {
                if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->PowerCostMod[x] += mod->m_amount;
        }
        if(mod->m_amount > 0)
            SetPositive();
        else
            SetNegative();
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
                if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->PowerCostMod[x] -= mod->m_amount;
        }
    }
}

void Aura::SpellAuraModPowerCostSchool(bool apply)
{
    if(apply)
    {
        for(uint32 x=0;x<7;x++)
            if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->PowerCostMod[x] += mod->m_amount;
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->PowerCostMod[x] -= mod->m_amount;
            }
        }
    }
}

void Aura::SpellAuraReflectSpellsSchool(bool apply)
{
    if(apply)
    {
        for(std::list<struct ReflectSpellSchool*>::iterator i = m_target->m_reflectSpellSchool.begin();i != m_target->m_reflectSpellSchool.end();i++)
        {
            if(GetSpellId() == (*i)->spellId)
            {
                m_target->m_reflectSpellSchool.erase(i);
            }
        }
        ReflectSpellSchool *rss = new ReflectSpellSchool;

        rss->chance = mod->m_amount;
        rss->spellId = GetSpellId();
        if(m_spellProto->Attributes == 0x400D0 && m_spellProto->AttributesEx == 0)
        {
            rss->school = (int)(log10((float)mod->m_miscValue) / log10((float)2));
        }
        else
            rss->school = m_spellProto->School;
            
        m_target->m_reflectSpellSchool.push_back(rss);
    }
    else
    {
        for(std::list<struct ReflectSpellSchool*>::iterator i = m_target->m_reflectSpellSchool.begin();i != m_target->m_reflectSpellSchool.end();i++)
        {
            if(GetSpellId() == (*i)->spellId)
            {
                delete *i;
                m_target->m_reflectSpellSchool.erase(i);
                break;
            }
        }
    }    
}

void Aura::SpellAuraModLanguage(bool apply)
{
    if(apply) 
        m_target->m_modlanguage = mod->m_miscValue;
    else 
        m_target->m_modlanguage = -1;
}

void Aura::SpellAuraAddFarSight(bool apply)
{
    if(apply)
    {
        if(m_target->GetTypeId() != TYPEID_PLAYER)
            return;
        
        //FIXME:Burlex aka Nublex will fix this
        //Make update circle bigger here
    }
    else
    {
        //Destroy new updated objects here if they are still out of update range
        //w/e
    }
}

void Aura::SpellAuraMechanicImmunity(bool apply)
{
    if(apply)
    {
        assert(mod->m_miscValue < 26);
        m_target->MechanicsDispels[mod->m_miscValue]++;

        if(mod->m_miscValue != 16 && mod->m_miscValue != 25 && mod->m_miscValue != 19) // dont remove bandages, Power Word and protection effect
        {
            for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
                if(m_target->m_auras[x])
                {
                    if(m_target->m_auras[x]->GetSpellProto()->MechanicsType == mod->m_miscValue)
                        m_target->m_auras[x]->Remove();
                    else if(mod->m_miscValue == 11) // if got immunity for slow, remove some that are not in the mechanics
                    {
                        for(int i=0;i<3;i++)
                            if(m_target->m_auras[x]->GetSpellProto()->EffectApplyAuraName[i] == SPELL_AURA_MOD_DECREASE_SPEED)
                            {
                                m_target->m_auras[x]->Remove();
                                break;
                            }
                    }    
                }
        }
        else
            SetNegative();
    }
    else
        m_target->MechanicsDispels[mod->m_miscValue]--;
}

void Aura::SpellAuraMounted(bool apply)
{
    if(!m_target->IsPlayer())
        return;        
        
    Player * plr = static_cast<Player*>(m_target);
    if(apply)
    {   
        plr->AdvanceSkillLine(762); // advance riding skill
        if(plr->m_bgInBattleground && plr->GetMapId() == 489)
        {
            if(plr->m_bgHasFlag)
                static_cast<WarsongGulch*>(plr->GetCurrentBattleground())->HandleBattlegroundEvent(m_target, NULL, BGEVENT_WSG_PLAYER_DIED_WITH_FLAG);
        }

        if(((Player*)m_target)->m_MountSpellId)
            m_target->RemoveAura(((Player*)m_target)->m_MountSpellId);
        ((Player*)m_target)->m_MountSpellId=m_spellProto->Id;
 
        SetPositive();
        m_target->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOUNT);
        m_target->SetFlag( UNIT_FIELD_FLAGS , U_FIELD_FLAG_MOUNT_SIT );
        CreatureInfo* ci = objmgr.GetCreatureName(mod->m_miscValue);
        if(!ci)
            return;
        uint32 displayId = ci->DisplayID;
        if(displayId != 0)
        {
            m_target->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID , displayId);
        }
    }
    else
    {
        ((Player*)m_target)->m_MountSpellId=0;
    
        m_target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID , 0);
        m_target->RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_MOUNT_SIT );

        if (m_target->GetUInt32Value(UNIT_FIELD_FLAGS) & U_FIELD_FLAG_LOCK_PLAYER )
            m_target->RemoveFlag( UNIT_FIELD_FLAGS, U_FIELD_FLAG_LOCK_PLAYER );
    }
}

void Aura::SpellAuraModDamagePercDone(bool apply)
{
     float val;
     if(apply)
     {
         val = mod->m_amount/100.0;
         if(val>0)
             SetPositive();
         else
             SetNegative();
     }
     else
        val =- mod->m_amount/100.0;

    if(m_target->IsPlayer())
    {    
        for(uint32 x=1;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->ModFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + x,val);
            }
        }
        
        if(mod->m_miscValue&1)
        {
            if(apply)
            {
                WeaponModifier md;
                md.value = mod->m_amount;
                md.wclass = GetSpellProto()->EquippedItemClass;
                md.subclass = GetSpellProto()->EquippedItemSubClass;
                ((Player*)m_target)->damagedone.insert(make_pair(GetSpellId(), md));
            }
            else 
            {
                /*std::list<WeaponModifier>::iterator i=((Player*)m_target)->damagedone.begin();

                for(;i!=((Player*)m_target)->damagedone.end();i++)
                {
                    if((*i).spellid==GetSpellId())
                    {
                        ((Player*)m_target)->damagedone.erase(i);
                        break;
                    }
                }*/
                ((Player*)m_target)->damagedone.erase(GetSpellId());
            }
        }    
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                ((Creature*)m_target)->ModDamageDonePct[x] += val;
            }
        }
    }
    
    if(mod->m_miscValue&1)
      m_target->CalcDamage();
}

void Aura::SpellAuraModPercStat(bool apply)    
{
    int32 val;
    if(apply)
    {
       val = mod->m_amount;
       if(val>0)
           SetPositive();
       else
           SetNegative();
    }
    else 
       val= -mod->m_amount;

    if (mod->m_miscValue == -1)//all stats
    {            
        if(m_target->IsPlayer())
        {
            for(uint32 x=0;x<5;x++)
            {
                if(mod->m_amount>0) 
                    ((Player*)m_target)->StatModPctPos[x] += val; 
                else  
                    ((Player*)m_target)->StatModPctNeg[x] -= val;
        
                ((Player*)m_target)->CalcStat(x);    
            }

            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            for(uint32 x=0;x<5;x++)
            {
                ((Creature*)m_target)->StatModPct[x] += val;
                ((Creature*)m_target)->CalcStat(x);            
            }
        }
    } 
    else
    {
        ASSERT(mod->m_miscValue < 5);
        if(m_target->IsPlayer())
        {
            if(mod->m_amount>0) 
                ((Player*)m_target)->StatModPctPos[mod->m_miscValue] += val; 
            else  
                ((Player*)m_target)->StatModPctNeg[mod->m_miscValue] -= val;

            ((Player*)m_target)->CalcStat(mod->m_miscValue);    
        
            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            ((Creature*)m_target)->StatModPct[mod->m_miscValue]+=val;
            ((Creature*)m_target)->CalcStat(mod->m_miscValue);
        }        
    }
}

void Aura::SpellAuraSplitDamage(bool apply)
{
    //FIXME:Find the flat one id of this.
    //This one uses percent as modifier.
    //Splits damage to party member.
    //There are two spells
    //Uses flags 1=2^0 is physical, 127 = all, 126 = spells etc
    if(apply)
    {
        if(mod->m_miscValue == 1)
        {
            m_target->m_meleeDamageSplitPCT++;
        }
        else if(mod->m_miscValue == 126)
        {
            m_target->m_meleeDamageSplitPCT++;
        }
        else if(mod->m_miscValue == 127)
        {
            m_target->m_meleeDamageSplitPCT++;
            m_target->m_meleeDamageSplitPCT++;
        }                
    }
    else
    {
        if(mod->m_miscValue == 1)
        {
            m_target->m_meleeDamageSplitPCT--;
        }
        else if(mod->m_miscValue == 126)
        {
            m_target->m_meleeDamageSplitPCT--;
        }
        else if(mod->m_miscValue == 127)
        {
            m_target->m_meleeDamageSplitPCT--;
            m_target->m_meleeDamageSplitPCT--;
        }      
        if(m_target->m_meleeDamageSplitPCT < 0)
            m_target->m_meleeDamageSplitPCT = 0;
    }
}

void Aura::SpellAuraModRegen(bool apply)
{
    if(apply)//seems like only positive
    {
        SetPositive ();
        sEventMgr.AddEvent(this, &Aura::EventPeriodicHeal1,(uint32)((this->GetSpellProto()->EffectBasePoints[mod->i]+1)/5)*3,
            EVENT_AURA_PERIODIC_HEAL,3000,0);
    }
}

void Aura::EventPeriodicHeal1(uint32 amount)
{
    if(!m_target->isAlive())
        return;

    uint32 ch = m_target->GetUInt32Value(UNIT_FIELD_HEALTH);
    ch+=amount;
    uint32 mh = m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
    
    if(ch>mh)
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH,mh);
    else
        m_target->SetUInt32Value(UNIT_FIELD_HEALTH,ch);

    if(GetSpellProto()->AuraInterruptFlags & AURA_INTERRUPT_ON_STAND_UP)
    {
        m_target->Emote(EMOTE_ONESHOT_EAT);
    }
}

void Aura::SpellAuraModPowerRegen(bool apply)
{
    if(apply)
    {
        SetPositive();
        sEventMgr.AddEvent(this, &Aura::EventPeriodicEnergize,(uint32)mod->m_amount,(uint32)mod->m_miscValue,
            EVENT_AURA_PERIODIC_ENERGIZE,5000,0);
    }    
}

void Aura::SpellAuraChannelDeathItem(bool apply)
{
    if(apply)
    {
        //dont need for now
    }
    else
    {
        if(m_target->GetCreatureName())
        {
            if(m_target->GetCreatureName()->Type != CRITTER)
            {
                if(m_target->getDeathState() == JUST_DIED)
                {
                       Player *pCaster = objmgr.GetPlayer(m_casterGuid);
                    if(!pCaster)
                        return;
                    /*int32 delta=pCaster->getLevel()-m_target->getLevel();
                    if(abs(delta)>5)
                        return;*/
                
                    uint32 itemid = GetSpellProto()->EffectSpellGroupRelation[mod->i];

                    ItemPrototype *proto = objmgr.GetItemPrototype(itemid);
                    if(pCaster->GetItemInterface()->CalculateFreeSlots(proto) > 0)
                    {
                        Item *item = objmgr.CreateItem(itemid,pCaster);
                        if(!item) return;


                        item->SetUInt64Value(ITEM_FIELD_CREATOR,pCaster->GetGUID());
                        if(!pCaster->GetItemInterface()->AddItemToFreeSlot(item))
                        {
                            pCaster->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
                            delete item;
                            return;
                        }
                        WorldPacket data;
                        pCaster->GetSession()->BuildItemPushResult(&data, pCaster->GetGUID(), 1, 1, itemid ,0,0xFF,1,0xFFFFFFFF);
                        pCaster->SendMessageToSet(&data, true);                    
                    }
                }
            }
        }
    }
}

void Aura::SpellAuraModDamagePercTaken(bool apply)
{
    float val;
    if(apply)
    {
        val = mod->m_amount/100.0;
        if(val > 0)
            SetNegative();
        else
            SetPositive();
    }
    else
    {
        val= -mod->m_amount/100.0;
    }
    
    if(m_spellProto->dummy = 2135) // Ardent Defender it only applys on 20% hp :/
    {
        m_target->DamageTakenPctModOnHP +=val;
        return;
    }
    
    for(uint32 x=0;x<7;x++)
    {
        if (mod->m_miscValue & (((uint32)1)<<x) )
        {
            m_target->DamageTakenPctMod[x]+=val;
            }
    }
}

void Aura::SpellAuraModRegenPercent(bool apply)
{
    if(apply)
        m_target->PctRegenModifier += mod->m_amount/100.0;
    else
        m_target->PctRegenModifier -= mod->m_amount/100.0;
}

void Aura::SpellAuraPeriodicDamagePercent(bool apply)
{
    if(apply)
    {
        //uint32 gr = GetSpellProto()->SpellGroupType;
        //if(gr)
        //{
        //    Unit*c=GetUnitCaster();
        //    if(c)
        //    {
        //        SM_FIValue(c->SM_FDOT,(int32*)&dmg,gr);
        //        SM_PIValue(c->SM_PDOT,(int32*)&dmg,gr);
        //    }        
        //}

        if(m_spellProto->Id == 28347) //Dimensional Siphon
        {
            uint32 dmg = (m_target->GetUInt32Value(UNIT_FIELD_MAXHEALTH)*5)/100;
            sEventMgr.AddEvent(this, &Aura::EventPeriodicDamagePercent, dmg, 
                EVENT_AURA_PERIODIC_DAMAGE_PERCENT, 1000, 0);
        }
        else
        {
            uint32 dmg = mod->m_amount;
            sEventMgr.AddEvent(this, &Aura::EventPeriodicDamagePercent, dmg, 
                EVENT_AURA_PERIODIC_DAMAGE_PERCENT,GetSpellProto()->EffectAmplitude[mod->i],0);
        }
        SetNegative();
    }
    else
    {
        sEventMgr.RemoveEvents(m_target, EVENT_AURA_PERIODIC_DAMAGE_PERCENT);
    }
}

void Aura::EventPeriodicDamagePercent(uint32 amount)
{
    //DOT
    if(!m_target->isAlive())
        return;
    if(m_target->SchoolImmunityList[GetSpellProto()->School])
        return;

    uint32 damage = amount;

    uint32 school = GetSpellProto()->School;
    Unit * c = GetUnitCaster();

 /*   if (c)
        SendPeriodicAuraLog(m_target, c, GetSpellProto()->Id, school, damage, FLAG_PERIODIC_DAMAGE);
    else
        SendPeriodicAuraLog(m_casterGuid, m_target, GetSpellProto()->Id, school, damage, FLAG_PERIODIC_DAMAGE);

    if(c)
        c->DealDamage(m_target, damage, 0, 0, GetSpellProto()->Id);
    else
        m_target->DealDamage(m_target, damage, 0, 0, GetSpellProto()->Id);*/
    if(c)
        c->SpellNonMeleeDamageLog(m_target, GetSpellProto()->Id, damage, true);
    else
        m_target->SpellNonMeleeDamageLog(m_target, GetSpellProto()->Id, damage, true);
}

void Aura::SpellAuraModResistChance(bool apply)
{
    apply ? m_target->m_resistChance = mod->m_amount : m_target->m_resistChance = 0;
}

void Aura::SpellAuraModDetectRange(bool apply)
{
    Unit*m_caster=GetUnitCaster();
    if(!m_caster)return;
    if(apply)
    {
        SetNegative();
        m_caster->setDetectRangeMod(m_target->GetGUID(), mod->m_amount);
    }
    else
    {
        m_caster->unsetDetectRangeMod(m_target->GetGUID());
    }
}

void Aura::SpellAuraPreventsFleeing(bool apply)
{
    // Curse of Recklessness 
}

void Aura::SpellAuraModUnattackable(bool apply)
{
    // U_FIELD_FLAG_MAKE_CHAR_UNTOUCHABLE
}

void Aura::SpellAuraInterruptRegen(bool apply)
{
    if(apply)
        m_target->m_interruptRegen++;
    else
    {
        m_target->m_interruptRegen--;
          if(m_target->m_interruptRegen < 0)
            m_target->m_interruptRegen = 0;
    }
}

void Aura::SpellAuraGhost(bool apply)
{
    if(apply)
    {
        SetNegative();
        ((Player*)m_target)->SetMovement(MOVE_WATER_WALK, 4);
    
    }else
    {
        ((Player*)m_target)->SetMovement(MOVE_LAND_WALK, 7);
    }  
}

void Aura::SpellAuraMagnet(bool apply)
{
    /*shaman got a totem called grounding totem
    if you cast it
    1 negative spell casted on you will be casted on that totem instead of you
    for example a damage spell
    so you wont get damage of that 1 spell
    next spell will deal damage on you of course*/
}

void Aura::SpellAuraManaShield(bool apply)
{
    if(apply)
    {
        SetPositive();
        m_target->m_manashieldamt = mod->m_amount ;
        m_target->m_manaShieldId = GetSpellId();
    }
    else
    {
        m_target->m_manashieldamt = 0;
        m_target->m_manaShieldId = 0;            
    }
}

void Aura::SpellAuraSkillTalent(bool apply)
{
    if (m_target->IsPlayer())
    {
        if(apply)
        {   
            SetPositive();
            ((Player*)m_target)->ModSkillBonus (mod->m_miscValue,mod->m_amount);
        }
        else  
            ((Player*)m_target)->ModSkillBonus (mod->m_miscValue,-mod->m_amount);
       
        ((Player*)m_target)->UpdateChances();
    }
}

void Aura::SpellAuraModAttackPower(bool apply)
{
    if(mod->m_amount>0)
        SetPositive();
    else
        SetNegative();
    m_target->ModUInt32Value(UNIT_FIELD_ATTACK_POWER_MODS,apply? mod->m_amount : -mod->m_amount);
    m_target->CalcDamage();
}

void Aura::SpellAuraVisible(bool apply)
{
    //Show positive spells on target
    if(apply)
    {
        SetNegative();
    }
}

void Aura::SpellAuraModResistancePCT(bool apply)
{
    uint32 Flag = mod->m_miscValue; 
    int32 amt;
    if(apply)
    {
        amt=mod->m_amount;
     //   if(amt>0)SetPositive();
       // else SetNegative();
    }
    else 
        amt= -mod->m_amount;  
      
    for(uint32 x=0;x<7;x++)
    {
        if(Flag & (((uint32)1)<< x))
        {
            if(m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if(mod->m_amount>0)
                {
                    ((Player*)m_target)->ResistanceModPctPos[x] += amt;
                }
                else
                {
                    ((Player*)m_target)->ResistanceModPctNeg[x] -= amt;
                }
                ((Player*)m_target)->CalcResistance(x);
              
            }
            else
            {
                ((Creature*)m_target)->ResistanceModPct[x] += amt;
                ((Creature*)m_target)->CalcResistance(x);
            }
        }
    }    
}

void Aura::SpellAuraModCreatureAttackPower(bool apply)
{
    if(apply)
    {
        for(uint32 x=0;x<7;x++)
            if (mod->m_miscValue & (((uint32)1)<<x) )
                m_target->CreatureAttackPowerMod[x+1] += mod->m_amount;
        if(mod->m_amount > 0)
            SetPositive();
        else
            SetNegative();
    }
    else
    {
        for(uint32 x=0;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                m_target->CreatureAttackPowerMod[x+1] -= mod->m_amount;
            }
        }
    }
}

void Aura::SpellAuraModTotalThreat(bool apply)
{
    if(apply)
    {
        if(mod->m_amount < 0)
            SetPositive();
        else
            SetNegative();
        if(m_target->GetThreatModifyer() > mod->m_amount) // replace old mod
        {
            m_target->ModThreatModifyer(0);
            m_target->ModThreatModifyer(mod->m_amount);
        }
    }
    else
    {
        if(m_target->GetThreatModifyer() == mod->m_amount) // only remove it if it hasn't been replaced yet
        {
            m_target->ModThreatModifyer(-(mod->m_amount));
        }
    }
}

void Aura::SpellAuraWaterWalk(bool apply)
{     
    if (m_target->IsPlayer())
    {
        WorldPacket data; 
        if(apply)
        {
            SetPositive();
            data.SetOpcode(SMSG_MOVE_WATER_WALK);
            data << m_target->GetNewGUID();
            data << uint32(8);       
        }
        else
        {
            data.SetOpcode(SMSG_MOVE_LAND_WALK);
            data << m_target->GetNewGUID();
            data << uint32(4);        
        }
        ((Player*)m_target)->GetSession()->SendPacket(&data);
    }
}

void Aura::SpellAuraFeatherFall(bool apply)
{
    //FIXME:Find true flag for this
    if(!m_target->IsPlayer())return;

    WorldPacket data;
    if(apply)
    {
        SetPositive();
        data.SetOpcode(SMSG_MOVE_FEATHER_FALL);
        ((Player*)m_target)->bSafeFall = true;
    }
    else 
    {
        data.SetOpcode(SMSG_MOVE_NORMAL_FALL);
        ((Player*)m_target)->bSafeFall = false;
    }
  
    data << m_target->GetNewGUID();
    data << uint32(0);
    ((Player*)m_target)->GetSession()->SendPacket(&data);
}

void Aura::SpellAuraHover(bool apply)
{
    SetPositive();
      
    //FIXME:Find true flag for this
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data(apply?SMSG_MOVE_SET_HOVER:SMSG_MOVE_UNSET_HOVER, 13);
        data << m_target->GetNewGUID();
        data << uint32(0);
        ((Player*)m_target)->GetSession()->SendPacket(&data);
    }
}

void Aura::SpellAuraAddPctMod(bool apply)
{
    int32 val = apply?mod->m_amount:-mod->m_amount;
    uint32 AffectedGroups = GetSpellProto()->EffectSpellGroupRelation[mod->i];

    switch (mod->m_miscValue)//let's generate warnings for unknown types of modifiers
    {
    case SMT_CRITICAL:
        ApplyFloatSM(&m_target->SM_CriticalChance,val,AffectedGroups, 0);
        break;

    case SMT_DURATION:
        ApplyIntSM(&m_target->SM_PDur,val,AffectedGroups, 0);
        break;

    case SMT_RADIUS:
        ApplyFloatPSM(&m_target->SM_PRadius,val,AffectedGroups, 1);
        break;

    case SMT_RANGE:
        ApplyFloatPSM(&m_target->SM_PRange,val,AffectedGroups, 1);
        break;

    case SMT_CAST_TIME:
        ApplyIntSM(&m_target->SM_PCastTime,val,AffectedGroups, 0); 
        break;

    case SMT_COST:
        ApplyIntSM(&m_target->SM_PCost,val,AffectedGroups, 0); 
        break;

    case SMT_CRITICAL_DAMAGE:
        ApplyIntSM(&m_target->SM_PCriticalDamage,val,AffectedGroups, 0); 
        break;

    case SMT_DAMAGE_DONE_PERIODIC:
        ApplyIntSM(&m_target->SM_PDOT,val,AffectedGroups, 0); 
        break;

    case SMT_NONINTERRUPT:
        ApplyFloatSM(&m_target->SM_PNonInterrupt,val,AffectedGroups, 0); 
        break;

    case SMT_JUMP_REDUCE:
        ApplyIntSM(&m_target->SM_PJumpReduce,val,AffectedGroups, 0); 
        break;

    case SMT_EFFECT_BONUS:
        ApplyIntSM(&m_target->SM_PEffectBonus,val,AffectedGroups, 0);
        break;

    case SMT_DAMAGE_DONE:
        ApplyIntSM(&m_target->SM_PDamageBonus,val,AffectedGroups, 0);
        break;

    case SMT_DUMMY:
        ApplyIntSM(&m_target->SM_PDummy,val,AffectedGroups, 0);
        break;

    case SMT_ATTACK_POWER_BONUS:
        ApplyIntSM(&m_target->SM_PAPBonus,val,AffectedGroups, 0); 
        break;

    case SMT_BLOCK:
    case SMT_TREAT_REDUCED:
    case SMT_COOLDOWN_DECREASE:
    case SMT_TRIGGER:
    case SMT_TIME:
        break;
    default://unknown Modifier type
        sLog.outError(
            "Unknown spell modifier type %u in spell %u.<<--report this line to the developer\n",
            mod->m_miscValue,GetSpellId());
        //don't add val, though we could formaly could do,but as we don't know what it is-> no sense
        break;
    }

    if(m_target->IsPlayer())
    {
        int32 val;
        if(apply)
            val = mod->m_amount;
        else
            val = 0;
        WorldPacket data(SMSG_SET_PCT_SPELL_MODIFIER, 6);
        for(uint32 x=0; x<SPELL_GROUPS; x++)
        {
            if(AffectedGroups & (1<<x))
            {
                data << uint8(x);//group
                data << uint8(mod->m_miscValue);//type 
                data << int32(val);//value
                ((Player*)m_target)->GetSession()->SendPacket(&data);
                data.clear();
            }
        }
    }
}

void Aura::SpellAuraAddTargetTrigger(bool apply)
{
    uint32 spellid = GetSpellId();
    int32 val = mod->m_amount;

    /*
    spellid: 11071
    amount: 5
    type = 109
    misc valye 0

    spellid = 11095
    amount 33
    type 109
    misc = 0

    spellid = 12499
    amount 15
    type 109
    misc = 0

    spellid = 14179
    amount 0
    type 109
    misc = 0

    */

}

void Aura::SpellAuraModPowerRegPerc(bool apply)
{
    if(apply)
        m_target->PctPowerRegenModifier += mod->m_amount/100.0;
    else
        m_target->PctPowerRegenModifier -= mod->m_amount/100.0;
}

void Aura::SpellAuraModRangedDamageTaken(bool apply)
{
    if(apply)
        m_target->RangedDamageTaken += mod->m_amount;
    else
    {
        m_target->RangedDamageTaken -= mod->m_amount;;
        if( m_target->RangedDamageTaken < 0)
            m_target->RangedDamageTaken = 0;
    }      
}

void Aura::SpellAuraModHealing(bool apply)
{
    int32 val;
    if(apply)
    {
         val = mod->m_amount;
         /*if(val>0)
             SetPositive();
         else 
             SetNegative();*/
    }
    else
        val=-mod->m_amount;
    
    for(uint32 x=0;x<7;x++)
    {
        if (mod->m_miscValue & (((uint32)1)<<x) )
        {
            m_target->HealTakenMod[x] += val;
        }
    }
}

void Aura::SpellAuraModMechanicResistance(bool apply)
{
    //FIXME:Find formula for this to implement mechanic resistance
}

void Aura::SpellAuraModRangedAttackPower(bool apply)
{
    if(apply)
    {
        if(mod->m_amount > 0)
            SetPositive();
        else
            SetNegative();
        m_target->ModUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS,mod->m_amount);
    }
    else
        m_target->ModUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS,-mod->m_amount);
}

void Aura::SpellAuraModMeleeDamageTaken(bool apply)
{
    if(apply)
    {
        if(mod->m_amount > 0)//does not exist but let it be
            SetNegative();
        else 
            SetPositive();
        m_target->DamageTakenMod[0] += mod->m_amount;
    }
    else
        m_target->DamageTakenMod[0] -= mod->m_amount;
}

void Aura::SpellAuraModMeleeDamageTakenPct(bool apply)
{
    if(apply)
    {
        if(mod->m_amount>0)//does not exist but let it be
            SetNegative();
        else 
            SetPositive();
        m_target->DamageTakenPctMod[0]+=mod->m_amount;
    }
    else
        m_target->DamageTakenPctMod[0]-=mod->m_amount;
}

void Aura::SpellAuraRAPAttackerBonus(bool apply)
{
    if(apply)
    {
        m_target->RAPvModifier += mod->m_amount;
    }
    else 
        m_target->RAPvModifier -= mod->m_amount;
}

void Aura::SpellAuraModIncreaseSpeedAlways(bool apply)
{
    if(apply)
    {  
        SetPositive();
        m_target->m_speedModifier -= mod->m_amount;//?????
    }
    else
        m_target->m_speedModifier += mod->m_amount;

    m_target->UpdateSpeed();
}

void Aura::SpellAuraModIncreaseEnergyPerc(bool apply)
{
    SetPositive();
    uint32 maxField ;
    uint8 powerType = m_target->GetPowerType();
    if(powerType == POWER_TYPE_MANA) // Mana
    {
        maxField = UNIT_FIELD_MAXPOWER1;
    }
    else if(powerType == POWER_TYPE_RAGE) // Rage
    {
        maxField = UNIT_FIELD_MAXPOWER2;
    }
    else if(powerType == POWER_TYPE_ENERGY) // Energy
    {
        maxField = UNIT_FIELD_MAXPOWER4;
    }
    if(apply)
    {
        m_target->ModPUInt32Value(maxField, mod->m_amount, apply);
    }
}

void Aura::SpellAuraModIncreaseHealthPerc(bool apply)
{
    SetPositive();
    m_target->ModPUInt32Value(UNIT_FIELD_MAXHEALTH, mod->m_amount,apply);
}

void Aura::SpellAuraModManaRegInterrupt(bool apply)
{
    //make regen always active
    // allows you to continue %30 of mana regen in combat
}

void Aura::SpellAuraModTotalStatPerc(bool apply)
{
    int32 val;
    if(apply)
    {
       val= mod->m_amount;
//       if(val>0)SetPositive();
  //     else
    //       SetNegative();
    }
    else 
       val= -mod->m_amount;

    if (mod->m_miscValue == -1)//all stats
    {
        if(m_target->IsPlayer())
        {
            for(uint32 x=0;x<5;x++)
            {
                if(mod->m_amount>0)
                    ((Player*)m_target)->TotalStatModPctPos[x] += val; 
                else  
                    ((Player*)m_target)->TotalStatModPctNeg[x] -= val;
                ((Player*)m_target)->CalcStat(x);    
            }

            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            for(uint32 x=0;x<5;x++)
            {
                ((Creature*)m_target)->TotalStatModPct[x] += val;
                ((Creature*)m_target)->CalcStat(x);            
            }
        }
    } 
    else
    {
        ASSERT(mod->m_miscValue < 5);
        if(m_target->IsPlayer())
        {
            if(mod->m_amount>0)
                ((Player*)m_target)->TotalStatModPctPos[mod->m_miscValue] += val; 
            else  
                ((Player*)m_target)->TotalStatModPctNeg[mod->m_miscValue] -= val;        
            
            ((Player*)m_target)->CalcStat(mod->m_miscValue);    
            ((Player*)m_target)->UpdateStats();
            ((Player*)m_target)->UpdateChances();
        }
        else
        {
            ((Creature*)m_target)->TotalStatModPct[mod->m_miscValue]+=val;
            ((Creature*)m_target)->CalcStat(mod->m_miscValue);
        }        
    }
}

void Aura::SpellAuraModHaste(bool apply)
{
    if(mod->m_amount>0)
        SetPositive();
    else SetNegative();

    if(abs(mod->m_amount) == 100)
        return; // hacky fix

    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        if(apply)
        {
            //SetPositive();            
              ((Player*)m_target)->m_meleeattackspeedmod += mod->m_amount;
        }
          else
            ((Player*)m_target)->m_meleeattackspeedmod -= mod->m_amount;

        uint32 speed;
        Item *weap = ((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
        if(weap)
            speed = weap->GetProto()->Delay;
        else 
            speed = 2000;
        m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, (uint32)(speed*(1.0-(float)(((Player*)m_target)->m_meleeattackspeedmod)/100.0)));

        weap = ((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
        if(weap)
        {
            speed=weap->GetProto()->Delay;
            m_target->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, (uint32)(speed*(1.0-(float)(((Player*)m_target)->m_meleeattackspeedmod)/100.0)));
        }
    }
    else
    {
        m_target->ModPUInt32Value(UNIT_FIELD_BASEATTACKTIME, mod->m_amount,!apply);
        m_target->ModPUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, mod->m_amount,!apply);
    }
}

void Aura::SpellAuraForceReaction(bool apply)
{
    if (apply)
    {
        //SetCasterFaction(m_target->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
        SetCasterFaction(m_target->_getFaction());
        SetPositive();
        m_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, mod->m_miscValue);
    }
    else
    {
        m_target->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, GetCasterFaction() );
    }
    m_target->_setFaction();
    m_target->UpdateOppFactionSet();
}

void Aura::SpellAuraModRangedHaste(bool apply)
{
    SetPositive();
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        if(apply)
            ((Player*)m_target)->m_rangedattackspeedmod -= mod->m_amount;
        else
            ((Player*)m_target)->m_rangedattackspeedmod += mod->m_amount;

        uint32 speed;
        Item *weap = ((Player*)m_target)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
        if(weap)
        {
            speed = weap->GetProto()->Delay;
            m_target->SetUInt32Value(UNIT_FIELD_RANGEDATTACKTIME, (uint32)(speed*(1.0+(float)(((Player*)m_target)->m_rangedattackspeedmod)/100.0)));
        }
    }
    else
    {
        m_target->ModPUInt32Value(UNIT_FIELD_RANGEDATTACKTIME, mod->m_amount,!apply);
    }
}

void Aura::SpellAuraModRangedAmmoHaste(bool apply)
{
    SetPositive();
    if (m_target->GetTypeId() == TYPEID_PLAYER)
    {
        if(apply)
        {
            if(!((Unit*)m_target)->GetCurrentSpell())
                return;
                
            AmmoAttackSpeed *as = new AmmoAttackSpeed;
            m_castedItemId = as->itemId = ((Unit*)m_target)->GetCurrentSpell()->castedItemId;

            ((Player*)m_target)->m_ammoAttackSpeed.push_back(as);
        }
        else
        {
            std::list<AmmoAttackSpeed*>::iterator itr;
            for(itr = ((Player*)m_target)->m_ammoAttackSpeed.begin();
                itr != ((Player*)m_target)->m_ammoAttackSpeed.end(); ++itr)
            {
                if((*itr)->itemId == m_castedItemId)
                {
                    delete (*itr);
                    ((Player*)m_target)->m_ammoAttackSpeed.erase(itr);               
                }
            }
        }
    }
}

void Aura::SpellAuraModResistanceExclusive(bool apply)
{
    SpellAuraModResistance(apply);
}

void Aura::SpellAuraRetainComboPoints(bool apply)
{
    if(!GetCaster()->IsPlayer())
        return;

    Player *p_caster = static_cast<Player*>(GetCaster());

    if(apply)
    {      
        if(p_caster->GetUInt64Value(PLAYER__FIELD_COMBO_TARGET) != p_caster->GetSelection())
        {
            p_caster->SetUInt64Value(PLAYER__FIELD_COMBO_TARGET, p_caster->GetSelection());
            p_caster->SetCP(mod->m_amount);
        }
        else
        {
            uint32 comboPoints = p_caster->GetCP()+mod->m_amount;
            p_caster->SetCP((comboPoints>=5) ? 5 : comboPoints);
        }
    }
    else
    {
        if(p_caster->GetUInt64Value(PLAYER__FIELD_COMBO_TARGET) != p_caster->GetSelection())
        {
            p_caster->SetUInt64Value(PLAYER__FIELD_COMBO_TARGET, p_caster->GetSelection());
            p_caster->SetCP(mod->m_amount);
        }
        else
        {
            uint32 comboPoints = p_caster->GetCP()-mod->m_amount;
            p_caster->SetCP((comboPoints<0) ? 0 : comboPoints);
        }

    }
}

void Aura::SpellAuraModShieldBlockPCT(bool apply)
{
    Unit *caster;
    if(!GetUnitCaster()->IsPlayer())
        return;

    caster = GetUnitCaster();

    caster->ModFloatValue(PLAYER_BLOCK_PERCENTAGE, apply ? mod->m_amount : -mod->m_amount);
}

void Aura::SpellAuraTrackStealthed(bool apply)
{
    if(!GetUnitCaster())
        return;

    GetUnitCaster()->trackStealth = (apply ? true : false);
}

void Aura::SpellAuraModStealthLevel(bool apply)
{
    if(apply)
    {
        SetPositive();
        m_target->m_stealthLevel += mod->m_amount;
    }
    else 
        m_target->m_stealthLevel -= mod->m_amount;
}

void Aura::SpellAuraSafeFall(bool apply)
{
    //FIXME:Find true flag
    if(m_target->GetTypeId() == TYPEID_PLAYER)
    {
        WorldPacket data;
        if(apply)
        {
            SetPositive();
            data.SetOpcode(SMSG_MOVE_FEATHER_FALL);
            ((Player*)m_target)->bSafeFall = true;
        }
        else
        {
            data.SetOpcode(SMSG_MOVE_NORMAL_FALL);
            ((Player*)m_target)->bSafeFall = false;
        }
        data << m_target->GetNewGUID();
        data << uint32(0);
   
        ((Player*)m_target)->GetSession()->SendPacket(&data);
    }
}

void Aura::SpellAuraModReputationAdjust(bool apply)
{
    /*SetPositive();
    bool updateclient = true;
    if(IsPassive())
        updateclient = false;     // dont update client on passive

    if(m_target->GetTypeId()==TYPEID_PLAYER)
    {
        if(apply)
              ((Player*)m_target)->modPercAllReputation(mod->m_amount, updateclient);
          else
            ((Player*)m_target)->modPercAllReputation(-mod->m_amount, updateclient);
    }*/

    // This is _actually_ "Reputation gains increased by x%."
    // not increase all rep by x%.

    SetPositive();
    if(apply)
        ((Player*)m_target)->pctReputationMod += mod->m_amount;
    else
        ((Player*)m_target)->pctReputationMod -= mod->m_amount;
}

void Aura::SpellAuraModHealthRegInCombat(bool apply)
{
    // demon armor etc, they all seem to be 5 sec.
    if(apply)
    {
        sEventMgr.AddEvent(this, &Aura::EventPeriodicHeal1, uint32(mod->m_amount), EVENT_AURA_PERIODIC_HEAL, 5000, 0);
    }
}

void Aura::SpellAuraWaterBreathing(bool apply)
{
   if(m_target->GetTypeId() == TYPEID_PLAYER)
   {
       if(apply)
       {
            SetPositive();
               WorldPacket data(4);
            data.SetOpcode(SMSG_STOP_MIRROR_TIMER);
            data << uint32(1);
            ((Player*)m_target)->GetSession()->SendPacket(&data);
            ((Player*)m_target)->m_UnderwaterState = 0;            
       }

       ((Player*)m_target)->m_bUnlimitedBreath=apply;
   }
}

void Aura::SpellAuraAddFlatModifier(bool apply)
{
    int32 val = apply?mod->m_amount:-mod->m_amount;
    uint32 AffectedGroups = GetSpellProto()->EffectSpellGroupRelation[mod->i];

    switch (mod->m_miscValue)//let's generate warnings for unknown types of modifiers
    {
    case SMT_CRITICAL:
        ApplyFloatSM(&m_target->SM_CriticalChance,val,AffectedGroups, 0);
        break;

    case SMT_DURATION:
        ApplyIntSM(&m_target->SM_FDur,val,AffectedGroups, 0);
        break;

    case SMT_RADIUS:
        ApplyFloatSM(&m_target->SM_FRadius,val,AffectedGroups, 0);
        break;

    case SMT_RANGE:
        ApplyFloatSM(&m_target->SM_FRange,val,AffectedGroups, 0);
        break;

    case SMT_CAST_TIME:
        ApplyIntSM(&m_target->SM_FCastTime,val,AffectedGroups, 0); 
        break;

    case SMT_COST:
        ApplyIntSM(&m_target->SM_FCost,val,AffectedGroups, 0); 
        break;

    case SMT_DAMAGE_DONE_PERIODIC:
        ApplyIntSM(&m_target->SM_FDOT,val,AffectedGroups, 0); 
        break;

    case SMT_ADDITIONAL_TARGET:
        ApplyIntSM(&m_target->SM_FAdditionalTargets,val,AffectedGroups, 0); 
        break;

    case SMT_DAMAGE_DONE:
        ApplyIntSM(&m_target->SM_FDamageBonus,val,AffectedGroups, 0);
        break;

    case SMT_SPEED:
        ApplyIntSM(&m_target->SM_FSpeedMod,val,AffectedGroups, 0);
        break;

    case SMT_DUMMY:
        ApplyIntSM(&m_target->SM_FDummy,val,AffectedGroups, 0);
        break;

    case SMT_RESIST:
        ApplyFloatSM(&m_target->SM_FResist,val,AffectedGroups, 0);
        break;

        // as far as I know its not yet used!!!
    case SMT_PENALTY:
        ApplyIntSM(&m_target->SM_FPenalty,val,AffectedGroups, 0);
        break;

    case SMT_TREAT_REDUCED:
    case SMT_BLOCK:

    case SMT_COOLDOWN_DECREASE:

    case SMT_TRIGGER:
    case SMT_TIME:
        break;
    default://unknown Modifier type
        sLog.outError(
            "Unknown spell modifier type %u in spell %u.<<--report this line to the developer\n",
            mod->m_miscValue,GetSpellId());
        //don't add val, though we could formaly could do,but as we don't know what it is-> no sense
        break;
    }

    //send packet to client (i.e. apply visual bonus)
    if(m_target->IsPlayer())
    {
        int32 val;
        if(apply)
            val = mod->m_amount;
        else
            val = 0;
        WorldPacket data(SMSG_SET_FLAT_SPELL_MODIFIER, 6);
        for(uint32 x=0;x<SPELL_GROUPS;x++)
        {
            if(AffectedGroups & (1<<x))
            {
                data << uint8(x);//group
                data << uint8(mod->m_miscValue);//type 
                data << int32(val);//value
                ((Player*)m_target)->GetSession()->SendPacket(&data);
                data.clear();
            }
        }
    }
}

void Aura::SpellAuraModHealingDone(bool apply)
{
    int32 val;
    if(apply)
    {
      val=mod->m_amount;
      if(val>0)
          SetPositive();
      else 
          SetNegative();
    }
    else 
        val = -mod->m_amount;

    for(uint32 x=0;x<7;x++)
    {
        if (mod->m_miscValue  & (((uint32)1)<<x) )
        {
            m_target->HealDoneMod[x] += val;
        }
    }
    if(m_target->IsPlayer())
        ((Player*)m_target)->UpdateChanceFields();
}

void Aura::SpellAuraModHealingDonePct(bool apply)
{
    int32 val;
    if(apply)
    {
      val=mod->m_amount;
      if(val>0)
          SetPositive();
      else 
          SetNegative();
    }
    else 
        val=-mod->m_amount;

    for(uint32 x=0;x<7;x++)
    {
        if (mod->m_miscValue  & (((uint32)1)<<x) )
        {
            m_target->HealDonePctMod[x] += val;
        }
    }
}

void Aura::SpellAuraEmphaty(bool apply)
{
    SetPositive();
    Unit * caster = GetUnitCaster();
    if(caster == 0 || !m_target || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // Show extra info about beast
    uint32 dynflags = m_target->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
    if(apply)
        dynflags |= U_DYN_FLAG_PLAYER_INFO;

    m_target->BuildFieldUpdatePacket(static_cast<Player*>(caster), UNIT_DYNAMIC_FLAGS, dynflags);
}

void Aura::SpellAuraModOffhandDamagePct(bool apply)
{
    //Used only by talents of rogue and warrior;passive,positive
    if(apply)
    {
        SetPositive();
        ((Player*)m_target)->offhand_dmg_mod += float(mod->m_amount)/100.0;
    }
    else
        ((Player*)m_target)->offhand_dmg_mod -= float(mod->m_amount)/100.0;

    m_target->CalcDamage();
}

void Aura::SpellAuraIncreaseArmorByPctInt(bool apply)
{
    uint32 i_Int = m_target->GetUInt32Value(UNIT_FIELD_STAT3);
    uint32 Flag = mod->m_miscValue;

    int32 amt = i_Int * ((float)mod->m_amount / 100.0f);
    if(apply)
    {
        m_dynamicValue = amt;
        if(amt > 0)
            SetPositive();
        else
            SetNegative();
    }
    else 
        amt = m_dynamicValue;

    for(uint32 x=0;x<7;x++)
    {
        if(Flag & (((uint32)1)<< x))
        {
            if(m_target->GetTypeId() == TYPEID_PLAYER)
            {
                if(mod->m_amount > 0)
                {
                    ((Player*)m_target)->FlatResistanceModifierPos[x] += amt;
                }
                else
                {
                    ((Player*)m_target)->FlatResistanceModifierNeg[x] -= amt;
                }
                ((Player*)m_target)->CalcResistance(x);

            }
            else
            {
                ((Creature*)m_target)->FlatResistanceMod[x] += amt;
                ((Creature*)m_target)->CalcResistance(x);
            }
        }
    }
}

void Aura::SpellAuraIncreaseTimeBetweenAttacksPCT(bool apply)
{
    if(!m_target)
        return;

    if(apply)
    {
        m_target->modAttackTimeIncreasePCT -= mod->m_amount;        
        m_target->ModPFloatValue(UNIT_MOD_CAST_SPEED, -mod->m_amount, true);
    }
    else
    {
        m_target->modAttackTimeIncreasePCT += mod->m_amount;
        m_target->ModPFloatValue(UNIT_MOD_CAST_SPEED, +mod->m_amount, false);
    }
}

void Aura::SpellAuraIncreaseSpellDamagebyInt(bool apply)
{
     float val;
     if(apply)
     {
         val = mod->m_amount/100.0;
         if(val>0)
             SetPositive();
         else
             SetNegative();
     }
     else
        val =- mod->m_amount/100.0;
        
    if(m_target->IsPlayer())
    {    
        for(uint32 x=1;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                ((Player*)m_target)->SpellDmgDoneByInt[x]+=val;
            }
        }
    }
}

void Aura::SpellAuraIncreaseHpbyInt(bool apply)
{
     float val;
     if(apply)
     {
         val = mod->m_amount/100.0;
         if(val>0)
             SetPositive();
         else
             SetNegative();
     }
     else
        val =- mod->m_amount/100.0;
        
    if(m_target->IsPlayer())
    {    
        for(uint32 x=1;x<7;x++)
        {
            if (mod->m_miscValue & (((uint32)1)<<x) )
            {
                ((Player*)m_target)->SpellHealDoneByInt[x]+=val;
            }
        }
    }

}

void Aura::SpellAuraIncreaseRageFromDamageDealtPCT(bool apply)
{
    if(!m_target)
        return;

    if(!m_target->IsPlayer())
        return;

    static_cast<Player *>(m_target)->rageFromDamageDealt += (apply) ? mod->m_amount : -mod->m_amount;
}

int32 Aura::event_GetInstanceID()
{
    return m_target->event_GetInstanceID();
}

void Aura::RelocateEvents()
{
    event_Relocate();
}

void Aura::SpellAuraEnableFlight(bool apply)
{
    if(!m_target->IsPlayer()) return;

    if(apply)
    {
        WorldPacket data(SMSG_MOVE_SET_FLY, 13);
        data << m_target->GetNewGUID();
        data << uint32(2);
        static_cast<Player*>(m_target)->GetSession()->SendPacket(&data);
    }
    else
    {
        WorldPacket data(SMSG_MOVE_SET_UNFLY, 13);
        data << m_target->GetNewGUID();
        data << uint32(5);
        static_cast<Player*>(m_target)->GetSession()->SendPacket(&data);
    }

    static_cast<Player*>(m_target)->FlyCheat = apply;
}
