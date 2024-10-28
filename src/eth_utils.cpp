#include <ETH.h>

#include "configuration.h"
#include "display.h"

extern Configuration    Config;

namespace ETH_Utils {

    void startETH() {
        
    }

    void Setup() {
        if (Config.ethernet.ethernet_enable) startETH();
    }
}