#!/usr/bin/python2

print("run_igen")

import os
import sys

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

arg_sep_index = sys.argv.index("--")
argv = sys.argv[1:arg_sep_index]
argv_rest = sys.argv[arg_sep_index + 1:]

G.debug = os.environ.get("DEBUG", False)
G.do_force = os.environ.get("FORCE", False) or "--force" in argv
G.do_check = G.do_force or os.environ.get("CHECK", False) or "--check" in argv
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

def source_path(path):
	return os.path.join("src/togo", path)

class Interface:
	def __init__(self, spec):
		self.slug = spec["slug"]
		self.header = source_path(spec["header"])
		self.source = source_path(spec["source"])
		self.gen_path = source_path(spec["gen_path"])
		self.namespace = spec["namespace"]
		self.doc_group = spec["doc_group"]

		self.group = None
		self.data = None
		self.data_hash = None

		cache = G.cache.get(self.source, {})
		self.check_time = cache.get("check_time", 0)
		self.source_time = mtime(self.source)
		# self.gen_time = mtime(self.gen_path)
		self.needs_check = G.do_check or self.check_time < self.source_time

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
				cursor.location.file.name == self.source and
				cursor.raw_comment != None
			)
		def post_filter(function):
			return (
				function.xqn == self.namespace or
				has_annotation(function.cursor, "igen_interface")
			)
		self.group = igen.collect(
			self.source, G.clang_args,
			pre_filter = pre_filter,
			post_filter = post_filter,
		)
		self.data = G.template.render_unicode(
			group = self.group,
			interface = self,
		).encode("utf-8", "replace")
		self.data_hash = not G.do_force and hashlib.md5(self.data).digest() or None
		self.needs_build = G.do_force or self.data_hash != self.gen_hash
		self.check_time = self.source_time

	def write(self):
		with open(self.gen_path, "w") as f:
			f.write(self.data)

	def cache(self):
		return {
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

	print("\ncheck: %s -> %s" % (i.source, i.gen_path))
	i.load()
	if G.debug:
		for f in i.group.funcs:
			print("  %s" % f.signature_fqn())

	if not i.needs_build:
		continue
	print("writing")
	i.write()
	i.link_doc()

G.cache = {}
for i in G.interfaces.values():
	G.cache[i.source] = i.cache()

with open(G.F_CACHE, "w") as f:
	json.dump(G.cache, f)
