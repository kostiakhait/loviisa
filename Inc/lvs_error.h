/********************************************************************************/
/* Loviisa Embedded Operating System * Copyright (C) Konstantin Khait, 2013-2021*/
/********************************************************************************/
/* Loviisa error codes                                                          */
/********************************************************************************/
#ifndef __LVS_ERROR_H__
#define __LVS_ERROR_H__

typedef enum __LVS_ERROR
{
  LVS_OK              = 0x0000,     // No error
  LVS_NO_HANDLER      = 0x0001,     // Event of operation has no handler
  LVS_INTERRUPT       = 0x0002,     // Event is interrupted for some reason
  LVS_RESOURCE_LOCKED = 0x0003,     // Given resource is locked
  LVS_NOT_READY       = 0x0004,     // Resource not ready
  LVS_BUSY            = 0x0005,     // Resource busy
  LVS_NOT_FOUND       = 0x0006,     // Resource not found
  LVS_ALREADY_EXISTS  = 0x0007,     // Resource already exists
  LVS_WRONG_TYPE      = 0x0008,     // Wrong value type
  LVS_IO_ERROR        = 0x0101,     // Input/output problem (generic)
  LVS_WRONG_ARGUMENT  = 0x0201,     // Improper argument value provided for the operation
  LVS_OVERFLOW        = 0x0202,     // Queue overflow
  LVS_UNDERFLOW       = 0x0203,     // Queue underflow
  LVS_WRONG_FLAG_NO   = 0x0204,     // Flag number too big
  LVS_DRV_NOT_FOUND   = 0x1001,     // Resource or address not found in the driver
  LVS_DRV_OUT_OF_RANGE= 0x1002,     // Address or parameter out of range in the driver
  LVS_DRV_SIGN_ERROR  = 0x1003,     // Wrong data signature or CRC
  LVS_DRV_OUTDATED    = 0x1004,     // Data outdated or replaced
  LVS_DRV_INVALID     = 0x1005,     // Block marked as invalid
  LVS_INIT_REQUIRED   = 0x2001      // Operation performed but initialization required
} LVS_ERROR_T;                      // Error response type

#endif