#include "map_parser.h"
#include "../../common_src/constants.h"
#include <cmath>

MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    MapData data;

    if (root["city"]) {
        data.city = root["city"].as<std::string>();
    }
    if (root["buildings"]) {
        parseBuildings(root["buildings"], data.buildings);
    }
    if (root["npc_cars"]) {
        parseNpcCars(root["npc_cars"], data.npcParked);
    }
    if (root["routes"] && root["routes"].IsSequence()) {
        for (auto routeNode : root["routes"]) {
            RouteConfig currentRoute;
            if (routeNode["checkpoints"]) {
                parseCheckpointList(routeNode["checkpoints"], currentRoute.checkpoints);
            }
            if (routeNode["spawn_points"]) {
                parseSpawnPoints(routeNode["spawn_points"], currentRoute.spawnPoints);
            }
            if (routeNode["recommended_path"]) {
                parseRecommendedPath(routeNode["recommended_path"],currentRoute.recommendedPath);
            }
            if (routeNode["name"]) {
                currentRoute.nameRoute = routeNode["name"].as<std::string>();
            }
            if (!currentRoute.checkpoints.empty() ||
                !currentRoute.spawnPoints.empty() ||
                !currentRoute.recommendedPath.empty()) {
                data.routes.push_back(std::move(currentRoute));
            }
        }
    }
    return data;
}

void MapParser::parseCheckpointList(const YAML::Node& cpList,
                                    std::vector<CheckpointConfig>& out) const {
    constexpr float MIN_CP_LEN = 0.05f;
    if (!cpList || !cpList.IsSequence()) return;

    for (auto cpNode : cpList) {
        CheckpointConfig cp{};
        float x_px, y_px, w_px, h_px, ang;
        try {
            cp.id = cpNode["id"].as<int>();
            std::string kindStr = cpNode["kind"].as<std::string>();
            if (kindStr == "Finish" || kindStr == "finish") {
                cp.kind = CheckpointKind::Finish;
            } else {
                cp.kind = CheckpointKind::Normal;
            }
            x_px = cpNode["x"].as<float>();
            y_px = cpNode["y"].as<float>();
            w_px = cpNode["w"].as<float>();
            h_px = cpNode["h"].as<float>();
            ang = cpNode["angle"].as<float>();
        } catch (...) { continue;}

        if (!std::isfinite(x_px) || !std::isfinite(y_px) || !std::isfinite(w_px) ||
            !std::isfinite(h_px)) { continue; }

        cp.x = x_px * PIXEL_TO_METER;
        cp.y = y_px * PIXEL_TO_METER;
        float w_m = std::fabs(w_px) * PIXEL_TO_METER;
        float h_m = std::fabs(h_px) * PIXEL_TO_METER;
        if (w_m < MIN_CP_LEN) w_m = MIN_CP_LEN;
        if (h_m < MIN_CP_LEN) h_m = MIN_CP_LEN;
        cp.w = w_m;
        cp.h = h_m;

        cp.angle = ang * M_PI / 180.0f;
        out.push_back(cp);
    }
}

void MapParser::parseSpawnPoints(const YAML::Node& spList,
                                 std::vector<SpawnPointConfig>& out) const {
    if (!spList || !spList.IsSequence()) {
        return;
    }

    for (auto spNode : spList) {
        if (!spNode["x"] || !spNode["y"]) {
            continue;
        }

        SpawnPointConfig sp{};
        float x_px, y_px, ang = 0.0f;

        try {
            sp.spawnId = spNode["id"].as<ID>();
            x_px = spNode["x"].as<float>();
            y_px = spNode["y"].as<float>();
            if (spNode["angle"]) {
                ang = spNode["angle"].as<float>();
            }
        } catch (...) {
            continue;
        }

        if (!std::isfinite(x_px) || !std::isfinite(y_px)) {
            continue;
        }

        sp.x = x_px * PIXEL_TO_METER;
        sp.y = y_px * PIXEL_TO_METER;
        float angleRad = ang * M_PI / 180.0f;
        sp.angle = angleRad;

        out.push_back(sp);
    }
}

void MapParser::parseRecommendedPath(const YAML::Node& pathList,
                                     std::vector<RecommendedPoint>& out) const {
    if (!pathList || !pathList.IsSequence()) {
        return;
    }

    for (auto node : pathList) {
        if (!node["x"] || !node["y"]) {
            continue;
        }

        float x_px, y_px;
        try {
            x_px = node["x"].as<float>();
            y_px = node["y"].as<float>();
        } catch (...) {
            continue;
        }

        if (!std::isfinite(x_px) || !std::isfinite(y_px)) {
            continue;
        }

        RecommendedPoint p{};
        p.x = x_px * PIXEL_TO_METER;
        p.y = y_px * PIXEL_TO_METER;

        out.push_back(p);
    }
}

void MapParser::parseBuildings(const YAML::Node& buildingsNode,
                               std::vector<BuildingConfig>& out) const {
    constexpr float MIN_W = 0.10f;  // ancho mínimo edificio
    constexpr float MIN_H = 0.10f;  // alto  mínimo edificio
    if (!buildingsNode || !buildingsNode.IsSequence()) return;

    for (auto b : buildingsNode) {
        float x_px, y_px, w_px, h_px, ang;
        try {
            x_px = b["x"].as<float>();
            y_px = b["y"].as<float>();
            w_px = b["w"].as<float>();
            h_px = b["h"].as<float>();
            ang = b["angle"].as<float>();
        } catch (...) {
            continue;
        }

        if (!std::isfinite(x_px) || !std::isfinite(y_px) ||
            !std::isfinite(w_px) || !std::isfinite(h_px)) {
            continue;
        }

        BuildingConfig cfg;
        cfg.x = x_px * PIXEL_TO_METER;
        cfg.y = y_px * PIXEL_TO_METER;
        float w_m = std::fabs(w_px) * PIXEL_TO_METER;
        float h_m = std::fabs(h_px) * PIXEL_TO_METER;
        if (w_m < MIN_W) w_m = MIN_W;
        if (h_m < MIN_H) h_m = MIN_H;

        cfg.w = w_m;
        cfg.h = h_m;
        cfg.angle = ang * M_PI / 180.0f;

        out.push_back(cfg);
    }
}

void MapParser::parseNpcCars(const YAML::Node& npcList,
                             std::vector<NpcConfig>& out) const {
    if (!npcList || !npcList.IsSequence()) return;

    for (auto node : npcList) {
        float x_px, y_px, ang_deg;
        try {
            x_px    = node["x"].as<float>();
            y_px    = node["y"].as<float>();
            ang_deg = node["angle"].as<float>();
        } catch (...) {
            continue;
        }

        if (!std::isfinite(x_px) || !std::isfinite(y_px) || !std::isfinite(ang_deg)) {
            continue;
        }

        NpcConfig cfg;
        cfg.x     = x_px * PIXEL_TO_METER;
        cfg.y     = y_px * PIXEL_TO_METER;
        cfg.angle = ang_deg * M_PI / 180.0f;

        if (node["carType"]) {
            cfg.carType = node["carType"].as<std::string>();
        } else {
            cfg.carType = "green";
        }

        out.push_back(cfg);
    }
}



