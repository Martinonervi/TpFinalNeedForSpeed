
#include "move_Info.h"
//#include "../client_src/client_protocol.h"

//void MoveMsg::serialize(ClientProtocol& p) const {
  //  p.sendClientMove(*this); //cambiar nombre
//}

//esta idea esta buenisima, lo unico que como MoveInfo pertence a common el server lo va a
// intentar linkear, y cuando busque clientprotocol tira error
// se podria hacer una clase writer que haga maso esto, te ahorras el switch. Es mas, seguro que
// lo que estoy diciendo es un patron.