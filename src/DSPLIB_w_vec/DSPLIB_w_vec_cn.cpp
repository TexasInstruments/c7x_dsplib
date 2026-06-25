// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "DSPLIB_w_vec_priv.h"

template <typename dataType>
DSPLIB_STATUS DSPLIB_w_vec_exec_cn(DSPLIB_kernelHandle handle,
                                   void *restrict pIn1,
                                   void *restrict pIn2,
                                   void *restrict pM,
                                   void *restrict pOut)
{
   DSPLIB_STATUS status = DSPLIB_SUCCESS;

   DSPLIB_w_vec_PrivArgs *pKerPrivArgs = (DSPLIB_w_vec_PrivArgs *) handle;
   uint32_t               blockSize    = pKerPrivArgs->blockSize;

#if DSPLIB_DEBUGPRINT
   printf("Enter DSPLIB_w_vec_exec_cn\n");
#endif

   dataType *pInLocal1 = (dataType *) pIn1;
   dataType *pInLocal2 = (dataType *) pIn2;
   dataType *pMLocal   = (dataType *) pM;
   dataType *pOutLocal = (dataType *) pOut;

#if DSPLIB_DEBUGPRINT
   printf("Enter pInLocal1 %p pInLocal2 %p pMLocal %p pOut %p\n", pInLocal1, pInLocal2, pMLocal, pOut);
#endif

   DSPLIB_w_vec_weightStyle weightFlag = pKerPrivArgs->initArgs.weightStyle;
   dataType                 a, b, w, out;

   // execute natc of scalar version
   if (weightFlag == DSPLIB_SCALAR) {
      w = *pMLocal;

      for (uint32_t counter = 0; counter < blockSize; counter++) {
         a = *pInLocal1++;
         b = *pInLocal2++;
#if DSPLIB_DEBUGPRINT
         if (counter <= 8)
            printf("counter %d a %f b %f w %f \n", counter, a, b, w);
#endif

         out = a * w + b;
#if DSPLIB_DEBUGPRINT
         if (counter <= 8)
            printf("counter %d y %f\n", counter, out);
#endif

         *pOutLocal++ = out;
      }

      //    // print natc out vector
   }

   // execute natc of vector version
   else {

      for (int32_t counter = 0; counter < blockSize; counter++) {
         a = *pInLocal1++;
         b = *pInLocal2++;
         w = *pMLocal++;
#if DSPLIB_DEBUGPRINT
         if (counter <= 8)
            printf("counter %d a %d b %d\n", counter, a, b);
#endif

         out = a * w + b;
#if DSPLIB_DEBUGPRINT
         if (counter <= 8)
            printf("counter %d y %d\n", counter, out);
#endif

         *pOutLocal++ = out;
      }
   }

   return (status);
}

// explicit instantiation for the different data type versions
template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<float>(DSPLIB_kernelHandle handle,
                                                   void *restrict pIn1,
                                                   void *restrict pIn2,
                                                   void *restrict pM,
                                                   void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<double>(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn1,
                                                    void *restrict pIn2,
                                                    void *restrict pM,
                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<int8_t>(DSPLIB_kernelHandle handle,
                                                    void *restrict pIn1,
                                                    void *restrict pIn2,
                                                    void *restrict pM,
                                                    void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<uint8_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<int16_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<uint16_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pM,
                                                      void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<int32_t>(DSPLIB_kernelHandle handle,
                                                     void *restrict pIn1,
                                                     void *restrict pIn2,
                                                     void *restrict pM,
                                                     void *restrict pOut);

template DSPLIB_STATUS DSPLIB_w_vec_exec_cn<uint32_t>(DSPLIB_kernelHandle handle,
                                                      void *restrict pIn1,
                                                      void *restrict pIn2,
                                                      void *restrict pM,
                                                      void *restrict pOut);
