#ifndef BLE_BGS_H__
#define BLE_BGS_H__

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_BGS_SERVICE 0x0001                      /**< The UUID of the BluChip Service. */
#define BLE_BGS_MAX_DATA_LEN 2//(GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

/* Forward declaration of the ble_bgs_t type. */
typedef struct ble_bgs_s ble_bgs_t;

/**@brief Nordic UART Service event handler type. */
typedef void (*ble_bgs_data_handler_t) (ble_bgs_t * p_bgs, uint8_t * p_data, uint16_t length);

/**@brief Nordic UART Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_bgs_init
 *          function.
 */
typedef struct
{
    ble_bgs_data_handler_t data_handler; /**< Event handler to be called for handling received data. */
} ble_bgs_init_t;

/**@brief Nordic UART Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_bgs_s
{
    uint8_t                  uuid_type;               /**< UUID type for Nordic UART Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of Nordic UART Service (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t gm_handles;              /**< Handles related to the TX characteristic (as provided by the S110 SoftDevice). */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the S110 SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_bgs_data_handler_t   data_handler;            /**< Event handler to be called for handling received data. */
};

/**@brief Function for initializing the Nordic UART Service.
 *
 * @param[out] p_bgs      Nordic UART Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_bgs_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_bgs or p_bgs_init is NULL.
 */
uint32_t ble_bgs_init(ble_bgs_t * p_bgs, const ble_bgs_init_t * p_bgs_init);

/**@brief Function for handling the Nordic UART Service's BLE events.
 *
 * @details The Nordic UART Service expects the application to call this function each time an
 * event is received from the S110 SoftDevice. This function processes the event if it
 * is relevant and calls the Nordic UART Service event handler of the
 * application if necessary.
 *
 * @param[in] p_bgs       Nordic UART Service structure.
 * @param[in] p_ble_evt   Event received from the S110 SoftDevice.
 */
void ble_bgs_on_ble_evt(ble_bgs_t * p_bgs, ble_evt_t * p_ble_evt);

#endif // BLE_BGS_H__

/** @} */
