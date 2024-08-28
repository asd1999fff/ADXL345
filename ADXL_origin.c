#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define DEVICE_NAME "/dev/i2c-1"
#define I2C_ADRRESS 0x53


int write_I2C(int file,unsigned char reg,unsigned char value){
    unsigned char buffer[2];
    buffer[0] = reg;
    buffer[1] = value;

    if(write(file,buffer,sizeof(buffer)) != sizeof(buffer)){
        perror("Failed to write the I2C bus");
        return -1;
    }
    return 0;
}

int read_I2C(int file,unsigned char reg,unsigned char *value){
    
    if((file = open(DEVICE_NAME,O_RDWR)) < 0) {
        perror("Failed to open the I2C bus");
        exit(1);
    }

    if(ioctl(file, I2C_SLAVE,I2C_ADRRESS) < 0){
        perror("Failed to talk the slave bus");
        close(file);
        exit(1);
    }

    if(write(file,&reg,1) != 1){
        perror("Failed to ask the slave ");
        printf("Failed to ask the slave %d\n",reg);
        return -1;
    }

    if(read(file,value,6) != 6){
        perror("Failed to read the slave data");
        return -1;
    }

    close(file);

    return 0;

}

void inital(int file){
    if((file = open(DEVICE_NAME,O_RDWR)) < 0) {
        perror("Failed to open the I2C bus");
        exit(1);
    }

    if(ioctl(file, I2C_SLAVE,I2C_ADRRESS) < 0){
        perror("Failed to talk the slave bus");
        close(file);
        exit(1);
    }

    if(write_I2C(file,0X2D,0X08)<0){
        close(file);
        exit(1);
    }

    if(write_I2C(file,0X31,0X0B)<0){
        close(file);
        exit(1);
    }

    if(write_I2C(file,0X2E,0X00)<0){
        close(file);
        exit(1);
    }

    close(file);
}


int main(){
    int file;
    int flag = 1;
    const char *dev = DEVICE_NAME;
    unsigned char value;
    unsigned char the_data[10];

    inital(file);

    while(read_I2C(file,0x32,the_data) <! 0){

    printf("=================================================\n");
    printf("X軸為%f\n",((int)the_data[1]*256+(int)the_data[0] )/(float)256);
    printf("Y軸為%f\n",((int)the_data[3]*256+(int)the_data[2] )/(float)256);
    printf("Z軸為%f\n",((int)the_data[5]*256+(int)the_data[4] )/(float)256);

    sleep(1);

    }

    close(file);
    exit(1);

    return 0;
}