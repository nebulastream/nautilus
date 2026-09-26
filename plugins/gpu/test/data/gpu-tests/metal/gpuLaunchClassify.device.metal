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
uint var_$6;
uint var_$7;
uint var_$8;
uint var_$9;
uint var_$10;
bool var_$11;
device uchar* var_$5;
device uchar* var_$4;
ulong var_$16;
ulong var_$17;
ulong var_$18;
device uchar* var_$19;
uint var_$22;
uint var_$23;
bool var_$24;
uint var_$26;
ulong var_$30;
ulong var_$31;
ulong var_$32;
device uchar* var_$33;
ulong var_$47;
ulong var_$48;
ulong var_$49;
device uchar* var_$50;
uint var_$53;
uint var_$54;
bool var_$55;
uint var_$57;
ulong var_$61;
ulong var_$62;
ulong var_$63;
device uchar* var_$64;
ulong var_$75;
ulong var_$76;
ulong var_$77;
device uchar* var_$78;
uint var_$81;
uint var_$82;
bool var_$83;
uint var_$85;
ulong var_$89;
ulong var_$90;
ulong var_$91;
device uchar* var_$92;
uint var_$100;
ulong var_$104;
ulong var_$105;
ulong var_$106;
device uchar* var_$107;
int __pc = 0;
while (true) {
switch (__pc) {
case 0: {
var_$6 = nautilus_blockIdx.x;
var_$7 = nautilus_blockDim.x;
var_$8 = var_$6*var_$7;
var_$9 = nautilus_threadIdx.x;
var_$10 = var_$8+var_$9;
var_$11 = var_$10 < var_$3;
if (var_$11){
{
device uchar* temp_0 = var_$2;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$1;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
__pc = 1; continue;
}else{
{
}
__pc = 9; continue;}
}
case 1: {
var_$16 = (ulong)4;
var_$17 = (ulong)var_$10;
var_$18 = var_$17*var_$16;
var_$19 = var_$4+var_$18;
var_$22 = (uint)200;
var_$23 = *((device uint*)(var_$19));
var_$24 = var_$23 > var_$22;
if (var_$24){
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
__pc = 2; continue;
}else{
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
__pc = 4; continue;}
}
case 2: {
var_$26 = (uint)3;
var_$30 = (ulong)4;
var_$31 = (ulong)var_$10;
var_$32 = var_$31*var_$30;
var_$33 = var_$5+var_$32;
*((device uint*)(var_$33)) = var_$26;
{
}
__pc = 3; continue;
}
case 3: {
return;
}
case 4: {
var_$47 = (ulong)4;
var_$48 = (ulong)var_$10;
var_$49 = var_$48*var_$47;
var_$50 = var_$4+var_$49;
var_$53 = (uint)100;
var_$54 = *((device uint*)(var_$50));
var_$55 = var_$54 > var_$53;
if (var_$55){
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
__pc = 5; continue;
}else{
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
device uchar* temp_2 = var_$4;
var_$5 = temp_0;
var_$10 = temp_1;
var_$4 = temp_2;
}
__pc = 6; continue;}
}
case 5: {
var_$57 = (uint)2;
var_$61 = (ulong)4;
var_$62 = (ulong)var_$10;
var_$63 = var_$62*var_$61;
var_$64 = var_$5+var_$63;
*((device uint*)(var_$64)) = var_$57;
{
}
__pc = 3; continue;
}
case 6: {
var_$75 = (ulong)4;
var_$76 = (ulong)var_$10;
var_$77 = var_$76*var_$75;
var_$78 = var_$4+var_$77;
var_$81 = (uint)0;
var_$82 = *((device uint*)(var_$78));
var_$83 = var_$82 > var_$81;
if (var_$83){
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
__pc = 7; continue;
}else{
{
device uchar* temp_0 = var_$5;
uint temp_1 = var_$10;
var_$5 = temp_0;
var_$10 = temp_1;
}
__pc = 8; continue;}
}
case 7: {
var_$85 = (uint)1;
var_$89 = (ulong)4;
var_$90 = (ulong)var_$10;
var_$91 = var_$90*var_$89;
var_$92 = var_$5+var_$91;
*((device uint*)(var_$92)) = var_$85;
{
}
__pc = 3; continue;
}
case 8: {
var_$100 = (uint)0;
var_$104 = (ulong)4;
var_$105 = (ulong)var_$10;
var_$106 = var_$105*var_$104;
var_$107 = var_$5+var_$106;
*((device uint*)(var_$107)) = var_$100;
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

