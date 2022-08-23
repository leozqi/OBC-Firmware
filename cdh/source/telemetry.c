#include "telemetry.h"
#include "supervisor.h"

#include <FreeRTOS.h>
#include <os_portmacro.h>
#include <os_queue.h>
#include <os_task.h>
#include <os_timer.h>

#include <sys_common.h>
#include <gio.h>

static TaskHandle_t telemetryTaskHandle = NULL;
static StaticTask_t telemetryTaskBuffer;
static StackType_t telemetryTaskStack[TELEMETRY_STACK_SIZE];

static QueueHandle_t telemetryQueueHandle = NULL;
static StaticQueue_t telemetryQueue;
static uint8_t telemetryQueueStack[TELEMETRY_QUEUE_LENGTH*TELEMETRY_QUEUE_ITEM_SIZE];

static TimerHandle_t ledTimerHandle = NULL;
static StaticTimer_t ledTimerBuffer;

/**
 * @brief	Telemetry task.
 * @param	pvParameters	Task parameters.
 */
static void vTelemetryTask(void * pvParameters);

/**
 * @brief Example timer callback function for "Turn on LED" event
 */
static void timerCallback(TimerHandle_t xTimer);

void initTelemetry(void) {
    ASSERT( (telemetryTaskStack != NULL) && (&telemetryTaskBuffer != NULL) );
    if (telemetryTaskHandle == NULL) {
        telemetryTaskHandle = xTaskCreateStatic(vTelemetryTask, TELEMETRY_NAME, TELEMETRY_STACK_SIZE, NULL, TELEMETRY_PRIORITY, telemetryTaskStack, &telemetryTaskBuffer);
    }

    ASSERT( (telemetryQueueStack != NULL) && (&telemetryQueue != NULL) );
    if (telemetryQueueHandle == NULL) {
        telemetryQueueHandle = xQueueCreateStatic(TELEMETRY_QUEUE_LENGTH, TELEMETRY_QUEUE_ITEM_SIZE, telemetryQueueStack, &telemetryQueue);
    }

    ASSERT(&ledTimerBuffer != NULL);
    if (ledTimerHandle == NULL) {
        ledTimerHandle = xTimerCreateStatic("ledTimer", pdMS_TO_TICKS(1000), false, NULL, timerCallback, &ledTimerBuffer);
    }
}

uint8_t sendToTelemetryQueue(telemetry_event_t *event) {
    if (telemetryQueueHandle == NULL || event == NULL) {
        return 0;
    }
    if ( xQueueSend(telemetryQueueHandle, (void *) event, portMAX_DELAY) == pdPASS ) {
        return 1;
    }
    return 0;
}

static void vTelemetryTask(void * pvParameters) {
    while(1){
        telemetry_event_t queueMsg;
        if(xQueueReceive(telemetryQueueHandle, &queueMsg, TELEMETRY_QUEUE_WAIT_PERIOD) != pdPASS){
            queueMsg.eventID = TELEMETRY_NULL_EVENT_ID;
        }

        switch(queueMsg.eventID)
        {
            case TURN_ON_LED_EVENT_ID:
                vTaskDelay(queueMsg.data.i);
                gioToggleBit(gioPORTB, 1);
                xTimerStart(ledTimerHandle, TELEMETRY_DELAY_TICKS);
                break;
            default:
                ;
        }
    }
}

static void timerCallback(TimerHandle_t xTimer) {
    supervisor_event_t newMsg;
    newMsg.eventID = TURN_OFF_LED_EVENT_ID;
    sendToSupervisorQueue(&newMsg);
}
