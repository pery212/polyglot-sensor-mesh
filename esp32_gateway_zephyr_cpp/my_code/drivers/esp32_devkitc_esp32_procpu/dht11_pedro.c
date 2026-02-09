#define DT_DRV_COMPAT pedro_dht11

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(DHT11, CONFIG_SENSOR_LOG_LEVEL);

// Configuración fija (se lee del DeviceTree)
struct dht11_config {
  struct gpio_dt_spec dio;
  const char *sample_text;
};

// Datos variables (memoria RAM)
struct dht11_data {
  uint8_t temp, hum;
};

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

// El DHT11 envía 40 bits (5 bytes)
// [0] Humedad Int, [1] Humedad Dec, [2] Temp Int, [3] Temp Dec, [4] Checksum
static uint8_t bytes[5];

static int dht11_sample_fetch(const struct device *dev,
                              enum sensor_channel chan) {
  const struct dht11_config *config = dev->config;
  struct dht11_data *data = dev->data;
  int i, j;
  uint32_t count;

  // 1. SECCIÓN CRÍTICA: Bloqueamos interrupciones para no perder el hilo de los
  // microsegundos
  int key = irq_lock();

  // 2. SEÑAL DE START
  // El host baja el bus al menos 18ms
  gpio_pin_configure_dt(&config->dio, GPIO_OUTPUT_LOW);
  k_busy_wait(18000);

  // El host sube el bus y espera 20-40us
  gpio_pin_configure_dt(&config->dio, GPIO_INPUT);
  k_busy_wait(40);

  // 3. ESPERAR RESPUESTA DEL SENSOR (80us LOW + 80us HIGH)
  // Esperamos a que el sensor baje el bus
  count = 0;
  while (gpio_pin_get_dt(&config->dio) == 1) {
    if (++count > 1000)
      goto timeout; // Timeout de seguridad
  }
  // Esperamos a que el sensor lo suba
  count = 0;
  while (gpio_pin_get_dt(&config->dio) == 0) {
    if (++count > 1000)
      goto timeout;
  }
  // Esperamos a que el sensor lo baje para empezar a enviar bits
  count = 0;
  while (gpio_pin_get_dt(&config->dio) == 1) {
    if (++count > 1000)
      goto timeout;
  }

  // 4. LECTURA DE LOS 40 BITS
  for (i = 0; i < 5; i++) {
    bytes[i] = 0;
    for (j = 7; j >= 0; j--) {
      // Cada bit empieza con 50us en LOW
      while (gpio_pin_get_dt(&config->dio) == 0)
        ;

      // Medimos cuánto tiempo está en HIGH
      // 26-28us = '0' lógico
      // 70us    = '1' lógico
      uint32_t start_time = k_cycle_get_32();
      while (gpio_pin_get_dt(&config->dio) == 1)
        ;
      uint32_t elapsed_cycles = k_cycle_get_32() - start_time;

      // Convertimos ciclos de reloj a microsegundos (aprox para ESP32)
      uint32_t elapsed_us = k_cyc_to_us_near32(elapsed_cycles);

      if (elapsed_us > 40) {
        bytes[i] |= (1 << j);
      }
    }
  }

  irq_unlock(key); // Liberamos el procesador

  // 5. CHECKSUM
  if (bytes[4] != ((bytes[0] + bytes[1] + bytes[2] + bytes[3]) & 0xFF)) {
    LOG_ERR("Error de Checksum");
    return -EIO;
  }

  // Guardamos los datos
  data->hum = bytes[0];
  data->temp = bytes[2];

  return 0;

timeout:
  irq_unlock(key);
  LOG_ERR("Timeout esperando respuesta del DHT11");
  return -ETIMEDOUT;
}

static int dht11_channel_get(const struct device *dev, enum sensor_channel chan,
                             struct sensor_value *val) {
  struct dht11_data *data = dev->data;
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

static int dht11_init(const struct device *dev) {
  const struct dht11_config *config = dev->config;

  // Verificar si el controlador GPIO está listo
  if (!device_is_ready(config->dio.port)) {
    LOG_ERR("Error: El controlador GPIO no está listo");
    return -ENODEV;
  }

  // Configurar el pin (el DHT11 empieza como salida para enviar el start
  // signal)
  int ret = gpio_pin_configure_dt(&config->dio, GPIO_INPUT);
  if (ret < 0) {
    LOG_ERR("Error al configurar el pin GPIO (%d)", ret);
    return ret;
  }

  LOG_INF("DHT11 inicializado correctamente en pin %d con el texto %s",
          config->dio.pin, config->sample_text);
  return 0;
}

// Macro de instanciación
#define DHT11_INIT(inst)                                                       \
  static struct dht11_data dht11_data_##inst;                                  \
                                                                               \
  static const struct dht11_config dht11_config_##inst = {                     \
      .dio = GPIO_DT_SPEC_INST_GET(inst, dio_gpios),                           \
      .sample_text = DT_INST_PROP(inst, sample_text),                          \
  };                                                                           \
                                                                               \
  DEVICE_DT_INST_DEFINE(inst, dht11_init, NULL, &dht11_data_##inst,            \
                        &dht11_config_##inst, POST_KERNEL,                     \
                        CONFIG_SENSOR_INIT_PRIORITY, &dht11_api);

DT_INST_FOREACH_STATUS_OKAY(DHT11_INIT)