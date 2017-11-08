#!/usr/bin/env python

# Copyright 2016-present Samsung Electronics Co., Ltd. and other contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

import argparse
import glob
import sys
import os
import json
from common_py import path
from common_py.system.filesystem import FileSystem as fs
from common_py.system.executor import Executor as ex
from common_py.system.platform import Platform
from check_tidy import check_tidy

TESTS=['host-linux', 'host-darwin', 'rpi2', 'nuttx', 'misc', 'no-snapshot',
       'artik10', 'artik053', 'coverity']
ROMFS_MODULES=['tests']
BUILDTYPES=['debug', 'release']

TIZENRT_REPO='https://github.com/tadziopazur/TizenRT.git'
TIZENRT_REVISION='IOTJS_20171108'
TIZNERT_BUILD_BRANCH='iotjs_build'

NUTTX_OS_REPO='https://bitbucket.org/nuttx/nuttx.git'
NUTTX_APP_REPO='https://bitbucket.org/nuttx/apps.git'
NUTTXTAG = 'nuttx-7.19'

def get_config():
    config_path = path.BUILD_MODULE_CONFIG_PATH
    with open(config_path, 'r') as f:
        config = json.loads(f.read().encode('ascii'))
    return config


def parse_option():
    parser = argparse.ArgumentParser(
         description='IoT.js pre-commit script.',
         epilog='If no arguments are given, runs full test.')
    parser.add_argument('--test', choices=TESTS, action='append')
    parser.add_argument('--buildtype', choices=BUILDTYPES, action='append')
    parser.add_argument('--buildoptions', action='store', default='',
                        help='A comma separated list of extra buildoptions')
    parser.add_argument('--skipbuild', choices=[True, False], action='store',
                        nargs='?', const=True, default=False)
    parser.add_argument('--romfs', choices=ROMFS_MODULES, action='append',
                        default=[])

    option = parser.parse_args(sys.argv[1:])
    if option.test is None:
        option.test = TESTS
    if option.buildtype is None:
        option.buildtype = BUILDTYPES
    return option


def setup_nuttx_root(nuttx_root):
    # Step 1
    fs.maybe_make_directory(nuttx_root)
    fs.chdir(nuttx_root)
    if not fs.exists('nuttx'):
        ex.check_run_cmd('git', ['clone', NUTTX_OS_REPO])
    fs.chdir('nuttx')
    ex.check_run_cmd('git', ['checkout', NUTTXTAG])
    fs.chdir('..')

    if not fs.exists('apps'):
        ex.check_run_cmd('git', ['clone', NUTTX_APP_REPO])
    fs.chdir('apps')
    ex.check_run_cmd('git', ['checkout', NUTTXTAG])
    fs.chdir('..')

    # Step 2
    config_src_dir = fs.join(path.PROJECT_ROOT, 'config',
                             'nuttx', 'stm32f4dis')
    nuttx_iotjs_appdir = fs.join(nuttx_root, 'apps', 'system', 'iotjs')
    fs.maybe_make_directory(nuttx_iotjs_appdir)
    for file in fs.listdir(fs.join(config_src_dir, 'app')):
        fs.copy(fs.join(config_src_dir, 'app', file), nuttx_iotjs_appdir)

    # Step 3: patch the nuttx OS
    fs.chdir(fs.join(nuttx_root, 'nuttx'))
    patchdir = fs.join(config_src_dir, 'patches', 'nuttx_os')
    for patchfile in sorted(fs.listdir(patchdir)):
        if (patchfile.endswith('.patch')):
            print('Applying OS patch %s/%s' % (patchdir, patchfile))
            ex.check_run_cmd('git', ['am',
                fs.join(patchdir, patchfile),
                '--committer-date-is-author-date'])
        else:
            print('Rejected file %s/%s: not a patch' % (patchdir, patchfile))

    # Step 4: configure nuttx OS & apps
    fs.chdir(fs.join(nuttx_root, 'nuttx', 'tools'))
    ex.check_run_cmd('./configure.sh', ['stm32f4discovery/usbnsh'])
    fs.copy(fs.join(config_src_dir, 'usbnshiotjs_defconfig'),
            fs.join(nuttx_root, 'nuttx', '.config'))

def build_nuttx(nuttx_root, buildtype, maketarget):
    fs.chdir(fs.join(nuttx_root, 'nuttx'))
    if buildtype == "release":
        rflag = 'R=1'
    else:
        rflag = 'R=0'
    ex.check_run_cmd('make',
                     [maketarget, 'IOTJS_ROOT_DIR=' + path.PROJECT_ROOT, rflag])

