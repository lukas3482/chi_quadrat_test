#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_mac.h"

#define RNG_BASE 0x60026000
#define RNG_DATA_REG_OFFSET 0xB0

#define LOWPOWER_MGR_BASE 0x60008000
#define RTC_CNTL_CLK_CONF_REG 0x70
#define RTC_CNTL_DIG_FOSC_EN_BIT 10
#define RTC_CNTL_DIG_FOSC_EN (0x1 << RTC_CNTL_DIG_FOSC_EN_BIT)

// Pointer auf Random Generator
volatile uint32_t* pRngDataReg = (volatile uint32_t*) (RNG_BASE | RNG_DATA_REG_OFFSET);
// Pointer zu Config (Bit 10 muss auf 1 gesetzt werden -> RTC_CNTL_DIG_FOSC_EN)
volatile uint32_t* pRtcCntlClkConfReg = (volatile uint32_t*) (LOWPOWER_MGR_BASE | RTC_CNTL_CLK_CONF_REG);

static void testRNG(uint32_t obervations, uint32_t m);

inline uint32_t nextRand(){
    return *pRngDataReg;
}

inline void switchOnRtc20MClk(){
    *pRtcCntlClkConfReg |= RTC_CNTL_DIG_FOSC_EN;
}

void testRNG(uint32_t obervations, uint32_t m){
    uint32_t* n = calloc(m, sizeof(uint32_t));
    for(int i = 0; i < obervations; i++){
        n[nextRand() % m]++;
    }
    
    printf("--------------------\n");
    for(uint32_t i = 0; i < m; i++){
        printf("%ld:%ld\n", i, n[i]);
    }

    free(n);
    n = NULL;
}


// Main
void app_main(void){
    switchOnRtc20MClk();

    while(1){
        testRNG(1000000, 10);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
