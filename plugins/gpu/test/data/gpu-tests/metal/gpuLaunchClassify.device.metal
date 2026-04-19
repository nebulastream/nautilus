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
int var_$13;
ulong var_$17;
ulong var_$18;
ulong var_$19;
device uchar* var_$20;
uint var_$23;
uint var_$24;
bool var_$25;
uint var_$27;
int var_$28;
ulong var_$32;
ulong var_$33;
ulong var_$34;
device uchar* var_$35;
int var_$46;
ulong var_$50;
ulong var_$51;
ulong var_$52;
device uchar* var_$53;
uint var_$56;
uint var_$57;
bool var_$58;
uint var_$60;
int var_$61;
ulong var_$65;
ulong var_$66;
ulong var_$67;
device uchar* var_$68;
int var_$76;
ulong var_$80;
ulong var_$81;
ulong var_$82;
device uchar* var_$83;
uint var_$86;
uint var_$87;
bool var_$88;
uint var_$90;
int var_$91;
ulong var_$95;
ulong var_$96;
ulong var_$97;
device uchar* var_$98;
uint var_$106;
int var_$107;
ulong var_$111;
ulong var_$112;
ulong var_$113;
device uchar* var_$114;
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
var_$13 = (int)var_$10;
var_$17 = (ulong)4;
var_$18 = (ulong)var_$13;
var_$19 = var_$18*var_$17;
var_$20 = var_$4+var_$19;
var_$23 = (uint)200;
var_$24 = *((device uint*)(var_$20));
var_$25 = var_$24 > var_$23;
if (var_$25){
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
var_$27 = (uint)3;
var_$28 = (int)var_$10;
var_$32 = (ulong)4;
var_$33 = (ulong)var_$28;
var_$34 = var_$33*var_$32;
var_$35 = var_$5+var_$34;
*((device uint*)(var_$35)) = var_$27;
{
}
__pc = 3; continue;
}
case 3: {
return;
}
case 4: {
var_$46 = (int)var_$10;
var_$50 = (ulong)4;
var_$51 = (ulong)var_$46;
var_$52 = var_$51*var_$50;
var_$53 = var_$4+var_$52;
var_$56 = (uint)100;
var_$57 = *((device uint*)(var_$53));
var_$58 = var_$57 > var_$56;
if (var_$58){
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
var_$60 = (uint)2;
var_$61 = (int)var_$10;
var_$65 = (ulong)4;
var_$66 = (ulong)var_$61;
var_$67 = var_$66*var_$65;
var_$68 = var_$5+var_$67;
*((device uint*)(var_$68)) = var_$60;
{
}
__pc = 3; continue;
}
case 6: {
var_$76 = (int)var_$10;
var_$80 = (ulong)4;
var_$81 = (ulong)var_$76;
var_$82 = var_$81*var_$80;
var_$83 = var_$4+var_$82;
var_$86 = (uint)0;
var_$87 = *((device uint*)(var_$83));
var_$88 = var_$87 > var_$86;
if (var_$88){
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
var_$90 = (uint)1;
var_$91 = (int)var_$10;
var_$95 = (ulong)4;
var_$96 = (ulong)var_$91;
var_$97 = var_$96*var_$95;
var_$98 = var_$5+var_$97;
*((device uint*)(var_$98)) = var_$90;
{
}
__pc = 3; continue;
}
case 8: {
var_$106 = (uint)0;
var_$107 = (int)var_$10;
var_$111 = (ulong)4;
var_$112 = (ulong)var_$107;
var_$113 = var_$112*var_$111;
var_$114 = var_$5+var_$113;
*((device uint*)(var_$114)) = var_$106;
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

