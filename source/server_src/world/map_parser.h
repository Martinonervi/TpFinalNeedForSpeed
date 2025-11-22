#ifndef TPFINALNEEDFORSPEED_MAP_PARSER_H
#define TPFINALNEEDFORSPEED_MAP_PARSER_H

#include <string>
#include <vector>

#include "entities/checkpoint.h"
#include <yaml-cpp/yaml.h>


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
    ID spawnId;
    float x; // en metros
    float y; // en metros
    float angle; // en radianes
};

struct RecommendedPointConfig {
    float x;
    float y;
};


struct RouteConfig {
    std::string nameCity;
    std::string nameRoute;

    std::vector<CheckpointConfig> checkpoints;
    std::vector<SpawnPointConfig> spawnPoints;
    std::vector<RecommendedPointConfig> recommendedPath;
};

struct MapData {
    std::string city;
    std::vector<BuildingConfig> buildings;
    std::vector<RouteConfig> routes;
};

class MapParser {
public:
    MapData load(const std::string& path);

private:

    void parseCheckpointList(const YAML::Node& cpList,
                                        std::vector<CheckpointConfig>& out) const;
    void parseSpawnPoints(const YAML::Node& spList,
                                     std::vector<SpawnPointConfig>& out) const;
    void parseRecommendedPath(const YAML::Node& pathList,
                                         std::vector<RecommendedPointConfig>& out) const;

    void parseBuildings(const YAML::Node& buildingsNode,
                        std::vector<BuildingConfig>& out) const;
};

#endif





