#pragma once
#include <string>
#include "../Menu/gui.h"

//ESP tab 
static bool TCToggle = false;
static bool PlayerToggle = false;
static bool OreToggle = false;
static bool ItemsToggle	= false;
static bool CollectiblesToggle = false;
static bool OtherToggle = false;
static bool BaseToggle = false;

//player settings
static bool PlayerBoxToggle	= false;
static bool PlayerSkeletonToggle = false;
static bool PlayerNameToggle = false;
static bool PlayerDistanceToggle = false;
static bool PlayerHealthToggle = false;
static bool PlayerHotbarToggle = false;
static bool PlayerWeaponToggle = false;


// TC settings
static bool TCAuthToggle = false;
static bool UpKeepToggle = false;
static bool TcEspToggle	= false; 

//Ore Settings

static bool StoneOreToggle = false;
static bool MetalOreToggle = false;
static bool SulfurOreToggle	= false;


//Collectibles


static bool MetalPickup = false;
static bool StonePickup = false;
static bool SulfurPickup = false;
static bool HempPickup = false;
static bool WoodPickup = false;
static bool MushroomPickup = false;

//Bases

static bool LargeStorage = false;
static bool SmallStorage = false;
static bool Coffins = false;
static bool VendingMachine = false;
static bool SleepingBag = false;
static bool Workbench1 = false;
static bool Workbench2 = false;
static bool Workbench3 = false;

//Other

static bool Stashes = false;
static bool AirDrop = false;
static bool Backpacks = false;
static bool HeliCrates = false;
static bool BrownCrates = false;
static bool MiliCrate = false;
static bool EliteCrate = false;



//Vehicles

static bool Boats = false;
static bool Horses = false;
static bool Helicopters = false;
static bool AttackHeli = false;
static bool Bradley = false;

///////////////////////////////////////////////////////////


//Aimbot Tab


static bool WoundedToggle = false;
static bool ScientistToggle	= false;
static bool SilentToggle = false;
static bool TriggerBotToggle = false;

static bool NoRecoil = false;
static bool SmoothnessToggle = false;
static bool MagicBulletToggle = false;

static bool HeadAimbot = false;
static bool ArmAimbot = false;
static bool ChestAimbot = false;
static bool FootAimbot = false;
static bool LegAimbot = false;





///////////////////////////////////////////////////////////


//Weapons Tab
static bool WeaponsList	= false;

//5.56  bullet guns
static bool AK47ESP	= false;
static bool LR300ESP = false;
static bool HMLMGESP = false;
static bool M249ESP	= false;
static bool L96ESP = false;
static bool BoltactionESP = false;
static bool SARESP = false;
static bool M39ESP = false;

//pistol bullet guns
static bool MP5ESP = false;
static bool CUSTOMSMGESP = false;
static bool THOMPSONESP = false;
static bool REVOLVERESP	= false;
static bool PYTHONESP = false;
static bool SAPESP = false;
static bool M92ESP = false;

//shotgun round guns
static bool PUMPESP	= false;
static bool WATERPIPEESP = false;
static bool DOUBLEBARRELESP	= false;
static bool SPASESP	= false;
static bool MGLESP = false;

//Other Guns
static bool RocketlauncherESP = false;
static bool EokaESP = false;

///////////////////////////////////////////////////////////


//Movement tab



///////////////////////////////////////////////////////////


//Misc tab

static int i = 70;

static bool SpiderMan = false;
static bool FOVToggle = false;
static bool NoFall = false;
static bool AdminMode = false;
static bool Debug = false;


///////////////////////////////////////////////////////////


//Config tab


///////////////////////////////////////////////////////////


static int playerHealth = 75;

ImU32 selectedColor = IM_COL32(255, 0, 0, 255);  // Initial color


char Previewname[24] = "";



///////////////////////////////////////////////////////////


//Hotkeys


///////////////////////////////////////////////////////////




char HeadAimbotHotkey[24] = "";
static bool HeadisEditingHotkey = false;
const char* HeadhotkeyText = HeadisEditingHotkey ? "Press a key..." : HeadAimbotHotkey;


char ArmAimbotHotkey[24] = "";
static bool ArmisEditingHotkey = false;
const char* ArmhotkeyText = ArmisEditingHotkey ? "Press a key..." : ArmAimbotHotkey;

char ChestAimbotHotkey[24] = "";
static bool ChestisEditingHotkey = false;
const char* ChestkeyText = ChestisEditingHotkey ? "Press a key..." : ChestAimbotHotkey;




char FootAimbotHotkey[24] = "";
static bool FootisEditingHotkey = false;
const char* FootkeyText = FootisEditingHotkey ? "Press a key..." : FootAimbotHotkey;


char LegAimbotHotkey[24] = "";
static bool LegisEditingHotkey = false;
const char* LegkeyText = LegisEditingHotkey ? "Press a key..." : LegAimbotHotkey;









