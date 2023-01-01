#pragma once

#include "global.h"
#include "xio/xio.h"

void cstr_encode(XIO *instream, XIO *outstream, size_t bufsize);
void cstr_decode(XIO *instream, XIO *outstream, size_t bufsize);

void cbytes_encode(XIO *instream, XIO *outstream, size_t bufsize);
void cbytes_decode(XIO *instream, XIO *outstream, size_t bufsize);
