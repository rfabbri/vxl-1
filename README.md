# LEMSVPE: LEMS Vision Programming Environment

LEMS VPE (the LEMS Vision Programming Environment) is a monolithic superproject (a.k.a.
monorepo) for tightly
coupled projects based on [VXL](http://vxl.sourceforge.net) and
[VXD](http://github.com/rfabbri/vxd).  It bundles VXL, VXD, LEMSVXL, project code and
related utilities into a unified programming environment, making the setup more
homogeneous among a team. Advantages:

- **No dependency mess:** everyone in the team uses the same VXL and VXD versions when working on master.
- **Plain old Git:** most of the time, updating to/from LEMSVXL, VXL or VXD within VPE is tracked in VPE.
  Only the VPE team sees these changes, uniformly.
- **Separate repositories for upstream sharing:** subrepos are simultaneously kept in their own upstream repositories.
- **Harder to break things:** Propagating changes to/from upstream subrepos
  requires a specific git procedure; this creates a buffer between LEMSVPE and
  upstream LEMSVXL/VXL/VXD.  While it is seamless to share VXL or VXD changes with the
  team through LEMSVPE, one has to be disciplined to share with upstream, which
  should only occur when needed, with proper branches and code quality.
- **Promoting across LEMSVXL/VXL/VXD is tracked:** If you promote code across the
  different VXL-based projects, these operations will get documented and tracked
  as commits within VPE.

The basic idea is to replicate a repo hierarchy most VXD developers would have:
```
    lemsvpe/
      vxl
      vxd
      lemsvxl
      scripts
      vxl-bin
      vxd-bin
      lemsvxl-bin
```
Within `scripts/` one can find general VXL development scripts, such as scripts to aid
switching between build '-bin' and source folders, and general developer
scripts for searching code, configuring your Ubuntu or Mac OS system for
programming, etc.
  
The technique for building LEMSVPE is a variant of *git subtree*, mainly for the
above advantages. If we had many submodules, git submodules would be used.

## First steps: bootstrapping the environment

First, ask to have access to the LEMSVPE repository. Currently, it resides as a
private repository on Bitbucket:
```
git clone git@bitbucket.org:rfabbri2/lemsvpe.git
cd lemsvpe/
```

### If you just want to build and use LEMSVPE

Run the following script within lemsvpe:
```
./setup-for-use
```
This will create symlinks for vxl-bin and vxd-bin

### If you want to develop VPE-based project heavily
Run the following script within vpe:
```
./setup-for-development
```
This will create useful `vxl-bin`, `vxd-bin` and `lemsvxl-bin` symlinks,
establish useful remotes and branches.

### A tour of VPE
Once bootstrapped for development, you will get the following files

```bash
doc/                        # Other possible workflows for LEMSVPE, maintenance, etc

scripts/devsetup/           # Scripts used by ./setup-for-development

scripts/utils/              # Utilities for VXL development, templates, shell/editor config

vxl/                        # VXL folder tracked within VPE
vxl-bin -> vxl-bin-dbg      # Default VXL build folder pointing to possible debug version
vxl-bin-dbg/                # VXL build folder with debug flags
vxl-bin-rel/                # VXL build folder without debug flags.
vxl-orig/                   # Original VXL as a separate repository (mostly for history)

vxd/                        # VXD folder tracked within VPE
vxd-bin -> vxd-bin-dbg      # Default VXL build folder pointing to possible debug version
vxd-bin-dbg/                # VXD build folder with debug flags
vxd-bin-rel/                # VXD build folder without debug flags.
vxd-orig/                   # Original VXD as a separate repository (mostly for history)

lemsvxl/                        # LEMSVXL folder tracked within VPE
lemsvxl-bin -> lemsvxl-bin-dbg  # Default VXL build folder pointing to possible debug version
lemsvxl-bin-dbg/                # LEMSVXL build folder with debug flags
lemsvxl-bin-rel/                # LEMSVXL build folder without debug flags.
lemsvxl-orig/                   # Original LEMSVXL as a separate repository (mostly for history)
```

You will also have remotes and branches setup for you

```bash
git remote
```

```
    origin git@bitbucket.org:rfabbri2/lemsvpe.git
    vxl	https://github.com/vxl/vxl.git
    vxd	https://github.com/rfabbri/vxd.git
    lemsvxl	git@visionserver.lems.brown.edu:kimia_group/lemsvxl.git (fetch)
    utils	git://git.code.sf.net/p/labmacambira/utils
```

```bash
git branch
```

```
  * master
    utils-master
    vxd-master
    vxl-master
```

Each of these branches point to the last commit from the remote that VPE merged
into master. For instance, to see what vxl master actually has, `git fetch` and
then inspect the `vxl/master` branch instead of `vxl-master`.

## Building VPE

### 1. Install dependencies for VXL

#### Ubuntu Linux
The following command installs many of the required packages for building VXL.

```bash
  sudo sh scripts/utils/configure-ubuntu-for-programming
```

#### Mac OS
The following website has information on configuring your Mac OS for
programming: http://wiki.nosdigitais.teia.org.br/Mac


### 2. Compile VXL
```bash
  cd ./vxl-bin
  ccmake -C ../config/DefaultVXL.cmake ../vxl   # loads default CMake config for this project
  # press 'c' (configure) multiple times until 'g' (generate) appears
  cd core
  make   # use mymake from scripts/utils/vxl to run it from from both vxl-bin and vxl

  # if success, compile as much of vxl as you can

  cd ..         # now we're in vxl-bin
  make -j9 -k   # compile in parallel and keep going past errors

  # Don't worry about errors at this point. We will not use everything.
```

### 3. Compile VXD
```bash
  cd ../vxd-bin
  ccmake -C ../config/DefaultVXD.cmake ../vxd   # loads our default CMake config for this project
  make -j9 -k
  # you can try compiling core/basic libs first, in case you get errors, as we
  # did above
```
### 4. Compile LEMSVXL
```bash
  cd ../lemsvxl-bin
  ccmake  -C ../config/DefaultLEMSVXL.cmake ../lemsvxl

  cd basic/
  make

  # For any project that uses edge detection, you can try building the edge
  # stuff first
  cd ../seg/dbdet/tests
  make

  cd ../algo/tests        # dbdet/algo/tests
  make

  # $LEMSVPE will be set if you followed scripts/devsetup/tips
  cd $LEMSVPE/contrib/edge_det
  make

  # now build the rest
  cd $LEMSVPE
  make -j9 -k
```

For further information on building each of these libraries and the best CMake flags to
use, see http://wiki.nosdigitais.teia.org.br/VXL.

## Scripts to help with sourcecode

Follow the tips in `scripts/devsetup/tips` closely. 

### Put `scripts/utils` in your PATH

I recommend having a ~/bin folder in your PATH for your personal scripts,
then issuing:

```
cd scripts/utils/vxl
ln -s $PWD/* ~/bin
```

### Switching between builds

For example, you are working in `vgl/algo` but would like to switch
to a bin folder in order to debug an executable.
```bash
pwd           # we are in vpe/vxl/core/vgl/algo
sw            # switches path between vxl source and vxl-bin
pwd           # we are in vpe/vxl-bin/core/vgl/algo
sw
pwd           # we are in vpe/vxl/core/vgl/algo
```

This requires a `.bash_profile` line described in `scripts/devsetup/tips`.

### Make anywhere with mymake

```bash
pwd           # we are in vpe/vxl/core/vgl/algo
mymake        # makes in vpe/vxl-bin
sw            # switches path between vxl source and vxl-bin
mymake        # works the same if you are already in vxl-bin
```

In my system, I have set `alias m='mymake'`.

### Cmake anywhere with cm
```bash
pwd           # we are in any folder, say, vpe/vxd/contrib/brld/bmvgd/bmcsd
cm            # runs cmake as if you went to vpe/vxd-bin
              # similarly in any folder of vxl or lemsvxl
```

### CD Path

If you setup your CDPATH per `scripts/devsetup/tips`,
you can get to any folder from anywhere, for instance:
```bash
cd vgl/algo
cd vpgl
cd vxl
cd vxd
cd seg/dbdet
```

### Switching builds between Debug/Release

```
ls -ld vxl-bin vxd-bin lemsvxl-bin

    vxd-bin -> vxd-bin-dbg
    vxl-bin -> vxl-bin-dbg
    lemsvxl-bin -> lemsvxl-bin-dbg
```
```
switchbuild rel
```

Will relink all build folders to release:

```
ls -ld vxl-bin vxd-bin

    vxd-bin -> vxd-bin-rel
    vxl-bin -> vxl-bin-rel
    lemsvxl-bin -> lemsvxl-bin-rel
```

It is up to the programmer to configure CMAKE to be consistent with DBG or REL naming.
The programmer is by no means tied to these folder names, but the scripts assume
this convention.

You will want relink by hand if you have different builds with different compile
flags. The following command is equivalent to `switchbuild rel`

```bash
rm vxl-bin vxd-bin lemsvxl-bin
ln -s vxl-bin-rel vxl-bin
ln -s vxd-bin-rel vxd-bin
ln -s lemsvxl-bin-rel lemsvxl-bin
```

I myself have something like this:
```
vxl-bin -> vxl-bin-clang-libstdcxx-c11-dbg-new
```

As long as you use the `vxl-bin/vxd-bin/lemsvxl-bin` symlinks, you are good to go with the
`sw` and `mymake` scripts.

### Inspecting history

- Currently, `git log --follow` doesn't work for sub folders to trace down to
  the original repository. This means that, to see all commits from VXL that are
  outside of VPE, you will need to do something like this:

```bash
cd vxl-orig
gitk --all
```

### Code searching with tags

See `scripts/devsetup/tips`.

## Use a recent Git
Always use Git version >= 2 when working with monorepos.


## Workflow Usage Patterns

### LEMSVPE
#### Basic usage (99% of the time)
Edit pattern

  - heavy edits to LEMSVXL
  - moderate edits to VXD
  - small edits or tweaks to VXL

Share pattern
  
  - LEMSVXL edits and pushes done in feature branches (eg, `curve-cues`),
    merged often into `master`
  - VXD edits and pushes done in agreed upon feature branches (eg, `vxd-curve-cues`),
    merged often into `master`
  - VXL small edits shared on VXL branches inside LEMSVPE (prefix the branches with vxl-)

New collab working with the team needs to know

  - Most of the time: nothing. The master branch already has proper VXD and VXL merged in.
  - If new collab wants to work on a feature, he needs to know the feature
    branch to work on LEMSVPE

#### About once a week
  - Integrate to LEMSVXL and VXD upstream done by more experienced/more active peer

#### About once a month
  - Pull changes from VXL and integrate into LEMSVPE

#### Very rare
  - Integrate to VXL upstream done by more experienced/more active peer
  - Useful changes to programming environment from the original VPE upstream can be merged/pushed
  
## Subtree maintenance for LEMSVPE
This is an improvement to the alternative process proposed in a [stackoverflow answer](http://stackoverflow.com/questions/10918244/git-subtree-without-squash-view-log/40349121#40349121).

For the curious, the procedure we used to create the VPE monorepo initially is
in the file [additional-maintenance](./doc/additional-maintenance.md). 

### Pulling in changes from VXL/VXD/LEMSVXL/UTILS
```bash
# do it in steps to make sure whats going on
git fetch vxl
# examine the commits (vxl-master tracks the last merge from vxl/master):
# gitk vxl-master vxl/master
# Optional:
# git branch vxl-master vxl/master # create optional branch if you're fetching 
                                   # non-master branch, else use vxl-master normally
git checkout master
git branch -D vxl-master-merge-old # forget previous point VXL was merged into LEMSVPE
git branch -m vxl-master-merge vxl-master-merge-old # store current VXL merge point
git checkout -b vxl-master-merge # store current merge point (current master)
# merges vxl/master into vxl-master-merge
# TO BE CAREFUL TODO: use master-vxl-merge below. vxl-master below seems to have no effect
git merge -s recursive vxl-master-merge -Xsubtree=vxl vxl/master    # optional branch vxl/anybranch
# git merge -s recursive utils-master-merge -Xsubtree=scripts/utils utils/master   # for utils

# Check that vxl-master-merge has the commits as you want them
# If necessary, checkout vxl-master-merge and see if the files are in the right subfolder
# If the changes are substantial, compile first before merging below!
git checkout master
git merge vxl-master-merge  # optional branch
# Update VPE upstream

git co vxl-master
git merge --ff-only vxl/master  # ff-only is to ensure vxl-master is really tracking vxl/master
git co master

# When updating 'utils', there's no utils-orig; its not central to LEMSVPE
cd vxl-orig
git checkout vxl-master
git pull vxl master
cd ..

git push origin master
git push origin vxl-master
git push origin vxl-master-merge
```

```bash
----------------------------
# PS you could also just merge directly! (be careful)
# git fetch vxl
# git merge -s recursive -Xsubtree=vxl vxl/master
```

Hint: try this with a smaller subrepo project, such as scripts/utils, to get the
hang of things before attempting to update vxl. Also, make sure updating VXL
won't break the work of your peers, so do it when the changes look harmless,
and when you have your test suite up and running.

### Pushing to VXL/VXD/LEMSVXL upstream from VPE

#### 1. Make edits to VXL/VXD/LEMSVXL in an organized way
```bash
# Edit vxl/ normally (best to organize your commits in a separate branch)
#
# eg:  
echo '// test' >> vxl/CMakeLists.txt   # an existing VXL file is edited
#
# if we want that change to be backported to VXL, we prepend TO VXL to the
# commit and (preferably) keep it in a separate branch:

git ci -am "VPE->VXL: cmakelists"  # this message shows up on upstream

# Or, if we forgot, we can tag the commit "TO-VXD"
```
Keep doing other commits anywhere in the tree.  When backporting, we have to
cherry-pick or rebase; if there are commits that touch a subfolder but which you
do not want to see, you will have to cherry-pick by hand. Otherwise, you can let
rebase pick the commits that touch the subtree. 

When you rebase or cherry-pick commits done freely on lemsvpe, three things
may occur:

1. Commits that touch only the VXL subproject will be kept cleanly
2. Commits that touch only files outside the VXL subproject will not show up. If
   you are rebasing, these are silently ignored. If you are cherry-picking this
   type of commit, an error will occur telling that empty commits are not
   allowed (future Git versions may have git cherry-pick --skipt-empty)
3. Commits that have changes to both VXL subproject and outside of it will be
   kept with the exact same message, but with only the relevant changes/files
   kept, the rest being cleanly ignored
4. Merge commits are cleanly accounted for, but you must ignore the latest merge
   commit you did to the monorepoo master branch coming from the VXL subproject
   (the commands below do this).
5. Renames to VXL are cleanly rewritten as creations


#### 2a. Rebase
```bash
git fetch vxd
git co master       # or use an earlier branch up to which you want to integrate upstream
git co -b vxd-integration
git branch vxd-master-integration vxd-master   # so we don't mess up vxd-master
# Take note of the latest merge you did from VXD to the monorepo's master, you
# need to skip it

# Rebase from vxd-master all the way up to the monorepo's master, picking and
# adapting the relevant commits automatically
git rebase -s subtree -Xsubtree=vxd --onto vxd-master-integration vxd-master-merge vxd-integration

# Transplant initial-ref to vxd-integration's monorepo commits adapted on top of
# vxd-master-integration, turning into subrepo commits
# Note that initial-ref must skip the last merge
git rebase -s subtree -Xsubtree=vxd --onto vxd-master-integration initial-ref vxd-integration

# If there is a merge commit with conflict, I had to:
git cherry-pick -x --strategy=subtree -Xsubtree=vxd/ -m 2 SHA1OFMERGE
# where for -m I tried 1 and 2 until I got it right. Triple-checked the tree
state is the same.

# check from git log or gitk if that generates a commit with the wrong prefix,
# if so, remove the branch and redo
# If all worked, clean up
git branch -D vxd-master-integration
```

#### 2b. Cherrypick
```bash
git fetch vxl
git checkout -b vxl-integration vxl-master
git checkout vxl-integration
# merge changes from master using subtree (_adapt_ to pick the commits you want)

git cherry-pick -x --strategy=subtree -Xsubtree=vxl/ master
# git cherry-pick -x --strategy=subtree -Xsubtree=vxl/ COMMIT_SHA1

# check from git log or gitk if that generates a commit with the wrong prefix,
# if so, undo the commit by resetting HEAD and give up, start again.
#
# --strategy=subtree (-s means something else in cherry-pick) also helps to make sure
# changes outside of the subtree (elsewhere in container code) will get quietly
# ignored. 

# use '-e' flag to cherry-pick to edit the commit message before passing upstream
# you may want to say something about it if it was a move from another package
#
# the -x in these commands annotate the commit message with the SHA1 of VPE
# Use the following to make sure files outside of the subtree (elsewhere in container code) 
# will get quietly ignored. This may be useful when cherypicking a rename, since move is rm+add

# Or, if you organized your VXL commits directly into eg vxl-integration, just rebase:

git branch master-reb master # master or any other branch tip to rebase
git rebase -s subtree -Xsubtree=vxl --onto vxl-integration feature-in-progress master-reb
# git rebase -s subtree -Xsubtree=vxd --onto vxd-integration vxd-additions-begin  vxd-additions
git checkout vxl-integration
git merge master-reb
```

#### 3. Push edits to VXL/VXD/LEMSVXL

```bash
# double-check your future vxl-master commits will look good and linear

git co vxl-master
git merge vxl-integration
git push origin vxl-master # to push the vxl-master branch to toplevel VPE
git push vxl HEAD:master
git branch -D master-reb

# you may want to tag master that you've done all integration up to here
git co master
git tag -d integrated-VXL 
git push origin :refs/tags/integrated-VXL
git tag -a integrated-VXL -m "Integrated all commits touching VXD up to this point."
git push origin --tags
```

Rebase is nice, since rebasing interactively means that you have a chance to
edit the commits which are rebased (inserting move-related info such as the
origin sha1 etc). You can reorder the commits, and you can
remove them (weeding out bad or otherwise unwanted patches).

## Internal projects based on VPE 

For creating a new internal project based on VPE (possibly private),
similar to LEMSVPE, or for migrating an existing VXL/VXD internal project to a
workflow based on VPE (as in LEMSVXL), see [Internal Projects and
VPE](./doc/internal-projects.md).  You may want to do this in case your internal
project is tightly coupled with VXL/VXD, and will be constantly moving code
to/from these packages.
