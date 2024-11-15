// Auto-generated code
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SmartThing.h>

#include "LouverController.h"

#define BUTTON_PIN 12

#define MOTOR_FIRST_PIN 27
#define MOTOR_SECOND_PIN 26
#define POT_PIN 32
#define LIGHT_SENSOR_PIN 39

#define AUTO_MODE_STATE "automode"
#define DISABLE_AUTO_MODE "disable_auto"
#define ENABLE_AUTO_MODE "enable_auto"
#define OPEN "open"
#define CLOSE "close"
#define MIDDLE "middle"
#define BRIGHT "bright"

#define CLOSE_SETTING "light_close"
#define OPEN_SETTING "light_open"
#define BRIGHT_SETTING "light_bright"
#define DELAY_SETTING "delay"
#define ACCURACY_SETTING "accuracy"

LouverController controller;

void addStates();
void addSensors();
void addActions();
void addConfigEntries();
void setupRestHandlers();
void processConfig();
void addHooks();

void setup() {
  addStates();
  addSensors();
  addActions();
  addConfigEntries();
  addHooks();

  if (SmartThing.init("louver")) {
    if (SmartThing.wifiConnected()) {
      setupRestHandlers();
    }
    LOGGER.info("main", "SmartThing successfully initialized");
    controller.init(MOTOR_FIRST_PIN, MOTOR_SECOND_PIN, POT_PIN, LIGHT_SENSOR_PIN);
    LOGGER.info("main", "Controller created");
    processConfig();
    LOGGER.info("main", "Config proceed");
  } else {
    LOGGER.error("main", "Failed to init SmartThing!");
  }

  if (SmartThing.wifiConnected()) {
    ArduinoOTA.begin();
  }

  LOGGER.info("main", "Setup finished");
}
void loop() {
  if (SmartThing.wifiConnected()) {
    ArduinoOTA.handle();
  }
  delay(500);
}

void addActions() {
  ActionsManager.add(ENABLE_AUTO_MODE, "Enable automode", []() {
    return ActionResult(controller.enableAutoMode());
  });
  ActionsManager.add(DISABLE_AUTO_MODE, "Disable automode", []() {
    return ActionResult(controller.disableAutoMode());
  });
  ActionsManager.add(OPEN, "Open", []() {
    return ActionResult(controller.open());
  });
  ActionsManager.add(CLOSE, "Close", []() {
    return ActionResult(controller.close());
  });
  ActionsManager.add(MIDDLE, "Middle", []() {
    return ActionResult(controller.middle());
  });
  ActionsManager.add(BRIGHT, "Bright", []() {
    return ActionResult(controller.bright());
  });
}
void addSensors() {
  ObservablesManager.addDigitalSensor("button", BUTTON_PIN);
  ObservablesManager.addAnalogSensor("light", LIGHT_SENSOR_PIN);
  ObservablesManager.addAnalogSensor("position", POT_PIN);
}
void addStates() {
  ObservablesManager.addDeviceState("position", []() {
    return controller.getPosition();
  });
  ObservablesManager.addDeviceState("automode", []() {
    if (controller.isAutoModeEnabled()) {
      return "true";
    }
    return "false";
  });
}
void addConfigEntries() {
  SettingsRepository.addConfigEntry(DELAY_SETTING, "Automode update delay", CONFIG_INTEGER);
  SettingsRepository.addConfigEntry(ACCURACY_SETTING, "Motor accuracy", CONFIG_INTEGER);
  SettingsRepository.addConfigEntry(CLOSE_SETTING, "Light close value", CONFIG_INTEGER);
  SettingsRepository.addConfigEntry(OPEN_SETTING, "Light open value",CONFIG_INTEGER);
  SettingsRepository.addConfigEntry(BRIGHT_SETTING, "Light bright value", CONFIG_INTEGER);
}

void addHooks() {
  LambdaHook<SensorHook, int16_t> * hook = new LambdaHook<SensorHook, int16_t>(
    [](int16_t &value) {
      LOGGER.info("main", "Hook called");
      if (controller.isAutoModeEnabled()) {
        controller.disableAutoMode();
      } else {
        controller.enableAutoMode();
      }
    },
    true
  );
  hook->setThreshold(0);
  hook->disableTrigger();

  HooksManager.addHook(ObservablesManager.getSensor("button"), hook);
}

void setupRestHandlers() {
  RestController.addConfigUpdatedHandler([]() {
    processConfig();
  });
}
void processConfig() {
  JsonDocument config = SettingsRepository.getConfig();
  int lightClose = config[CLOSE_SETTING];
  int lightOpen = config[OPEN_SETTING];
  int lightBright = config[BRIGHT_SETTING];
  if (lightClose != 0 && lightOpen != 0 && lightBright != 0) {
    controller.setLightValues(lightClose, lightOpen, lightBright);
  }

  int delay = config[DELAY_SETTING];
  if (delay > 0) {
    controller.setMonitorTaskDelay(delay);
  }

  uint8_t accuracy = config[ACCURACY_SETTING];
  if (accuracy > 0) {
    controller.setMotorAccuracy(accuracy);
  }
}
