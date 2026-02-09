#define DT_DRV_COMPAT pedro_temperature_sensor
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

struct temperature_sensor_data {
  uint8_t temp, hum;
};

static int temperature_sensor_sample_fetch(const struct device *dev,
                                           enum sensor_channel chan) {
  struct temperature_sensor_data *data =
      (struct temperature_sensor_data *)dev->data;
  // Simulación
  data->temp = 24;
  data->hum = 63;
  return 0;
}

static int temperature_sensor_channel_get(const struct device *dev,
                                          enum sensor_channel chan,
                                          struct sensor_value *val) {
  struct temperature_sensor_data *data =
      (struct temperature_sensor_data *)dev->data;
  if (chan == SENSOR_CHAN_AMBIENT_TEMP) {
    val->val1 = data->temp;
    val->val2 = 0;
  } else if (chan == SENSOR_CHAN_HUMIDITY) {
    val->val1 = data->hum;
    val->val2 = 0;
  } else {
    return -ENOTSUP;
  }
  return 0;
}

static const struct sensor_driver_api temperature_sensor_api = {
    .sample_fetch = temperature_sensor_sample_fetch,
    .channel_get = temperature_sensor_channel_get,
};

// Función de inicialización del dispositivo
static int temperature_sensor_init(const struct device *dev) {
  // Do nothing, since this a simulated linux device
  return 0;
}

#define TEMPERATURE_SENSOR_INIT(n)                                             \
  static struct temperature_sensor_data temperature_sensor_data_##n;           \
  DEVICE_DT_INST_DEFINE(n, temperature_sensor_init, NULL,                      \
                        &temperature_sensor_data_##n, NULL, POST_KERNEL,       \
                        CONFIG_SENSOR_INIT_PRIORITY, &temperature_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(TEMPERATURE_SENSOR_INIT)