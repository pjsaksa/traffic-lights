package main

/*
#include <errno.h>
#include <linux/spi/spidev.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

static void InitSpiDevice(int fd)
{
   //  MODE | CPOL | CPHA
   // ------+------+------
   //   0   |  0   |  0
   //   1   |  0   |  1
   //   2   |  1   |  0
   //   3   |  1   |  1

   uint8_t spiMode = 0;
   if (ioctl(fd, SPI_IOC_WR_MODE, &spiMode) < 0) {
      printf("ioctl(SPI_IOC_WR_MODE): %s\n", strerror(errno));
   }
}

static void SpiTransfer(int fd, unsigned char* sendFrame, unsigned char* recvFrame, int frameSize)
{
   struct spi_ioc_transfer mediator;
   memset(&mediator, 0, sizeof(mediator));

   mediator.tx_buf        = (unsigned long) sendFrame;
   mediator.rx_buf        = (unsigned long) recvFrame;
   mediator.len           = frameSize;
   mediator.speed_hz      = 500000;
   mediator.bits_per_word = 8;
   mediator.delay_usecs   = 1;

   if (ioctl(fd, SPI_IOC_MESSAGE(1), &mediator) < 0) {
      printf("ioctl(SPI_IOC_MESSAGE): %s\n", strerror(errno));
   }
}
*/
import "C"

import (
	"fmt"
	"os"
	"unsafe"
)

const (
	SpiFrameSize = 12
)

type WireSpi struct {
	device *os.File
	round   int
}

func NewWireSpi(deviceId int) *WireSpi {
	var deviceName string

	switch deviceId {
	case 1: deviceName = "/dev/spidev0.0"
	case 2: deviceName = "/dev/spidev0.1"
	default:
		panic("invalid device id")
	}

	// -----

	file,err := os.Open(deviceName)

	if err != nil {
		panic( fmt.Sprintf("can't open %s", deviceName) )
	}

	C.InitSpiDevice( (C.int)(file.Fd()) )

	return &WireSpi{
		device: file,
		round: 0,
	}
}

func (wire *WireSpi) Transfer() {
	var sendFrame [SpiFrameSize]byte
	var recvFrame [SpiFrameSize]byte

	wire.round++
	if wire.round >= 256 {
		wire.round = 1;
	}

	sendFrame[0]  = (byte)(wire.round) // frame_id
	sendFrame[1]  = 0                  // checksum
	sendFrame[2]  = 0                  // reserved
	sendFrame[3]  = 2                  // command_id
	sendFrame[4]  = (byte)(wire.round)
	sendFrame[5]  = (byte)(wire.round)
	sendFrame[6]  = 0
	sendFrame[7]  = 0
	sendFrame[8]  = 0
	sendFrame[9]  = 0
	sendFrame[10] = (byte)(wire.round)
	sendFrame[11] = (byte)(wire.round)

	C.SpiTransfer(
		(C.int)( wire.device.Fd() ),
		(*C.uchar)( unsafe.Pointer(&sendFrame[0]) ),
		(*C.uchar)( unsafe.Pointer(&recvFrame[0]) ),
		SpiFrameSize)

	// -----

	fmt.Printf("RECV:")
	for i := 0; i < SpiFrameSize; i++ {
		fmt.Printf(" %X", recvFrame[i])
	}
	fmt.Printf("\n")
}
