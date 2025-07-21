FROM archlinux:base-devel

MAINTAINER singlerr <singlerr@naver.com>

RUN pacman -Syu --noconfirm
RUN pacman -Sy gobject-introspection gtk-doc libtool autoconf automake glade libgirepository git --noconfirm
RUN git clone https://github.com/aluntzer/gtknodes.git && cd gtknodes && ./autogen.sh && ./configure && make && make install
RUN pacman -Sy cmake gtk4 meson flex bison git libadwaita glib2 --noconfirm
