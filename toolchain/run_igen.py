#!/usr/bin/python2

print("run_igen")

import os
import sys

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

G.F_USERS = "toolchain/igen_users"
G.F_TEMPLATE = "toolchain/igen_interface.template"
G.debug = os.environ.get("DEBUG", False)

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

		self.source_time = mtime(self.source)
		self.gen_time = mtime(self.gen_path)
		self.needs_rebuild = self.gen_time < self.source_time
		self.group = None

		# Make sure the file exists, since we're parsing the hierarchy that
		# #includes it. Due to -fsyntax-only this doesn't cause issues, but
		# it's better to be on the safe side.
		if not os.path.isfile(self.gen_path):
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

	def build(self):
		def pre_filter(cursor):
			return cursor.location.file.name == i.source
		def post_filter(function):
			return (
				function.xqn == i.namespace or
				has_annotation(function.cursor, "igen_interface")
			)
		self.group = igen.generate(
			self.source, self.gen_path, G.clang_args, G.template,
			pre_filter = pre_filter,
			post_filter = post_filter,
			userdata = self,
		)

arg_sep_index = sys.argv.index("--")
argv = sys.argv[1:arg_sep_index]
argv_rest = sys.argv[arg_sep_index + 1:]

flag_exclusions = {
	"-MMD",
	"-MP",
}
G.clang_args = [
	"-fsyntax-only",
	"-DIGEN_RUNNING",
] + [a for a in argv_rest if a not in flag_exclusions]

G.template = Template(filename = G.F_TEMPLATE)

#if G.debug:
#	print("clang_args: %r" % G.clang_args)

G.interfaces = {}
with open(G.F_USERS, "r") as f:
	users = json.load(f)["users"]
	for spec in users:
		i = Interface(spec)
		G.interfaces[i.slug] = i

force = "--force" in argv or os.environ.get("FORCE", False)

for i in G.interfaces.values():
	if not force and not i.needs_rebuild:
		continue
	i.build()
	i.link_doc()
	if G.debug:
		print("")
