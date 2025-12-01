#ifndef TPFINALNEEDFORSPEED_CONFIG_PARSER_H
#define TPFINALNEEDFORSPEED_CONFIG_PARSER_H

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include "../../../common_src/constants.h"
#include <yaml-cpp/yaml.h>

struct LoopsConfig {
    double lobbyHz;
    double raceHz;
};

struct LobbyConfig {
    uint8_t   maxPlayers;
    double upgradesPhaseSec;
    float preRaceCountdownSec;
    float maxRaceTimeSec;
};

struct PhysicsConfig {
    float   timeStep; //cuánto tiempo avanza el mundo en esa llamada.
    int     subStepCount; //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)
};

struct CarHandlingConfig {
    float baseHealth;
    float maxFwdSpeed;
    float maxBckSpeed;
    float engineImpulse;
    float brakeAccel;
    float maxAngularVel; // 2.5–3.5
    float lateralDamp; // 6–12
};


struct BuildingCollisionConfig {
    float minImpactSpeed;
    float baseDamageFactor;
    float minFrontalAlignment;
    float frontalMultiplier;
    float velocityDampFactor;
};

struct CarCollisionConfig {
    float minImpactSpeed;
    float baseDamageFactor;
    float minFrontalAlignment;
    float frontalMultiplier;
    float velocityDampFactor;
};

struct CollisionsConfig {
    BuildingCollisionConfig building;
    CarCollisionConfig car;
};


struct CheatsConfig {
    bool enabled = true;
    bool allowHealthCheat = true;
    bool allowFreeSpeedCheat = true;
    bool allowNextCheckpointCheat = true;
    bool allowWinRaceCheat = true;
    bool allowLostRaceCheat = false;
};

struct Config {
    LoopsConfig loops;
    LobbyConfig  lobby;
    PhysicsConfig physics;
    CarHandlingConfig carHandling;
    CollisionsConfig collisions;
    std::vector<UpgradeDef> upgrades;
    CheatsConfig  cheats;

};


class ConfigParser {
public:
    ConfigParser() = default;
    Config load(const std::string& path);

private:
    void loadLoops(const YAML::Node& root, Config& cfg);
    void loadLobby(const YAML::Node& root, Config& cfg);
    void loadPhysics(const YAML::Node& root, Config& cfg);
    void loadCarHandling(const YAML::Node& root, Config& cfg);
    void loadCollisionOverrides(const YAML::Node& root, Config& cfg);
    void loadUpgradesFromYaml(const YAML::Node& root, Config& cfg);
    void loadCheats(const YAML::Node& root, Config& cfg);
};


#endif
