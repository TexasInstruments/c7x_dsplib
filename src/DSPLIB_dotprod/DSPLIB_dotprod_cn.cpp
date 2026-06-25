// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_dotprod_priv.h"

/**********************************************************************/
/*         Execute for datatypes float and double                     */
/**********************************************************************/

// This is the generic implementation of exec_ci. It is used for float and double.
// Other datatypes have their own explicet implementation.
template <typename dataType>
DSPLIB_STATUS
DSPLIB_dotprod_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn1, void *restrict pIn2, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   dataType a, b;
   dataType out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += a * b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype int8_t                          */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                             void *restrict pIn1,
                                             void *restrict pIn2,
                                             void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   int8_t  *pInLocal1 = (int8_t *) pIn1;
   int8_t  *pInLocal2 = (int8_t *) pIn2;
   int32_t *pOutLocal = (int32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int8_t  a, b;
   int32_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += (int32_t) a * (int32_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint8_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn1,
                                              void *restrict pIn2,
                                              void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   uint8_t  *pInLocal1 = (uint8_t *) pIn1;
   uint8_t  *pInLocal2 = (uint8_t *) pIn2;
   uint32_t *pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint8_t  a, b;
   uint32_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += (uint32_t) a * (uint32_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype int16_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn1,
                                              void *restrict pIn2,
                                              void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   int16_t *pInLocal1 = (int16_t *) pIn1;
   int16_t *pInLocal2 = (int16_t *) pIn2;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int16_t a, b;
   int64_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += (int64_t) a * (int64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint16_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   uint16_t *pInLocal1 = (uint16_t *) pIn1;
   uint16_t *pInLocal2 = (uint16_t *) pIn2;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint16_t a, b;
   uint64_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += (uint64_t) a * (uint64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;
   return (status);
}

/**********************************************************************/
/*               Execute for datatype int32_t                         */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                              void *restrict pIn1,
                                              void *restrict pIn2,
                                              void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   int32_t *pInLocal1 = (int32_t *) pIn1;
   int32_t *pInLocal2 = (int32_t *) pIn2;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   int32_t a, b;
   int64_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += (int64_t) a * (int64_t) b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint32_t                        */
/**********************************************************************/

template <>
DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                               void *restrict pIn1,
                                               void *restrict pIn2,
                                               void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_dotprod_PrivArgs *pKerPrivArgs = (DSPLIB_dotprod_PrivArgs *) handle;
   int32_t                  blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_dotprod_exec_cn\n");
#endif

   uint32_t *pInLocal1 = (uint32_t *) pIn1;
   uint32_t *pInLocal2 = (uint32_t *) pIn2;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p  pOut %p\n", pInLocal1, pInLocal2, pOut);
#endif

   uint32_t a, b;
   uint64_t out;
   out = 0;
   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
      b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %d b %d\n", counter, a, b);
#endif

      out += a * b;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d out %d\n", counter, out);
#endif
   }
   *pOutLocal = out;

   return (status);
}

// explicit insaddtiation for the different data type versions
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                       void *restrict pIn1,
                                                       void *restrict pIn2,
                                                       void *restrict pOut);
template DSPLIB_STATUS DSPLIB_dotprod_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                        void *restrict pIn1,
                                                        void *restrict pIn2,
                                                        void *restrict pOut);
