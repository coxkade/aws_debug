/**
 * @file aws_demo_config.h
 * @author Kade Cox
 * @date Created: Sep 5, 2019
 * @details
 * 
 */

#ifndef INCLUDE_AWS_DEMO_CONFIG_H_
#define INCLUDE_AWS_DEMO_CONFIG_H_

/* To run a particular demo you need to define one of these.
 * Only one demo can be configured at a time
 *
 *          CONFIG_MQTT_DEMO_ENABLED
 *          CONFIG_SHADOW_DEMO_ENABLED
 *          CONFIG_GREENGRASS_DISCOVERY_DEMO_ENABLED
 *          CONFIG_TCP_ECHO_CLIENT_DEMO_ENABLED
 *          CONFIG_DEFENDER_DEMO_ENABLED
 *          CONFIG_OTA_UPDATE_DEMO_ENABLED
 *          CONFIG_BLE_GATT_SERVER_DEMO_ENABLED
 *
 *
 *  These defines are used in iot_demo_runner.h for demo selection */

#define CONFIG_MQTT_DEMO_ENABLED

/* Default configuration for all demos. Individual demos can override these below */
#define democonfigDEMO_STACKSIZE    ( configMINIMAL_STACK_SIZE * 8 )
#define democonfigDEMO_PRIORITY     ( tskIDLE_PRIORITY + 5 )
#define democonfigNETWORK_TYPES     ( AWSIOT_NETWORK_TYPE_WIFI )

#if defined( CONFIG_MQTT_DEMO_ENABLED )
    #undef democonfigNETWORK_TYPES
    #define democonfigNETWORK_TYPES    ( AWSIOT_NETWORK_TYPE_WIFI | AWSIOT_NETWORK_TYPE_BLE )
#endif

#if defined( CONFIG_OTA_UPDATE_DEMO_ENABLED )
    #undef democonfigNETWORK_TYPES
    #define democonfigNETWORK_TYPES                       ( AWSIOT_NETWORK_TYPE_WIFI )
#endif

#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 12 )


#endif /* INCLUDE_AWS_DEMO_CONFIG_H_ */
