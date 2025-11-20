#ifndef TPFINALNEEDFORSPEED_MAP_PARSER_H
#define TPFINALNEEDFORSPEED_MAP_PARSER_H

#include <string>
#include <vector>

#include "entities/checkpoint.h"

struct BuildingConfig {
    float x;
    float y;
    float w;
    float h;
    float angle;
};

struct CheckpointConfig {
    int id;
    CheckpointKind kind;
    float x;
    float y;
    float w;
    float h;
    float angle;
};

struct SpawnPointConfig {
    float x; // en metros
    float y; // en metros
    float angle; // en radianes
};

struct RouteConfig {
    std::string nameCity;
    std::string nameRoute;

    std::vector<CheckpointConfig> checkpoints;
    std::vector<SpawnPointConfig> spawnPoints;
};

struct MapData {
    std::string city;
    std::vector<BuildingConfig> buildings;
    std::vector<RouteConfig> routes;
};

class MapParser {
public:
    MapData load(const std::string& path);
};

#endif





