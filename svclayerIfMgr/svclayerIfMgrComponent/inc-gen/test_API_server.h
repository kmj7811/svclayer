

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef TEST_API_INTERFACE_H_INCLUDE_GUARD
#define TEST_API_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"


//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t test_API_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t test_API_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void test_API_AdvertiseService
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
le_result_t test_API_SendCommand1
(
    int32_t value1
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
le_result_t test_API_GetCommand1
(
    char* message,
        ///< [OUT]
    size_t messageSize
        ///< [IN]
);


#endif // TEST_API_INTERFACE_H_INCLUDE_GUARD