#!/bin/sh

aclocal  \
  && libtoolize --force --copy \
  && automake --add-missing --foreign --copy \
  && autoconf
