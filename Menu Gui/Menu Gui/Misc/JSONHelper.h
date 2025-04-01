#pragma once

#include "../imgui/imgui.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Button Values.h"





using json = nlohmann::json;

void SaveSettingsToJson()
{
    // Create a JSON object
    json j;
    //ESP

    j["TCToggle"] = TCToggle;
    j["PlayerToggle"] = PlayerToggle;
    j["BaseToggle"] = BaseToggle;
    j["OtherToggle"] = OtherToggle;
    j["CollectiblesToggle"] = CollectiblesToggle;
    j["OreToggle"] = OreToggle;
    j["WoundedToggle"] = WoundedToggle;
    j["PlayerBoxToggle"] = PlayerBoxToggle;
    j["PlayerSkeletonToggle"] = PlayerSkeletonToggle;
    j["PlayerNameToggle"] = PlayerNameToggle;
    j["PlayerDistanceToggle"] = PlayerDistanceToggle;
    j["PlayerHealthToggle"] = PlayerHealthToggle;
    j["PlayerHotbarToggle"] = PlayerHotbarToggle;
    j["PlayerWeaponToggle"] = PlayerWeaponToggle;
    j["TCAuthToggle"] = TCAuthToggle;
    j["UpKeepToggle"] = UpKeepToggle;
    j["TcEspToggle"] = TcEspToggle;
    j["StoneOreToggle"] = StoneOreToggle;
    j["MetalOreToggle"] = MetalOreToggle;
    j["SulfurOreToggle"] = SulfurOreToggle;
    j["MetalPickup"] = MetalPickup;
    j["StonePickup"] = StonePickup;
    j["SulfurPickup"] = SulfurPickup;
    j["HempPickup"] = HempPickup;
    j["WoodPickup"] = WoodPickup;
    j["MushroomPickup"] = MushroomPickup;
    j["LargeStorage"] = LargeStorage;
    j["SmallStorage"] = SmallStorage;
    j["Coffins"] = Coffins;
    j["VendingMachine"] = VendingMachine;
    j["SleepingBag"] = SleepingBag;
    j["Workbench1"] = Workbench1;
    j["Workbench2"] = Workbench2;
    j["Workbench3"] = Workbench3;
    j["Stashes"] = Stashes;
    j["AirDrop"] = AirDrop;
    j["Backpacks"] = Backpacks;
    j["HeliCrates"] = HeliCrates;
    j["BrownCrates"] = BrownCrates;
    j["MiliCrate"] = MiliCrate;
    j["EliteCrate"] = EliteCrate;
    j["Boats"] = Boats;
    j["Horses"] = Horses;
    j["Helicopters"] = Helicopters;
    j["AttackHeli"] = AttackHeli;
    j["Bradley"] = Bradley;

    //Aimbot
    

    j["Wounded"] = WoundedToggle;
    j["ScientistToggle"] = ScientistToggle;
    j["SilentToggle"] = SilentToggle;
    j["TriggerBotToggle"] = TriggerBotToggle;
    j["NoRecoil"] = NoRecoil;
    j["SmoothnessToggle"] = SmoothnessToggle;
    j["MagicBulletToggle"] = MagicBulletToggle;
    j["HeadAimbot"] = HeadAimbot;
    j["ArmAimbot"]= ArmAimbot;
    j["ChestAimbot"]= ChestAimbot;
    j["FootAimbot"]= FootAimbot;
    j["LegAimbot"]= LegAimbot;
    j["HeadAimbotHotkey"] = std::string(HeadAimbotHotkey);
        j["ArmAimbotHotkey"] = std::string(ArmAimbotHotkey);
        j["ChestAimbotHotkey"] = std::string(ChestAimbotHotkey);
        j["FootAimbotHotkey"] = std::string(FootAimbotHotkey);
        j["LegAimbotHotkey"] = std::string(LegAimbotHotkey);

    //weapon
    j["WeaponsList"] = WeaponsList;
    j["AK47ESP"] = AK47ESP;
    j["LR300ESP"] = LR300ESP;
    j["HMLMGESP"] = HMLMGESP;
    j["M249ESP"] = M249ESP;
    j["L96ESP"] = L96ESP;
    j["BoltactionESP"] = BoltactionESP;
    j["SARESP"] = SARESP;
    j["M39ESP"] = M39ESP;
    j["MP5ESP"] = MP5ESP;
    j["CUSTOMSMGESP"] = CUSTOMSMGESP;
    j["THOMPSONESP"] = THOMPSONESP;
    j["REVOLVERESP"] = REVOLVERESP;
    j["PYTHONESP"] = PYTHONESP;
    j["SAPESP"] = SAPESP;
    j["M92ESP"] = M92ESP;
    j["PUMPESP"] = PUMPESP;
    j["WATERPIPEESP"] = WATERPIPEESP;
    j["DOUBLEBARRELESP"] = DOUBLEBARRELESP;
    j["SPASESP"] = SPASESP;
    j["MGLESP"] = MGLESP;
    j["RocketlauncherESP"] = RocketlauncherESP;
    j["EokaESP"] = EokaESP;

    //Misc

    j["FOV"] = i;


    j["SpiderMan"]= SpiderMan;
    j["FOVToggle"]= FOVToggle;
    j["NoFall"]= NoFall;
    j["AdminMode"]= AdminMode;
    j["Debug"] = Debug;


    //Other
    j["PlayerHealth"] = playerHealth;
    j["Previewname"] = std::string(Previewname);


    // Save the JSON object to a file
    std::ofstream file("settings.json");
    file << j.dump();
    file.close();
}

