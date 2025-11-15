#include "map_parser.h"
#include <yaml-cpp/yaml.h>
#include "../../common_src/constants.h"
#include <cmath>


MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);

    MapData data;

    if (root["city"]) {
        data.city = root["city"].as<std::string>();
    }

    // mínimos para que no rompa
    constexpr float MIN_W      = 0.10f;  // ancho mínimo edificio
    constexpr float MIN_H      = 0.10f;  // alto  mínimo edificio
    constexpr float MIN_CP_LEN = 0.05f;  // longitud mínima de checkpoint (en metros)

    if (root["buildings"]) {
        for (auto b : root["buildings"]) {
            if (!b["x"] || !b["y"] || !b["w"] || !b["h"]) {
                continue;
            }

            float x_px, y_px, w_px, h_px;
            float ang = 0.0f;

            try {
                x_px = b["x"].as<float>();
                y_px = b["y"].as<float>();
                w_px = b["w"].as<float>();
                h_px = b["h"].as<float>();
                if (b["angle"]) {
                    ang = b["angle"].as<float>();
                }
            } catch (...) {
                continue;
            }

            // evitamos infinitos (por division de numero chico con grande)
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
            cfg.angle = ang;

            if (cfg.w <= 0.f || cfg.h <= 0.f) {
                continue;
            }

            data.buildings.push_back(cfg);
        }
    }

    if (root["route"] && root["route"]["checkpoints"]) {
        for (auto cpNode : root["route"]["checkpoints"]) {
            CheckpointConfig cp{};

            try {
                cp.id = cpNode["id"].as<int>();
            } catch (...) {
                continue;
            }

            std::string kindStr = "normal";
            if (cpNode["kind"]) {
                kindStr = cpNode["kind"].as<std::string>();
            }
            if (kindStr == "start")      cp.kind = CheckpointKind::Start;
            else if (kindStr == "finish") cp.kind = CheckpointKind::Finish;
            else                          cp.kind = CheckpointKind::Normal;

            if (!cpNode["x1"] || !cpNode["y1"] ||
                !cpNode["x2"] || !cpNode["y2"]) {
                continue;
            }

            float x1_px, y1_px, x2_px, y2_px;
            try {
                x1_px = cpNode["x1"].as<float>();
                y1_px = cpNode["y1"].as<float>();
                x2_px = cpNode["x2"].as<float>();
                y2_px = cpNode["y2"].as<float>();
            } catch (...) {
                continue;
            }

            if (!std::isfinite(x1_px) || !std::isfinite(y1_px) ||
                !std::isfinite(x2_px) || !std::isfinite(y2_px)) {
                continue;
            }

            cp.x1 = x1_px * PIXEL_TO_METER;
            cp.y1 = y1_px * PIXEL_TO_METER;
            cp.x2 = x2_px * PIXEL_TO_METER;
            cp.y2 = y2_px * PIXEL_TO_METER;

            // evitamos checkpoints muy chicos
            float dx  = cp.x2 - cp.x1;
            float dy  = cp.y2 - cp.y1;
            float len = std::sqrt(dx * dx + dy * dy);

            if (len < MIN_CP_LEN) {
                continue;
            }

            data.checkpoints.push_back(cp);
        }
    }

    return data;
}

