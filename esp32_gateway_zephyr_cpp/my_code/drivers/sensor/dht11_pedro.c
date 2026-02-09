#define DT_DRV_COMPAT pedro_dht11
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

struct dht11_data {
  struct gpio_dt_spec dio;
  uint8_t temp, hum;
};

static int dht11_sample_fetch(const struct device *dev,
                              enum sensor_channel chan) {
  struct dht11_data *data = (struct dht11_data *)dev->data;
  // Simulación
  data->temp = 24;
  data->hum = 63;
  return 0;
}

static int dht11_channel_get(const struct device *dev, enum sensor_channel chan,
                             struct sensor_value *val) {
  struct dht11_data *data = (struct dht11_data *)dev->data;
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

static const struct sensor_driver_api dht11_api = {
    .sample_fetch = dht11_sample_fetch,
    .channel_get = dht11_channel_get,
};

// Función de inicialización del dispositivo
static int dht11_init(const struct device *dev) {
  // struct dht11_data *data = (struct dht11_data *)dev->data;
  //
  //  //// Obtenemos la config del GPIO desde el Device Tree (gracias al YAML)
  // data->dio = GPIO_DT_SPEC_INST_GET(0, dio_gpios);
  //
  //  //if (!gpio_is_ready_dt(&data->dio)) {
  //  return -ENODEV;
  //}
  return 0;
}

#define DHT11_INIT(n)                                                          \
  static struct dht11_data dht11_data_##n;                                     \
  DEVICE_DT_INST_DEFINE(n, dht11_init, NULL, &dht11_data_##n, NULL,            \
                        POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &dht11_api);

DT_INST_FOREACH_STATUS_OKAY(DHT11_INIT)