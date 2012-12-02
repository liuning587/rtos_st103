//cc1101��������cc1101��SPI��ʼ��ʱ��Ƚ���֣���ʱ����ͳһ��SPI�����ӿ�

#include <stdio.h>
#include <types.h>
#include <sched.h>
#include <shell.h>
#include <spi.h>
#include <stm32f1lib.h>
#include "cc1101.h"

 #define GPIO_CS                  GPIOB
 #define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOB
 #define GPIO_Pin_CS              GPIO_Pin_12

 #define GPIO_Pin_SCLK            GPIO_Pin_13
 #define GPIO_Pin_SO              GPIO_Pin_14
 #define GPIO_Pin_SI              GPIO_Pin_15
 #define GPIO_Pin_GD2             GPIO_Pin_10
 #define GPIO_Pin_GD0             GPIO_Pin_11

/* Select SPI WLM: Chip Select pin low  */
#define SPI_WLM_CS_LOW()       GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)
/* Deselect SPI WLM: Chip Select pin high */
#define SPI_WLM_CS_HIGH()      GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)

#define BUSY_Flag  0x01 /* Ready/busy status flag */

#define DUMMY_BYTE 0xff

#define     WRITE_BURST         0x40                        //����д��
#define     READ_SINGLE         0x80                        //��
#define     READ_BURST          0xC0                        //������
#define     BYTES_IN_RXFIFO     0x7F                        //���ջ���������Ч�ֽ���
#define     CRC_OK              0x80                        //CRCУ��ͨ��λ��־


// Configuration Registers
#define CC1101_IOCFG2           0x00        // GDO2 output pin configuration
#define CC1101_IOCFG1           0x01        // GDO1 output pin configuration
#define CC1101_IOCFG0           0x02        // GDO0 output pin configuration
#define CC1101_FIFOTHR          0x03        // RX FIFO and TX FIFO thresholds
#define CC1101_SYNC1            0x04        // Sync word, high byte
#define CC1101_SYNC0            0x05        // Sync word, low byte
#define CC1101_PKTLEN           0x06        // Packet length
#define CC1101_PKTCTRL1         0x07        // Packet automation control
#define CC1101_PKTCTRL0         0x08        // Packet automation control
#define CC1101_ADDR             0x09        // Device address
#define CC1101_CHANNR           0x0A        // Channel number
#define CC1101_FSCTRL1          0x0B        // Frequency synthesizer control
#define CC1101_FSCTRL0          0x0C        // Frequency synthesizer control
#define CC1101_FREQ2            0x0D        // Frequency control word, high byte
#define CC1101_FREQ1            0x0E        // Frequency control word, middle byte
#define CC1101_FREQ0            0x0F        // Frequency control word, low byte
#define CC1101_MDMCFG4          0x10        // Modem configuration
#define CC1101_MDMCFG3          0x11        // Modem configuration
#define CC1101_MDMCFG2          0x12        // Modem configuration
#define CC1101_MDMCFG1          0x13        // Modem configuration
#define CC1101_MDMCFG0          0x14        // Modem configuration
#define CC1101_DEVIATN          0x15        // Modem deviation setting
#define CC1101_MCSM2            0x16        // Main Radio Cntrl State Machine config
#define CC1101_MCSM1            0x17        // Main Radio Cntrl State Machine config
#define CC1101_MCSM0            0x18        // Main Radio Cntrl State Machine config
#define CC1101_FOCCFG           0x19        // Frequency Offset Compensation config
#define CC1101_BSCFG            0x1A        // Bit Synchronization configuration
#define CC1101_AGCCTRL2         0x1B        // AGC control
#define CC1101_AGCCTRL1         0x1C        // AGC control
#define CC1101_AGCCTRL0         0x1D        // AGC control
#define CC1101_WOREVT1          0x1E        // High byte Event 0 timeout
#define CC1101_WOREVT0          0x1F        // Low byte Event 0 timeout
#define CC1101_WORCTRL          0x20        // Wake On Radio control
#define CC1101_FREND1           0x21        // Front end RX configuration
#define CC1101_FREND0           0x22        // Front end TX configuration
#define CC1101_FSCAL3           0x23        // Frequency synthesizer calibration
#define CC1101_FSCAL2           0x24        // Frequency synthesizer calibration
#define CC1101_FSCAL1           0x25        // Frequency synthesizer calibration
#define CC1101_FSCAL0           0x26        // Frequency synthesizer calibration
#define CC1101_RCCTRL1          0x27        // RC oscillator configuration
#define CC1101_RCCTRL0          0x28        // RC oscillator configuration
#define CC1101_FSTEST           0x29        // Frequency synthesizer cal control
#define CC1101_PTEST            0x2A        // Production test
#define CC1101_AGCTEST          0x2B        // AGC test
#define CC1101_TEST2            0x2C        // Various test settings
#define CC1101_TEST1            0x2D        // Various test settings
#define CC1101_TEST0            0x2E        // Various test settings

