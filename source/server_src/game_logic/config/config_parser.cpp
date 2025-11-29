#include "config_parser.h"
#include <yaml-cpp/yaml.h>

Upgrade parseUpgradeType(const std::string& s) {
    if (s == "ENGINE_FORCE") return Upgrade::ENGINE_FORCE;
    if (s == "HEALTH")       return Upgrade::HEALTH;
    if (s == "SHIELD")       return Upgrade::SHIELD;
    if (s == "DAMAGE")       return Upgrade::DAMAGE;
    throw std::runtime_error("Tipo de upgrade desconocido: " + s);
}

Config ConfigParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    Config cfg;

    auto loops = root["loops"];
    cfg.loops.lobbyHz = loops["lobby_hz"].as<double>();
    cfg.loops.raceHz  = loops["race_hz"].as<double>();

    auto lobby = root["lobby"];
    cfg.lobby.maxPlayers        = lobby["max_players"].as<int>();
    cfg.lobby.betweenRacesSec   = lobby["between_races_sec"].as<double>();
    cfg.lobby.maxRaceTimeSec    = lobby["max_race_time_sec"].as<float>();

    cfg.carHandling = {
        50.0f,   // maxFwdSpeed
        -10.0f,  // maxBckSpeed
        800.0f,  // engineImpulse
        100.0f,  // brakeAccel
        2.5f,    // maxAngularVel
        9.0f     // lateralDamp
    };

    auto phys = root["physics"];
    cfg.physics.timeStep      = phys["time_step"].as<float>();
    cfg.physics.subStepCount  = phys["sub_step_count"].as<int>();

    if (root["car_handling"]) {
        auto ch = root["car_handling"];
        if (ch["base_health"])
            cfg.carHandling.baseHealth = ch["base_health"].as<float>();
        if (ch["max_fwd_speed"])
            cfg.carHandling.maxFwdSpeed = ch["max_fwd_speed"].as<float>();
        if (ch["max_bck_speed"])
            cfg.carHandling.maxBckSpeed = ch["max_bck_speed"].as<float>();
        if (ch["engine_impulse"])
            cfg.carHandling.engineImpulse = ch["engine_impulse"].as<float>();
        if (ch["brake_accel"])
            cfg.carHandling.brakeAccel = ch["brake_accel"].as<float>();
        if (ch["max_angular_vel"])
            cfg.carHandling.maxAngularVel = ch["max_angular_vel"].as<float>();
        if (ch["lateral_damp"])
            cfg.carHandling.lateralDamp = ch["lateral_damp"].as<float>();
    }

    // --- COLLISIONS: defaults primero ---
    cfg.collisions.building = {
        1.5f,  // minImpactSpeed
        0.5f,  // baseDamageFactor
        0.7f,  // minFrontalAlignment
        2.0f,  // frontalMultiplier
        0.5f   // velocityDampFactor
    };

    cfg.collisions.car = {
        1.0f,  // minImpactSpeed
        0.4f,  // baseDamageFactor
        0.7f,  // minFrontalAlignment
        1.5f,  // frontalMultiplier
        0.4f   // velocityDampFactor
    };

    auto collisions = root["collisions"];
    if (collisions) {
        auto b = collisions["building"];
        if (b) {
            if (b["min_impact_speed"])
                cfg.collisions.building.minImpactSpeed =
                        b["min_impact_speed"].as<float>();
            if (b["base_damage_factor"])
                cfg.collisions.building.baseDamageFactor =
                        b["base_damage_factor"].as<float>();
            if (b["min_frontal_alignment"])
                cfg.collisions.building.minFrontalAlignment =
                        b["min_frontal_alignment"].as<float>();
            if (b["frontal_multiplier"])
                cfg.collisions.building.frontalMultiplier =
                        b["frontal_multiplier"].as<float>();
            if (b["velocity_damp_factor"])
                cfg.collisions.building.velocityDampFactor =
                        b["velocity_damp_factor"].as<float>();
        }

        auto c = collisions["car"];
        if (c) {
            if (c["min_impact_speed"])
                cfg.collisions.car.minImpactSpeed =
                        c["min_impact_speed"].as<float>();
            if (c["base_damage_factor"])
                cfg.collisions.car.baseDamageFactor =
                        c["base_damage_factor"].as<float>();
            if (c["min_frontal_alignment"])
                cfg.collisions.car.minFrontalAlignment =
                        c["min_frontal_alignment"].as<float>();
            if (c["frontal_multiplier"])
                cfg.collisions.car.frontalMultiplier =
                        c["frontal_multiplier"].as<float>();
            if (c["velocity_damp_factor"])
                cfg.collisions.car.velocityDampFactor =
                        c["velocity_damp_factor"].as<float>();
        }
    }


    //valores default
    cfg.upgrades = {
        { Upgrade::ENGINE_FORCE, 2.5f, 8.0f },
        { Upgrade::HEALTH,       1.5f, 5.0f },
        { Upgrade::SHIELD,       0.7f, 2.0f },
        { Upgrade::DAMAGE,       2.0f, 3.0f },
    };

    if (root["upgrades"]) {
        cfg.upgrades.clear();
        for (auto node : root["upgrades"]) {
            UpgradeDef u;
            u.type      = parseUpgradeType(node["type"].as<std::string>());
            u.value    = node["factor"].as<float>();     // o el nombre que uses
            u.penaltySec = node["penalty"].as<float>();  // idem
            cfg.upgrades.push_back(u);
        }
    }

    auto cheats = root["cheats"];
    if (cheats) {
        cfg.cheats.enabled                = cheats["enabled"]
        ? cheats["enabled"].as<bool>()                 : true;
        cfg.cheats.allowHealthCheat       = cheats["allow_health_cheat"]
        ? cheats["allow_health_cheat"].as<bool>()      : true;
        cfg.cheats.allowFreeSpeedCheat    = cheats["allow_free_speed_cheat"]
        ? cheats["allow_free_speed_cheat"].as<bool>()  : true;
        cfg.cheats.allowNextCheckpointCheat = cheats["allow_next_checkpoint_cheat"]
        ? cheats["allow_next_checkpoint_cheat"].as<bool>() : true;
        cfg.cheats.allowWinRaceCheat      = cheats["allow_win_race_cheat"]
        ? cheats["allow_win_race_cheat"].as<bool>()    : true;
        cfg.cheats.allowLostRaceCheat     = cheats["allow_lost_race_cheat"]
        ? cheats["allow_lost_race_cheat"].as<bool>()   : true;
    }

    return cfg;
}
