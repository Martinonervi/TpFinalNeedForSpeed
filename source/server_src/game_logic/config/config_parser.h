#ifndef TPFINALNEEDFORSPEED_CONFIG_PARSER_H
#define TPFINALNEEDFORSPEED_CONFIG_PARSER_H

#include <string>
#include <vector>
#include "../../../common_src/constants.h"


struct LoopsConfig {
    int maxPlayers;
    double lobbyHz;
    double raceHz;
};

struct LobbyConfig {
    int   maxPlayers;
    double betweenRacesSec;
    float maxRaceTimeSec;
};

struct PhysicsConfig {
    float   timeStep; //cuánto tiempo avanza el mundo en esa llamada.
    int     subStepCount; //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)
};

struct CarHandlingConfig {
    float baseHealth;
    // longitudinal
    float maxFwdSpeed;
    float maxBckSpeed;
    float engineImpulse;
    // lateral
    float brakeAccel;
    float maxAngularVel; // podria probar 2.5–3.5
    float lateralDamp; // probá 6–12
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
};


#endif
