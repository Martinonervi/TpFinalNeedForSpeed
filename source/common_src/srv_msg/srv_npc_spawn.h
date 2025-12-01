#ifndef TPFINALNEEDFORSPEED_SRV_NPC_SPAWN_H
#define TPFINALNEEDFORSPEED_SRV_NPC_SPAWN_H

#include "server_msg.h"
#include "../../common_src/constants.h"

class SrvNpcSpawn : public SrvMsg {
public:
    SrvNpcSpawn(ID id, CarType carType, float x, float y, float angleRad)
        : id(id)
        , carType(carType)
        , x(x)
        , y(y)
        , angleRad(angleRad) {}

    Op type() const override { return Opcode::NPC_SPAWN; }

    ID       getId()       const { return id; }
    CarType  getCarType()  const { return carType; }
    float    getX()        const { return x; }
    float    getY()        const { return y; }
    float    getAngleRad() const { return angleRad; }

private:
    ID      id;
    CarType carType;
    float   x;
    float   y;
    float   angleRad;
};

#endif
