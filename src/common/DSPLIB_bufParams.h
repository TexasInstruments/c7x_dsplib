// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#ifndef DSPLIB_BUFPARAMS_H_
#define DSPLIB_BUFPARAMS_H_

#ifndef __OPENCL_VERSION__
#include <stdint.h>
#endif

/**
 * @addtogroup DSPLIB_COMMON
 * @details
 */

/**@{*/

static inline int32_t DSPLIB_sizeof(uint32_t type);

/** \brief The enumeration of all data types. */
typedef enum _DSPLIB_data_type_e {
   /* Standard fixed point data types */
   DSPLIB_INT8   = 0, /**<  Signed 8-bit integer */
   DSPLIB_INT16  = 1, /**<  Signed 16-bit integer */
   DSPLIB_INT32  = 2, /**<  Signed 32-bit integer */
   DSPLIB_INT64  = 3, /**<  Signed 64-bit integer */
   DSPLIB_INT128 = 4, /**<  Signed 128-bit integer */

   DSPLIB_UINT8   = 5,  /**<  Unsigned 8-bit integer */
   DSPLIB_UINT16  = 6,  /**<  Unsigned 16-bit integer */
   DSPLIB_UINT24  = 7,  /**<  Unsigned 24-bit integer */
   DSPLIB_UINT32  = 8,  /**<  Unsigned 32-bit integer */
   DSPLIB_UINT64  = 9,  /**<  Unsigned 64-bit integer */
   DSPLIB_UINT128 = 10, /**<  Unsigned 128-bit integer */

   /* Standard floating point data types */
   DSPLIB_FLOAT16 = 11, /**<  16-bit floating point number */
   DSPLIB_FLOAT32 = 12, /**<  32-bit floating point number */
   DSPLIB_FLOAT64 = 13  /**<  64-bit floating point number */

} DSPLIB_data_type_e;

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not     */
/*  being addressed                                                 */
/* ---------------------------------------------------------------- */
// MACRO for determining signedness of DSPLIB data types; fragile in that it
// depends on ordering of DSPLIB_data_type_e
#define DSPLIB_ISSIGNED(type) ((type) < DSPLIB_UINT8 || (type) > DSPLIB_UINT128)
#define DSPLIB_SIGNEDNESS(type) (DSPLIB_ISSIGNED((type)) ? "signed" : "unsigned")

/** \brief The enumeration of multidimensional buffer types. */
typedef enum _DSPLIB_dimension_e {
   DSPLIB_2D = 0, /**<  Two dimensional buffer */
   DSPLIB_3D = 1, /**<  Three dimensional buffer */
   DSPLIB_4D = 2, /**<  Four dimensional buffer */
   DSPLIB_5D = 3, /**<  Five dimensional buffer */
   DSPLIB_6D = 4  /**<  Six dimensional buffer */
} DSPLIB_dimension_e;

/** \brief A structure for a 1 dimensional buffer descriptor. */
typedef struct {
   uint32_t data_type; /**< \brief Values are of type DSPLIB_data_type_e. */

   uint32_t dim_x; /**< \brief Width of buffer in X dimension in elements. */

} DSPLIB_bufParams1D_t;

/** \brief A structure for a 2 dimensional buffer descriptor. */
typedef struct {
   uint32_t data_type; /**< \brief Values are of type DSPLIB_data_type_e. */

   uint32_t dim_x; /**< \brief Width of buffer in X dimension in elements. */
   uint32_t dim_y; /**< \brief Height of buffer in Y dimension in elements. */

   int32_t stride_y; /**< \brief Stride in Y dimension in bytes. */

} DSPLIB_bufParams2D_t;

/** \brief A structure for a 3 dimensional buffer descriptor. */
typedef struct {
   uint32_t data_type; /**< \brief Values are of type DSPLIB_data_type_e. */

   uint32_t dim_x; /**< \brief Width of buffer in X dimension in elements. */

   uint32_t dim_y;    /**< \brief Height of buffer in Y dimension in elements. */
   int32_t  stride_y; /**< \brief Stride in Y dimension in bytes. */

   uint32_t dim_z;    /**< \brief Depth of patch in Z dimension in elements. */
   int32_t  stride_z; /**< \brief Stride in Z dimension in bytes. */

} DSPLIB_bufParams3D_t;

/** \brief A structure for a N dimensional buffer descriptor. */
typedef struct {
   uint32_t data_type; /**< \brief Values are of type DSPLIB_data_type_e. */

   uint32_t dim_0;

   uint32_t dim_1;
   int32_t  stride_1; /**< \brief  Stride in bytes */

   uint32_t dim_2;    /**< \brief  Uninitialized when dimension = {DSPLIB_2D} */
   int32_t  stride_2; /**< \brief  Stride in bytes; Uninitialized when dimension =
                         {DSPLIB_2D} */

   uint32_t dim_3;   /**< \brief  Uninitialized when dimension = {DSPLIB_2D,
                        DSPLIB_3D} */
   int32_t stride_3; /**< \brief  Stride in bytes; Uninitialized when dimension =
                        {DSPLIB_2D, DSPLIB_3D} */

   uint32_t dim_4;   /**< \brief  Uninitialized when dimension = {DSPLIB_2D,
                        DSPLIB_3D, DSPLIB_4D} */
   int32_t stride_4; /**< \brief  Stride in bytes; Uninitialized when dimension =
                        {DSPLIB_2D, DSPLIB_3D, DSPLIB_4D} */

   uint32_t dim_5;   /**< \brief  Uninitialized when dimension = {DSPLIB_2D,
                        DSPLIB_3D, DSPLIB_4D, DSPLIB_5D} */
   int32_t stride_5; /**< \brief  Stride in bytes; Uninitialized when dimension =
                        {DSPLIB_2D, DSPLIB_3D, DSPLIB_4D, DSPLIB_5D} */

   uint32_t dimension; /**< \brief Values are of type DSPLIB_dimension_e. */

} DSPLIB_bufParamsND_t;

/** \brief Inline function returns number of bytes per element given a type of
 * \ref DSPLIB_data_type_e. */
static inline int32_t DSPLIB_sizeof(uint32_t type)
{

   int32_t size = 1;

   if ((type == (uint32_t) DSPLIB_INT8) || (type == (uint32_t) DSPLIB_UINT8)) {
      size = 1;
   }
   else if ((type == (uint32_t) DSPLIB_INT16) || (type == (uint32_t) DSPLIB_UINT16) ||
            (type == (uint32_t) DSPLIB_FLOAT16)) {
      size = 2;
   }
   else if (type == (uint32_t) DSPLIB_UINT24) {
      size = 3;
   }
   else if ((type == (uint32_t) DSPLIB_INT32) || (type == (uint32_t) DSPLIB_UINT32) ||
            (type == (uint32_t) DSPLIB_FLOAT32)) {
      size = 4;
   }
   else if ((type == (uint32_t) DSPLIB_INT64) || (type == (uint32_t) DSPLIB_UINT64) ||
            (type == (uint32_t) DSPLIB_FLOAT64)) {
      size = 8;
   }
   else if ((type == (uint32_t) DSPLIB_INT128) || (type == (uint32_t) DSPLIB_UINT128)) {
      size = 16;
   }
   else {
      size = 1;
   }

   return size;
}

/**@}*/
#endif /* DSPLIB_BUFPARAMS_H_ */
