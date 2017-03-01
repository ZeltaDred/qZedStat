# QZedStat

Qt-based directory statistics:

   QDirstat is  copyright 2015 Stefan Hundhammer <Stefan.Hundhammer@gmx.de> )
   portions of  qzedStat (c) 2016 Douglas Osborn <douglas.osborn@dmail.dixie.edu>

Target Platforms: Linux, BSD, Unix-like systems

License: GPL V2

Updated: 2016-08-11


## Overview

QZedStat is a fork of the QDirStat ((c) 2015 Stefan Hundhammer <Stefan.Hundhammer@gmx.de> ) program
with a slightly different focus.   It leverages many of the existing features of qDirStat (Directory scanning,
file type identification by filename suffix, graphical representaion of  file tree structure) to drive source 
code  analysis tools.

The cleanup features of qDirStat have been removed.  Since full file tree access on *nix systems generally
requires elevated permissions, it was deemed  safer to disable these features  than to attempt to add safety checks
to all the possible cleanup actions that can be configured in the original  program.

The Mime configuration categories have been 'hijacked' to segregate and identify source code files related to particular
programming languages.  The  file suffix defaults that are hard coded are the result of an attempt to maintain
some compatability  with some of the other  open-source code analysis utilities available.


##  Under Construction   .....

The remainder of this page is currently under construction...   references  to the old program  will be replaced  
as new ones come available.


## Screenshots

![Main Window Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-main-win.png)

_Main window screenshot - notice the multi-selection in the tree and the treemap_


![MIME Categories Configuration Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-mime-config.png)

_Screenshot of MIME category configuration where you can set the treemap colors
for different file types (MIME types), complete with a real treemap widget as
preview._

![Exclude Rules Configuration Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-exclude-rules-config.png)

_Screenshot of the exclude rules configuration where you can define rules which
directories to exclude when reading directory trees._

-----------------------

![Tree Column Configuration Screenshot]
(https://github.com/shundhammer/qdirstat/blob/master/screenshots/QDirStat-column-config.png)

_Context menu of the tree header where you can configure the columns._


## Current Development Status

**Beta release: V1.0**

- 2016-08-10

  - Forked Release 1.0 of qDirStat after testing latest commit to fix qDirStat issue #22

    Cache files containing the root file system would not display correctly or
    segfault under certain conditions. This is now fixed.

  
  
