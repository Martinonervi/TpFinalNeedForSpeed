#ifndef TPFINALNEEDFORSPEED_MAP_PARSER_H
#define TPFINALNEEDFORSPEED_MAP_PARSER_H

#include <string>
#include <vector>
#include "checkpoint.h"


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
    float x1, y1;
    float x2, y2;
};

struct MapData {
    std::string city;
    std::vector<BuildingConfig> buildings;
    std::vector<CheckpointConfig> checkpoints;
};

class MapParser {
public:
    MapData load(const std::string& path);
    MapData loadd(const std::string& path);
};

#endif





