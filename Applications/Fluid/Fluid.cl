#ifndef STRINGIFY
#define STRINGIFY(str) str
#endif

STRINGIFY(
          
          struct FluidVars
          {
              float timeDiff;
              float restDensity;
              float internalStiffness;
              float viscosity;
              
              float boundaryStiffness;
              float boundaryDampening;
              float3 boundaryMin;
              float3 boundaryMax;
              
              float3 initMin;
              float3 initMax;
              
              float accelerationLimit;
              float velocityLimit;
              float simulationScale;
              
              float3 gravity;
              
              float particleMass;
              float smoothingRadius;
              float boundaryRadius;
              
              float d2;
              float rd2;
              float poly6Kern;
              float spikyKern;
              float lapKern;
              
              uint gridSize;
              uint gridCount;
          };
          
          inline uint3 getGridPosition(float3 position, float simulationScale, uint gridSize)
          {
              position *= simulationScale * gridSize;
              position = clamp(position, 0.0f, gridSize - 1.0f);
              return (uint3)(position.x, position.y, position.z);
          }
          
          inline uint getGridIndex(float3 position, float simulationScale, uint gridSize)
          {
              uint3 gridPosition = getGridPosition(position, simulationScale, gridSize);
              return (gridPosition.z * gridSize + gridPosition.y) * gridSize + gridPosition.x;
          }
          
          inline uint getGridIndex2(uint3 gridPosition, uint gridSize)
          {
              return (gridPosition.z * gridSize + gridPosition.y) * gridSize + gridPosition.x;
          }
          
          __kernel void init(__global __write_only float3* positionBuffer,
                             __global __write_only float3* velocityBuffer,
                             __global __write_only float3* velocityBufferEval,
                             __private struct FluidVars vars)
          {
              uint globalIndex0 = get_global_id(0);
              uint globalIndex1 = get_global_id(1);
              uint globalIndex2 = get_global_id(2);
              uint globalSize0 = get_global_size(0);
              uint globalSize1 = get_global_size(1);
              uint globalSize2 = get_global_size(2);
              uint globalIndex = (globalSize0 * globalSize1 * globalIndex2) +
              (globalSize0 * globalIndex1) +
              globalIndex0;
              
              float3 t = (float3)(globalIndex0, globalIndex1, globalIndex2);
              t /= (float3)(globalSize0, globalSize1, globalSize2);
              
              positionBuffer[globalIndex] = vars.initMin + (vars.initMax - vars.initMin) * t;
              velocityBuffer[globalIndex] = 0.0f;
              velocityBufferEval[globalIndex] = 0.0f;
          }
          
          __kernel void copy(__global __read_only float3* positionBuffer,
                             __global __write_only float4* glBuffer)
          {
              uint globalIndex = get_global_id(0);
              glBuffer[globalIndex] = (float4)(positionBuffer[globalIndex], 1.0f);
          }
          
          __kernel void pressure(__global __read_only uint* gridScanBuffer,
                                 __global __read_only uint* gridCountBuffer,
                                 __global __read_only float3* positionSortBuffer,
                                 __global __write_only float* pressureBuffer,
                                 __global __write_only float* densityBuffer,
                                 __private struct FluidVars vars)
          {
              uint globalIndex = get_global_id(0);
              
              float sum = 0.0f;
              float3 ri = positionSortBuffer[globalIndex];
              
              uint3 gridPos = getGridPosition(ri, vars.simulationScale, vars.gridSize) - 1;
              for (int i = 0; i < 27; ++i)
              {
                  uint3 gridPos2 = gridPos + (uint3)(i % 3, (i / 3) % 3, i / 9);
                  uint3 gridPos2cl = clamp(gridPos2, 0U, vars.gridSize - 1);
                  uint index = 0;
                  uint indexEnd = 0;
                  if (gridPos2cl.x == gridPos2.x && gridPos2cl.y == gridPos2.y && gridPos2cl.z == gridPos2.z)
                  {
                      uint gridIndex = getGridIndex2(gridPos2, vars.gridSize);
                      index = gridScanBuffer[gridIndex];
                      indexEnd = index + gridCountBuffer[gridIndex];
                  }
                  for (uint ii = index; ii < indexEnd; ++ii)
                  {
                      
                      float3 rj = positionSortBuffer[ii];
                      float3 rij = ri - rj;
                      float r2 = dot(rij, rij);
                      if (r2 > 0.0f && r2 < vars.rd2)
                      {
                          sum += pow((vars.rd2 - r2) * vars.d2, 3.0f);
                      }
                  }
              }
              
              sum = max(vars.particleMass * vars.poly6Kern * sum, 1.0f);
              pressureBuffer[globalIndex] = (sum - vars.restDensity) * vars.internalStiffness;
              densityBuffer[globalIndex] = 1.0f / sum;
          }
          
          __kernel void force(__global __read_only uint* gridScanBuffer,
                              __global __read_only uint* gridCountBuffer,
                              __global __read_only float3* positionSortBuffer,
                              __global __read_only float3* velocityEvalSortBuffer,
                              __global __read_only float* pressureBuffer,
                              __global __read_only float* densityBuffer,
                              __global __write_only float3* forceBuffer,
                              __private struct FluidVars vars)
          {
              uint globalIndex = get_global_id(0);
              
              float3 ri = positionSortBuffer[globalIndex];
              float3 vi = velocityEvalSortBuffer[globalIndex];
              float pi = pressureBuffer[globalIndex];
              float di = densityBuffer[globalIndex];
              
              float3 fp = 0.0f;
              float3 fv = 0.0f;
              
              uint3 gridPos = getGridPosition(ri, vars.simulationScale, vars.gridSize) - 1;
              for (int i = 0; i < 27; ++i)
              {
                  uint3 gridPos2 = gridPos + (uint3)(i % 3, (i / 3) % 3, i / 9);
                  uint3 gridPos2cl = clamp(gridPos2, 0U, vars.gridSize - 1);
                  uint index = 0;
                  uint indexEnd = 0;
                  if (gridPos2cl.x == gridPos2.x && gridPos2cl.y == gridPos2.y && gridPos2cl.z == gridPos2.z)
                  {
                      uint gridIndex = getGridIndex2(gridPos2, vars.gridSize);
                      index = gridScanBuffer[gridIndex];
                      indexEnd = index + gridCountBuffer[gridIndex];
                  }
                  for (uint ii = index; ii < indexEnd; ++ii)
                  {
                      float3 rj = positionSortBuffer[ii];
                      float3 rij = ri - rj;
                      float r2 = dot(rij, rij);
                      if (r2 > 0.0f && r2 < vars.rd2)
                      {
                          float r = sqrt(r2 * vars.d2);
                          float c = vars.smoothingRadius - r;
                          float3 vj = velocityEvalSortBuffer[ii];
                          float pj = pressureBuffer[ii];
                          float dj = densityBuffer[ii];
                          
                          fp += rij * c * c * (pi + pj) * dj / r;
                          fv += (vj - vi) * c * dj;
                      }
                  }
              }
              
              forceBuffer[globalIndex] = ( fp * -0.5f * vars.spikyKern * vars.simulationScale +
                                          fv * vars.viscosity * vars.lapKern ) * di;
          }
          
          __kernel void advance(__global __read_only float3* positionSortBuffer,
                                __global __read_only float3* velocitySortBuffer,
                                __global __read_only float3* velocityEvalSortBuffer,
                                __global __read_only float3* forceBuffer,
                                __global __write_only float3* positionBuffer,
                                __global __write_only float3* velocityBuffer,
                                __global __write_only float3* velocityEvalBuffer,
                                __private struct FluidVars vars)
          {
              uint globalIndex = get_global_id(0);
              
              float3 position = positionSortBuffer[globalIndex];
              float3 velocity = velocitySortBuffer[globalIndex];
              float3 velocityEval = velocityEvalSortBuffer[globalIndex];
              float3 acceleration = forceBuffer[globalIndex];
              
              acceleration *= vars.particleMass;
              
              float3 diff = max(vars.boundaryRadius - (position - vars.boundaryMin) * vars.simulationScale, 0.0f);
              acceleration += vars.boundaryStiffness * diff - vars.boundaryDampening * velocityEval * sign(diff);
              diff = max(vars.boundaryRadius - (vars.boundaryMax - position) * vars.simulationScale, 0.0f);
              acceleration -= vars.boundaryStiffness * diff + vars.boundaryDampening * velocityEval * sign(diff);
              
              acceleration += vars.gravity;
              
              acceleration *= vars.accelerationLimit / max(vars.accelerationLimit, length(acceleration));
              velocity *= vars.velocityLimit / max(vars.velocityLimit, length(velocity));
              
              float3 vnext = acceleration * vars.timeDiff + velocity;
              velocityEvalBuffer[globalIndex] = (velocity + vnext) * 0.5f;
              velocityBuffer[globalIndex] = vnext;
              positionBuffer[globalIndex] = position + vnext * vars.timeDiff / vars.simulationScale;
          }
          
          __kernel void gridSort(__global __read_only float3* positionBuffer,
                                 __global __read_only float3* velocityBuffer,
                                 __global __read_only float3* velocityEvalBuffer,
                                 __global __write_only float3* positionSortBuffer,
                                 __global __write_only float3* velocitySortBuffer,
                                 __global __write_only float3* velocityEvalSortBuffer,
                                 __global uint* gridCountBuffer,
                                 __global uint* gridScanBuffer,
                                 __private uint particleCount,
                                 __private struct FluidVars vars)
          {
              uint localIndex = get_local_id(0);              
              uint16* gridCountBuffer16 = (uint16*)(gridCountBuffer);
              
              // Clear grid count buffer
              for (uint i = localIndex; i * 16 < vars.gridCount; i += 256)
              {
                  gridCountBuffer16[i] = (uint16)(0);
              }
              barrier(CLK_GLOBAL_MEM_FENCE);
              
              // Count particles per grid
              for (uint i = localIndex; i < particleCount; i += 256)
              {
                  uint gridIndex = getGridIndex(positionBuffer[i], vars.simulationScale, vars.gridSize);
                  atomic_inc(&gridCountBuffer[gridIndex]);
              }
              barrier(CLK_GLOBAL_MEM_FENCE);
              
              // Create prefix sums into grid scan buffer
              __local uint prefixScan[257];
              prefixScan[0] = 0;
              int remain = 0;
              
              uint4* gridCountBuffer4 = (uint4*)(gridCountBuffer);
              uint4* gridScanBuffer4 = (uint4*)(gridScanBuffer);
              
              for (uint i = localIndex; i * 4 < vars.gridCount; i += 256)
              {
                  uint4 value = gridCountBuffer4[i];
                  prefixScan[localIndex + 1] = value.x + value.y + value.z + value.w;
                  barrier(CLK_LOCAL_MEM_FENCE);
                  
                  for (int step = 2; step <= 256; step <<= 1)
                  {
                      int targetIdx = step * localIndex + step - 1;
                      if (targetIdx < 256)
                      {
                          prefixScan[targetIdx] += prefixScan[targetIdx - (step >> 1)];
                      }
                      barrier(CLK_LOCAL_MEM_FENCE);
                  }
                  for (int step = 128; step > 1; step >>= 1)
                  {
                      int sourceIdx = step * localIndex + step - 1;
                      int targetIdx = sourceIdx + (step >> 1);
                      if (targetIdx <= 256)
                      {
                          prefixScan[targetIdx] += prefixScan[sourceIdx];
                      }
                      barrier(CLK_LOCAL_MEM_FENCE);
                  }
                  
                  value.w += value.z;
                  value.zw += value.y;
                  value.yzw += value.x;
                  value += prefixScan[localIndex] + remain;
                  
                  gridScanBuffer4[i] = value;
                  
                  remain += prefixScan[256];
                  
                  barrier(CLK_LOCAL_MEM_FENCE);
              }
              barrier(CLK_GLOBAL_MEM_FENCE);
              
              // Reorder position and velocity buffers PLUS decrease scan buffer indices
              for (uint i = localIndex; i < particleCount; i += 256)
              {
                  float3 position = positionBuffer[i];
                  uint gridIndex = getGridIndex(position, vars.simulationScale, vars.gridSize);
                  uint outIndex = atomic_dec(&gridScanBuffer[gridIndex]) - 1;
                  
                  positionSortBuffer[outIndex] = position;
                  velocitySortBuffer[outIndex] = velocityBuffer[i];
                  velocityEvalSortBuffer[outIndex] = velocityEvalBuffer[i];
              }
          }
          
          )