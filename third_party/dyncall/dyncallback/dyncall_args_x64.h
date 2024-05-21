/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_x64.h
 Description: Callback's Arguments VM - Header for x64
 License:

   Copyright (c) 2007-2022 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/


#ifndef DYNCALLBACK_ARGS_X64_H
#define DYNCALLBACK_ARGS_X64_H

#include "dyncall_args.h"
#include "dyncall_callvm_x64.h" /* reuse DCRegCount_x64 and DCRegData_x64_s */
#include "dyncall_aggregate.h"



struct DCArgs
{
  /* state */
  int64*          stack_ptr;              /* offset 0 */
  DCRegCount_x64  reg_count;              /* offset 8, size:win 4, size:*nix 8 */
#if defined(DC_WINDOWS)
  int             pad_w;                  /* alignment helper for win/x64 */
#endif
  int             aggr_return_register;   /* offset 16 */
  int             pad;                    /* offset 20 */
  DCaggr**        aggrs;                  /* offset 24 */

  /* reg data */
  DCRegData_x64_s reg_data;               /* offset 32 */
};

#endif /* DYNCALLBACK_ARGS_X64_H */

