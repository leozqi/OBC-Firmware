#include "supervisor.h"
#include "telemetry.h"
#include "adcs_manager.h"
#include "comms_manager.h"
#include "eps_manager.h"
#include "payload_manager.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>

#include <sys_common.h>
#include <gio.h>

#include "obc_sci_io.h"
#include <sci.h>

#include <redposix.h>

static TaskHandle_t supervisorTaskHandle = NULL;
static StaticTask_t supervisorTaskBuffer;
static StackType_t supervisorTaskStack[SUPERVISOR_STACK_SIZE];

static QueueHandle_t supervisorQueueHandle = NULL;
static StaticQueue_t supervisorQueue;
static uint8_t supervisorQueueStack[SUPERVISOR_QUEUE_LENGTH*SUPERVISOR_QUEUE_ITEM_SIZE];

/**
 * @brief	Supervisor task.
 * @param	pvParameters	Task parameters.
 */
static void vSupervisorTask(void * pvParameters);

/**
 * @brief Send all startup messages from the supervisor task to other tasks.
 */
static void sendStartupMessages(void);

void initSupervisor(void) {
    ASSERT( (supervisorTaskStack != NULL) && (&supervisorTaskBuffer != NULL) );
    if (supervisorTaskHandle == NULL) {
        supervisorTaskHandle = xTaskCreateStatic(vSupervisorTask, SUPERVISOR_NAME, SUPERVISOR_STACK_SIZE, NULL, SUPERVISOR_PRIORITY, supervisorTaskStack, &supervisorTaskBuffer);
    }

    ASSERT( (supervisorQueueStack != NULL) && (&supervisorQueue != NULL) );
    if (supervisorQueueHandle == NULL) {
        supervisorQueueHandle = xQueueCreateStatic(SUPERVISOR_QUEUE_LENGTH, SUPERVISOR_QUEUE_ITEM_SIZE, supervisorQueueStack, &supervisorQueue);
    }
}

obc_error_code_t sendToSupervisorQueue(supervisor_event_t *event) {
    ASSERT(supervisorQueueHandle != NULL);

    if (event == NULL)
        return OBC_ERR_CODE_INVALID_ARG;
    
    if (xQueueSend(supervisorQueueHandle, (void *) event, SUPERVISOR_QUEUE_TX_WAIT_PERIOD) == pdPASS)
        return OBC_ERR_CODE_SUCCESS;
    
    return OBC_ERR_CODE_QUEUE_FULL;
}

static void sendStartupMessages(void) {
    /* Send startup message to telemetry task as example */
    telemetry_event_t newMsg;
    newMsg.eventID = TURN_ON_LED_EVENT_ID;
    newMsg.data.i = TELEMETRY_DELAY_TICKS;
    sendToTelemetryQueue(&newMsg);

    /* TODO: Add startup messages to other tasks */
}

static void vSupervisorTask(void * pvParameters) {
    ASSERT(supervisorQueueHandle != NULL);

    /* Initialize other tasks */
    initTelemetry();
    initADCSManager();
    initCommsManager();
    initEPSManager();
    initPayloadManager();

    /* Send initial messages to system queues */
    sendStartupMessages();    

    int32_t ret;
    
    ret = red_init();
    sciPrintf(scilinREG, "red_init() returned %d\r\n", ret);
    if(ret == 0) {
        ret = red_format("");
        sciPrintf(scilinREG, "red_format() returned %d\r\n", ret);
        if(ret == 0) {
            ret = red_mount("");
            sciPrintf(scilinREG, "red_mount() returned %d\r\n", ret);
            if (ret == 0) {
                int32_t file = red_open("/reliance.txt", RED_O_CREAT | RED_O_EXCL | RED_O_RDWR);
                sciPrintf(scilinREG, "red_open() opened %d\r\n", file);

                unsigned char buf_wr[] = "Hello World!\r\n";
                ret = red_write(file, buf_wr, sizeof(buf_wr));
                sciPrintf(scilinREG, "red_write() returned %d\r\n", ret);
                // red_write(file, "Hello World!\n", 14);
                unsigned char buf[20];
                ret = red(file, buf, 15);
                sciPrintf(scilinREG, "red_read() returned %d\r\n", ret);

                printTextSci(scilinREG, buf, 14);
                sciPrintf(scilinREG, "\r\n", NULL);

                RED_STAT stat;
                red_stat("/reliance.txt", );
                
                ret = red_close(file);
                sciPrintf(scilinREG, "red_close() returned %d\r\n", ret);
            }
        }
    }
    
    while(1) {
        supervisor_event_t inMsg;
        telemetry_event_t outMsgTelemetry;

        if (xQueueReceive(supervisorQueueHandle, &inMsg, SUPERVISOR_QUEUE_RX_WAIT_PERIOD) != pdPASS)
            inMsg.eventID = SUPERVISOR_NULL_EVENT_ID;

        switch (inMsg.eventID) {
            case TURN_OFF_LED_EVENT_ID:
                gioToggleBit(gioPORTB, 0);
                outMsgTelemetry.eventID = TURN_ON_LED_EVENT_ID;
                outMsgTelemetry.data.i = TELEMETRY_DELAY_TICKS;
                sendToTelemetryQueue(&outMsgTelemetry);
                break;
            case SUPERVISOR_NULL_EVENT_ID:
                break;
            default:
                ;
        }
    }
}
