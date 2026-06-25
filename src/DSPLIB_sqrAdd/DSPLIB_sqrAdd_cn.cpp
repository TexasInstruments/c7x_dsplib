// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_sqrAdd_priv.h"

/**********************************************************************/
/*         Execute for datatypes float and double                     */
/**********************************************************************/

// This is the generic implementation of exec_ci. It is used for float and double.
// Other datatypes have their own explicet implementation.
template <typename dataType>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   dataType a;
   dataType out = 0;

   for (uint32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += (a * a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype int8_t                          */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   int8_t  *pInLocal1 = (int8_t *) pIn;
   int32_t *pOutLocal = (int32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   int8_t  a;
   int32_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((int32_t) a * (int32_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint8_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   uint8_t  *pInLocal1 = (uint8_t *) pIn;
   uint32_t *pOutLocal = (uint32_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   uint8_t  a;
   uint32_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((uint32_t) a * (uint32_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype int16_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   int16_t *pInLocal1 = (int16_t *) pIn;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   int16_t a;
   int64_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((int64_t) a * (int64_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint16_t                        */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   uint16_t *pInLocal1 = (uint16_t *) pIn;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   uint16_t a;
   uint64_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((uint64_t) a * (uint64_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype int32_t                         */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   int32_t *pInLocal1 = (int32_t *) pIn;
   int64_t *pOutLocal = (int64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   int32_t a;
   int64_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((int64_t) a * (int64_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

/**********************************************************************/
/*               Execute for datatype uint32_t                        */
/**********************************************************************/
template <>
DSPLIB_STATUS DSPLIB_sqrAdd_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_sqrAdd_PrivArgs *pKerPrivArgs = (DSPLIB_sqrAdd_PrivArgs *) handle;
   uint32_t                blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_sqrAdd_exec_cn\n");
#endif

   uint32_t *pInLocal1 = (uint32_t *) pIn;
   uint64_t *pOutLocal = (uint64_t *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pOut %p\n", pInLocal1, pOut);
#endif

   uint32_t a;
   uint64_t out = 0;

   for (int32_t counter = 0; counter < blockSize; counter++) {
      a = *pInLocal1++;
#if DSPLIB_DEBUGPRINT
      if (counter <= 8)
         printf("counter %d a %f\n", counter, a);
#endif

      out += ((uint64_t) a * (uint64_t) a);
   }
   *pOutLocal = out;

#if DSPLIB_DEBUGPRINT
   printf("DSPLIB_DEBUGPRINT DSPLIB_sqrAdd_exec_cn result %lf\n", out);
#endif
   return (status);
}

// explicit inssqrAddtiation for the different data type versions
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<float>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);

template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<double>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<int8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<uint8_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<int16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<uint16_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<int32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
template DSPLIB_STATUS
DSPLIB_sqrAdd_exec_cn<uint32_t>(DSPLIB_kernelHandle handle, void *restrict pIn, void *restrict pOut);
