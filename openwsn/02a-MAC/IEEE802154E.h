#ifndef __IEEE802154E_H
#define __IEEE802154E_H

#include "openwsn.h"


#define SYNCHRONIZING_CHANNEL 26

/*----------------------------- IEEE802.15.4E ACK ---------------------------------------*/

typedef struct IEEE802154E_ACK_ht {
   uint8_t     dhrAckNack;
   uint16_t    timeCorrection;
} IEEE802154E_ACK_ht;

enum IEEE802154E_ACK_dhrAckNack_enums {
   IEEE154E_ACK_dhrAckNack_DEFAULT = 0x82,
};

/*----------------------------- IEEE802.15.4E ADV ---------------------------------------*/

typedef struct IEEE802154E_ADV_t {
   uint8_t     commandFrameId;
   uint32_t    timingInformation;   //needs to be 6 bytes long
   uint8_t     securityControlField;
   uint8_t     joinControl;
   uint8_t     timeslotHopping;
   uint8_t     channelPage;
   uint32_t    channelMap;
   uint8_t     numberSlotFrames;
   uint8_t     slotFrameID;
   uint16_t    slotFrameSize;
   uint8_t     numberLinks;
   uint32_t    linkInfo1;
   uint32_t    linkInfo2;
   uint8_t     DAGrank;
} IEEE802154E_ADV_t;

enum ieee154e_commandFrameId_enums {
   IEEE154E_ADV      = 0x0a,
   IEEE154E_JOIN     = 0x0b,
   IEEE154E_ACTIVATE = 0x0c,
};

enum ieee154e_ADV_defaults_enums {
   IEEE154E_ADV_SEC_DEFAULT           = 0x00,
   IEEE154E_ADV_JOINCONTROL_DEFAULT   = 0x00,
   IEEE154E_ADV_HOPPING_DEFAULT       = 0x00,
   IEEE154E_ADV_CHANNELPAGE_DEFAULT   = 0x04,
   IEEE154E_ADV_CHANNELMAP_DEFAULT    = 0x07FFF800,
   IEEE154E_ADV_NUMSLOTFRAMES_DEFAULT = 0x01,
   IEEE154E_ADV_SLOTFRAMEID_DEFAULT   = 0x00,
   IEEE154E_ADV_SLOTFRAMESIZE_DEFAULT = 0x001F,
   IEEE154E_ADV_NUMLINKS_DEFAULT      = 0x02,
   IEEE154E_ADV_LINKINFO1_DEFAULT     = 0x00000002,
   IEEE154E_ADV_LINKINFO2_DEFAULT     = 0x00010001,
};

enum ieee154e_state_enum {
   S_SLEEP         = 0x00, // ready for next slot
   // synchronizing
   S_SYNCHRONIZING = 0x01,
   // TX
   S_TXDATAOFFSET  = 0x02, // waiting to prepare for Tx data
   S_TXDATAPREPARE = 0x03, // preparing for Tx data
   S_TXDATAREADY   = 0x04, // ready to Tx data, waiting for 'go'
   S_TXDATADELAY   = 0x05, // 'go' signal given, waiting for SFD Tx data
   S_TXDATA        = 0x06, // Tx data SFD received, sending bytes
   S_RXACKOFFSET   = 0x07, // Tx data done, waiting to prepare for Rx ACK
   S_RXACKPREPARE  = 0x08, // preparing for Rx ACK
   S_RXACKREADY    = 0x09, // ready to Rx ACK, waiting for 'go'
   S_RXACKLISTEN   = 0x0a, // idle listening for ACK
   S_RXACK         = 0x0b, // Rx ACK SFD received, receiving bytes
   S_TXPROC        = 0x0c, // processing sent data
   // RX
   S_RXDATAOFFSET  = 0x0d, // waiting to prepare for Rx data
   S_RXDATAPREPARE = 0x0e, // preparing for Rx data
   S_RXDATAREADY   = 0x0f, // ready to Rx data, waiting for 'go'
   S_RXDATALISTEN  = 0x10, // idle listening for data
   S_RXDATA        = 0x11, // data SFD received, receiving more bytes
   S_TXACKOFFSET   = 0x12, // waiting to prepare for Tx ACK
   S_TXACKPREPARE  = 0x13, // preparing for Tx ACK
   S_TXACKREADY    = 0x14, // Tx ACK ready, waiting for 'go'
   S_TXACKDELAY    = 0x15, // 'go' signal given, waiting for SFD Tx ACK
   S_TXACK         = 0x16, // Tx ACK SFD received, sending bytes
   S_RXPROC        = 0x17, // processing received data
};

