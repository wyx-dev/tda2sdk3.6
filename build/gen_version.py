#!/usr/bin/python
#########################################################################
# File Name: gen_version.py
#########################################################################
'''
Run this scripts to generate "version.h"
Configure with tag and extra info
'''

import os, sys
from commands import getstatusoutput
import argparse
import re

os.chdir(os.path.join(os.path.dirname(__file__), "../"))


def safe_get_message(cmd):
    ret, st = getstatusoutput(cmd)
    if ret:
        print st
        sys.exit(ret)
    return st


def get_default_message():
    info = "%s:%s" % (safe_get_message("git describe --dirty"),
                      safe_get_message("git log -1 --pretty=format:%s"))
    tag = safe_get_message('git describe --match "v?.?.?"')
    tag = re.search("v[0-9]*.[0-9]*.[0-9]*", tag).group()[1:]
    return info, tag


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("-i", "--info", default=None, help="brief info of package")
    p.add_argument("-t", "--tag", default=None, help="version, such as 1.0.2")
    p.add_argument("-o",
                   "--output",
                   default="apps/src/hlos/adas/src/usecases/hal_version/hal_version.h",
                   help="output file")
    args = p.parse_args()

    ouf = args.output
    default_info, default_tag = get_default_message()
    args.info = args.info or default_info
    args.tag = args.tag or default_tag

    with open(ouf, 'w') as ouf:
        ouf.write("/******************************************************************************\n\n")
        ouf.write("Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.\n\n")
        ouf.write("******************************************************************************/\n\n")
        ouf.write("#ifndef __HAL_VERSION_H__\n#define __HAL_VERSION_H__\n\n")
        ouf.write('#ifdef __cplusplus\nextern "C" {\n#endif // __cplusplus\n\n')
        ouf.write("#define HAL_VSDK_MAJOR    3\n")
        ouf.write("#define HAL_VSDK_MINOR    6\n")
        v1, v2, v3 = tuple(args.tag.split('.'))
        ouf.write("#define HAL_VERSION_MAJOR    %s\n" % v1)
        ouf.write("#define HAL_VERSION_MINOR    %s\n" % v2)
        ouf.write("#define HAL_VERSION_END      %s\n" % v3)
        ouf.write("\n")        
        ouf.write("void hal_get_version(void);\n")
        ouf.write("\n")
        ouf.write("#ifdef __cplusplus\n}\n")
        ouf.write("#endif // __cplusplus\n")
        ouf.write("\n")
        ouf.write("#endif //__HAL_VERSION_H__\n")
