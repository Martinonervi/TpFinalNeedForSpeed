#ifndef METADATAGAMES_H
#define METADATAGAMES_H

#include <vector>
#include "constants.h"

class MetadataGames: public SrvMsg {
public:
    MetadataGames(std::vector<GameMetadata> games): games(games){};

    std::vector<GameMetadata> getMetadata() const {return games;}
    uint32_t size() const {return games.size();}
    Op type() const override { return Opcode::REQUEST_GAMES; }

private:
    std::vector<GameMetadata> games;
};


#endif //METADATAGAMES_H
