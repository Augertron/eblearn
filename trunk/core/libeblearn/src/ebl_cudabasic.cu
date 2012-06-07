/***************************************************************************
 *   Copyright (C) 2011 by Soumith Chintala*
 *   soumith@gmail.com  *
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Redistribution under a license not approved by the Open Source
 *       Initiative (http://www.opensource.org) must display the
 *       following acknowledgement in all advertising material:
 *        This product includes software developed at the Courant
 *        Institute of Mathematical Sciences (http://cims.nyu.edu).
 *     * The names of the authors may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ThE AUTHORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include "ebl_cudabasic.h"

#ifdef __CUDA__

namespace ebl {

#define CUDA_SHARED_MEM_SIZE 0
  //(4*1024-32) 
// this is given by nVidia: max shared mem per block

/*
 * Description:
 *   base conv2D routine: 3D input, 3D output, 4D kernel
 *
 *   - all chunks of data should be contiguous
 *   - the swapkernel flag can be used to generate a conv2 instead of xcorr2
 *   - the templated kernel size is useful to generate code that's 2x faster
 *     but can be set to 0 to allow arbitrary kernel sizes
 */
template <bool swapkernel, int T_kernel_h, int T_kernel_w>
  __global__ void conv2generic(float *input, float *kernel, float *output,
                               int input_n, int input_h, int input_w,
                               int kernel_n, int kernel_h, int kernel_w,
                               int stride_h, int stride_w)
{
  // output dimensions
  int output_h = (input_h - kernel_h) / stride_h + 1;
  int output_w = (input_w - kernel_w) / stride_w + 1;

  // xcorr or conv
  int koffset = swapkernel ? kernel_w*kernel_h-1 : 0;

  // nb outputs
  int output_n = kernel_n / input_n;

  // generate offsets according to block/thread ids
  int xx_start = threadIdx.x;
  int xx_end = output_w;
  int xx_step = blockDim.x;

  int yy_start = blockDim.y*blockIdx.y + threadIdx.y;
  int yy_end = output_h;
  int yy_step = blockDim.y*gridDim.y;

  int oo_start = blockIdx.x;
  int oo_end = oo_start+1;

  int ii_start = (blockIdx.x / output_n) * input_n;
  int ii_end = ii_start + input_n;

  // nb threads, unique thread id
  int tid = blockDim.x*blockDim.y*threadIdx.z + blockDim.x * threadIdx.y + threadIdx.x;
  int nthreads = blockDim.x * blockDim.y * blockDim.z;

  // iterators
  int oo, ii, xx, yy, kx, ky, kk;

  // do the kernels fit in shared mem ?
  if (input_n*kernel_w*kernel_h <= CUDA_SHARED_MEM_SIZE) {

    // put the kernel in shared memory
    __shared__ float shared_kernel[CUDA_SHARED_MEM_SIZE];

    // first thread of each block does the copy
    for (kk = tid; kk < kernel_w*kernel_h*input_n; kk += nthreads) {
      shared_kernel[kk] = kernel[input_n*kernel_w*kernel_h*(oo_start % output_n) + kk];
    }
    __syncthreads();

    // templated kernel size
    if ((T_kernel_w > 0) && (T_kernel_h > 0)) {
      // unrolled convolution loop
      for(oo = oo_start; oo < oo_end; oo++) {
        for(ii = ii_start; ii < ii_end; ii++) {
          for(yy = yy_start; yy < yy_end; yy+=yy_step) {
            for(xx = xx_start; xx < xx_end; xx+=xx_step) {
              // Dot product in two dimensions... (between input image and the mask)
              float *input_p = input + ii*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
              float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
              float *kernel_p = shared_kernel + (ii % input_n)*kernel_w*kernel_h + koffset;
              float sum = 0;
              if (swapkernel) {
#pragma unroll
                for(ky = 0; ky < T_kernel_h; ky++) {
#pragma unroll
                  for(kx = 0; kx < T_kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p--);
                  }
                  input_p += input_w;
                }
              } else {
#pragma unroll
                for(ky = 0; ky < T_kernel_h; ky++) {
#pragma unroll
                  for(kx = 0; kx < T_kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p++);
                  }
                  input_p += input_w;
                }
              }
              *output_p += sum;
            }
          }
        }
      }
    } else {
      // default convolution loop
      for(oo = oo_start; oo < oo_end; oo++) {
        for(ii = ii_start; ii < ii_end; ii++) {
          for(yy = yy_start; yy < yy_end; yy+=yy_step) {
            for(xx = xx_start; xx < xx_end; xx+=xx_step) {
              // Dot product in two dimensions... (between input image and the mask)
              float *input_p = input + ii*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
              float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
              float *kernel_p = shared_kernel + (ii % input_n) * kernel_w * kernel_h + koffset;
              float sum = 0;
              if (swapkernel) {
                for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                  for(kx = 0; kx < kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p--);
                  }
                  input_p += input_w;
                }
              } else {
                for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                  for(kx = 0; kx < kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p++);
                  }
                  input_p += input_w;
                }
              }
              *output_p += sum;
            }
          }
        }
      }
    }

  } else { // not enough shared mem for kernels, simply stream them

    // convolution loop
    for(oo = oo_start; oo < oo_end; oo++) {
      for(ii = ii_start; ii < ii_end; ii++) {
        for(yy = yy_start; yy < yy_end; yy+=yy_step) {
          for(xx = xx_start; xx < xx_end; xx+=xx_step) {
            // Dot product in two dimensions... (between input image and the mask)
            float *input_p = input + ii*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
            float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
            float *kernel_p = kernel + ((oo % output_n) * input_n + (ii % input_n))*kernel_w*kernel_h + koffset;
            float sum = 0;
            if (swapkernel) {
              for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                for(kx = 0; kx < kernel_w; kx++) {
                  sum += input_p[kx]*(*kernel_p--);
                }
                input_p += input_w;
              }
            } else {
              for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                for(kx = 0; kx < kernel_w; kx++) {
                  sum += input_p[kx]*(*kernel_p++);
                }
                input_p += input_w;
              }
            }
            *output_p += sum;
          }
        }
      }
    }
  }
}

  void ebl::cuda_convolution_3d(idx<float32> &in, idx<float32> &ker,
			      idx<float32> &out, 
                         intg stride_x, intg stride_y, int devid) {
    // cout << "Executing on Device " << devid << " (CUDA)"<<endl;
    if (devid != -1)
      cudaSetDevice(devid);
    // copy input and kernels to gpu, allocate output on gpu
    cudaError_t err;
    float *input_data, *kernel_data, *output_data;
    LOCAL_TIMING_START();
    cudaMalloc((void**) &input_data, in.nelements() * sizeof(float));
    cudaMemcpy(input_data, in.idx_ptr(), in.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMalloc((void**) &kernel_data, ker.nelements() * sizeof(float));
    cudaMemcpy( kernel_data, ker.idx_ptr(), ker.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMalloc((void**) &output_data, out.nelements() * sizeof(float));
    cudaMemcpy(output_data, out.idx_ptr(), out.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    // set the number of blocks and threads
    int nthreads_x = 32;
    int nthreads_y = 8;
    int block_height = floor(16 / out.dim(0));
    if (block_height < 1)
      block_height = 1;
    dim3 blocks(out.dim(0),block_height);
    dim3 threads(nthreads_x,nthreads_y);

    // sync any previous kernel exec
    cudaDeviceSynchronize();
    LOCAL_TIMING_REPORT("convgpu initial transfer time" << in.dim(0));
    LOCAL_TIMING2_START();
    if ((ker.dim(2) == 3) && (ker.dim(1) == 3))
      conv2generic <false, 3, 3> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 5) && (ker.dim(1) == 5))
      conv2generic <false, 5, 5> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 7) && (ker.dim(1) == 7))
      conv2generic <false, 7, 7> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 9) && (ker.dim(1) == 9))
      conv2generic <false, 9, 9> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 11) && (ker.dim(1) == 11))
      conv2generic <false, 11, 11> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y);
    else if ((ker.dim(2) == 13) && (ker.dim(1) == 13))
      conv2generic <false, 13, 13> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y);
    else if ((ker.dim(2) == 4) && (ker.dim(1) == 4))
      conv2generic <false, 4, 4> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 6) && (ker.dim(1) == 6))
      conv2generic <false, 6, 6> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 8) && (ker.dim(1) == 8))
      conv2generic <false, 8, 8> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y);
    else if ((ker.dim(2) == 10) && (ker.dim(1) == 10))
      conv2generic <false, 10, 10> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y);
    else if ((ker.dim(2) == 12) && (ker.dim(1) == 12))
      conv2generic <false, 12, 12> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y);
    else
      conv2generic <false, 0 , 0> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                         in.dim(0), in.dim(1), in.dim(2),
                                                         out.dim(0)*in.dim(0), ker.dim(1), ker.dim(2),
                                                         stride_x, stride_y);



  LOCAL_TIMING2_REPORT("convgpu kernel execution time");
  // sync & clean
  cudaDeviceSynchronize();
  cudaMemcpy(out.idx_ptr(), output_data, out.nelements() * sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(input_data);
  cudaFree(kernel_data);
  cudaFree(output_data);

  // check for errors
  err = cudaGetLastError();
  if (err != cudaSuccess)
    eblerror("cuda error" << cudaGetErrorString(err));
}


  /////////////////////////////////////////////////////////////////////////////
  //////////////////////
  /// ebl_convolution_3dmap
  /////////////////////////////////////////////////////////////////////////////
  /*
   * Description:
   *   base conv2D routine: 3D input, 3D output, 4D kernel
   *
   *   - all chunks of data should be contiguous
   *   - the swapkernel flag can be used to generate a conv2 instead of xcorr2
   *   - the templated kernel size is useful to generate code that's 2x faster
   *     but can be set to 0 to allow arbitrary kernel sizes
   *   ---- the table should have the first dim with the outputs, each output 
   *   ---- should have a fanin set of inputs contiguously
   */
  template <bool swapkernel, int T_kernel_h, int T_kernel_w>
  __global__ void conv2mapgeneric(float *input, float *kernel, float *output,
                               int input_n, int input_h, int input_w,
                               int kernel_n, int kernel_h, int kernel_w,
                                  int stride_h, int stride_w,
                                  long *table, int fanin)
  {
    // output dimensions
    int output_h = (input_h - kernel_h) / stride_h + 1;
    int output_w = (input_w - kernel_w) / stride_w + 1;

    // xcorr or conv
    int koffset = swapkernel ? kernel_w*kernel_h-1 : 0;

    // nb outputs
    int output_n = kernel_n / fanin;

    // generate offsets according to block/thread ids
    int xx_start = threadIdx.x;
    int xx_end = output_w;
    int xx_step = blockDim.x;

    int yy_start = blockDim.y*blockIdx.y + threadIdx.y;
    int yy_end = output_h;
    int yy_step = blockDim.y*gridDim.y;

    int oo_start = blockIdx.x;
    int oo_end = oo_start+1;

    int table_start = blockIdx.x * (fanin * 2);
    int table_end = table_start + (fanin * 2);

    // nb threads, unique thread id
    int tid = blockDim.x*blockDim.y*threadIdx.z + blockDim.x * threadIdx.y + threadIdx.x;
    int nthreads = blockDim.x * blockDim.y * blockDim.z;

    // iterators
    int oo, ii, xx, yy, kx, ky, kk;

    // do the kernels fit in shared mem ?
    if (fanin*kernel_w*kernel_h <= CUDA_SHARED_MEM_SIZE) {

      // put the kernel in shared memory
      __shared__ float shared_kernel[CUDA_SHARED_MEM_SIZE];

      // first thread of each block does the copy
      for (kk = tid; kk < kernel_w*kernel_h*fanin; kk += nthreads) {
        shared_kernel[kk] = kernel[fanin*kernel_w*kernel_h*(oo_start % output_n) + kk];
      }
      __syncthreads();

      // templated kernel size
      if ((T_kernel_w > 0) && (T_kernel_h > 0)) {
        // unrolled convolution loop
        for(oo = oo_start; oo < oo_end; oo++) {
          for (ii = table_start; ii < table_end; ii = ii + 2) {
            for(yy = yy_start; yy < yy_end; yy+=yy_step) {
              for(xx = xx_start; xx < xx_end; xx+=xx_step) {
                // Dot product in two dimensions... (between input image and the mask)
                float *input_p = input + table[ii]*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
                float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
                // float *kernel_p = shared_kernel + (table[ii] % fanin)*kernel_w*kernel_h + koffset;
                float *kernel_p = shared_kernel + table[ii + 1] *kernel_w*kernel_h + koffset;
                float sum = 0;
                if (swapkernel) {
#pragma unroll
                  for(ky = 0; ky < T_kernel_h; ky++) {
#pragma unroll
                    for(kx = 0; kx < T_kernel_w; kx++) {
                      sum += input_p[kx]*(*kernel_p--);
                    }
                    input_p += input_w;
                  }
                } else {
#pragma unroll
                  for(ky = 0; ky < T_kernel_h; ky++) {
#pragma unroll
                    for(kx = 0; kx < T_kernel_w; kx++) {
                      sum += input_p[kx]*(*kernel_p++);
                    }
                    input_p += input_w;
                  }
                }
                *output_p += sum;
              }
            }
          }
        }
      } else {
        // default convolution loop
        for(oo = oo_start; oo < oo_end; oo++) {
          for (ii = table_start; ii < table_end; ii++) {
            for(yy = yy_start; yy < yy_end; yy+=yy_step) {
              for(xx = xx_start; xx < xx_end; xx+=xx_step) {
                // Dot product in two dimensions... (between input image and the mask)
                float *input_p = input + table[ii]*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
                float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
                float *kernel_p = shared_kernel + (table[ii] % fanin) * kernel_w * kernel_h + koffset;
                float sum = 0;
                if (swapkernel) {
                  for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                    for(kx = 0; kx < kernel_w; kx++) {
                      sum += input_p[kx]*(*kernel_p--);
                    }
                    input_p += input_w;
                  }
                } else {
                  for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                    for(kx = 0; kx < kernel_w; kx++) {
                      sum += input_p[kx]*(*kernel_p++);
                    }
                    input_p += input_w;
                  }
                }
                *output_p += sum;
              }
            }
          }
        }
      }

    } else { // not enough shared mem for kernels, simply stream them

      // convolution loop
      for(oo = oo_start; oo < oo_end; oo++) {
        for (ii = table_start; ii < table_end; ii = ii + 2) {
          for(yy = yy_start; yy < yy_end; yy+=yy_step) {
            for(xx = xx_start; xx < xx_end; xx+=xx_step) {
              // Dot product in two dimensions... (between input image and the mask)
              float *input_p = input + table[ii]*input_h*input_w + yy*stride_h*input_w + xx*stride_w;
              float *output_p = output + oo*output_h*output_w + yy*output_w + xx;
              //float *kernel_p = kernel + ((oo % output_n) * fanin + (table[ii + 1] % fanin))*kernel_w*kernel_h + koffset;
              float *kernel_p = kernel + table[ii + 1] *kernel_w*kernel_h + koffset;
              float sum = 0;
              if (swapkernel) {
                for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                  for(kx = 0; kx < kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p--);
                  }
                  input_p += input_w;
                }
              } else {
                for(ky = 0; ky < kernel_h; ky++) {
#pragma unroll 5
                  for(kx = 0; kx < kernel_w; kx++) {
                    sum += input_p[kx]*(*kernel_p++);
                  }
                  input_p += input_w;
                }
              }
              *output_p += sum;
            }
          }
        }
      }
    }
  }


  void ebl::cuda_convolution_3dmap(idx<float32> &in, idx<float32> &ker,
			      idx<float32> &out, 
                                   intg stride_x, intg stride_y, 
                                   idx<intg> table, int fanin, int devid) {
    // cout << "Executing on Device " << devid << " (CUDA)"<<endl;
    if (devid != -1)
      cudaSetDevice(devid);
    // copy input and kernels to gpu, allocate output on gpu
    cudaError_t err;
    float *input_data, *kernel_data, *output_data;
    long *table_data;
    LOCAL_TIMING_START();
    cudaMalloc((void**) &input_data, in.nelements() * sizeof(float));
    cudaMemcpy(input_data, in.idx_ptr(), in.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMalloc((void**) &kernel_data, ker.nelements() * sizeof(float));
    cudaMemcpy( kernel_data, ker.idx_ptr(), ker.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMalloc((void**) &output_data, out.nelements() * sizeof(float));
    cudaMemcpy(output_data, out.idx_ptr(), out.nelements() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMalloc((void**) &table_data, table.nelements() * sizeof(intg));
    cudaMemcpy(table_data, table.idx_ptr(), table.nelements() * sizeof(intg), cudaMemcpyHostToDevice);
    // set the number of blocks and threads
    int nthreads_x = 32;
    int nthreads_y = 8;
    int block_height = floor(16 / out.dim(0));
    if (block_height < 1)
      block_height = 1;
    dim3 blocks(out.dim(0),block_height);
    dim3 threads(nthreads_x,nthreads_y);
    
    // cout << "DEBUG INFO:" <<"\nnthread_x:" << nthreads_x
    //      <<"\nnthread_y:" << nthreads_y
    //      <<"\nblock_height:" << block_height
    //      <<"\nblock_width:" << out.dim(0)
    //      <<"\tinput_elements: "<<in.nelements()
    //      <<"\tkernel_elements: "<<ker.nelements()
    //      <<"\toutput_elements: "<<out.nelements()
    //      << "\nin0:"<<in.dim(0)<<"\n in1:"<< in.dim(1)<<"\n in2:"<< in.dim(2)
    //      <<"\nkernel_n: "<<out.dim(0) *fanin<<"\n ker.dim1:"<<ker.dim(1)<<"\n ker.dim2:"
    //      <<ker.dim(2)
    //      <<"\nfanin:" <<fanin <<endl;
    // sync any previous kernel exec
    cudaDeviceSynchronize();
    LOCAL_TIMING_REPORT("convgpu initial transfer time" << in.dim(0));
    LOCAL_TIMING2_START();
    if ((ker.dim(2) == 3) && (ker.dim(1) == 3))
      conv2mapgeneric <false, 3, 3> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 5) && (ker.dim(1) == 5))
      conv2mapgeneric <false, 5, 5> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 7) && (ker.dim(1) == 7))
      conv2mapgeneric <false, 7, 7> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 9) && (ker.dim(1) == 9))
      conv2mapgeneric <false, 9, 9> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 11) && (ker.dim(1) == 11))
      conv2mapgeneric <false, 11, 11> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 13) && (ker.dim(1) == 13))
      conv2mapgeneric <false, 13, 13> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 4) && (ker.dim(1) == 4))
      conv2mapgeneric <false, 4, 4> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 6) && (ker.dim(1) == 6))
      conv2mapgeneric <false, 6, 6> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 8) && (ker.dim(1) == 8))
      conv2mapgeneric <false, 8, 8> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                        in.dim(0), in.dim(1), in.dim(2),
                                                        out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                        stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 10) && (ker.dim(1) == 10))
      conv2mapgeneric <false, 10, 10> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y, table_data, fanin);
    else if ((ker.dim(2) == 12) && (ker.dim(1) == 12))
      conv2mapgeneric <false, 12, 12> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                          in.dim(0), in.dim(1), in.dim(2),
                                                          out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                          stride_x, stride_y, table_data, fanin);
    else
      conv2mapgeneric <false, 0 , 0> <<<blocks, threads>>> (input_data, kernel_data, output_data,
                                                         in.dim(0), in.dim(1), in.dim(2),
                                                         out.dim(0)*fanin, ker.dim(1), ker.dim(2),
                                                         stride_x, stride_y, table_data, fanin);



  LOCAL_TIMING2_REPORT("convgpu kernel execution time");
  // sync & clean
  cudaDeviceSynchronize();
  cudaMemcpy(out.idx_ptr(), output_data, out.nelements() * sizeof(float), cudaMemcpyDeviceToHost);
  cudaFree(input_data);
  cudaFree(kernel_data);
  cudaFree(output_data);
  cudaFree(table_data);

  // check for errors
  err = cudaGetLastError();
  if (err != cudaSuccess)
    eblerror("cuda error: " << cudaGetErrorString(err));
}



#endif

} // end namespace ebl
