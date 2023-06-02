#include <zephyr.h>
#include <device.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/uart.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "cab/cab.h"
#include "imgProc/imgProcAlg.h"

#define STACK_SIZE 1024
#define csa_detection_prio 3
#define output_update_prio 3
#define orientation_position_prio 2
#define obstacle_counting_prio 1

#define SLEEP_TIME_MS 2000

K_THREAD_STACK_DEFINE(csa_detection_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(output_update_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(orientation_position_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(obstacle_counting_stack, STACK_SIZE);

struct k_thread csa_detection_data;
struct k_thread output_update_data;
struct k_thread orientation_position_data;
struct k_thread obstacle_counting_data;

k_tid_t csa_detection_tid;
k_tid_t output_update_tid;
k_tid_t orientation_position_tid;
k_tid_t obstacle_counting_tid;

/* Function declaration */
void csa_detection_code(void *argA, void *argB, void *argC);
void output_update_code(void *argA, void *argB, void *argC);
void orientation_position_code(void *argA, void *argB, void *argC);
void obstacle_counting_code(void *argA, void *argB, void *argC);
void serial_cb(const struct device *dev, void *user_data);

/* UART */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
#define MSG_SIZE (IMGWIDTH * IMGWIDTH + 1)
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 40, 4);
static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static char rx_buf[MSG_SIZE];
static int rx_buf_pos;


/* Position & Orientation */
typedef struct
{
    int16_t position;
    float angle;
} OrientationPosition;

/* CABs */
Image img_gl;
CAB *cab_image;

int csa_temp;
CAB *cab_csa_detection;

OrientationPosition ori_temp;
CAB *cab_orientation_position;

int obs_temp;
CAB *cab_obstacle_counting;

void csa_detection_code(void *argA, void *argB, void *argC)
{
    cab_csa_detection = open_cab("cab_csa_detection", 10, sizeof(int), NULL);
    while (1)
    {
        Image *img = get_mes(cab_image);
        if (img != NULL)
        {
            int *buffer = reserve(cab_csa_detection);
            if (buffer == NULL)
                buffer = &csa_temp;

            *buffer = nearObstSearch(*img);
            put_mes(cab_csa_detection, buffer);

            k_msleep(200);
        }
    }
}

void orientation_position_code(void *argA, void *argB, void *argC)
{
    cab_orientation_position = open_cab("cab_orientation_position", 10, sizeof(OrientationPosition), NULL);
    while (1)
    {
        Image *img = get_mes(cab_image);
        if (img != NULL)
        {
            OrientationPosition *buffer = reserve(cab_orientation_position);
            if (buffer == NULL)
                buffer = &ori_temp;
            
            guideLineSearch(*img, &(buffer->position), &(buffer->angle));
            put_mes(cab_orientation_position, buffer);

            k_msleep(200);
        }
    }
}

void obstacle_counting_code(void *argA, void *argB, void *argC)
{
    cab_obstacle_counting = open_cab("cab_obstacle_counting", 10, sizeof(int), NULL);
    while (1)
    {
        Image *img = get_mes(cab_image);
        if (img != NULL)
        {
            int *buffer = reserve(cab_obstacle_counting);
            if (buffer == NULL)
                buffer = &obs_temp;
            
            *buffer = obstCount(*img);
            put_mes(cab_obstacle_counting, buffer);

            k_msleep(200);
        }
    }
}

void output_update_code(void *argA, void *argB, void *argC)
{
    while (1)
    {
        /* Read from all CABs except from cab_image */
        int *a = get_mes(cab_csa_detection);
        int *b = get_mes(cab_obstacle_counting);
        OrientationPosition *res = get_mes(cab_orientation_position);

        if (a != NULL && b != NULL && res != NULL)
            printk("Obstacles in CSA:\t\t%d\nObstacles (total):\t\t%d\nPosition:\t\t\t%d%%\nAngle:\t\t\t\t%f\n\n", *a, *b, res->position, res->angle);
        
        unget(a, cab_csa_detection);
        unget(b, cab_obstacle_counting);
        unget(res, cab_orientation_position);

        Image *img = get_mes(cab_image);
        unget(img, cab_image);

        k_msleep(200);
    }
}

void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	while (uart_irq_rx_ready(uart_dev)) {

		uart_fifo_read(uart_dev, &c, 1);

		if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
			/* terminate string */
			rx_buf[rx_buf_pos] = '\0';

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
		/* else: characters beyond buffer size are dropped */
	}
}

void read_uart(char *buf)
{
	int msg_len = strlen(buf);
    Image *img = reserve(cab_image);

    if ( img == NULL )
        img = &img_gl;

    for (int i = 0; i < msg_len; i++) {
        if (buf[i] == '0') {
            img->data[i] = 0x00;
        }else{
            img->data[i] = (unsigned char)buf[i];
        }
    }

    put_mes(cab_image, img);
}

void main(void)
{
    /* Initialize CAB */
    cab_image = open_cab("cab_image", 10, sizeof(Image), NULL);

    /* Initialize threads */
    csa_detection_tid = k_thread_create(&csa_detection_data, csa_detection_stack, K_THREAD_STACK_SIZEOF(csa_detection_stack), csa_detection_code, NULL, NULL, NULL, csa_detection_prio, 0, K_NO_WAIT);
    output_update_tid = k_thread_create(&output_update_data, output_update_stack, K_THREAD_STACK_SIZEOF(output_update_stack), output_update_code, NULL, NULL, NULL, output_update_prio, 0, K_NO_WAIT);
    orientation_position_tid = k_thread_create(&orientation_position_data, orientation_position_stack, K_THREAD_STACK_SIZEOF(orientation_position_stack), orientation_position_code, NULL, NULL, NULL, orientation_position_prio, 0, K_NO_WAIT);
    obstacle_counting_tid = k_thread_create(&obstacle_counting_data, obstacle_counting_stack, K_THREAD_STACK_SIZEOF(obstacle_counting_stack), obstacle_counting_code, NULL, NULL, NULL, obstacle_counting_prio, 0, K_NO_WAIT);

    char tx_buf[MSG_SIZE];
    if (!device_is_ready(uart_dev))
    {
        printk("UART device not found!");
        return;
    }

    /* configure interrupt and callback to receive data */
    uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);
	uart_irq_rx_enable(uart_dev);

    while (k_msgq_get(&uart_msgq, &tx_buf, K_FOREVER) == 0) {
		read_uart(tx_buf);
	}
}