// Status registers
#define CC1101_PARTNUM          0x30        // Part number
#define CC1101_VERSION          0x31        // Current version number
#define CC1101_FREQEST          0x32        // Frequency offset estimate
#define CC1101_LQI              0x33        // Demodulator estimate for link quality
#define CC1101_RSSI             0x34        // Received signal strength indication
#define CC1101_MARCSTATE        0x35        // Control state machine state
#define CC1101_WORTIME1         0x36        // High byte of WOR timer
#define CC1101_WORTIME0         0x37        // Low byte of WOR timer
#define CC1101_PKTSTATUS        0x38        // Current GDOx status and packet status
#define CC1101_VCO_VC_DAC       0x39        // Current setting from PLL cal module
#define CC1101_TXBYTES          0x3A        // Underflow and # of bytes in TXFIFO
#define CC1101_RXBYTES          0x3B        // Overflow and # of bytes in RXFIFO

// Multi byte memory locations
#define CC1101_PATABLE          0x3E
#define CC1101_TXFIFO           0x3F
#define CC1101_RXFIFO           0x3F

// Definitions for burst/single access to registers
#define CC1101_WRITE_BURST      0x40
#define CC1101_READ_SINGLE      0x80
#define CC1101_READ_BURST       0xC0

// Strobe commands
#define CC1101_SRES             0x30        // Reset chip.
#define CC1101_SFSTXON          0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                            // If in RX/TX: Go to a wait state where only the synthesizer is
                                            // running (for quick RX / TX turnaround).
#define CC1101_SXOFF            0x32        // Turn off crystal oscillator.
#define CC1101_SCAL             0x33        // Calibrate frequency synthesizer and turn it off
                                            // (enables quick start).
#define CC1101_SRX              0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                            // MCSM0.FS_AUTOCAL=1.
#define CC1101_STX              0x35        // In IDLE state: Enable TX. Perform calibration first if
                                            // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                            // Only go to TX if channel is clear.
#define CC1101_SIDLE            0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                            // Wake-On-Radio mode if applicable.
#define CC1101_SAFC             0x37        // Perform AFC adjustment of the frequency synthesizer
#define CC1101_SWOR             0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CC1101_SPWD             0x39        // Enter power down mode when CSn goes high.
#define CC1101_SFRX             0x3A        // Flush the RX FIFO buffer.
#define CC1101_SFTX             0x3B        // Flush the TX FIFO buffer.
#define CC1101_SWORRST          0x3C        // Reset real time clock.
#define CC1101_SNOP             0x3D        // No operation. May be used to pad strobe commands to two
                                            // bytes for simpler software.


//----------------------------------------------------------------------------------
// Chip Status Byte
//----------------------------------------------------------------------------------

// Bit fields in the chip status byte
#define CC1101_STATUS_CHIP_RDYn_BM             0x80
#define CC1101_STATUS_STATE_BM                 0x70
#define CC1101_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F

// Chip states
#define CC1101_STATE_IDLE                      0x00
#define CC1101_STATE_RX                        0x10
#define CC1101_STATE_TX                        0x20
#define CC1101_STATE_FSTXON                    0x30
#define CC1101_STATE_CALIBRATE                 0x40
#define CC1101_STATE_SETTLING                  0x50
#define CC1101_STATE_RX_OVERFLOW               0x60
#define CC1101_STATE_TX_UNDERFLOW              0x70


