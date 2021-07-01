#include <esp_task_wdt.h>
#include "soc/rtc_wdt.h"

#define WATCHDOG_TIMEOUT 30
#define RTC_WATCHDOG_TIMEOUT WATCHDOG_TIMEOUT * 1000

////////////////////////
// Watchdog Controls

void watchdog_enable(){
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true); //enabled panic to trigger ESP restart
  esp_task_wdt_add(NULL); //add current thread to WDT watch
}

void rtc_watchdog_enable(){
  rtc_wdt_protect_off();
  rtc_wdt_disable();
  rtc_wdt_set_length_of_reset_signal(RTC_WDT_SYS_RESET_SIG, RTC_WDT_LENGTH_3_2us);
  rtc_wdt_set_stage(RTC_WDT_STAGE0, RTC_WDT_STAGE_ACTION_RESET_RTC);
  rtc_wdt_set_time(RTC_WDT_STAGE0, RTC_WATCHDOG_TIMEOUT);
  rtc_wdt_enable();
  rtc_wdt_protect_on();
}

void watchdog_pet() {
  esp_task_wdt_reset();
}

void rtc_watchdog_pet() {
  rtc_wdt_feed();
}
