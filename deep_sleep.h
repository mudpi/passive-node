////////////////////////
// Sleep Controls

void print_wakeup_reason(){
  //ESP_RST_UNKNOWN
  //ESP_RST_POWERON
  //ESP_RST_EXT
  //ESP_RST_SW
  //ESP_RST_PANIC
  //ESP_RST_INT_WDT
  //ESP_RST_TASK_WDT
  //ESP_RST_WDT
  //ESP_RST_DEEPSLEEP
  //ESP_RST_BROWNOUT
  //ESP_RST_SDIO
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


void enter_deep_sleep(int sleep_seconds = 0) {
  if (sleep_seconds == 0) {
    sleep_seconds = TIME_TO_SLEEP;
  }
  Serial.println("Entering sleep for " + String(sleep_seconds) + " Seconds");
  Serial.flush(); 
  esp_deep_sleep_start();
}
