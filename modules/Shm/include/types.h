//
// Created by hez on 5/25/17.
//

#ifndef SHM_LIB_TYPES_H
#define SHM_LIB_TYPES_H

#include <errno.h>
#include <string>
#include <vector>
#include <cstddef>

using std::string;
using std::vector;
namespace pi{

typedef unsigned char u_char;
typedef unsigned int u_int;

typedef enum {
  RX_OK,
  TX_OK,
  RX_TIMEOUT,
  RX_INCOMP,
  DATA_CORUP,
  DEV_DISCON,
  TX_BLOCK,
  DEV_OK,
  DEV_FAIL,
  RTS_FAIL,
}DSPMSG;

typedef enum {
  SUCCESS = 0,
  FAILURE = -1,
  TIMEOUT = -2,
  PERMISSION_DENIED = -3,
  INVALID_PARAM = -4,
  UNINIT = -5,
}SHM_ERR;

struct dsp_stat {
  int cpuid;
  char *device;
  int baudrate;
};

struct dsp_clk {
  u_int clk_curr; // current time stamp;
  u_int clk_prev; // previous time stamp for successful read data;
  u_int clk_pred; // predicated time stamp for next data;
  int clk_diff; // consecutive data time different;
};

struct dsp_count {
  int rx_err_count;
  int tx_err_count;
  int reconnect_count;
};

typedef struct dsp_stat _dsp_stat;
typedef struct dsp_clk _dsp_clk;
typedef struct dsp_count _dsp_count;

}//namespace pi
#endif //SHM_LIB_TYPES_H
