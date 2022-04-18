#!/usr/bin/env python3

from typing import List, Tuple

import os

from os.path import dirname, abspath, join as joinpath, splitext

base_dir = dirname(abspath(__file__))
files = os.listdir(joinpath(base_dir, "testdata"))

buf: List[str] = [
"""\
fn get_testdata(fname: &str) -> std::path::PathBuf {
    let mut path = std::path::PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    path.push("testdata");
    path.push(fname);
    return path;
}
"""
]

funcs: List[Tuple[str, str]] = []

for fname in files:
    ident = fname.replace('.', '_')
    path = f"testdata/{fname}"

    leaf, ext = splitext(fname)
    format = ext[1:].upper() if ext != ".exr" else "OpenEXR"

    ident = '_'.join([ ext[1:], *leaf.split('_')[1:] ])

    funcs.append((ident, f"""
#[test]
fn {ident}() {{
    let info = imsz::imsz_from_path(get_testdata("{fname}"));
    match info {{
        Ok(info) => {{
            assert_eq!(info.format, imsz::ImFormat::{format});
            assert_eq!(info.width,  32);
            assert_eq!(info.height, 16);
        }}
        Err(error) => {{
            assert!(false, "{{}}", error);
        }}
    }}
}}
"""))

funcs.sort()
buf.extend(code for _, code in funcs)

with open(joinpath(base_dir, "tests", "ok_files.rs"), "w") as fp:
    fp.write('\n'.join(buf))
