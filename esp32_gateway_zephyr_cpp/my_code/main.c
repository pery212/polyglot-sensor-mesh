#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h> // API estándar de sensores
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main_app);

int main(void) {
  LOG_INF("Main arrancando");
  // 1. Obtener el puntero al dispositivo usando el "label" del Device Tree
  // 'mi_dht11' es el nombre que pusimos en el app.overlay
  //
  const struct device *const dev = DEVICE_DT_GET(DT_NODELABEL(temp_sensor));
  // const struct device *const dev =
  //     DEVICE_DT_GET(DT_PATH(my_sensors, dht11_sensor));

  // 2. Verificar que el driver cargó correctamente
  if (!device_is_ready(dev)) {
    LOG_ERR("El driver personalizado de Pedro NO está listo.");
    return 0;
  }

  LOG_INF("Driver listo. Empezando lecturas...");

  struct sensor_value temp, hum;

  while (1) {
    // 3. Pedir al driver que capture una muestra (llama a tu
    // dht11_sample_fetch)
    int rc = sensor_sample_fetch(dev);
    if (rc != 0) {
      LOG_ERR("Error al leer el sensor: %d", rc);
    } else {
      // 4. Extraer los datos procesados (llama a tu dht11_channel_get)
      sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
      sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &hum);

      // 5. Imprimir resultados usando el conversor a double (o val1/val2)
      LOG_INF("Temp: %d.%06d C | Hum: %d.%06d %%", temp.val1, temp.val2,
              hum.val1, hum.val2);
    }

    k_msleep(2000);
  }
  return 0;
}