void LoadSettingsFromJson()
{
    // Load the JSON file into a JSON object
    std::ifstream file("settings.json");
    if (file.is_open())
    {
        json j;
        file >> j;

        // Set the state of the checkboxes based on the values in the JSON object


        //ESP
        TCToggle = j["TCToggle"].get<bool>();
        PlayerToggle = j["PlayerToggle"].get<bool>();
        BaseToggle = j["BaseToggle"].get<bool>();
        OtherToggle = j["OtherToggle"].get<bool>();
        CollectiblesToggle = j["CollectiblesToggle"].get<bool>();
        OreToggle = j["OreToggle"].get<bool>();
        PlayerBoxToggle = j["PlayerBoxToggle"].get<bool>();
        PlayerSkeletonToggle = j["PlayerSkeletonToggle"].get<bool>();
        PlayerNameToggle = j["PlayerNameToggle"].get<bool>();
        PlayerDistanceToggle = j["PlayerDistanceToggle"].get<bool>();
        PlayerHealthToggle = j["PlayerHealthToggle"].get<bool>();
        PlayerHotbarToggle = j["PlayerHotbarToggle"].get<bool>();
        PlayerWeaponToggle = j["PlayerWeaponToggle"].get<bool>();
        TCAuthToggle = j["TCAuthToggle"].get<bool>();
        UpKeepToggle = j["UpKeepToggle"].get<bool>();
        TcEspToggle = j["TcEspToggle"].get<bool>();
        StoneOreToggle = j["StoneOreToggle"].get<bool>();
        MetalOreToggle = j["MetalOreToggle"].get<bool>();
        SulfurOreToggle = j["SulfurOreToggle"].get<bool>();
        MetalPickup = j["MetalPickup"].get<bool>();
        StonePickup = j["StonePickup"].get<bool>();
        HempPickup = j["HempPickup"].get<bool>();
        WoodPickup = j["WoodPickup"].get<bool>();
        SulfurPickup = j["SulfurPickup"].get<bool>();
        LargeStorage = j["LargeStorage"].get<bool>();
        SmallStorage = j["SmallStorage"].get<bool>();
        Coffins = j["Coffins"].get<bool>();
        VendingMachine = j["VendingMachine"].get<bool>();
        SleepingBag = j["SleepingBag"].get<bool>();
        Workbench1 = j["Workbench1"].get<bool>();
        Workbench2 = j["Workbench2"].get<bool>();
        Workbench3 = j["Workbench3"].get<bool>();
        Stashes = j["Stashes"].get<bool>();
        AirDrop = j["AirDrop"].get<bool>();
        Backpacks = j["Backpacks"].get<bool>();
        HeliCrates = j["HeliCrates"].get<bool>();
        BrownCrates = j["BrownCrates"].get<bool>();
        MiliCrate = j["MiliCrate"].get<bool>();
        EliteCrate = j["EliteCrate"].get<bool>();
        Boats = j["Boats"].get<bool>();
        Horses = j["Horses"].get<bool>();
        Helicopters = j["Helicopters"].get<bool>();
        AttackHeli = j["AttackHeli"].get<bool>();
        Bradley = j["Bradley"].get<bool>();


        //Aimbot

        WoundedToggle = j["Wounded"].get<bool>();
        ScientistToggle = j["ScientistToggle"].get<bool>();
        SilentToggle = j["SilentToggle"].get<bool>();
        TriggerBotToggle = j["TriggerBotToggle"].get<bool>();
        NoRecoil = j["NoRecoil"].get<bool>();
        SmoothnessToggle = j["SmoothnessToggle"].get<bool>();
        MagicBulletToggle = j["MagicBulletToggle"].get<bool>();
        HeadAimbot = j["HeadAimbot"].get<bool>();
        ArmAimbot = j["ArmAimbot"].get<bool>();
        ChestAimbot = j["ChestAimbot"].get<bool>();
        FootAimbot = j["FootAimbot"].get<bool>();
        LegAimbot = j["LegAimbot"].get<bool>();


        strcpy_s(HeadAimbotHotkey, sizeof(HeadAimbotHotkey), j["HeadAimbotHotkey"].get<std::string>().c_str());
        strcpy_s(ArmAimbotHotkey, sizeof(ArmAimbotHotkey), j["ArmAimbotHotkey"].get<std::string>().c_str());
        strcpy_s(ChestAimbotHotkey, sizeof(ChestAimbotHotkey), j["ChestAimbotHotkey"].get<std::string>().c_str());
        strcpy_s(FootAimbotHotkey, sizeof(FootAimbotHotkey), j["FootAimbotHotkey"].get<std::string>().c_str());
        strcpy_s(LegAimbotHotkey, sizeof(LegAimbotHotkey), j["LegAimbotHotkey"].get<std::string>().c_str());


        //Weapons

        WeaponsList                     =              j        ["WeaponsList"]             .       get     <bool>();
        AK47ESP                         =              j        ["AK47ESP"]                 .       get     <bool>();
        HMLMGESP                        =              j        ["HMLMGESP"]                .       get     <bool>();
        M249ESP                         =              j        ["M249ESP"]                 .       get     <bool>();
        L96ESP                          =              j        ["L96ESP"]                  .       get     <bool>();
        BoltactionESP                   =              j        ["BoltactionESP"]           .       get     <bool>();
        SARESP                          =              j        ["SARESP"]                  .       get     <bool>();
        M39ESP                          =              j        ["M39ESP"]                  .       get     <bool>();
        MP5ESP                          =              j        ["MP5ESP"]                  .       get     <bool>();
        CUSTOMSMGESP                    =              j        ["CUSTOMSMGESP"]            .       get     <bool>();
        THOMPSONESP                     =              j        ["THOMPSONESP"]             .       get     <bool>();
        REVOLVERESP                     =              j        ["REVOLVERESP"]             .       get     <bool>();
        PYTHONESP                       =              j        ["PYTHONESP"]               .       get     <bool>();
        SAPESP                          =              j        ["SAPESP"]                  .       get     <bool>();
        M92ESP                          =              j        ["M92ESP"]                  .       get     <bool>();
        PUMPESP                         =              j        ["PUMPESP"]                 .       get     <bool>();
        WATERPIPEESP                    =              j        ["WATERPIPEESP"]            .       get     <bool>();
        DOUBLEBARRELESP                 =              j        ["DOUBLEBARRELESP"]         .       get     <bool>();
        SPASESP                         =              j        ["SPASESP"]                 .       get     <bool>();
        MGLESP                          =              j        ["MGLESP"]                  .       get     <bool>();
        RocketlauncherESP               =              j        ["RocketlauncherESP"]       .       get     <bool>();
        EokaESP                         =              j        ["EokaESP"]                 .       get     <bool>();



        //Misc
        i = j["FOV"].get<int>();

        SpiderMan = j["SpiderMan"].get<bool>();
        FOVToggle = j["FOVToggle"].get<bool>();
        NoFall = j["NoFall"].get<bool>();
        AdminMode = j["AdminMode"].get<bool>();
        Debug = j["Debug"].get<bool>();



        //Other
        playerHealth = j["PlayerHealth"].get<int>();
        strcpy_s(Previewname, sizeof(Previewname), j["Previewname"].get<std::string>().c_str());



    }
}