def create_test_romfs():
    if 'tests' in option.romfs:
        print("Creating test ROMFS")
        romfs_src_dir = create_romfs_srcdir()
    else:
        print("Creating empty ROMFS")
        romfs_src_dir = create_romfs_srcdir(True)

    rom_file = fs.join(path.PROJECT_ROOT, 'rom.img')
    create_romfs_image(rom_file, romfs_src_dir)
    fs.rmtree(romfs_src_dir)
    return rom_file

def setup_tizen_root(tizen_root):
    if fs.exists(tizen_root):
        fs.chdir(tizen_root)
        ex.check_run_cmd('git', ['pull'])
        fs.chdir(path.PROJECT_ROOT)
    else:
        ex.check_run_cmd('git', ['clone',
            'https://github.com/pmarcinkiew/tizen3.0_rootstrap.git',
            tizen_root])

def copy_tiznert_stuff(tizenrt_root, iotjs_dir):
    tizenrt_iotjsapp_dir = fs.join(tizenrt_root, 'apps/system/iotjs')
    iotjs_tizenrt_appdir = fs.join(iotjs_dir, 'config/tizenrt/artik05x/app')
    ex.check_run_cmd('cp',
                    ['-rfT', iotjs_tizenrt_appdir, tizenrt_iotjsapp_dir])

    iotjs_config_dir = fs.join(iotjs_dir, 'config/tizenrt/artik05x/configs')
    tizenrt_config_dir = fs.join(tizenrt_root, 'build/configs/artik053/iotjs')
    ex.check_run_cmd('cp', ['-rfT', iotjs_config_dir, tizenrt_config_dir])

def setup_tizenrt_repo(tizenrt_root):
    if fs.exists(tizenrt_root):
        fs.chdir(tizenrt_root)
        ex.check_run_cmd('git', ['fetch', 'origin'])
        fs.chdir(path.PROJECT_ROOT)
    else:
        ex.check_run_cmd('git', ['clone', TIZENRT_REPO, tizenrt_root])

    # The following two do not have to succeed
    # Checkout master, so we're not sitting on
    ex.run_cmd('git', ['--git-dir', tizenrt_root + '/.git/',
                       '--work-tree', tizenrt_root,
                       'checkout', 'master'])
    ex.run_cmd('git', ['--git-dir', tizenrt_root + '/.git/',
                       '--work-tree', tizenrt_root,
                       'branch', '-D', TIZNERT_BUILD_BRANCH])
    # This has to however
    ex.check_run_cmd('git', ['--git-dir', tizenrt_root + '/.git/',
                             '--work-tree', tizenrt_root,
                             'checkout', '-b', TIZNERT_BUILD_BRANCH,
                             TIZENRT_REVISION])
    copy_tiznert_stuff(tizenrt_root, path.PROJECT_ROOT)

def configure_trizenrt(tizenrt_root, buildtype):
    # TODO: handle buildtype (build vs release) for tizenrt build
    tizenrt_tools = fs.join(tizenrt_root, 'os/tools')
    fs.chdir(tizenrt_tools)
    ex.check_run_cmd('./configure.sh', ['artik053/iotjs'])
    fs.chdir('..')
    ex.check_run_cmd('make', ['context'])

def build_tizenrt(tizenrt_root, iotjs_rootdir, buildtype):
    fs.chdir(fs.join(tizenrt_root, 'os'))
    iotjs_libdir = iotjs_rootdir + '/build/arm-tizenrt/' + buildtype + '/lib'
    ex.check_run_cmd('make', ['IOTJS_ROOT_DIR=' + iotjs_rootdir,
                              'IOTJS_LIB_DIR=' + iotjs_libdir])

def create_romfs_image(romfs_image, source_directory):
    ex.check_run_cmd('genromfs', ['-f', romfs_image, '-d',
        source_directory, '-V', 'NuttXBootVol'])

def split_entry(entry):
    '''Validate romfs.def entry, which should be in the form os 'glob, target',
       where the target is resolved relative to romfs root directory'''
    entry = entry.strip();
    if entry == "" or entry.startswith("#"):
        return
    index = entry.rfind(' ')
    rel_target = None
    if index > 0:
        rel_target = entry[index+1:].lstrip('/')
        entry = entry[0:index]

    # The right place to handle links and such
    return rel_target, glob.glob(entry)

def create_romfs_srcdir(empty = False):
    '''Must be run in $TOPDIR'''
    srcdirobj = fs.mkdtemp()
    srcdir = str(srcdirobj)
    if empty:
        return srcdir

    with open(fs.join(path.PROJECT_ROOT, 'config', 'romfs.def')) as f:
        content = f.readlines()
    for entry in content:
        rel_target, dentry_list = split_entry(entry)
        if dentry_list:
            if rel_target and rel_target != '/':
                target = fs.join(srcdir, rel_target)
                ex.check_run_cmd('mkdir', ['-p', target], True)
            else:
                target = srcdir
            cp_args = ['-rfL']
            cp_args.extend(dentry_list)
            cp_args.append(target)
            ex.check_run_cmd('cp', cp_args)
    return srcdir

