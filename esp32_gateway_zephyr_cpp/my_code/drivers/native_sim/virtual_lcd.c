#define DT_DRV_COMPAT pedro_virtual_lcd
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <zephyr/device.h>
#include <zephyr/drivers/auxdisplay.h>

// Estructura de datos para nuestra instancia
struct sim_lcd_data {
  const char *path;
};

// La función que implementa la escritura
static int sim_lcd_write(const struct device *dev, const uint8_t *data,
                         uint16_t len) {
  const struct sim_lcd_data *config = dev->config;

  // Usamos fopen estándar de C (que en native_sim apunta a Linux)

  int fd = open(config->path, O_RDWR | O_NONBLOCK);
  if (fd < 0)
    return -EIO;

  write(fd, data, len);
  write(fd, "\n", 1);
  close(fd);
  return 0;
}

static int sim_lcd_init(const struct device *dev) {
  const struct sim_lcd_data *config = dev->config;

  // Intentamos abrir el archivo para ver si el path es válido
  int fd = open(config->path, O_RDWR | O_NONBLOCK);
  // FILE *f = fopen(config->path, "w"); // "w" para truncar y limpiar el inicio
  if (fd < 0) {

    // If path is not valid, we create a name pipe
    mkfifo(config->path, 0666);
    fd = open(config->path, O_RDWR | O_NONBLOCK);
  }

  write(fd, "--- Simulación LCD Iniciada ---\n", 32);
  write(fd, "\n", 1);
  close(fd);

  printk("Driver Sim LCD inicializado correctamente vinculando a: %s\n",
         config->path);
  return 0;
}

// Mapa de funciones de la API auxdisplay
static const struct auxdisplay_driver_api sim_lcd_api = {
    .write = sim_lcd_write,
};

// Macro para instanciar el driver desde el DeviceTree
#define SIM_LCD_INIT(inst)                                                     \
  static const struct sim_lcd_data sim_lcd_config_##inst = {                   \
      .path = DT_INST_PROP(inst, file),                                        \
  };                                                                           \
                                                                               \
  DEVICE_DT_INST_DEFINE(inst, sim_lcd_init, NULL, NULL,                        \
                        &sim_lcd_config_##inst, POST_KERNEL,                   \
                        CONFIG_AUXDISPLAY_INIT_PRIORITY, &sim_lcd_api);

DT_INST_FOREACH_STATUS_OKAY(SIM_LCD_INIT)