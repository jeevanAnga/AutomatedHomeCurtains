#include "ble_ahc_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "ble_gatts.h"

// AHC UART Service
#define BLE_UUID_AHC_SERVICE_CURTAIN_CHARACTERISTIC 0x0902                      /**< The UUID of the Curtain Characteristic. */
#define BLE_UUID_AHC_SERVICE_LIMIT_SWITCH_CHARACTERISTIC 0x0903                      /**< The UUID of the Limit Switch (State) Characteristic. */

#define BLE_AHC_SERVICE_MAX_CURTAIN_CHAR_LEN        BLE_AHC_SERVICE_MAX_DATA_LEN        /**< Maximum length of the Curtain Characteristic (in bytes). */
#define BLE_AHC_SERVICE_MAX_LIMIT_SWITCH_CHAR_LEN        BLE_AHC_SERVICE_MAX_DATA_LEN        /**< Maximum length of the Limit Switch Characteristic (in bytes). */

// UUID randomly generated
// ef07432c-dab9-4378-9553-d713264dc9e3
#define AHC_SERVICE_BASE_UUID                  {{0xef, 0x07, 0x43, 0x2c, 0xda, 0xb9, 0x43, 0x78, 0x95, 0x53, 0xd7, 0x13, 0x26, 0x4d, 0xc9, 0xe3}} /**< Used vendor specific UUID. */

uint8_t xfer_data[2] = {0xCC, 0xCC};

/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_ahc_service     AHC Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(ble_ahc_service_t * p_ahc_service, ble_evt_t * p_ble_evt)
{
    p_ahc_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the S110 SoftDevice.
 *
 * @param[in] p_ahc_service     AHC Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(ble_ahc_service_t * p_ahc_service, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_ahc_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the S110 SoftDevice.
 *
 * @param[in] p_ahc_service     AHC Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write(ble_ahc_service_t * p_ahc_service, ble_evt_t * p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    if (
             (p_evt_write->handle == p_ahc_service->curtain_char_handles.value_handle)
             &&
             (p_ahc_service->curtain_write_handler != NULL)
            )
    {
        p_ahc_service->curtain_write_handler(p_ahc_service, p_evt_write->data, p_evt_write->len);
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}

/**@brief Function for adding Curtain characteristic.
 *
 * @param[in] p_ahc_service       AHC Service structure.
 * @param[in] p_ahc_service_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t curtain_char_add(ble_ahc_service_t * p_ahc_service, const ble_ahc_service_init_t * p_ahc_service_init)
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

    ble_uuid.type = p_ahc_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_AHC_SERVICE_CURTAIN_CHARACTERISTIC;

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
    attr_char_value.max_len   = BLE_AHC_SERVICE_MAX_CURTAIN_CHAR_LEN;

    return sd_ble_gatts_characteristic_add(p_ahc_service->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ahc_service->curtain_char_handles);
}

/**@brief Function for adding Limit Switch characteristic.
 *
 * @param[in] p_ahc_service       AHC Service structure.
 * @param[in] p_ahc_service_init  Information needed to initialize the service.
 *
 * @return NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t limit_switch_char_add(ble_ahc_service_t * p_ahc_service, const ble_ahc_service_init_t * p_ahc_service_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read          = 1;
    char_md.char_props.notify 			 = 1;
    char_md.p_char_user_desc         = NULL;
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = &cccd_md;
    char_md.p_sccd_md                = NULL;

    ble_uuid.type = p_ahc_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_AHC_SERVICE_LIMIT_SWITCH_CHARACTERISTIC;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);

    attr_md.vloc    = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth = 0;
    attr_md.wr_auth = 0;
    attr_md.vlen    = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);//BLE_AHC_SERVICE_MAX_LIMIT_SWITCH_CHAR_LEN

    return sd_ble_gatts_characteristic_add(p_ahc_service->service_handle,
                                           &char_md,
                                           &attr_char_value,
                                           &p_ahc_service->limit_switch_char_handles);
}

void ble_ahc_service_on_ble_evt(ble_ahc_service_t * p_ahc_service, ble_evt_t * p_ble_evt)
{
    if ((p_ahc_service == NULL) || (p_ble_evt == NULL))
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
			sd_ble_gatts_rw_authorize_reply(p_ahc_service->conn_handle, &p_rw_authorize_reply_params);
			break;
	    
		case BLE_GATTS_EVT_SYS_ATTR_MISSING:
			sd_ble_gatts_sys_attr_set(p_ahc_service->conn_handle, NULL, 0, 0);
			break;
	    
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ahc_service, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_ahc_service, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_ahc_service, p_ble_evt);
            break;

        default:
            // No implementation needed.
            break;
    }
}


uint32_t ble_ahc_service_init(ble_ahc_service_t * p_ahc_service, const ble_ahc_service_init_t * p_ahc_service_init)
{
    uint32_t      err_code;
    ble_uuid_t    ble_uuid;
    ble_uuid128_t ahc_service_base_uuid = AHC_SERVICE_BASE_UUID;
    //(bool open;	//custom parameter)
	  //(bool close;	//custom parameter)
	
    if ((p_ahc_service == NULL) || (p_ahc_service_init == NULL))
    {
        return NRF_ERROR_NULL;
    }

    // Initialize the service structure.
    p_ahc_service->conn_handle             = BLE_CONN_HANDLE_INVALID;
    p_ahc_service->curtain_write_handler   = p_ahc_service_init->curtain_write_handler;

    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    // Add a custom base UUID.
    err_code = sd_ble_uuid_vs_add(&ahc_service_base_uuid, &p_ahc_service->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    ble_uuid.type = p_ahc_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_AHC_SERVICE;

    // Add the service.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &ble_uuid,
                                        &p_ahc_service->service_handle);
    /**@snippet [Adding proprietary Service to S110 SoftDevice] */
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the Curtain Characteristic.
    err_code = curtain_char_add(p_ahc_service, p_ahc_service_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the Limit Switch Characteristic.
		err_code = limit_switch_char_add(p_ahc_service, p_ahc_service_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		
    return NRF_SUCCESS;
}

uint32_t ble_ahc_service_on_opened_limit_switch_change(ble_ahc_service_t * p_ahc_service, uint8_t opened_limit_switch_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(opened_limit_switch_state);
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_ahc_service->limit_switch_char_handles.value_handle;
    params.p_data = &opened_limit_switch_state;
    params.p_len = &len;
  
    return sd_ble_gatts_hvx(p_ahc_service->conn_handle, &params);

	
	
	}
uint32_t ble_ahc_service_on_closed_limit_switch_change(ble_ahc_service_t * p_ahc_service, uint8_t closed_limit_switch_state)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(closed_limit_switch_state);
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_ahc_service->limit_switch_char_handles.value_handle;
    params.p_data = &closed_limit_switch_state;
    params.p_len = &len;
  
    return sd_ble_gatts_hvx(p_ahc_service->conn_handle, &params);

	
	
	}

