#include <metal_stdlib>
using namespace metal;

kernel void classify(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant uint& var_$3 [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
uint var_$4;
uint var_$5;
uint var_$6;
uint var_$7;
uint var_$8;
bool var_$9;
int var_$11;
ulong var_$15;
ulong var_$16;
ulong var_$17;
device uchar* var_$18;
uint var_$21;
uint var_$22;
bool var_$23;
uint var_$25;
int var_$26;
ulong var_$30;
ulong var_$31;
ulong var_$32;
device uchar* var_$33;
int var_$44;
ulong var_$48;
ulong var_$49;
ulong var_$50;
device uchar* var_$51;
uint var_$54;
uint var_$55;
bool var_$56;
uint var_$58;
int var_$59;
ulong var_$63;
ulong var_$64;
ulong var_$65;
device uchar* var_$66;
int var_$74;
ulong var_$78;
ulong var_$79;
ulong var_$80;
device uchar* var_$81;
uint var_$84;
uint var_$85;
bool var_$86;
uint var_$88;
int var_$89;
ulong var_$93;
ulong var_$94;
ulong var_$95;
device uchar* var_$96;
uint var_$104;
int var_$105;
ulong var_$109;
ulong var_$110;
ulong var_$111;
device uchar* var_$112;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$4 = nautilus_blockIdx.x;
var_$5 = nautilus_blockDim.x;
var_$6 = var_$4*var_$5;
var_$7 = nautilus_threadIdx.x;
var_$8 = var_$6+var_$7;
var_$9 = var_$8 < var_$3;
if (var_$9){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
device uchar* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
__pc = 1; continue;
}else{
{
}
__pc = 9; continue;}
}
case 1: {
var_$11 = (int)var_$8;
var_$15 = (ulong)4;
var_$16 = (ulong)var_$11;
var_$17 = var_$16*var_$15;
var_$18 = var_$1+var_$17;
var_$21 = (uint)200;
var_$22 = *((device uint*)(var_$18));
var_$23 = var_$22 > var_$21;
if (var_$23){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
__pc = 2; continue;
}else{
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
device uchar* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
__pc = 4; continue;}
}
case 2: {
var_$25 = (uint)3;
var_$26 = (int)var_$8;
var_$30 = (ulong)4;
var_$31 = (ulong)var_$26;
var_$32 = var_$31*var_$30;
var_$33 = var_$2+var_$32;
*((device uint*)(var_$33)) = var_$25;
{
}
__pc = 3; continue;
}
case 3: {
return;
}
case 4: {
var_$44 = (int)var_$8;
var_$48 = (ulong)4;
var_$49 = (ulong)var_$44;
var_$50 = var_$49*var_$48;
var_$51 = var_$1+var_$50;
var_$54 = (uint)100;
var_$55 = *((device uint*)(var_$51));
var_$56 = var_$55 > var_$54;
if (var_$56){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
__pc = 5; continue;
}else{
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
device uchar* temp_2 = var_$1;
var_$2 = temp_0;
var_$8 = temp_1;
var_$1 = temp_2;
}
__pc = 6; continue;}
}
case 5: {
var_$58 = (uint)2;
var_$59 = (int)var_$8;
var_$63 = (ulong)4;
var_$64 = (ulong)var_$59;
var_$65 = var_$64*var_$63;
var_$66 = var_$2+var_$65;
*((device uint*)(var_$66)) = var_$58;
{
}
__pc = 3; continue;
}
case 6: {
var_$74 = (int)var_$8;
var_$78 = (ulong)4;
var_$79 = (ulong)var_$74;
var_$80 = var_$79*var_$78;
var_$81 = var_$1+var_$80;
var_$84 = (uint)0;
var_$85 = *((device uint*)(var_$81));
var_$86 = var_$85 > var_$84;
if (var_$86){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
__pc = 7; continue;
}else{
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$8;
var_$2 = temp_0;
var_$8 = temp_1;
}
__pc = 8; continue;}
}
case 7: {
var_$88 = (uint)1;
var_$89 = (int)var_$8;
var_$93 = (ulong)4;
var_$94 = (ulong)var_$89;
var_$95 = var_$94*var_$93;
var_$96 = var_$2+var_$95;
*((device uint*)(var_$96)) = var_$88;
{
}
__pc = 3; continue;
}
case 8: {
var_$104 = (uint)0;
var_$105 = (int)var_$8;
var_$109 = (ulong)4;
var_$110 = (ulong)var_$105;
var_$111 = var_$110*var_$109;
var_$112 = var_$2+var_$111;
*((device uint*)(var_$112)) = var_$104;
{
}
__pc = 3; continue;
}
case 9: {
{
}
__pc = 3; continue;
}
}
}
}

