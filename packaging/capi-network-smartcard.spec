Name:       capi-network-smartcard
Summary:    A Smartcard library in Native API
Version:    0.0.4
Release:    1
Group:      Network & Connectivity/Smartcard
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gobject-2.0)
BuildRequires:  pkgconfig(smartcard-service)
BuildRequires:  pkgconfig(smartcard-service-common)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-system-info)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description

%package devel
Summary:  A Smartcard library in Native API (Development)
Group:    Network & Connectivity/Development
Requires: %{name} = %{version}-%{release}

%description devel

%prep
%setup -q

%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`

cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} \
        -DMAJORVER=${MAJORVER} -DCMAKE_LIB_DIR=%{_libdir} \
	-DTIZEN_SMARTCARD_SUPPORT=1

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license/smartcard/
cp -af %{_builddir}/%{name}-%{version}/LICENSE.APLv2 %{buildroot}/usr/share/license/smartcard/

%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest capi-network-smartcard.manifest
%{_libdir}/libcapi-network-smartcard.so*
/usr/share/license/smartcard/LICENSE.APLv2

%files devel
%{_includedir}/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-network-smartcard.so
