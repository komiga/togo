#!/usr/bin/python2

print("run_igen")

import os
import sys

IGEN_ROOT = os.environ["IGEN_ROOT"]
assert IGEN_ROOT, "IGEN_ROOT not set"

sys.path.append(os.path.join(IGEN_ROOT, "src"))

from igen import interface

interface.configure(IGEN_ROOT, "togo", "tmp", "scripts/igen_interface.template")

c = interface.Collector()
c.add_groups("lib", "")
c.add_groups("tool", "tool_")
c.collect()
c.write()

interface.build(sys.argv)
