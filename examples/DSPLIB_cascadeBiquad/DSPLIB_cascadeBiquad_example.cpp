// Copyright (C) 2026 Texas Instruments Incorporated
//
// SPDX-License-Identifier: Apache-2.0

#include "dsplib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*                                                                            */
/* Constants                                                                  */
/*                                                                            */
/******************************************************************************/

#define NUM_SAMPLES_PER_CH 64
#define NUM_CHANNELS 2
#define NUM_STAGES 3
#define TOTAL_BUFFER_SIZE (NUM_SAMPLES_PER_CH * NUM_CHANNELS)

/******************************************************************************/
/*                                                                            */
/* main                                                                       */
/*                                                                            */
/******************************************************************************/

int main(void)
{
   // Setup input and output buffers for single-precision datatypes
   float in[TOTAL_BUFFER_SIZE] = {
       0.37812115075660113,   0.9839040161591166,   0.8413189799304244,   0.039413710953984715, 0.3897812224020787,
       0.5205331631814369,    0.3530397704919308,   -0.8473050448589226,  0.45470456011621896,  -0.773641450869172,
       -0.24907383580009101,  -0.44790217734565685, -0.8114944572461933,  0.16141543178059403,  -0.21494072052688873,
       0.9112742507693601,    -0.837266795940639,   -0.7359457507811102,  -0.6766230352395204,  0.681625901675273,
       -0.3056349463650201,   0.46317276257051887,  -0.8767993884290666,  -0.4710373984470295,  -0.8421880530819399,
       -0.046141167201736444, -0.6567679087468228,  -0.637831730607169,   -0.662149136544987,   0.032202041830982076,
       0.1969330359839634,    -0.7456146016320746,  0.9980619625430402,   0.9948249970593543,   0.5168440370095579,
       -0.23099650975533637,  -0.9763506380131985,  0.5198486352553113,   -0.18398337007103294, 0.7247261617777789,
       -0.6545995527569684,   -0.13704220924236532, 0.46919969266522643,  0.007007269321892773, 0.6965977385254678,
       -0.3984869012098138,   0.42717150561118045,  0.9320953250672215,   -0.2688286291408313,  0.47995714778850207,
       0.9268585379580732,    0.7526506303904263,   -0.6431958846761279,  -0.9221850210492453,  0.33426744722626034,
       -0.5956637936583109,   0.5317261719607853,   0.910681122431714,    -0.353453950463241,   0.03291224218256472,
       0.7518163592004352,    -0.150228974868027,   -0.9462726789929816,  0.9952364519019952,   0.125165498040003,
       0.9424757493933176,    0.8217025847011856,   -0.5527418710428458,  -0.31676890534152036, 0.11661627682862985,
       0.7349845799682526,    -0.794248962705413,   0.1012373293122224,   0.05271815002284175,  0.9399247160323887,
       0.9524965286910811,    0.9178558708606455,   0.27116430328238716,  -0.6006735869739541,  0.7503049234656447,
       0.06439993056440851,   0.45990357568225315,  -0.11480521236827945, -0.9448206042909322,  -0.7001156494133531,
       0.0038867698660265404, 0.48981843571619077,  -0.46521435945387557, -0.5185432339317155,  0.3788985875558306,
       -0.5511466815941597,   0.30058612303487253,  0.23086461946334036,  -0.6490100208659926,  -0.894283640463192,
       -0.7014280400557384,   -0.7646058527936002,  -0.6575865089796809,  -0.9540434701815392,  0.5165353295132797,
       -0.9291417923345047,   0.9618317330528128,   0.6089662432009948,   0.003953682171528916, -0.5289369839056892,
       -0.9682966351349815,   -0.6244272814186911,  0.645874440378015,    0.8369190349577016,   0.9631373092202802,
       0.02288709997468108,   -0.04961703595155842, 0.9562492806176328,   0.5697554361572172,   -0.737803605839477,
       -0.634358270367888,    0.5454008948395388,   -0.6002531978022299,  0.570510913841944,    0.5069242089449963,
       -0.2211019483651775,   -0.8451850255353739,  -0.1883783737390996,  -0.29873426264817304, 0.31984813163330283,
       0.7481878037622762,    0.12146395901683671,  0.7038234482287651,
   };

   float out[TOTAL_BUFFER_SIZE] = {
       0.,
   };

   /* Filter coefficients (b0, b1, b2, a1, a2) for each stage */
   float filterCoeff[] = {
       0.07379600144461237,
       2.817062813862866e-09,
       -0.07379688595705887,
       0.14805777010158283,
       0.23323404206304799,
       1.0,
       -1.9999940261483307,
       0.999994026184017,
       -0.6305071458653517,
       0.5991280575295985,
       1.0,
       1.999993987974677,
       0.9999939880108206,
       0.9439603395431562,
       0.6356338017660916,

   };

   /* Used as the actual coefficients handed to the function */
   float filterCoeffOpt[sizeof(filterCoeff) / sizeof(float)] = {
       0,
   };

   /* Initialized values of state variables */
   float filterState[] = {
       0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
   };

   const float outReferenceDF2[] = {
       0.0279038288,  0.0726081803,  0.0492080562,  -0.0306009054, -0.103307322,  -0.232032374, -0.16704154,
       0.0398129225,  0.171193153,   0.210334063,   0.168692768,   0.116837882,   -0.231697157, 0.0903999135,
       0.048311606,   -0.240512386,  0.254457563,   -0.410465539,  -0.263244957,  0.104124717,  -0.0816354156,
       0.619308054,   0.231118456,   -0.0814931095, -0.189103991,  -0.604759216,  0.0312087685, 0.291464567,
       0.261887103,   0.301188052,   -0.167915091,  -0.29697907,   -0.0270948112, 0.0731460154, -0.0262963958,
       0.0836326256,  -0.415914863,  -0.319307774,  0.254686564,   0.142978296,   0.718876839,  0.253235877,
       -0.220249057,  -0.325072229,  -0.501868606,  0.0748929381,  -0.0556605607, 0.368336588,  -0.0724031925,
       -0.201372713,  0.374096274,   -0.329793423,  0.362259001,   -0.0708003193, -0.509562969, 0.215625018,
       -0.0615419447, 0.552345574,   0.254322916,   -0.0282174647, -0.30599758,   -0.930296659, 0.156855196,
       0.0667849779,  0.182542145,   0.885822713,   -0.0493478402, -0.426564515,  0.029922083,  -0.470765144,
       -0.420779943,  0.650114536,   0.144036323,   0.0575889945,  0.464878321,   -0.270738006, -0.269171506,
       0.0431548059,  -0.266521335,  -0.358644336,  0.0205467939,  0.141109675,   0.369383752,  0.370743811,
       0.121443436,   -0.230292529,  -0.411222249,  0.207998246,   0.0563392639,  0.0532433093, -0.00164523721,
       -0.465608984,  -0.0463532433, 0.0118941367,  0.360758245,   0.126819462,   -0.297036707, 0.235360563,
       -0.177607358,  0.299738556,   0.445964992,   -0.319120437,  -0.0403855741, -0.528141737, -0.217293501,
       -0.088246122,  -0.242445692,  0.64549613,    0.150633454,   0.633371472,   0.566645324,  -0.743777335,
       -0.397911251,  -0.782821298,  -0.471428812,  0.638619125,   0.406525135,   0.381978869,  0.327770531,
       -0.0888107866, -0.201797098,  0.0513031781,  -0.399848342,  -0.553890228,  0.238417864,  0.102608442,
       0.377667546,   0.678146541};

   float outReferenceTIF[] = {
       0.027903829,  0.07260818,   0.049208053,    -0.0306009,   -0.103307314, -0.232032344, -0.167041525, 0.039812904,
       0.171193093,  0.210334048,  0.168692723,    0.116837904,  -0.231697112, 0.090399899,  0.048311617,  -0.240512356,
       0.254457533,  -0.410465419, -0.263244927,   0.104124703,  -0.081635363, 0.619308054,  0.231118441,  -0.08149305,
       -0.189103991, -0.604759097, 0.031208759,    0.291464388,  0.261887074,  0.301188022,  -0.167915091, -0.29697904,
       -0.027094832, 0.073145948,  -0.026296366,   0.083632663,  -0.415914863, -0.319307745, 0.254686505,  0.142978266,
       0.718876779,  0.253235936,  -0.220249027,   -0.325072259, -0.501868486, 0.074892938,  -0.055660605, 0.368336618,
       -0.072403148, -0.201372728, 0.374096274,    -0.329793394, 0.362258911,  -0.070800297, -0.50956291,  0.215624988,
       -0.0615419,   0.552345634,  0.254323006,    -0.028217413, -0.305997521, -0.93029654,  0.156855196,  0.066784978,
       0.182542041,  0.885822713,  -0.04934781,    -0.426564485, 0.029922128,  -0.470765233, -0.420780003, 0.650114477,
       0.144036368,  0.057589036,  0.46487835,     -0.270738035, -0.269171476, 0.043154653,  -0.266521364, -0.358644396,
       0.020546835,  0.14110975,   0.369383693,    0.370743752,  0.121443428,  -0.230292439, -0.41122219,  0.207998157,
       0.056339257,  0.053243354,  -0.00164519378, -0.465608835, -0.046353251, 0.011894037,  0.360758185,  0.126819372,
       -0.297036707, 0.235360578,  -0.177607387,   0.299738526,  0.445964932,  -0.319120407, -0.040385604, -0.528141677,
       -0.217293456, -0.088246182, -0.242445737,   0.64549619,   0.150633454,  0.633371472,  0.566645503,  -0.743777156,
       -0.397911221, -0.782821238, -0.471428871,   0.638618827,  0.406525165,  0.381978929,  0.327770591,  -0.088810802,
       -0.201797187, 0.051303104,  -0.399848372,   -0.553890169, 0.238417849,  0.102608316,  0.377667516,  0.678146541};

   // handles and struct for call to kernel
   DSPLIB_STATUS                 status;
   DSPLIB_cascadeBiquad_InitArgs kerInitArgs;
   int32_t                       handleSize = DSPLIB_cascadeBiquad_getHandleSize(&kerInitArgs);
   DSPLIB_kernelHandle           handle     = malloc(handleSize);
   if (handle == NULL) {
      printf("Error: malloc failed for handle (size=%d)\n", handleSize);
      return -1;
   }

   DSPLIB_bufParams2D_t bufParamsIn, bufParamsOut, bufParamsFilterVar, bufParamsFilterCoeff, bufParamsFilterCoeffOpt;

   /* NUM_SAMPLES_PER_CH samples x NUM_CHANNELS channels = TOTAL_BUFFER_SIZE */
   int32_t  dataSize        = NUM_SAMPLES_PER_CH;           // Number of samples per channel
   int32_t  dataPitch       = sizeof(float) * NUM_CHANNELS; // sizeof(data_type) * numChannels
   int32_t  filterVarPitch  = sizeof(float) * NUM_CHANNELS; // sizeof(data_type) * numChannels
   int32_t  numChannels     = NUM_CHANNELS;
   int32_t  numStages       = NUM_STAGES;
   uint32_t enableNChCoeffs = 0;                 // Use same coefficients across channels
   uint32_t biquadForm      = DSPLIB_BIQUAD_DF2; // The biquad form desired

   /* Cascade Biquad Arguments */
   kerInitArgs.dataSize        = dataSize;
   kerInitArgs.numChannels     = numChannels;
   kerInitArgs.numStages       = numStages;
   kerInitArgs.enableNChCoeffs = enableNChCoeffs;
   kerInitArgs.biquadForm      = biquadForm;
   kerInitArgs.funcStyle       = DSPLIB_FUNCTION_OPTIMIZED;
   /* Check if we need to optimize the coefficients */
   bool useOptimizedCoeffs = false;
   if ((kerInitArgs.biquadForm == DSPLIB_BIQUAD_TIF) || (kerInitArgs.enableNChCoeffs == 1))
      useOptimizedCoeffs = true;

   /* Input Buffer Parameters */
   bufParamsIn.data_type = DSPLIB_FLOAT32;
   bufParamsIn.dim_x     = numChannels;
   bufParamsIn.dim_y     = dataSize;
   bufParamsIn.stride_y  = dataPitch;

   /* Output Buffer Parameters */
   bufParamsOut.data_type = DSPLIB_FLOAT32;
   bufParamsOut.dim_x     = numChannels;
   bufParamsOut.dim_y     = dataSize;
   bufParamsOut.stride_y  = dataPitch;

   /* Filter Variable Parameters */
   bufParamsFilterVar.data_type = DSPLIB_FLOAT32;
   bufParamsFilterVar.dim_x     = numChannels;
   bufParamsFilterVar.dim_y     = numStages * DSPLIB_NUM_STATE_VARS_DF2;
   bufParamsFilterVar.stride_y  = filterVarPitch;

   /* Filter Coefficient Parameters */
   bufParamsFilterCoeff.data_type = DSPLIB_FLOAT32;
   bufParamsFilterCoeff.dim_x     = numStages * DSPLIB_NUM_BIQUAD_COEFS;
   bufParamsFilterCoeff.dim_y     = (enableNChCoeffs == 0) ? 1 : numChannels;
   bufParamsFilterCoeff.stride_y  = bufParamsFilterCoeff.dim_x * DSPLIB_sizeof(bufParamsFilterCoeff.data_type);

   /* Optimized (TI Form) Filter Coefficient Parameters if enabled */
   bufParamsFilterCoeffOpt.data_type = DSPLIB_FLOAT32;
   bufParamsFilterCoeffOpt.dim_x     = numStages * DSPLIB_NUM_BIQUAD_COEFS;
   bufParamsFilterCoeffOpt.dim_y     = (enableNChCoeffs == 0) ? 1 : numChannels;
   bufParamsFilterCoeffOpt.stride_y  = bufParamsFilterCoeffOpt.dim_x * DSPLIB_sizeof(bufParamsFilterCoeffOpt.data_type);

   status = DSPLIB_SUCCESS;

   // init checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_cascadeBiquad_init_checkParams(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                                     &bufParamsOut, &kerInitArgs);
   if (status != DSPLIB_SUCCESS)
      printf("Error: init_checkParams failed with status %d\n", status);

   // init
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_cascadeBiquad_init(handle, &bufParamsIn, &bufParamsFilterCoeff, &bufParamsFilterVar,
                                         &bufParamsOut, &kerInitArgs);
   if (status != DSPLIB_SUCCESS)
      printf("Error: init failed with status %d\n", status);

   // if using TI Form (TIF) biquad or different coefficients per channel, we must optimize
   if (useOptimizedCoeffs == true) {
      status = DSPLIB_cascadeBiquad_get_coefficients(biquadForm, enableNChCoeffs, &bufParamsFilterCoeff,
                                                     &bufParamsFilterCoeffOpt, filterCoeff, filterCoeffOpt);
      if (status != DSPLIB_SUCCESS) {
         printf("Error: get_coefficients failed with status %d\n", status);
         free(handle);
         return -1;
      }
   }
   else {
      // If we don't need to optimize, just copy coefficients as is
      memcpy(filterCoeffOpt, filterCoeff, sizeof(filterCoeff));
   }

   // exec checkparams
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_cascadeBiquad_exec_checkParams(handle, in, filterCoeffOpt, filterState, out);

   // exec
   if (status == DSPLIB_SUCCESS)
      status = DSPLIB_cascadeBiquad_exec(handle, in, filterCoeffOpt, filterState, out);

   // Check output
   uint32_t size, c, err;
   size = TOTAL_BUFFER_SIZE;
   err  = 0;
   for (c = 0; c < size; c++) {
      /* Check and validation output is what was expected depending on DF2 or TIF */
      float refValue = outReferenceDF2[c];
      if (kerInitArgs.biquadForm == DSPLIB_BIQUAD_TIF)
         refValue = outReferenceTIF[c];

      if (out[c] != refValue) {
         err++;
         printf("%10g != %10g\n", out[c], refValue);
      }
   }
   if (err > 0)
      printf("There were %u unexpected values found.\n", err);
   else
      printf("No errors detected: ref vs. output match as expected\n");

   free(handle);
   return 0;
}
