#include "StateMachine.hpp"

ControlSM_t make_sm(ControlSMContext &csmc){
    QtSMLLogger logger;
    return  ControlSM_t(csmc, logger);
}