//----------------------------------------------------------------------------------
// Other register bit fields
//----------------------------------------------------------------------------------
#define CC1101_LQI_CRC_OK_BM                   0x80
#define CC1101_LQI_EST_BM                      0x7F
/*
const uint8_t the_pa_tab[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   ������С
const uint8_t the_pa_tab[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
*/
const uint8_t the_pa_tab[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm    �������

//RF_SETTINGS is a data structure which contains all relevant CC1101 registers
typedef struct S_RF_SETTINGS
{
    uint8_t FSCTRL2;      //���Ѽӵ�
    uint8_t FSCTRL1;   // Frequency synthesizer control.
    uint8_t FSCTRL0;   // Frequency synthesizer control.
    uint8_t FREQ2;     // Frequency control word, high uint8_t.
    uint8_t FREQ1;     // Frequency control word, middle uint8_t.
    uint8_t FREQ0;     // Frequency control word, low uint8_t.
    uint8_t MDMCFG4;   // Modem configuration.
    uint8_t MDMCFG3;   // Modem configuration.
    uint8_t MDMCFG2;   // Modem configuration.
    uint8_t MDMCFG1;   // Modem configuration.
    uint8_t MDMCFG0;   // Modem configuration.
    uint8_t CHANNR;    // Channel number.
    uint8_t DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).
    uint8_t FREND1;    // Front end RX configuration.
    uint8_t FREND0;    // Front end RX configuration.
    uint8_t MCSM0;     // Main Radio Control State Machine configuration.
    uint8_t FOCCFG;    // Frequency Offset Compensation Configuration.
    uint8_t BSCFG;     // Bit synchronization Configuration.
    uint8_t AGCCTRL2;  // AGC control.
    uint8_t AGCCTRL1;  // AGC control.
    uint8_t AGCCTRL0;  // AGC control.
    uint8_t FSCAL3;    // Frequency synthesizer calibration.
    uint8_t FSCAL2;    // Frequency synthesizer calibration.
    uint8_t FSCAL1;    // Frequency synthesizer calibration.
    uint8_t FSCAL0;    // Frequency synthesizer calibration.
    uint8_t FSTEST;    // Frequency synthesizer calibration control
    uint8_t TEST2;     // Various test settings.
    uint8_t TEST1;     // Various test settings.
    uint8_t TEST0;     // Various test settings.
    uint8_t IOCFG2;    // GDO2 output pin configuration
    uint8_t IOCFG0;    // GDO0 output pin configuration
    uint8_t PKTCTRL1;  // Packet automation control.
    uint8_t PKTCTRL0;  // Packet automation control.
    uint8_t ADDR;      // Device address.
    uint8_t PKTLEN;    // Packet length.
} RF_SETTINGS;

// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 433.000000 MHz
// Channel spacing = 199.951172 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2) 4 bytes
// Append status = 1
// Address check = (11) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6)
// GDO2 signal selection = (11) Serial Clock
const RF_SETTINGS rfSettings = {
    0x00,
    0x08, // FSCTRL1 Frequency synthesizer control.
    0x00, // FSCTRL0 Frequency synthesizer control.
    /* 432.999817 MHz */
    0x10, // FREQ2 Frequency control word, high byte.
    0xA7, // FREQ1 Frequency control word, middle byte.
    0x62, // FREQ0 Frequency control word, low byte.
    /* data rate: 100K */
    0x5B,  //0x2D, // MDMCFG4 Modem configuration.
    0xF8,  //0x3B, // MDMCFG3 Modem configuration.
    0x02,   //0x73, // MDMCFG2 Modem configuration.
    0x22, // MDMCFG1 Modem configuration.
    0xF8, // MDMCFG0 Modem configuration.

    0x00, // CHANNR Channel number.
    0x47, // DEVIATN Modem deviation setting (when FSK modulation is enabled).
    0xB6, // FREND1 Front end RX configuration.
    0x10, // FREND0 Front end RX configuration.
    0x18, // MCSM0 Main Radio Control State Machine configuration.
    0x1D, // FOCCFG Frequency Offset Compensation Configuration.
    0x1C, // BSCFG Bit synchronization Configuration.
    0x07, // AGCCTRL2 AGC control.
    0x00, //0x00, // AGCCTRL1 AGC control.
    0xB2, // AGCCTRL0 AGC control.

    0xEA, // FSCAL3 Frequency synthesizer calibration.
    0x2A, // FSCAL2 Frequency synthesizer calibration.
    0x00, // FSCAL1 Frequency synthesizer calibration.
    0x11, // FSCAL0 Frequency synthesizer calibration.
    0x59, // FSTEST Frequency synthesizer calibration.
    0x81, // TEST2 Various test settings.
    0x35, // TEST1 Various test settings.
    0x09, // TEST0 Various test settings.
    0x0B, // IOCFG2 GDO2 output pin configuration.
    0x06, // IOCFG0D GDO0 output pin configuration.
    0x05, // PKTCTRL1 Packet automation control.
    0x05, // PKTCTRL0 Packet automation control.
    0x00, // ADDR Device address.   //��ַ
    0xff // PKTLEN Packet length.
};

