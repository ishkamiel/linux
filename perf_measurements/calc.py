#!/usr/bin/env python

import math
import pandas
import numpy
import sys
import os.path

def get_not_nan(arr):
    return arr[~numpy.isnan(arr)]

def print_memcpy_line(label, arr, select):
    mpxk = arr[arr['type'] == 'mpxk'];
    kasan = arr[arr['type'] == 'KASAN'];
    base = arr[arr['type'] == 'base'];

    base_mean = numpy.mean(get_not_nan(base[select]))
    base_std = numpy.std(get_not_nan(base[select]))
    kasan_mean = numpy.mean(get_not_nan(kasan[select]))
    kasan_std = numpy.std(get_not_nan(kasan[select]))
    mpxk_mean = numpy.mean(get_not_nan(mpxk[select]))
    mpxk_std = numpy.std(get_not_nan(mpxk[select]))

    kasan_mean_diff = kasan_mean - base_mean
    kasan_mean_std = math.sqrt(math.pow(base_std, 2) + math.pow(kasan_std, 2))
    mpxk_mean_diff = mpxk_mean - base_mean
    mpxk_mean_std = math.sqrt(math.pow(base_std, 2) + math.pow(mpxk_std, 2))

    kasan_per = (kasan_mean_diff / base_mean) * 100
    mpxk_per = (mpxk_mean_diff / base_mean) * 100

    print "%12s: %11.1f (%7.2f) "\
            "%11.1f (%7.2f) %11.1f (%6.2f%%) (%7.2f) " \
            "%11.1f (%7.2f) %11.1f (%6.2f%%) (%7.2f)" % (
            label,
            base_mean, base_std,
            kasan_mean, kasan_std, kasan_mean_diff, kasan_per, kasan_mean_std,
            mpxk_mean, mpxk_std, mpxk_mean_diff, mpxk_per, mpxk_mean_std)

def comp_size_memcpy(arr, size):
    print "%5dB size   %21s %43s %43s" % (size, "base", "kasan", "mpxk")

    # print_memcpy_line("legacy", arr[arr['size'] == size], 'timea1')
    print_memcpy_line("got_bounds", arr[arr['size'] == size], 'timeb1')
    print_memcpy_line("load_bounds", arr[arr['size'] == size], 'timec1')



##############################################################
# START SCRIPT
#


if (len(sys.argv) < 2):
    print "Need to supply data file basename"
    sys.exit()

kmalloc_fn = sys.argv[1] + '.kmalloc.csv'
memcpy_fn = sys.argv[1] + '.memcpy.csv'

if (not (os.path.isfile(kmalloc_fn) and os.path.isfile(memcpy_fn))):
    print("Cannot find data files")
    sys.exit()

kmalloc = pandas.read_csv(kmalloc_fn)
memcpy = pandas.read_csv(memcpy_fn)

memcpy['timea'] = memcpy['timea'].replace('X', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
memcpy['timeb'] = memcpy['timeb'].replace('X', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
memcpy['timec'] = memcpy['timec'].replace('X', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
memcpy['timea1'] = memcpy['timea1'].replace('#VALUE!', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
memcpy['timeb1'] = memcpy['timeb1'].replace('#VALUE!', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
memcpy['timec1'] = memcpy['timec1'].replace('#VALUE!', '0.0').astype(numpy.float).replace(0.0, numpy.nan)
# [memcpy['timeb'] == 'X'] = '0.0'

# fixup(memcpy.timea)
# fixup(memcpy.timeb)
# fixup(memcpy.timec)

# comp_size_memcpy(memcpy, 4)
comp_size_memcpy(memcpy, 256)
comp_size_memcpy(memcpy, 65536)
# comp_size_memcpy(memcpy, 1048576)
