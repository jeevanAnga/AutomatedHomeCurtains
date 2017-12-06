#ifndef BLE_AHC_SERVICE_H__
#define BLE_AHC_SERVICE_H__

#include "ble.h"
#include "ble_srv_common.h"
#include <stdint.h>
#include <stdbool.h>

#define BLE_UUID_AHC_SERVICE 0x0901                      /**< The UUID of the BluChip Service. */ //Automated Home Curtain Service
#define BLE_AHC_SERVICE_MAX_DATA_LEN 1//2//(GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the AHC service module. */

/* Forward declaration of the ble_ahc_service_t type. */
typedef struct ble_ahc_service_s ble_ahc_service_t;

/**@brief AHC Service event handler type. */
typedef void (*ble_ahc_service_curtain_write_handler_t) (ble_ahc_service_t * p_ahc_service, uint8_t * p_data, uint16_t length);

/**@brief AHC Service initialization structure.
 *
 * @details This structure contains the initialization information for the service. The application
 * must fill this structure and pass it to the service using the @ref ble_ahc_service_init
 *          function.
 */
typedef struct
{
    ble_ahc_service_curtain_write_handler_t curtain_write_handler; /**< Event handler to be called when the Curtain Characteristic is written. */
    bool opened;	//custom parameter
    bool closed;	//custom parameter
} ble_ahc_service_init_t;

/**@brief AHC Service structure.
 *
 * @details This structure contains status information related to the service.
 */
struct ble_ahc_service_s
{
    uint8_t                  uuid_type;               /**< UUID type for AHC Service Base UUID. */
    uint16_t                 service_handle;          /**< Handle of AHC Service (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t curtain_char_handles;              /**< Handles related to the Curtain characteristic (as provided by the S110 SoftDevice). */
    ble_gatts_char_handles_t limit_switch_char_handles; /**< Handles related to the Limit Switch Characteristic. */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the S110 SoftDevice). BLE_CONN_HANDLE_INVALID if not in a connection. */
    ble_ahc_service_curtain_write_handler_t   curtain_write_handler;	//Event handler to be called when the Curtain Characteristic is written.            /**< Event handler to be called for handling received data. */
    bool open;	//custom parameter
	  bool close;	//custom parameter
};

/**@brief Function for initializing the AHC Service.
 *
 * @param[out] p_ahc_service      AHC Service structure. This structure must be supplied
 *                        by the application. It is initialized by this function and will
 *                        later be used to identify this particular service instance.
 * @param[in] p_ahc_service_init  Information needed to initialize the service.
 *
 * @retval NRF_SUCCESS If the service was successfully initialized. Otherwise, an error code is returned.
 * @retval NRF_ERROR_NULL If either of the pointers p_ahc_service or p_ahc_service_init is NULL.
 */
uint32_t ble_ahc_service_init(ble_ahc_service_t * p_ahc_service, const ble_ahc_service_init_t * p_ahc_service_init);

/**@brief Function for handling the AHC Service's BLE events.
 *
 * @details The AHC Service expects the application to call this function each time an
 * event is received from the S110 SoftDevice. This function processes the event if it
 * is relevant and calls the AHC Service event handler of the
 * application if necessary.
 *
 * @param[in] p_ahc_service       AHC Service structure.
 * @param[in] p_ble_evt   Event received from the S110 SoftDevice.
 */
void ble_ahc_service_on_ble_evt(ble_ahc_service_t * p_ahc_service, ble_evt_t * p_ble_evt);

/**@brief Function for sending a limit switch state notification.
 *
 * @param[in] p_lbs      AHC Service structure.
 * @param[in] limit_switch_state  New limit switch state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ahc_service_on_opened_limit_switch_change(ble_ahc_service_t * p_lbs, uint8_t limit_switch_state);

/**@brief Function for sending a limit switch state notification.
 *
 * @param[in] p_lbs      AHC Service structure.
 * @param[in] limit_switch_state  New limit switch state.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_ahc_service_on_closed_limit_switch_change(ble_ahc_service_t * p_lbs, uint8_t closed_limit_switch_state);

#endif // BLE_AHC_SERVICE_H__

/** @} */
