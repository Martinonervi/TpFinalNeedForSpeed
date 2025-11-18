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


struct HintDef {
    uint8_t id;
    float x, y;
    float dirX, dirY;
};

struct MapData {
    std::string city;
    std::vector<BuildingConfig> buildings;
    std::vector<CheckpointConfig> checkpoints; //la primera
    // todos las recorridos
    std::vector<std::vector<CheckpointConfig>> routes;
};

class MapParser {
public:
    MapData load(const std::string& path);
};

#endif





