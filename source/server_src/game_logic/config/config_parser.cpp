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

    auto lobby = root["lobby"];
    cfg.lobby.maxPlayers        = lobby["max_players"].as<int>();
    cfg.lobby.betweenRacesSec   = lobby["between_races_sec"].as<double>();
    cfg.lobby.maxRaceTimeSec    = lobby["max_race_time_sec"].as<float>();

    auto loops = root["loops"];
    cfg.loops.lobbyHz = loops["lobby_hz"].as<double>();
    cfg.loops.raceHz  = loops["race_hz"].as<double>();

    auto phys = root["physics"];
    cfg.physics.timeStep      = phys["time_step"].as<float>();
    cfg.physics.subStepCount  = phys["sub_step_count"].as<int>();

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
