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

            // evitamos infinitos
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

            if (!cpNode["x"] || !cpNode["y"] ||
                !cpNode["w"] || !cpNode["h"]) {
                continue;
            }

            CheckpointConfig cp{};

            // id
            try {
                cp.id = cpNode["id"].as<int>();
            } catch (...) {
                continue;
            }

            // kind
            std::string kindStr = "Normal";
            if (cpNode["kind"]) {
                kindStr = cpNode["kind"].as<std::string>();
            }

            if (kindStr == "Start" || kindStr == "start") {
                cp.kind = CheckpointKind::Start;
            } else if (kindStr == "Finish" || kindStr == "finish") {
                cp.kind = CheckpointKind::Finish;
            } else {
                cp.kind = CheckpointKind::Normal;
            }

            float x_px, y_px, w_px, h_px;
            float ang = 0.0f;

            try {
                x_px = cpNode["x"].as<float>();
                y_px = cpNode["y"].as<float>();
                w_px = cpNode["w"].as<float>();
                h_px = cpNode["h"].as<float>();
                if (cpNode["angle"]) {
                    ang = cpNode["angle"].as<float>();
                }
            } catch (...) {
                continue;
            }

            if (!std::isfinite(x_px) || !std::isfinite(y_px) ||
                !std::isfinite(w_px) || !std::isfinite(h_px)) {
                continue;
            }

            cp.x = x_px * PIXEL_TO_METER;
            cp.y = y_px * PIXEL_TO_METER;

            float w_m = std::fabs(w_px) * PIXEL_TO_METER;
            float h_m = std::fabs(h_px) * PIXEL_TO_METER;

            if (w_m < MIN_CP_LEN) w_m = MIN_CP_LEN;
            if (h_m < MIN_CP_LEN) h_m = MIN_CP_LEN;

            cp.w = w_m;
            cp.h = h_m;
            cp.angle = ang;

            if (cp.w <= 0.f || cp.h <= 0.f) {
                continue;
            }

            data.checkpoints.push_back(cp);
        }
    }

    return data;
}