/*----------------------------- SLOT STRUCTURES ---------------------------------------*/

enum {
   FRAME_BASED_RESYNC = TRUE,
   ACK_BASED_RESYNC   = FALSE,
};

enum {
   WAS_ACKED = TRUE,
   WAS_NOT_ACKED = FALSE,
};

//Timing (in 32kHz ticks: ticks = duration (s) * 32768)
enum {
   TsTxOffset	        =  69, //  2120us
   TsLongGT	        =  33, //  1000us
   TsTxAckDelay	        =  33, //  1000us
   TsShortGT	        =  16, //   500us
   TsSlotDuration	= 328, // 10000us
   maxTxDataPrepare	=  33, //  1000us (TBC)
   maxRxAckPrepare	=  33, //  1000us (TBC)
   delayTx	        =   0, //     0us (TBC)
   wdRadioTx	        =  33, //  1000us
   wdDataDuration	= 164, //  5000us
   delayRx	        =   0, //     0us (TBC)
   wdAckDuration	=  98, //  3000us
   maxRxDataPrepare	=  13, //   400us (TBC)
   maxTxAckPrepare	=  29, //   900us (TBC)
};

// TX
#define DURATION_tt1 capturedTime.timestamp+TsTxOffset-delayRx-maxRxDataPrepare
#define DURATION_tt2 capturedTime.timestamp+TsTxOffset-delayTx
#define DURATION_tt3 capturedTime.timestamp+TsTxOffset-delayTx+wdRadioTx
#define DURATION_tt4 capturedTime.timestamp+wdDataDuration
#define DURATION_tt5 capturedTime.timestamp+TsTxAckDelay-TsShortGT-delayRx-maxRxAckPrepare
#define DURATION_tt6 capturedTime.timestamp+TsTxAckDelay-TsShortGT-delayRx
#define DURATION_tt7 capturedTime.timestamp+TsTxAckDelay+TsShortGT
#define DURATION_tt8 capturedTime.timestamp+wdAckDuration
// RX
#define DURATION_rt1 capturedTime.timestamp+TsTxOffset-TsLongGT-delayRx-maxRxDataPrepare
#define DURATION_rt2 capturedTime.timestamp+TsTxOffset-TsLongGT-delayRx
#define DURATION_rt3 capturedTime.timestamp+TsTxOffset+TsLongGT
#define DURATION_rt4 capturedTime.timestamp+wdDataDuration
#define DURATION_rt5 capturedTime.timestamp+TsTxAckDelay-delayTx-maxTxAckPrepare
#define DURATION_rt6 capturedTime.timestamp+TsTxAckDelay-delayTx
#define DURATION_rt7 capturedTime.timestamp+TsTxAckDelay-delayTx+wdRadioTx
#define DURATION_rt8 capturedTime.timestamp+wdAckDuration

//timer wait times (in 1/32768 seconds)
//for now, we will use 40ms (10*4) slots
enum {
   PERIODICTIMERPERIOD   =    326*5,// 10 ms  //uses TIMER_MAC_PERIODIC
   MINBACKOFF            =    65*5,// 2ms     //uses TIMER_MAC_BACKOFF 
                                            //will add EXTRA_WAIT_TIME later if receiving 
   GUARDTIME             =    130*5,//4 ms    //uses TIMER_MAC_WATCHDOG
   ACK_WAIT_TIME         =    195*5,// 6ms    //uses TIMER_MAC_BACKOFF 
                                            //will add EXTRA_WAIT_TIME later if receiving
   EXTRA_WAIT_TIME       =    32*5, //1 ms    //this is used to add 1ms to the receiver for overlap
};

/*----------------------------- interface ------------------------------------*/

void    mac_init();
error_t mac_send(OpenQueueEntry_t* msg);
bool    mac_debugPrint();

void    ieee154e_startOfFrame();
void    ieee154e_endOfFrame();

#endif
