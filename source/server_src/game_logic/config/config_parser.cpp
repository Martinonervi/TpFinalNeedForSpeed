#include "config_parser.h"

Upgrade parseUpgradeType(const std::string& s) {
    if (s == "ENGINE_FORCE") return Upgrade::ENGINE_FORCE;
    if (s == "HEALTH")       return Upgrade::HEALTH;
    if (s == "SHIELD")       return Upgrade::SHIELD;
    if (s == "DAMAGE")       return Upgrade::DAMAGE;
    throw std::runtime_error("Tipo de upgrade desconocido: " + s);
}

template <typename CollisionCfg>
void loadCollisionBlock(const YAML::Node& node, CollisionCfg& cfg) {
    if (node["min_impact_speed"])
        cfg.minImpactSpeed = node["min_impact_speed"].as<float>();

    if (node["base_damage_factor"])
        cfg.baseDamageFactor = node["base_damage_factor"].as<float>();

    if (node["min_frontal_alignment"])
        cfg.minFrontalAlignment = node["min_frontal_alignment"].as<float>();

    if (node["frontal_multiplier"])
        cfg.frontalMultiplier = node["frontal_multiplier"].as<float>();

    if (node["velocity_damp_factor"])
        cfg.velocityDampFactor = node["velocity_damp_factor"].as<float>();
}


Config ConfigParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    Config cfg{};

    loadLoops(root, cfg);
    loadLobby(root, cfg);
    loadPhysics(root, cfg);
    loadCarHandling(root, cfg);
    loadCollisionOverrides(root, cfg);
    loadUpgradesFromYaml(root, cfg);
    loadCheats(root, cfg);

    return cfg;
}

void ConfigParser::loadLoops(const YAML::Node& root, Config& cfg) {
    auto loops = root["loops"];
    if (!loops) return;

    if (loops["lobby_hz"])
        cfg.loops.lobbyHz = loops["lobby_hz"].as<double>();
    if (loops["race_hz"])
        cfg.loops.raceHz  = loops["race_hz"].as<double>();
}

void ConfigParser::loadLobby(const YAML::Node& root, Config& cfg) {
    auto lobby = root["lobby"];
    if (!lobby) return;

    if (lobby["max_players"]) {
        int maxPlayersInt = lobby["max_players"].as<int>();
        cfg.lobby.maxPlayers = static_cast<std::uint8_t>(maxPlayersInt);
    }
    if (lobby["upgrades_phase_sec"])
        cfg.lobby.upgradesPhaseSec = lobby["upgrades_phase_sec"].as<double>();
    if (lobby["pre_race_countdown_sec"])
        cfg.lobby.preRaceCountdownSec = lobby["pre_race_countdown_sec"].as<float>();
    if (lobby["max_race_time_sec"])
        cfg.lobby.maxRaceTimeSec  = lobby["max_race_time_sec"].as<float>();
}

void ConfigParser::loadPhysics(const YAML::Node& root, Config& cfg) {
    auto phys = root["physics"];
    if (!phys) return;

    if (phys["time_step"])
        cfg.physics.timeStep     = phys["time_step"].as<float>();
    if (phys["sub_step_count"])
        cfg.physics.subStepCount = phys["sub_step_count"].as<int>();
}

void ConfigParser::loadCarHandling(const YAML::Node& root, Config& cfg) {
    auto ch = root["car_handling"];
    if (!ch) return;

    if (ch["base_health"])
        cfg.carHandling.baseHealth    = ch["base_health"].as<float>();
    if (ch["max_fwd_speed"])
        cfg.carHandling.maxFwdSpeed   = ch["max_fwd_speed"].as<float>();
    if (ch["max_bck_speed"])
        cfg.carHandling.maxBckSpeed   = ch["max_bck_speed"].as<float>();
    if (ch["engine_impulse"])
        cfg.carHandling.engineImpulse = ch["engine_impulse"].as<float>();
    if (ch["brake_accel"])
        cfg.carHandling.brakeAccel    = ch["brake_accel"].as<float>();
    if (ch["max_angular_vel"])
        cfg.carHandling.maxAngularVel = ch["max_angular_vel"].as<float>();
    if (ch["lateral_damp"])
        cfg.carHandling.lateralDamp   = ch["lateral_damp"].as<float>();
}

void ConfigParser::loadCollisionOverrides(const YAML::Node& root, Config& cfg) {
    auto collisions = root["collisions"];
    if (!collisions) return;

    if (auto b = collisions["building"]) {
        loadCollisionBlock(b, cfg.collisions.building);
    }

    if (auto c = collisions["car"]) {
        loadCollisionBlock(c, cfg.collisions.car);
    }
}

void ConfigParser::loadUpgradesFromYaml(const YAML::Node& root, Config& cfg) {
    auto ups = root["upgrades"];
    if (!ups) return;

    cfg.upgrades.clear();
    for (auto node : ups) {
        UpgradeDef u;
        u.type       = parseUpgradeType(node["type"].as<std::string>());
        u.value      = node["factor"].as<float>();
        u.penaltySec = node["penalty"].as<float>();
        cfg.upgrades.push_back(u);
    }
}

void ConfigParser::loadCheats(const YAML::Node& root, Config& cfg) {
    auto cheats = root["cheats"];
    if (!cheats) return;

    if (cheats["enabled"])
        cfg.cheats.enabled =
            cheats["enabled"].as<bool>();

    if (cheats["allow_health_cheat"])
        cfg.cheats.allowHealthCheat =
            cheats["allow_health_cheat"].as<bool>();

    if (cheats["allow_free_speed_cheat"])
        cfg.cheats.allowFreeSpeedCheat =
            cheats["allow_free_speed_cheat"].as<bool>();

    if (cheats["allow_next_checkpoint_cheat"])
        cfg.cheats.allowNextCheckpointCheat =
            cheats["allow_next_checkpoint_cheat"].as<bool>();

    if (cheats["allow_win_race_cheat"])
        cfg.cheats.allowWinRaceCheat =
            cheats["allow_win_race_cheat"].as<bool>();

    if (cheats["allow_lost_race_cheat"])
        cfg.cheats.allowLostRaceCheat =
            cheats["allow_lost_race_cheat"].as<bool>();
}
