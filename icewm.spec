
Name:		icewm
Version:	1.3.8
Release:	1
Obsoletes:	icewm-common <= 1.2.2
Summary:	Fast and small X11 window manager
Group:		User Interface/Desktops
License:	LGPL
URL:		http://www.icewm.org/
Packager:       Marko Macek <Marko.Macek@gmx.net>
Source:		http://ftp.sourceforge.net/icewm/%{name}-%{version}.tar.gz
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot

%define pkgdata %{_datadir}/%{name}

%description
A lightweight window manager for the X Window System. Optimized for
"feel" and speed, not looks. Features multiple workspaces, opaque
move/resize, task bar, window list, clock, mailbox, CPU, Network, APM
status. 

%package l10n
Group:		%{group}
Summary:        Message translations for icewm
Requires:       icewm = %{version}

%description l10n
Message translations for icewm.

%package themes
Group:		%{group}
Summary:        Extra themes for icewm
Requires:       icewm > 1.2.2

%description themes
Extra themes for icewm. 

%if %{?_with_menus_gnome2:1}%{!?_with_menus_gnome2:0}

%package menu-gnome2
Group:		%{group}
Summary:        GNOME menu support for icewm (using gnome 2.x).
Requires:       icewm > 1.2.2
Requires:       gnome-libs >= 1.4

%description menu-gnome2
GNOME 1.0 menu support for icewm (using gnome 2.x).

%endif

%prep

%setup

%build
  CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
     --prefix=%{_prefix} \
     --exec-prefix=%{_exec_prefix} \
     --datadir=%{_datadir} \
     --sysconfdir=%{_sysconfdir} \
     --with-docdir=%{_docdir} \
     %{?_with_menus_gnome2:--enable-menus-gnome2} \
     %{?_with_debug:--enable-debug}
  make

%install
  make DESTDIR=$RPM_BUILD_ROOT install install-desktop
  mkdir -p $RPM_BUILD_ROOT/etc/icewm

%clean
  test -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != "/" && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc README COPYING AUTHORS CHANGES BUGS doc/*.html doc/icewm.sgml
%doc icewm.lsm

%config %{pkgdata}/keys
%config %{pkgdata}/menu
%config %{pkgdata}/preferences
%config %{pkgdata}/toolbar
%config %{pkgdata}/winoptions

%dir /etc/icewm

%dir %{pkgdata}/icons
%dir %{pkgdata}/ledclock
%dir %{pkgdata}/mailbox
%dir %{pkgdata}/taskbar
%dir %{pkgdata}/themes

%{_bindir}/*
%{pkgdata}/icons/*
%{pkgdata}/ledclock/*
%{pkgdata}/mailbox/*
%{pkgdata}/taskbar/*
%{pkgdata}/themes/icedesert/*

/usr/share/xsessions/icewm-session.desktop
/usr/share/applications/icewm.desktop

%if %{?_with_menus_gnome2:1}%{!?_with_menus_gnome2:0}

%files menu-gnome2
%defattr(-,root,root)
%{_bindir}/icewm-menu-gnome2

%endif

%files l10n
%defattr(-,root,root)
%dir %{_datadir}/locale
%{_datadir}/locale/*

%files themes
%defattr(-,root,root)
%dir %{pkgdata}/themes/nice
%{pkgdata}/themes/nice/*
%dir %{pkgdata}/themes/nice2
%{pkgdata}/themes/nice2/*
%dir %{pkgdata}/themes/gtk2
%{pkgdata}/themes/gtk2/*
%dir %{pkgdata}/themes/warp3
%{pkgdata}/themes/warp3/*
%dir %{pkgdata}/themes/warp4
%{pkgdata}/themes/warp4/*
%dir %{pkgdata}/themes/motif
%{pkgdata}/themes/motif/*
%dir %{pkgdata}/themes/win95
%{pkgdata}/themes/win95/*
%dir %{pkgdata}/themes/metal2
%{pkgdata}/themes/metal2/*
%dir %{pkgdata}/themes/Infadel2
%{pkgdata}/themes/Infadel2/*
%dir %{pkgdata}/themes/yellowmotif
%{pkgdata}/themes/yellowmotif/*

%changelog
* Sun Mar 06 2005 Jiri Slaby <jirislaby@gmail.com>
1.2.20 - Repaired uid in packages (themes, il2 and gnomefiles)

* Sun Feb 02 2003 Christian W. Zuckschwerdt <zany@triq.net>
1.2.6 - Switched to rpm build in macros.

* Sun Dec 15 2002 Marko Macek <marko.macek@gmx.net>
1.2.3pre2 - Completely rewritten and simplified packaging.

