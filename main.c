#include "iot_config.h"

/* FreeRTOS includes. */

#include "FreeRTOS.h"
#include "task.h"

/* Demo includes */
#include "aws_demo.h"
#include "aws_dev_mode_key_provisioning.h"

/* AWS System includes. */
#include "bt_hal_manager.h"
#include "iot_system_init.h"
#include "iot_logging_task.h"

#include "nvs_flash.h"

#include "FreeRTOS_Sockets.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_interface.h"
#include "esp_bt.h"
#if CONFIG_NIMBLE_ENABLED == 1
    #include "esp_nimble_hci.h"
#else
    #include "esp_gap_ble_api.h"
    #include "esp_bt_main.h"
#endif

#include "driver/uart.h"
#include "aws_application_version.h"

#include "iot_network_manager_private.h"

#if BLE_ENABLED
    #include "bt_hal_manager_adapter_ble.h"
    #include "bt_hal_manager.h"
    #include "bt_hal_gatt_server.h"

    #include "iot_ble.h"
    #include "iot_ble_config.h"
    #include "iot_ble_wifi_provisioning.h"
    #include "iot_ble_numericComparison.h"
#endif

/* Logging Task Defines. */
#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 32 )
#define mainLOGGING_TASK_STACK_SIZE         ( configMINIMAL_STACK_SIZE * 4 )
#define mainDEVICE_NICK_NAME                "Espressif_Demo"

QueueHandle_t spp_uart_queue = NULL;

/* Static arrays for FreeRTOS+TCP stack initialization for Ethernet network connections
 * are use are below. If you are using an Ethernet connection on your MCU device it is
 * recommended to use the FreeRTOS+TCP stack. The default values are defined in
 * FreeRTOSConfig.h. */

/**
 * @brief Initializes the board.
 */
static void prvMiscInitialization( void );

#if BLE_ENABLED
/* Initializes bluetooth */
    static esp_err_t prvBLEStackInit( void );
    static void spp_uart_init( void );
#endif

/*-----------------------------------------------------------*/

/**
 * @brief Application runtime entry point.
 */
int app_main( void )
{
    /* Perform any hardware initialization that does not require the RTOS to be
     * running.  */
    prvMiscInitialization();

    if( SYSTEM_Init() == pdPASS )
    {
        /* A simple example to demonstrate key and certificate provisioning in
         * microcontroller flash using PKCS#11 interface. This should be replaced
         * by production ready key provisioning mechanism. */
        vDevModeKeyProvisioning();

        #if BLE_ENABLED
            /* Initialize BLE. */
            if( prvBLEStackInit() != ESP_OK )
            {
                configPRINTF( ( "Failed to initialize the bluetooth stack\n " ) );

                while( 1 )
                {
                }
            }
        #else
            ESP_ERROR_CHECK( esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT ) );
            ESP_ERROR_CHECK( esp_bt_controller_mem_release( ESP_BT_MODE_BLE ) );
        #endif /* if BLE_ENABLED */
        /* Run all demos. */
        DEMO_RUNNER_RunDemos();
    }

    /* Start the scheduler.  Initialization that requires the OS to be running,
     * including the WiFi initialization, is performed in the RTOS daemon task
     * startup hook. */
    /* Following is taken care by initialization code in ESP IDF */
    /* vTaskStartScheduler(); */
    return 0;
}

/*-----------------------------------------------------------*/
extern void vApplicationIPInit( void );
static void prvMiscInitialization( void )
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if( ( ret == ESP_ERR_NVS_NO_FREE_PAGES ) || ( ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) )
    {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK( ret );

    #if BLE_ENABLED
        NumericComparisonInit();
        spp_uart_init();
    #endif

    /* Create tasks that are not dependent on the WiFi being initialized. */
    xLoggingTaskInitialize( mainLOGGING_TASK_STACK_SIZE,
                            tskIDLE_PRIORITY + 5,
                            mainLOGGING_MESSAGE_QUEUE_LENGTH );

    vApplicationIPInit();
}

/*-----------------------------------------------------------*/

