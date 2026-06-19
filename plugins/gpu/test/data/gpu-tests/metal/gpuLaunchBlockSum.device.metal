#include <metal_stdlib>
using namespace metal;

kernel void blockSum(
    device uchar* var_$1 [[buffer(0)]],
    device uchar* var_$2 [[buffer(1)]],
    constant uint& var_$3 [[buffer(2)]],
    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],
    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],
    uint3 nautilus_blockDim [[threads_per_threadgroup]],
    uint3 nautilus_gridDim [[threadgroups_per_grid]]
) {
ulong var_$4;
ulong var_$5;
threadgroup uchar nautilus_shared_6[1024];
threadgroup uchar* var_$6;
uint var_$7;
uint var_$8;
uint var_$9;
uint var_$10;
uint var_$11;
uint var_$12;
bool var_$13;
int var_$15;
ulong var_$19;
ulong var_$20;
ulong var_$21;
device uchar* var_$22;
uint var_$25;
int var_$27;
ulong var_$31;
ulong var_$32;
ulong var_$33;
threadgroup uchar* var_$34;
uint var_$40;
uint var_$41;
uint var_$42;
uint var_$43;
bool var_$44;
bool var_$46;
int var_$48;
ulong var_$52;
ulong var_$53;
ulong var_$54;
threadgroup uchar* var_$55;
uint var_$58;
int var_$59;
ulong var_$63;
ulong var_$64;
ulong var_$65;
threadgroup uchar* var_$66;
uint var_$69;
uint var_$70;
uint var_$71;
int var_$72;
ulong var_$76;
ulong var_$77;
ulong var_$78;
threadgroup uchar* var_$79;
uint var_$85;
uint var_$86;
uint var_$87;
uint var_$94;
bool var_$95;
int var_$97;
ulong var_$101;
ulong var_$102;
ulong var_$103;
threadgroup uchar* var_$104;
uint var_$107;
uint var_$109;
int var_$110;
ulong var_$114;
ulong var_$115;
ulong var_$116;
device uchar* var_$117;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$4 = (ulong)1024;
var_$5 = (ulong)4;
var_$6 = nautilus_shared_6;
var_$7 = nautilus_threadIdx.x;
var_$8 = nautilus_blockIdx.x;
var_$9 = nautilus_blockDim.x;
var_$10 = var_$8*var_$9;
var_$11 = var_$10+var_$7;
var_$12 = (uint)0;
var_$13 = var_$11 < var_$3;
if (var_$13){
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
device uchar* temp_3 = var_$1;
uint temp_4 = var_$11;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$1 = temp_3;
var_$11 = temp_4;
}
__pc = 1; continue;
}else{
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$12;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
__pc = 12; continue;}
}
case 1: {
var_$15 = (int)var_$11;
var_$19 = (ulong)4;
var_$20 = (ulong)var_$15;
var_$21 = var_$20*var_$19;
var_$22 = var_$1+var_$21;
var_$25 = *((device uint*)(var_$22));
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$25;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
__pc = 2; continue;
}
case 2: {
var_$27 = (int)var_$7;
var_$31 = (ulong)4;
var_$32 = (ulong)var_$27;
var_$33 = var_$32*var_$31;
var_$34 = var_$6+var_$33;
*((threadgroup uint*)(var_$34)) = var_$12;
threadgroup_barrier(mem_flags::mem_threadgroup);
var_$40 = nautilus_blockDim.x;
var_$41 = (uint)2;
var_$42 = var_$40/var_$41;
var_$43 = (uint)0;
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
uint temp_4 = var_$43;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
var_$43 = temp_4;
}
__pc = 3; continue;
}
case 3: {
var_$44 = var_$42 > var_$43;
if (var_$44){
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
__pc = 4; continue;
}else{
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
}
__pc = 8; continue;}
}
case 4: {
var_$46 = var_$7 < var_$42;
if (var_$46){
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
__pc = 5; continue;
}else{
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
__pc = 7; continue;}
}
case 5: {
var_$48 = (int)var_$7;
var_$52 = (ulong)4;
var_$53 = (ulong)var_$48;
var_$54 = var_$53*var_$52;
var_$55 = var_$6+var_$54;
var_$58 = var_$7+var_$42;
var_$59 = (int)var_$58;
var_$63 = (ulong)4;
var_$64 = (ulong)var_$59;
var_$65 = var_$64*var_$63;
var_$66 = var_$6+var_$65;
var_$69 = *((threadgroup uint*)(var_$55));
var_$70 = *((threadgroup uint*)(var_$66));
var_$71 = var_$69+var_$70;
var_$72 = (int)var_$7;
var_$76 = (ulong)4;
var_$77 = (ulong)var_$72;
var_$78 = var_$77*var_$76;
var_$79 = var_$6+var_$78;
*((threadgroup uint*)(var_$79)) = var_$71;
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
__pc = 6; continue;
}
case 6: {
threadgroup_barrier(mem_flags::mem_threadgroup);
var_$85 = (uint)2;
var_$86 = var_$42/var_$85;
var_$87 = (uint)0;
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$86;
uint temp_4 = var_$87;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
var_$43 = temp_4;
}
__pc = 3; continue;
}
case 7: {
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$42;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$42 = temp_3;
}
__pc = 6; continue;
}
case 8: {
var_$94 = (uint)0;
var_$95 = var_$7 == var_$94;
if (var_$95){
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
var_$2 = temp_0;
var_$6 = temp_1;
}
__pc = 9; continue;
}else{
{
}
__pc = 11; continue;}
}
case 9: {
var_$97 = (int)0;
var_$101 = (ulong)4;
var_$102 = (ulong)var_$97;
var_$103 = var_$102*var_$101;
var_$104 = var_$6+var_$103;
var_$107 = *((threadgroup uint*)(var_$104));
var_$109 = nautilus_blockIdx.x;
var_$110 = (int)var_$109;
var_$114 = (ulong)4;
var_$115 = (ulong)var_$110;
var_$116 = var_$115*var_$114;
var_$117 = var_$2+var_$116;
*((device uint*)(var_$117)) = var_$107;
{
}
__pc = 10; continue;
}
case 10: {
return;
}
case 11: {
{
}
__pc = 10; continue;
}
case 12: {
{
device uchar* temp_0 = var_$2;
threadgroup uchar* temp_1 = var_$6;
uint temp_2 = var_$7;
uint temp_3 = var_$12;
var_$2 = temp_0;
var_$6 = temp_1;
var_$7 = temp_2;
var_$12 = temp_3;
}
__pc = 2; continue;
}
}
}
}

