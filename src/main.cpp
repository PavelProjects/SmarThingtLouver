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
    controller.addLedIndicator(SmartThing.getLed());
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
  SmartThing.addActionHandler(ENABLE_AUTO_MODE, "Enable automode", []() {
    return ActionResult(controller.enableAutoMode());
  });
  SmartThing.addActionHandler(DISABLE_AUTO_MODE, "Disable automode", []() {
    return ActionResult(controller.disableAutoMode());
  });
  SmartThing.addActionHandler(OPEN, "Open", []() {
    return ActionResult(controller.open());
  });
  SmartThing.addActionHandler(CLOSE, "Close", []() {
    return ActionResult(controller.close());
  });
  SmartThing.addActionHandler(MIDDLE, "Middle", []() {
    return ActionResult(controller.middle());
  });
  SmartThing.addActionHandler(BRIGHT, "Bright", []() {
    return ActionResult(controller.bright());
  });
}
void addSensors() {
  SmartThing.addDigitalSensor("button", BUTTON_PIN);
  SmartThing.addAnalogSensor("light", LIGHT_SENSOR_PIN);
  SmartThing.addAnalogSensor("position", POT_PIN);
}
void addStates() {
  SmartThing.addDeviceState("position", []() {
    return controller.getPosition();
  });
  SmartThing.addDeviceState("automode", []() {
    if (controller.isAutoModeEnabled()) {
      return "true";
    }
    return "false";
  });
}
void addConfigEntries() {
  SmartThing.addConfigEntry(DELAY_SETTING, "Automode update delay", "number");
  SmartThing.addConfigEntry(ACCURACY_SETTING, "Motor accuracy", "number");
  SmartThing.addConfigEntry(CLOSE_SETTING, "Light close value", "number");
  SmartThing.addConfigEntry(OPEN_SETTING, "Light open value", "number");
  SmartThing.addConfigEntry(BRIGHT_SETTING, "Light bright value", "number");
}

void addHooks() {
  Hook::LambdaHook<int16_t> * hook = new Hook::LambdaHook<int16_t>(
    [](int16_t &value) {
      LOGGER.info("main", "Hook called");
      if (controller.isAutoModeEnabled()) {
        controller.disableAutoMode();
      } else {
        controller.enableAutoMode();
      }
      LOGGER.statistics();
    },
    true
  );
  hook->setTriggerValue(1);
  hook->setTriggerDisabled(false);

  HooksManager.addHook(SmartThing.getSensor("button"), hook);
}

void setupRestHandlers() {
  RestController.addConfigUpdatedHandler([]() {
    processConfig();
  });
}
void processConfig() {
  JsonObject config = STSettings.getConfig();
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
