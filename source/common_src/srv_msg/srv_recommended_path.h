#ifndef TPFINALNEEDFORSPEED_SRV_RECOMMENDED_PATH_H
#define TPFINALNEEDFORSPEED_SRV_RECOMMENDED_PATH_H

#include "server_msg.h"
#include "../../common_src/constants.h"

struct RecommendedPoint {
    float x;
    float y;
};

class RecommendedPath: public SrvMsg {
public:
    explicit RecommendedPath(std::vector<RecommendedPoint> path): path(std::move(path)) {};
    Op type() const override { return Opcode::RECOMMENDED_PATH; }
    std::vector<RecommendedPoint> getPath() const { return path; }
private:
    std::vector<RecommendedPoint> path;
};

#endif
