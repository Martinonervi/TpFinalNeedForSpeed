#ifndef TPFINALNEEDFORSPEED_CONFIG_PARSER_H
#define TPFINALNEEDFORSPEED_CONFIG_PARSER_H

#include <string>
#include <vector>
#include "../../../common_src/constants.h"


struct LobbyConfig {
    int   maxPlayers;
    double betweenRacesSec;
    float maxRaceTimeSec;
};

struct LoopsConfig {
    double lobbyHz;
    double raceHz;
};

struct PhysicsConfig {
    float   timeStep; //cuánto tiempo avanza el mundo en esa llamada.
    int     subStepCount; //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)
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
    LobbyConfig  lobby;
    PhysicsConfig physics;
    LoopsConfig loops;
    std::vector<UpgradeDef> upgrades;
    CheatsConfig  cheats;
};


class ConfigParser {
public:
    ConfigParser() = default;
    Config load(const std::string& path);
};


#endif
