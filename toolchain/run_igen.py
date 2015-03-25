#!/usr/bin/python2

print("run_igen")

import os
import sys
import time
import hashlib
import json
from mako.template import Template

IGEN_ROOT = os.environ["IGEN_ROOT"]
assert IGEN_ROOT, "IGEN_ROOT not set"

sys.path.append(os.path.join(IGEN_ROOT, "src"))

from igen.include import *
from igen import igen
from igen.util import *

os.stat_float_times(False)
cindex.Config.set_library_path(IGEN_ROOT)

G.F_TEMPLATE = "toolchain/igen_interface.template"
G.F_CACHE = "toolchain/igen_cache"
G.F_USERS = "toolchain/igen_users"

G.PASS_ANNOTATIONS = [
	"igen_interface",
	"igen_private",
]

arg_sep_index = sys.argv.index("--")
argv = sys.argv[1:arg_sep_index]
argv_rest = sys.argv[arg_sep_index + 1:]

def opt(name):
	return (
		os.environ.get(name.upper(), False) or
		("--" + name) in argv
	)

G.debug = opt("debug") != False
G.do_force = opt("force") != False
G.do_check = opt("check") != False or G.do_force
G.template = Template(filename = G.F_TEMPLATE)
G.cache = {}

if os.path.isfile(G.F_CACHE):
	with open(G.F_CACHE, "r") as f:
		G.cache = json.load(f)

flag_exclusions = {
	"-MMD",
	"-MP",
}
G.clang_args = [
	"-fsyntax-only",
	"-DIGEN_RUNNING",
] + [a for a in argv_rest if a not in flag_exclusions]

#if G.debug:
#	print("clang_args: %r" % G.clang_args)

def mtime(path):
	if os.path.exists(path):
		stat = os.stat(path)
		return stat.st_mtime
	return 0

class Source:
	def __init__(self, path):
		self.path = path
		assert os.path.exists(self.path), "source does not exist: %s" % (self.path)
		self.time = mtime(self.path)

class Interface:
	def __init__(self, spec):
		self.slug = spec["slug"]
		self.path = spec["path"]
		self.sources = [Source(path) for path in spec["sources"]]
		self.gen_path = spec["gen_path"]
		self.doc_group = spec["doc_group"]

		self.group = None
		self.data = None
		self.data_hash = None

		assert len(self.sources) > 0, "no sources for %s" % (self.gen_path)

		cache = G.cache.get(self.gen_path, {})
		self.check_time = cache.get("check_time", 0)
		self.path_time = mtime(self.path)

		self.needs_check = (
			G.do_check or
			self.check_time < self.path_time or
			True in (self.check_time < source.time for source in self.sources)
		)

		gen_exists = os.path.isfile(self.gen_path)
		self.gen_hash = None
		if self.needs_check and gen_exists:
			with open(self.gen_path, "r") as f:
				self.gen_hash = hashlib.md5(f.read()).digest()
		elif not gen_exists:
			# Make sure the file exists, since we're parsing the hierarchy
			# that #includes it. Due to -fsyntax-only this doesn't cause
			# issues, but it's better to be on the safe side.
			with open(self.gen_path, "w") as f:
				pass

	def link_doc(self):
		if not os.path.exists("doc/gen_interface"):
			os.mkdir("doc/gen_interface")
		doc_path = os.path.join(
			"doc/gen_interface",
			self.slug.replace("/", "_") + ".dox"
		)
		if os.path.exists(doc_path):
			os.remove(doc_path)
		os.symlink(os.path.join("../..", self.gen_path), doc_path)

	def load(self):
		def pre_filter(cursor):
			return (
				True in (s.path == cursor.location.file.name for s in self.sources) and (
					cursor.raw_comment != None or
					has_annotation(cursor, G.PASS_ANNOTATIONS)
				)
			)
		def post_filter(function):
			# function.xqn == self.namespace or
			function.annotations = get_annotations(function.cursor)
			function.anno_private = "igen_private" in function.annotations
			return True

		self.group = igen.Group(None)
		for source in self.sources:
			funcs = igen.parse_and_collect(source.path, G.clang_args, pre_filter, post_filter)
			self.group.add_funcs(funcs)

		self.data = G.template.render_unicode(
			group = self.group,
			interface = self,
		).encode("utf-8", "replace")
		self.data_hash = not G.do_force and hashlib.md5(self.data).digest() or None
		self.needs_build = G.do_force or self.data_hash != self.gen_hash
		self.check_time = int(time.mktime(time.localtime()))

	def write(self):
		with open(self.gen_path, "w") as f:
			f.write(self.data)

	def set_cache(self):
		G.cache[self.gen_path] = {
			"check_time" : self.check_time,
		}

G.interfaces = {}
with open(G.F_USERS, "r") as f:
	users = json.load(f)["users"]
	for spec in users:
		i = Interface(spec)
		G.interfaces[i.slug] = i

for i in G.interfaces.values():
	if not i.needs_check:
		continue

	print("\ncheck: %s" % (i.gen_path))
	for source in i.sources:
		print("  from %s" % (source.path))
	i.load()
	if G.debug and len(i.group.funcs) > 0:
		print("")
		for f in i.group.funcs:
			print("  %s" % f.signature_fqn())

	if not i.needs_build:
		continue
	print("writing")
	i.write()
	i.link_doc()

G.cache = {}
for i in G.interfaces.values():
	i.set_cache()

with open(G.F_CACHE, "w") as f:
	json.dump(G.cache, f)