def build(buildtype, args=[]):
    fs.chdir(path.PROJECT_ROOT)
    ex.check_run_cmd('./tools/build.py', ['--buildtype=' + buildtype] + args)


if __name__ == '__main__':
    option = parse_option()
    config = get_config()
    extend_module = config['module']['supported']['extended']
    os_dependency_module = {}

    # Travis will test all implemented modules.
    for os_name in extend_module.keys():
        os_dependency_module[os_name] = \
        ['--iotjs-include-module=' + ','.join(extend_module[os_name])]

    build_args = []

    if option.buildoptions:
        build_args.extend(option.buildoptions.split(','))

    for test in option.test:
        if test == "host-linux":
            for buildtype in option.buildtype:
                build(buildtype, os_dependency_module['linux'] + build_args)

        if test == "host-darwin":
            for buildtype in option.buildtype:
                build(buildtype, os_dependency_module['darwin'] + build_args)

        elif test == "rpi2":
            for buildtype in option.buildtype:
                build(buildtype, ['--target-arch=arm', '--target-board=rpi2']
                                + os_dependency_module['linux'] + build_args)

        elif test == "artik10":
            for buildtype in option.buildtype:
                tizen_root = fs.join(path.PROJECT_ROOT, 'deps', 'tizen')
                setup_tizen_root(tizen_root)
                build(buildtype, ['--target-arch=arm',
                                '--target-os=tizen',
                                '--target-board=artik10',
                                '--compile-flag=--sysroot=' + tizen_root
                                ] + os_dependency_module['linux'] + build_args)

        elif test == "artik053":
            tizenrt_root = fs.join(path.PROJECT_ROOT, 'deps', 'tizenrt')
            if not option.skipbuild:
                for buildtype in option.buildtype:
                    setup_tizenrt_repo(tizenrt_root)
                    configure_trizenrt(tizenrt_root, buildtype)
                    build(buildtype, ['--target-arch=arm',
                                    '--target-os=tizenrt',
                                    '--target-board=artik05x',
                                    '--sysroot=' + tizenrt_root + '/os',
                                    '--jerry-heaplimit=128',
                                    '--clean',
                                    ] + os_dependency_module['tizenrt']
                                    + build_args)
                    build_tizenrt(tizenrt_root, path.PROJECT_ROOT, buildtype)
                    fs.chdir(path.PROJECT_ROOT)
            # For now only romfs=tests works
            if 'tests' in option.romfs:
                rom_file = create_test_romfs()
                romfs_image_file = fs.join(tizenrt_root, "build", "output",
                                           "bin", "romfs.img")
                ex.check_run_cmd('cp', ['-f', rom_file, romfs_image_file])
                #romfs_src_dir = create_romfs_srcdir()
                #create_romfs_image(romfs_image_file, romfs_src_dir)
                #fs.rmtree(romfs_src_dir)

        elif test == "nuttx":
            current_dir = os.getcwd()
            rom_file = create_test_romfs()
            for buildtype in option.buildtype:
                nuttx_root=fs.join(path.PROJECT_ROOT, 'deps', 'nuttx')
                setup_nuttx_root(nuttx_root)
                build_nuttx(nuttx_root, buildtype, 'context')
                build(buildtype, ['--target-arch=arm',
                                '--target-os=nuttx',
                                '--nuttx-home=' + fs.join(nuttx_root, 'nuttx'),
                                '--target-board=stm32f4dis',
                                '--jerry-heaplimit=78']
                                + os_dependency_module['nuttx'] + build_args)
                ex.check_run_cmd('cp',
                    ['-f', rom_file, fs.join(nuttx_root, 'nuttx', 'rom.img')])
                build_nuttx(nuttx_root, buildtype, 'all')
                fs.chdir(current_dir)

        elif test == "misc":
            args = []
            if os.getenv('TRAVIS') != None:
                args = ['--travis']
            ex.check_run_cmd('tools/check_signed_off.sh', args)

        elif test == "no-snapshot":
            args = []
            if os.getenv('TRAVIS') != None:
                args = ['--travis']

            build("debug", ['--no-snapshot', '--jerry-lto']
                        + os_dependency_module['linux'] + build_args)


        elif test == "coverity":
            build("debug", ['--no-check-test']
                        + os_dependency_module['linux'] + build_args)
