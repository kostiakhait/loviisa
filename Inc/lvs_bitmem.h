#ifndef __LVS_BITMEM_H__
#define __LVS_BITMEM_H__

// Single bit type
typedef char LVS_BIT_T;

// Callback type
typedef void (*LVS_BIT_CALLBACK)(void*);

// Bit-oriented ring buffer
typedef struct
{
	unsigned char* memory;          // Memory buffer allocated for the ring buffer
	unsigned long  size;		// Size of the buffer (in bits)
	unsigned long  free;		// Free space of the buffer (in bits)
	unsigned long  bit_read_ptr;	// Reading pointer (in bits)
	unsigned long  bit_write_ptr;	// Writing pointer (in bits)
	LVS_BIT_CALLBACK   overflow_cb;	// Callback on buffer overflow
	LVS_BIT_CALLBACK   underflow_cb;// Callback on buffer underflow
} LVS_BIT_RINGBUF;

// *** Direct bit manipulations

// Retrieve one bit
LVS_BIT_T lvs_BitmemGet1(unsigned char* mem, unsigned int index);
// Put one bit
void lvs_BitmemPut1(unsigned char* mem, unsigned int index, LVS_BIT_T bitval);
// Retrieve given number of bits
unsigned long long lvs_BitmemGet(unsigned char* mem, unsigned int index, unsigned char numbits);
// Put given number of bits
void lvs_BitmemPut(unsigned char* mem, unsigned int index, unsigned long long val, unsigned char numbits);

// *** Ring buffer manipulations

// Ring buffer initialization
void lvs_BitmemInitRingbuf(LVS_BIT_RINGBUF* brb, unsigned char* mem, unsigned long memsize_bytes, LVS_BIT_CALLBACK overflow_cb, LVS_BIT_CALLBACK underflow_cb);
// Write bits to ring buffer
void lvs_BitmemWriteRingbuf(LVS_BIT_RINGBUF* brb, unsigned long long val, unsigned char numbits);
// Read bits from the ring buffer
unsigned long long lvs_BitmemReadRingbuf(LVS_BIT_RINGBUF* brb, unsigned char numbits);

#define LVS_ALLOCATE_BITMEM(name, size)                         \
  unsigned char __lvs_bitmem_##name[(size + 7) / 8];            \
  const unsigned int __lvs_bitmem_size_##name = (size + 7) / 8; \
  unsigned char* name = & __lvs_bitmem_##name[0];

#define LVS_USE_BITMEM(name)                                    \
  extern const unsigned int __lvs_bitmem_size_##name;           \
  extern unsigned char* name;

#define LVS_BITMEM(name) (name)
#define LVS_BITMEM_SIZE(name) (__lvs_bitmem_size_##name)

#endif