/*
inline bool isFiniteF(float v) { return std::isfinite(v); }

inline float readF(const YAML::Node& n, const char* k, float def) {
    auto v = n[k];
    if (!v) return def;
    try { return v.as<float>(); } catch (...) { return def; }
}

inline std::string readS(const YAML::Node& n, const char* k, const std::string& def) {
    auto v = n[k];
    if (!v) return def;
    try { return v.as<std::string>(); } catch (...) { return def; }
}

// Garantiza dimensiones > 0 en metros, con mínimo seguro (half extents de Box2D no aceptan 0)
inline float sanitizeDim(float meters, float minMeters) {
    if (!isFiniteF(meters)) return 0.f;
    float m = std::fabs(meters);
    if (m < minMeters) m = minMeters;
    return m;
}





MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    MapData data;

    if (root["city"]) {
        data.city = readS(root, "city", "");
    }

    // sino rompe todo por valores muy chicos
    constexpr float MIN_W = 0.10f;   // ancho mínimo edificio
    constexpr float MIN_H = 0.10f;   // alto  mínimo edificio
    constexpr float MIN_CP_LEN = 0.05f; // min distancia entre x1,y1 y x2,y2

    if (root["buildings"]) {
        for (auto b : root["buildings"]) {
            BuildingConfig cfg{};

            // Leer en px → a m
            float x_px = readF(b, "x", NAN);
            float y_px = readF(b, "y", NAN);
            float w_px = readF(b, "w", NAN);
            float h_px = readF(b, "h", NAN);
            float ang  = readF(b, "angle", 0.f);

            if (!isFiniteF(x_px) || !isFiniteF(y_px) ||
                !isFiniteF(w_px) || !isFiniteF(h_px)) {
                 continue;
            }

            float w_m = sanitizeDim(w_px * PIXEL_TO_METER, MIN_W);
            float h_m = sanitizeDim(h_px * PIXEL_TO_METER, MIN_H);

            cfg.x = x_px * PIXEL_TO_METER;
            cfg.y = y_px * PIXEL_TO_METER;
            cfg.w = w_m;
            cfg.h = h_m;
            cfg.angle = ang;

            if (cfg.w <= 0.f || cfg.h <= 0.f) {
                continue;
            }

            data.buildings.push_back(cfg);
        }
    }

    if (root["route"] && root["route"]["checkpoints"]) {
        for (auto cpNode : root["route"]["checkpoints"]) {
            CheckpointConfig cp{};

            try {
                cp.id = cpNode["id"].as<int>();
            } catch (...) {
                continue;
            }

            std::string kindStr = cpNode["kind"].as<std::string>();
            if (kindStr == "start")      cp.kind = CheckpointKind::Start;
            else if (kindStr == "finish") cp.kind = CheckpointKind::Finish;
            else                          cp.kind = CheckpointKind::Normal;

            float x1_px = readF(cpNode, "x1", NAN);
            float y1_px = readF(cpNode, "y1", NAN);
            float x2_px = readF(cpNode, "x2", NAN);
            float y2_px = readF(cpNode, "y2", NAN);

            if (!isFiniteF(x1_px) || !isFiniteF(y1_px) ||
                !isFiniteF(x2_px) || !isFiniteF(y2_px)) {
                continue;
            }

            cp.x1 = x1_px * PIXEL_TO_METER;
            cp.y1 = y1_px * PIXEL_TO_METER;
            cp.x2 = x2_px * PIXEL_TO_METER;
            cp.y2 = y2_px * PIXEL_TO_METER;

            float dx = cp.x2 - cp.x1, dy = cp.y2 - cp.y1;
            float len = std::sqrt(dx*dx + dy*dy);
            if (!(len >= MIN_CP_LEN)) {
                continue;
            }

            data.checkpoints.push_back(cp);
        }
    }

    return data;
}
*/
/*

#include "map_parser.h"
#include <yaml-cpp/yaml.h>

#include "../../common_src/constants.h"

MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);

    MapData data;

    if (root["city"]) {
        data.city = root["city"].as<std::string>();
    }

    if (root["buildings"]) {
        for (auto b : root["buildings"]) {
            BuildingConfig cfg;
            cfg.x = b["x"].as<float>()* PIXEL_TO_METER;
            cfg.y = b["y"].as<float>()* PIXEL_TO_METER;
            cfg.w = b["w"].as<float>()* PIXEL_TO_METER;
            cfg.h = b["h"].as<float>()* PIXEL_TO_METER;
            cfg.angle = b["angle"].as<float>();
            data.buildings.push_back(cfg);
        }
    }

    if (root["route"] && root["route"]["checkpoints"]) {
        for (auto cpNode : root["route"]["checkpoints"]) {
            CheckpointConfig cp;
            cp.id = cpNode["id"].as<int>();

            std::string kindStr = cpNode["kind"].as<std::string>();
            if (kindStr == "start")      cp.kind = CheckpointKind::Start;
            else if (kindStr == "finish") cp.kind = CheckpointKind::Finish;
            else                          cp.kind = CheckpointKind::Normal;

            cp.x1 = cpNode["x1"].as<float>()* PIXEL_TO_METER;
            cp.y1 = cpNode["y1"].as<float>()* PIXEL_TO_METER;
            cp.x2 = cpNode["x2"].as<float>()* PIXEL_TO_METER;
            cp.y2 = cpNode["y2"].as<float>()* PIXEL_TO_METER;

            data.checkpoints.push_back(cp);
        }
    }
    return data;
}
*/