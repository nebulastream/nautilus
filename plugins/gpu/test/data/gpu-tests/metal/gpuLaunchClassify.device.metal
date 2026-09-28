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
ulong var_$13;
ulong var_$14;
ulong var_$15;
device uchar* var_$16;
uint var_$17;
uint var_$18;
bool var_$19;
uint var_$21;
ulong var_$22;
ulong var_$23;
ulong var_$24;
device uchar* var_$25;
ulong var_$33;
ulong var_$34;
ulong var_$35;
device uchar* var_$36;
uint var_$37;
uint var_$38;
bool var_$39;
uint var_$41;
ulong var_$42;
ulong var_$43;
ulong var_$44;
device uchar* var_$45;
ulong var_$50;
ulong var_$51;
ulong var_$52;
device uchar* var_$53;
uint var_$54;
uint var_$55;
bool var_$56;
uint var_$58;
ulong var_$59;
ulong var_$60;
ulong var_$61;
device uchar* var_$62;
uint var_$67;
ulong var_$68;
ulong var_$69;
ulong var_$70;
device uchar* var_$71;
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
uint temp_0 = var_$10;
device uchar* temp_1 = var_$2;
device uchar* temp_2 = var_$1;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
__pc = 1; continue;
}else{
{
}
__pc = 9; continue;}
}
case 1: {
var_$13 = (ulong)4;
var_$14 = (ulong)var_$10;
var_$15 = var_$14*var_$13;
var_$16 = var_$4+var_$15;
var_$17 = (uint)200;
var_$18 = *((device uint*)(var_$16));
var_$19 = var_$18 > var_$17;
if (var_$19){
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
__pc = 2; continue;
}else{
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
device uchar* temp_2 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
__pc = 4; continue;}
}
case 2: {
var_$21 = (uint)3;
var_$22 = (ulong)4;
var_$23 = (ulong)var_$10;
var_$24 = var_$23*var_$22;
var_$25 = var_$5+var_$24;
*((device uint*)(var_$25)) = var_$21;
{
}
__pc = 3; continue;
}
case 3: {
return;
}
case 4: {
var_$33 = (ulong)4;
var_$34 = (ulong)var_$10;
var_$35 = var_$34*var_$33;
var_$36 = var_$4+var_$35;
var_$37 = (uint)100;
var_$38 = *((device uint*)(var_$36));
var_$39 = var_$38 > var_$37;
if (var_$39){
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
__pc = 5; continue;
}else{
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
device uchar* temp_2 = var_$4;
var_$10 = temp_0;
var_$5 = temp_1;
var_$4 = temp_2;
}
__pc = 6; continue;}
}
case 5: {
var_$41 = (uint)2;
var_$42 = (ulong)4;
var_$43 = (ulong)var_$10;
var_$44 = var_$43*var_$42;
var_$45 = var_$5+var_$44;
*((device uint*)(var_$45)) = var_$41;
{
}
__pc = 3; continue;
}
case 6: {
var_$50 = (ulong)4;
var_$51 = (ulong)var_$10;
var_$52 = var_$51*var_$50;
var_$53 = var_$4+var_$52;
var_$54 = (uint)0;
var_$55 = *((device uint*)(var_$53));
var_$56 = var_$55 > var_$54;
if (var_$56){
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
__pc = 7; continue;
}else{
{
uint temp_0 = var_$10;
device uchar* temp_1 = var_$5;
var_$10 = temp_0;
var_$5 = temp_1;
}
__pc = 8; continue;}
}
case 7: {
var_$58 = (uint)1;
var_$59 = (ulong)4;
var_$60 = (ulong)var_$10;
var_$61 = var_$60*var_$59;
var_$62 = var_$5+var_$61;
*((device uint*)(var_$62)) = var_$58;
{
}
__pc = 3; continue;
}
case 8: {
var_$67 = (uint)0;
var_$68 = (ulong)4;
var_$69 = (ulong)var_$10;
var_$70 = var_$69*var_$68;
var_$71 = var_$5+var_$70;
*((device uint*)(var_$71)) = var_$67;
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

