#include "ble_bgs.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "ble_gatts.h"

// BluChip GPIO Service
#define BLE_UUID_BGS_GM_CHARACTERISTIC 0x0002                      /**< The UUID of the TX Characteristic. */

#define BLE_BGS_MAX_GM_CHAR_LEN        BLE_BGS_MAX_DATA_LEN        /**< Maximum length of the TX Characteristic (in bytes). */

// UUID randomly generated
// ef07432c-dab9-4378-9553-d713264dc9e3
#define BGS_BASE_UUID                  {{0xef, 0x07, 0x43, 0x2c, 0xda, 0xb9, 0x43, 0x78, 0x95, 0x53, 0xd7, 0x13, 0x26, 0x4d, 0xc9, 0xe3}} /**< Used vendor specific UUID. */

//uint8_t xfer_data[2] = {0xCC, 0xCC};

/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_bgs     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_bgs_t * p_bgs, ble_evt_t * p_ble_evt)
{
    p_bgs->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_bgs     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_bgs_t * p_bgs, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_bgs->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_bgs     Nordic UART Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write(ble_bgs_t * p_bgs, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (
             (p_evt_write->handle == p_bgs->gm_handles.value_handle)
             &&
             (p_bgs->data_handler != NULL)
            )
    {
        p_bgs->data_handler(p_bgs, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}

/**@brief Function for adding Curtain characteristic.
 *
 * @param[in] p_bgs       Nordic UART Service structure.
 * @param[in] p_bgs_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t curtain_char_add(ble_bgs_t * p_bgs, const ble_bgs_init_t * p_bgs_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.write         = 1;
    char_md.char_props.write_wo_resp = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_bgs->uuid_type;
    ble_uuid.uuid = BLE_UUID_BGS_GM_CHARACTERISTIC;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = 1;
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = BLE_BGS_MAX_GM_CHAR_LEN;

    return sd_ble_gatts_characteristic_add(p_bgs->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_bgs->gm_handles);
}

void ble_bgs_on_ble_evt(ble_bgs_t * p_bgs, ble_evt_t * p_ble_evt)
{
    if ((p_bgs == NULL) || (p_ble_evt == NULL))
    {
        return;
    }
	
	ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
			
    switch (p_ble_evt->header.evt_id)
    {
		case BLE_GATTS_AUTHORIZE_TYPE_READ:
		case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
			p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
			p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
			p_rw_authorize_reply_params.params.read.update = 0;
			sd_ble_gatts_rw_authorize_reply(p_bgs->conn_handle, &p_rw_authorize_reply_params);
			break;
	    
		case BLE_GATTS_EVT_SYS_ATTR_MISSING:
			sd_ble_gatts_sys_attr_set(p_bgs->conn_handle, NULL, 0, 0);
			break;
	    
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_bgs, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_bgs, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_bgs, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_bgs_init(ble_bgs_t * p_bgs, const ble_bgs_init_t * p_bgs_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t bgs_base_uuid = BGS_BASE_UUID;

    if ((p_bgs == NULL) || (p_bgs_init == NULL))
    {
        return NRF_ERROR_NULL;
    }

    // Initialize the service structure.
    p_bgs->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_bgs->data_handler            = p_bgs_init->data_handler;

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&bgs_base_uuid, &p_bgs->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    ble_uuid.type = p_bgs->uuid_type;
    ble_uuid.uuid = BLE_UUID_BGS_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_bgs->service_handle);
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the GPIO modification Characteristic.
    err_code = curtain_char_add(p_bgs, p_bgs_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

