
Debian
====================
This directory contains files used to package giracoind/giracoin-qt
for Debian-based Linux systems. If you compile giracoind/giracoin-qt yourself, there are some useful files here.

## giracoin: URI support ##


giracoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install giracoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your giracoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/giracoin128.png` to `/usr/share/pixmaps`

giracoin-qt.protocol (KDE)