/**
 ******************************************************************************
 * @brief      cc1101 SPI ��ʼ��
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
spi2_init(void)
{
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC , ENABLE);

    /* Configure SPI2 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_SCLK | GPIO_Pin_SO | GPIO_Pin_SI;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Configure I/O for WLM Chip select */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_CS, &GPIO_InitStructure);

    /* Configure I/O for PC10,PC11  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_GD2 | GPIO_Pin_GD0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Deselect the WLM: Chip Select high */
    SPI_WLM_CS_HIGH();

    /* Enable SPI2 and GPIO clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* SPI2 configuration */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Enable SPI2  */
    SPI_Cmd(SPI2, ENABLE);
}

/**
 ******************************************************************************
 * @brief      spi2�շ��ֽ�
 * @param[in]  byte ���͵��ֽ�
 * @param[out] None
 * @retval     ���յ����ֽ�
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
spi2_send_byte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

/**
 ******************************************************************************
 * @brief      ��ʱ todo:�Ƿ����ȡ��?
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
delay(uint32_t count)
{
    int32_t i, j;
    for (j = 0; j < count; j++)
    {
        for (i = 0; i < 10; i++)
            ;
    }
}

/**
 ******************************************************************************
 * @brief      ��ȡcc1101��ID
 * @param[in]  None
 * @param[out] None
 * @retval     id  Current version number
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
cc1101_readid(void)
{
    uint8_t id;

    SPI_WLM_CS_LOW();
    spi2_send_byte(CC1101_VERSION);
    id = spi2_send_byte(0xff);
    SPI_WLM_CS_HIGH();

    return id;
}

/**
 ******************************************************************************
 * @brief      cc1101�ϵ縴λ
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_power_reset(void)
{
    SPI_WLM_CS_HIGH();  //Ƭѡ�ø�
    GPIO_SetBits(GPIOB, GPIO_Pin_SCLK); //SCLK=1
    GPIO_ResetBits(GPIOB, GPIO_Pin_SI); //SI=0

    delay(5000);
    SPI_WLM_CS_HIGH();
    delay(1);
    SPI_WLM_CS_LOW();
    delay(1);
    SPI_WLM_CS_HIGH();
    delay(41);
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO)); //waite SO =0
    spi2_send_byte(CC1101_SRES); //���͸�λָ��
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO)); //waite SO =0 again
    SPI_WLM_CS_HIGH();
}

/**
 ******************************************************************************
 * @brief   дcc1101�Ĵ���
 * @param[in]  addr     cc1101�Ĵ�����ַ
 * @param[in]  value    ������
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_write_reg(uint8_t addr, uint8_t value)
{
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO));
    spi2_send_byte(addr);       //д��ַ
    spi2_send_byte(value);      //д������
    SPI_WLM_CS_HIGH();
}

/**
 ******************************************************************************
 * @brief      cc1101����д���üĴ���
 * @param[in]  addr     cc1101�Ĵ�����ַ
 * @param[in]  *pbuf    д�뻺����
 * @param[in]  count    д�����
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_write_burst_reg(uint8_t addr, const uint8_t *pbuf, uint8_t count)
{
    uint8_t i, temp;

    temp = addr | WRITE_BURST;
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO));
    spi2_send_byte(temp);        //д��ַ
    for (i = 0; i < count; i++)
    {
        spi2_send_byte(pbuf[i]); //д������
    }
    SPI_WLM_CS_HIGH();
}

/**
 ******************************************************************************
 * @brief      cc1101��״̬�Ĵ���
 * @param[in]  addr  ״̬�Ĵ�����ַ
 * @param[out] None
 * @retval     ��״̬�Ĵ�����ǰֵ
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
cc1101_read_status(uint8_t addr)
{
    uint8_t value,temp;

    temp = addr | READ_BURST;   //д��Ҫ����״̬�Ĵ����ĵ�ַͬʱд�������
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO));
    spi2_send_byte(temp);
    value = spi2_send_byte(0);
    SPI_WLM_CS_HIGH();
    return value;
}

/**
 ******************************************************************************
 * @brief ��cc1101���üĴ���
 * @param[in]  addr     ��ַ
 * @param[out] None
 * @retval     �üĴ�����������
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint8_t
cc1101_read_reg(uint8_t addr)
{
    uint8_t temp, value;

    temp = addr | READ_SINGLE; //���Ĵ�������
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO)); //MISO
    spi2_send_byte(temp);
    value = spi2_send_byte(0);
    SPI_WLM_CS_HIGH();

    return value;
}

/**
 ******************************************************************************
 * @brief      ������cc1101���üĴ���
 * @param[in]  addr     cc1101�Ĵ�����ַ
 * @param[in]  *pbuf    �������ݺ��ݴ�Ļ�����
 * @param[in]  count    �������ø���
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_read_burst_reg(uint8_t addr, uint8_t *pbuf, uint8_t count)
{
    uint8_t i, temp;

    temp = addr | READ_BURST; //д��Ҫ�������üĴ�����ַ�Ͷ�����
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO));
    spi2_send_byte(temp);
    for (i = 0; i < count; i++)
    {
        pbuf[i] = spi2_send_byte(0);
    }
    SPI_WLM_CS_HIGH();
}

/**
 ******************************************************************************
 * @brief      cc1101д�ֽ�����
 * @param[in]  None
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_write_cmd(uint8_t cmd)
{
    SPI_WLM_CS_LOW();
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_SO)) ;
    spi2_send_byte(cmd); //д������
    SPI_WLM_CS_HIGH();
}

/**
 ******************************************************************************
 * @brief      ��������
 * @param[out]  *prbuf  ���ջ���
 * @retval       0  ���մ���
 * @retval      >0  ʵ�ʽ����ֽ���
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static uint32_t
cc1101_receive_packet(uint8_t *prbuf, uint32_t len)
{
    uint8_t status[2];
    uint8_t rlen;

    cc1101_write_cmd(CC1101_SRX); //�������״̬

    // Wait for GDO0 to be set -> sync transmitted
    while (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_GD0)); //while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_GD0)); // while (GDO0);

    if ((cc1101_read_status(CC1101_RXBYTES) & BYTES_IN_RXFIFO)) //����ӵ��ֽ�����Ϊ0
    {
        rlen = cc1101_read_reg(CC1101_RXFIFO); //������һ���ֽڣ����ֽ�Ϊ��֡���ݳ���

        if (len < rlen - 1)
        {
            cc1101_write_cmd(CC1101_SFRX); //��ս��ջ�����
            return 0;
        }
        (void) cc1101_read_reg(CC1101_RXFIFO); //�������յ�ַ
        cc1101_read_burst_reg(CC1101_RXFIFO, prbuf, rlen - 1); //�������н��յ�������

        // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
        cc1101_read_burst_reg(CC1101_RXFIFO, status, 2); //����CRCУ��λ
        cc1101_write_cmd(CC1101_SFRX); //��ս��ջ�����
        if (status[1] & CRC_OK) //���У��ɹ����ؽ��ճɹ�
        {
            return rlen;
        }
    }

    return 0;
}

/**
 ******************************************************************************
 * @brief      �������ݰ�
 * @param[in]  addr     ���͵�ַ
 * @param[in]  *ptbuf   ���ͻ���
 * @param[in]  len      ���ͳ���
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
static void
cc1101_send_packet(uint8_t addr, const uint8_t* ptbuf, uint8_t len)
{
    cc1101_write_reg(CC1101_TXFIFO, len + 1);               //д�볤��
    cc1101_write_reg(CC1101_TXFIFO, addr);                  //д����յ�ַ
    cc1101_write_burst_reg(CC1101_TXFIFO, ptbuf, len);      //д��Ҫ���͵�����

    cc1101_write_cmd(CC1101_STX);                           //���뷢��ģʽ��������

    // Wait for GDO0 to be set -> sync transmitted
    while (!GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_GD0));//while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_GD0));// while (GDO0);
    cc1101_write_cmd(CC1101_SFTX);
    //too:�������ģʽ
}

/**
 ******************************************************************************
 * @brief      ����cc1100�ļĴ���
 * @param[in]  addr     ����ģ���ʼ����ַ
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
void
cc1101_write_rf_settings(uint8_t addr)
{
    cc1101_write_reg(CC1101_FSCTRL0,  rfSettings.FSCTRL2);//���Ѽӵ�
    // Write register settings
    cc1101_write_reg(CC1101_FSCTRL1,  rfSettings.FSCTRL1);
    cc1101_write_reg(CC1101_FSCTRL0,  rfSettings.FSCTRL0);
    cc1101_write_reg(CC1101_FREQ2,    rfSettings.FREQ2);
    cc1101_write_reg(CC1101_FREQ1,    rfSettings.FREQ1);
    cc1101_write_reg(CC1101_FREQ0,    rfSettings.FREQ0);
    cc1101_write_reg(CC1101_MDMCFG4,  rfSettings.MDMCFG4);
    cc1101_write_reg(CC1101_MDMCFG3,  rfSettings.MDMCFG3);
    cc1101_write_reg(CC1101_MDMCFG2,  rfSettings.MDMCFG2);
    cc1101_write_reg(CC1101_MDMCFG1,  rfSettings.MDMCFG1);
    cc1101_write_reg(CC1101_MDMCFG0,  rfSettings.MDMCFG0);
    cc1101_write_reg(CC1101_CHANNR,   rfSettings.CHANNR);
    cc1101_write_reg(CC1101_DEVIATN,  rfSettings.DEVIATN);
    cc1101_write_reg(CC1101_FREND1,   rfSettings.FREND1);
    cc1101_write_reg(CC1101_FREND0,   rfSettings.FREND0);
    cc1101_write_reg(CC1101_MCSM0,    rfSettings.MCSM0);
    cc1101_write_reg(CC1101_FOCCFG,   rfSettings.FOCCFG);
    cc1101_write_reg(CC1101_BSCFG,    rfSettings.BSCFG);
    cc1101_write_reg(CC1101_AGCCTRL2, rfSettings.AGCCTRL2);
    cc1101_write_reg(CC1101_AGCCTRL1, rfSettings.AGCCTRL1);    //
    cc1101_write_reg(CC1101_AGCCTRL0, rfSettings.AGCCTRL0);
    cc1101_write_reg(CC1101_FSCAL3,   rfSettings.FSCAL3);
    cc1101_write_reg(CC1101_FSCAL2,   rfSettings.FSCAL2);
    cc1101_write_reg(CC1101_FSCAL1,   rfSettings.FSCAL1); //
    cc1101_write_reg(CC1101_FSCAL0,   rfSettings.FSCAL0);
    cc1101_write_reg(CC1101_FSTEST,   rfSettings.FSTEST);
    cc1101_write_reg(CC1101_TEST2,    rfSettings.TEST2);
    cc1101_write_reg(CC1101_TEST1,    rfSettings.TEST1);
    cc1101_write_reg(CC1101_TEST0,    rfSettings.TEST0);
    cc1101_write_reg(CC1101_IOCFG2,   rfSettings.IOCFG2);
    cc1101_write_reg(CC1101_IOCFG0,   rfSettings.IOCFG0);
    cc1101_write_reg(CC1101_PKTCTRL1, rfSettings.PKTCTRL1);
    cc1101_write_reg(CC1101_PKTCTRL0, rfSettings.PKTCTRL0);
    cc1101_write_reg(CC1101_ADDR,     addr);//rfSettings.ADDR);//��ַ��Ӧ��������
    cc1101_write_reg(CC1101_PKTLEN,   rfSettings.PKTLEN);
}

/**
 ******************************************************************************
 * @brief      cc1101��ʼ��
 * @param[in]  addr ����ģ���ʼ����ַ
 * @param[out] None
 * @retval     None
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
status_t
cc1101_init(uint8_t addr)
{
    spi2_init();    //spi��ʼ��
    cc1101_power_reset();   //�ϵ��ʼ��
    cc1101_write_rf_settings(addr); //Ĭ������
    cc1101_write_burst_reg(CC1101_PATABLE, the_pa_tab, 8);//���书������
    printf("cc1101 id:0x%02x; addr:0x%02x\n", cc1101_readid(), addr);

    return OK;
}

/**
 ******************************************************************************
 * @brief      cc1101����
 * @param[in]  * pbuf ָ��Ҫ���͵ı��Ļ���
 * @param[out] None
 * @retval     -1       ����ʧ��
 * @retval     other    �����ֽ���
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
cc1101_send(uint8_t addr, const uint8_t* pbuf, int32_t len)
{
    cc1101_send_packet(addr, pbuf, len);
    return len;
}

/**
 ******************************************************************************
 * @brief      cc1101����
 * @param[in]  * pbuf ָ��Ҫ���յı��Ļ���
 * @param[out] None
 * @retval     -1       ����ʧ��
 * @retval     other    �����ֽ���
 *
 * @details
 *
 * @note
 ******************************************************************************
 */
int32_t
cc1101_recv(uint8_t addr, uint8_t* pbuf, int32_t len)
{
    (void)addr;
    int32_t rlen = (int32_t)cc1101_receive_packet(pbuf, len);
    cc1101_write_cmd(CC1101_SRX);   //�������״̬
    return rlen;
}


