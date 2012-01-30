/**
 */
#ifndef EPYX_ASSERT_H
#define EPYX_ASSERT_H

#include <iostream>
#include "log.h"

#define EPYX_ASSERT(condition)\
{\
    if(!(bool)(condition)){\
        Epyx::log::fatal << "\n[ASSERT] " #condition "\n";\
        Epyx::log::fatal << "    - Function: " << __FUNCTION__ << " \n";\
        Epyx::log::fatal << "    - File: " << __FILE__ << " : " << __LINE__ << Epyx::log::endl;\
    }\
}

#define EPYX_ASSERT_NO_LOG(condition)\
{\
    if(!(bool)(condition)){\
        std::cerr << "[ASSERT] " #condition "\n";\
        std::cerr << "    - Function: " << __FUNCTION__ << " \n";\
        std::cerr << "    - File: " << __FILE__ << " : " << __LINE__ << std::endl;\
    }\
}


#endif /* EPYX_ASSERT_H */