#if BLE_ENABLED

    #if CONFIG_NIMBLE_ENABLED == 1
        esp_err_t prvBLEStackInit( void )
        {
            /* Initialize BLE */
            esp_err_t xRet = ESP_OK;

            xRet = esp_nimble_hci_and_controller_init();

            return xRet;
        }

    #else /* if CONFIG_NIMBLE_ENABLED == 1 */

        static esp_err_t prvBLEStackInit( void )
        {
            /* Initialize BLE */
            esp_err_t xRet = ESP_OK;
            esp_bt_controller_config_t xBtCfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();


            ESP_ERROR_CHECK( esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT ) );

            xRet = esp_bt_controller_init( &xBtCfg );

            if( xRet == ESP_OK )
            {
                xRet = esp_bt_controller_enable( ESP_BT_MODE_BLE );
            }
            else
            {
                configPRINTF( ( "Failed to initialize bt controller, err = %d", xRet ) );
            }

            if( xRet == ESP_OK )
            {
                xRet = esp_bluedroid_init();
            }
            else
            {
                configPRINTF( ( "Failed to initialize bluedroid stack, err = %d", xRet ) );
            }

            if( xRet == ESP_OK )
            {
                xRet = esp_bluedroid_enable();
            }

            return xRet;
        }
    #endif /* if CONFIG_NIMBLE_ENABLED == 1 */
#endif /* if BLE_ENABLED */

/*-----------------------------------------------------------*/

#if BLE_ENABLED
    static void spp_uart_init( void )
    {
        uart_config_t uart_config =
        {
            .baud_rate           = 115200,
            .data_bits           = UART_DATA_8_BITS,
            .parity              = UART_PARITY_DISABLE,
            .stop_bits           = UART_STOP_BITS_1,
            .flow_ctrl           = UART_HW_FLOWCTRL_RTS,
            .rx_flow_ctrl_thresh = 122,
        };

        /* Set UART parameters */
        uart_param_config( UART_NUM_0, &uart_config );
        /*Set UART pins */
        uart_set_pin( UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE );
        /*Install UART driver, and get the queue. */
        uart_driver_install( UART_NUM_0, 4096, 8192, 10, &spp_uart_queue, 0 );
    }

/*-----------------------------------------------------------*/

    BaseType_t getUserMessage( INPUTMessage_t * pxINPUTmessage,
                               TickType_t xAuthTimeout )
    {
        uart_event_t xEvent;
        BaseType_t xReturnMessage = pdFALSE;

        if( xQueueReceive( spp_uart_queue, ( void * ) &xEvent, ( portTickType ) xAuthTimeout ) )
        {
            switch( xEvent.type )
            {
                /*Event of UART receiving data */
                case UART_DATA:

                    if( xEvent.size )
                    {
                        pxINPUTmessage->pcData = ( uint8_t * ) malloc( sizeof( uint8_t ) * xEvent.size );

                        if( pxINPUTmessage->pcData != NULL )
                        {
                            memset( pxINPUTmessage->pcData, 0x0, xEvent.size );
                            uart_read_bytes( UART_NUM_0, ( uint8_t * ) pxINPUTmessage->pcData, xEvent.size, portMAX_DELAY );
                            xReturnMessage = pdTRUE;
                        }
                        else
                        {
                            configPRINTF( ( "Malloc failed in main.c\n" ) );
                        }
                    }

                    break;

                default:
                    break;
            }
        }

        return xReturnMessage;
    }
#endif /* if BLE_ENABLED */

/*-----------------------------------------------------------*/

extern void esp_vApplicationTickHook();
void IRAM_ATTR vApplicationTickHook()
{
    esp_vApplicationTickHook();
}

/*-----------------------------------------------------------*/
extern void esp_vApplicationIdleHook();
void vApplicationIdleHook()
{
    esp_vApplicationIdleHook();
}

/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
}

/*-----------------------------------------------------------*/

void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
    uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
    system_event_t evt;

    if( eNetworkEvent == eNetworkUp )
    {
        /* Print out the network configuration, which may have come from a DHCP
         * server. */
        FreeRTOS_GetAddressConfiguration(
            &ulIPAddress,
            &ulNetMask,
            &ulGatewayAddress,
            &ulDNSServerAddress );

        evt.event_id = SYSTEM_EVENT_STA_GOT_IP;
        evt.event_info.got_ip.ip_changed = true;
        evt.event_info.got_ip.ip_info.ip.addr = ulIPAddress;
        evt.event_info.got_ip.ip_info.netmask.addr = ulNetMask;
        evt.event_info.got_ip.ip_info.gw.addr = ulGatewayAddress;
        esp_event_send( &evt );
    }
}
