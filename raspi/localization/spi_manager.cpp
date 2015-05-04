#include <atomic>
#include <stdexcept>
#include <thread>

#include "global.h"
#include "imu.h"
#include "log.h"
#include "spi_manager.h"
#include "display.h"
#include <time.h>

#include <bcm2835.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

namespace wins {

#define WINSD_VER "monitor_2.0"
#define CHUNK_SIZE 128
#define TIME_BETWEEN_CHUNK 5000
#define TIME_BETWEEN_BUF 0
#define MS_2_PER_UNIT (9.8 * 2 / 65536)

#define SPI_INTERVAL 2400
#define PACKET_SIZE 12

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu(0x69);
// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
#define OUTPUT_READABLE_QUATERNION
unsigned char devStatus;
unsigned char mpuIntStatus;
unsigned int packetSize;
bool dmpReady = false;

Quaternion q;
Quaternion qWorld;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWorld;
VectorFloat gravity;
float ypr[3];

bool setup() {
    // initialize device
    FILE_LOG(logSPI) << "Initializing I2C devices...\n";
    mpu.initialize();

    // verify connection
    FILE_LOG(logSPI) << "Testing device connections...\n";
    FILE_LOG(logSPI) << (mpu.testConnection() ?
        "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    FILE_LOG(logSPI) << ("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        FILE_LOG(logSPI) << ("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        FILE_LOG(logSPI) << ("DMP ready!\n");
        dmpReady = true;
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

        return true;
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        FILE_LOG(logSPI) << "DMP Initialization failed code: " << devStatus;
        return false;
    }
}

atomic_bool SPI::terminate_(false);
thread SPI::spi_thread_;

// SPI Transfer.
uint8_t SPI::Exchange(uint8_t send_byte) {
  if (not init_success_) {
    return ERROR;
  }
	// For now until fixed.
	return ERROR;
}

// The Kenzhebalin Protocol.
void SPI::MainLoop() {
    bool dmp_success = setup();
    usleep(100000);

    if (!dmp_success or !bcm2835_init()) {
      FILE_LOG(logERROR) << "Unable to init SPI!";
      while(not terminate_.load());
      return;
    }

    bcm2835_spi_begin();

    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); //4096); //2048);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

    for (int var = 0; var < BUF_SIZE; var++) {
        imu_buf[var] = 0x00;
    }

    init_success_ = true;
    while(not terminate_.load()) {
		    clock_t total = clock();

        // Establishing communication for getting battery data
        /*
        TX = BAT;
        RX = bcm2835_spi_transfer(TX);
        RX = bcm2835_spi_transfer(TX);
        while (RX != BAT) {
            op_err++;
            usleep(10000);
            RX = bcm2835_spi_transfer(TX);
            printf("| tx:%x rx:%x(%c) |", TX, RX, RX);
            if (op_err % 10 == 0)
                printf("\n");
        }
        RX = bcm2835_spi_transfer(TX);
        bat1 = RX;
        RX = bcm2835_spi_transfer(TX);
        bat2 = RX;
        */
        // battery status is stored in bat1 and bat2
        data_p = 0;

        auto& display = Display::GetInstance();
        lcd_buffer_ = display.GetBufferCopy();

        //trasnferring 1024 bytes of data: 12 at a time
        while (data_p < BUF_SIZE) {
            TX = ACCEL;
            RX = bcm2835_spi_transfer(TX);
            RX = bcm2835_spi_transfer(TX);
            while (RX != ACCEL) {
                op_err++;
                usleep(10000);
                RX = bcm2835_spi_transfer(TX);
                //printf("| tx:%x rx:%x(%c) |", TX, RX, RX);
                //if (op_err % 10 == 0)
                //    printf("\n");
            }

            //printf("%d:Bat: %d%% %f Data: ", data_p, bat1, (bat1 + (float) bat2 / 256));
            for (i = 0; i < CHUNK_SIZE; i++) {
                TX = lcd_buffer_.get()[data_p + i];
                RX = bcm2835_spi_transfer(TX);
                imu_buf[data_p + i] = RX;
                //printf("%x ", RX);
            }

            /*
            int x = (imu_buf[data_p + 0] << 8) | imu_buf[data_p + 1];
            int y = (imu_buf[data_p + 2] << 8) | imu_buf[data_p + 3];
            int z = (imu_buf[data_p + 4] << 8) | imu_buf[data_p + 5];
            int gx = (imu_buf[data_p + 6] << 8) | imu_buf[data_p + 7];
            int gy = (imu_buf[data_p + 8] << 8) | imu_buf[data_p + 9];
            int gz = (imu_buf[data_p + 10] << 8) | imu_buf[data_p + 11];
            printf("\nax:%d y:%d z:%d. gx:%d y:%d z:%d\n", x, y, z, gx, gy, gz);
            */
            TX = VALID;
            RX = bcm2835_spi_transfer(TX);

            if (RX == VALID) {
                data_p += CHUNK_SIZE;
            }
            else{
              FILE_LOG(logSPI) << ("Invalid\n");
            }


            mpu.dmpGetQuaternion(&q, &imu_buf[data_p-CHUNK_SIZE]);

            //q = Quaternion(Imu::RelativeToNorth(q.w, q.x, q.y, q.z));

            //mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            //mpu.dmpGetAccel(&aa, &imu_buf[data_p-CHUNK_SIZE]);
            //mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            //mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
            Imu::AddReading(
                aa.x * MS_2_PER_UNIT,
                aa.y * MS_2_PER_UNIT,
                aa.z * MS_2_PER_UNIT,
                q.w, q.x, q.y, q.z, ypr[0]);

            if(packets%10==5){
              char buffer[100];
              sprintf(buffer, "ypr  %7.2f %7.2f %7.2f - %7.2f %7.2f %7.2f  ",
                  ypr[0] * 180 / M_PI, ypr[1] * 180 / M_PI, ypr[2] * 180 / M_PI,
                  aaWorld.x * MS_2_PER_UNIT, aaWorld.y * MS_2_PER_UNIT,
                  aaWorld.z * MS_2_PER_UNIT);
              FILE_LOG(logSPI) << buffer;
              float diffsec = (float) (clock() - total) / CLOCKS_PER_SEC;
              sprintf(buffer, "ERR %d|| total time = %f\n", op_err, diffsec);
              FILE_LOG(logSPI) << buffer;
            }
            //  gettimeofday(&tval_after, NULL);
            //   timersub(&tval_after, &tval_before, &tval_result);
            //   printf(" ---- transfer time %ld.%06ld sec", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
            //   ct++;
            //   printf("\n");
            //if(packets%100==0)
            usleep(TIME_BETWEEN_CHUNK);
        }
 //       FILE_LOG(logSPI) << ("PACKET %d successfully transferred\n", packets);
        packets++;
        usleep(TIME_BETWEEN_BUF);
    }
    bcm2835_spi_end();
    bcm2835_close();
}

SPI::SPI() {
  terminate_ = false;
}

SPI& SPI::GetInstance() {
  static SPI spi;
  return spi;
}

void SPI::StartThread() {
  if (not spi_thread_.joinable()) {
    spi_thread_ = thread(&SPI::MainLoop, &SPI::GetInstance());
  }
}

void SPI::TerminateThread() {
  terminate_ = true;
  spi_thread_.join();
}

}
