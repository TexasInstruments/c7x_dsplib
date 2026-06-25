// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_dotp_sqr_priv.h"

/**********************************************************************/
/*         Execute for datatypes float and double                     */
/**********************************************************************/

// This is the generic implementation of exec_ci. It is used for float and double.
// Other datatypes have their own explicet implementation.
template <typename dataType>
DSPLIB_STATUS
DSPLIB_dotp_sqr_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   dataType a, b;
   dataType out_dotp;
   dataType out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += a * b;
      out_sqrAdd += b * b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %d out_dotp %d\n", counter, out_dotp);
         printf("counter %d out_sqrAdd %d\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype int8_t                          */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn1,
                                              void *restrict pIn2,
                                              void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   int8_t  *pInLocal1 = (int8_t *) pIn1;
   int8_t  *pInLocal2 = (int8_t *) pIn2;
   int32_t *pOutLocal = (int32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int8_t  a, b;
   int32_t out_dotp;
   int32_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += (int32_t) a * (int32_t) b;
      out_sqrAdd += (int32_t) b * (int32_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %d out_dotp %d\n", counter, out_dotp);
         printf("counter %d out_sqrAdd %d\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint8_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   uint8_t  *pInLocal1 = (uint8_t *) pIn1;
   uint8_t  *pInLocal2 = (uint8_t *) pIn2;
   uint32_t *pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint8_t  a, b;
   uint32_t out_dotp;
   uint32_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += (uint32_t) a * (uint32_t) b;
      out_sqrAdd += (uint32_t) b * (uint32_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %d out_dotp %d\n", counter, out_dotp);
         printf("counter %d out_sqrAdd %d\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype int16_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   int16_t *pInLocal1 = (int16_t *) pIn1;
   int16_t *pInLocal2 = (int16_t *) pIn2;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int16_t a, b;
   int64_t out_dotp;
   int64_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += (int64_t) a * (int64_t) b;
      out_sqrAdd += (int64_t) b * (int64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %ld out_dotp %ld\n", counter, out_dotp);
         printf("counter %ld out_sqrAdd %ld\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint16_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn1,
                                                void *restrict pIn2,
                                                void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   uint16_t *pInLocal1 = (uint16_t *) pIn1;
   uint16_t *pInLocal2 = (uint16_t *) pIn2;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint16_t a, b;
   uint64_t out_dotp;
   uint64_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += (uint64_t) a * (uint64_t) b;
      out_sqrAdd += (uint64_t) b * (uint64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %ld out_dotp %ld\n", counter, out_dotp);
         printf("counter %ld out_sqrAdd %ld\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;
   return (status);
}

/**********************************************************************/
/*               Execute for datatype int32_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   int32_t *pInLocal1 = (int32_t *) pIn1;
   int32_t *pInLocal2 = (int32_t *) pIn2;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int32_t a, b;
   int64_t out_dotp;
   int64_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += (int64_t) a * (int64_t) b;
      out_sqrAdd += (int64_t) b * (int64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %ld out_dotp %ld\n", counter, out_dotp);
         printf("counter %ld out_sqrAdd %ld\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint32_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                void *restrict pIn1,
                                                void *restrict pIn2,
                                                void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotp_sqr_PrivArgs *pKerPrivArgs = (DSPLIB_dotp_sqr_PrivArgs *) handle;
   int32_t                   blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotp_sqr_exec_cn\n");
#endif

   uint32_t *pInLocal1 = (uint32_t *) pIn1;
   uint32_t *pInLocal2 = (uint32_t *) pIn2;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint32_t a, b;
   uint64_t out_dotp;
   uint64_t out_sqrAdd;
   out_dotp   = 0;
   out_sqrAdd = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out_dotp += a * b;
      out_sqrAdd += b * b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8) {
         printf("counter %ld out_dotp %ld\n", counter, out_dotp);
         printf("counter %ld out_sqrAdd %ld\n", counter, out_sqrAdd);
      }
#endif
   }
   *pOutLocal   = out_dotp;
   *++pOutLocal = out_sqrAdd;

   return (status);
}

// explicit insaddtiation for the different data type versions
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                         void *restrict pIn1,
                                                         void *restrict pIn2,
                                                         void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotp_sqr_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                         void *restrict pIn1,
                                                         void *restrict pIn2,
                                                         void *restrict pOut);
