# Maintainer: Antonio Gutierrez <chibby0ne@gmail.com>
pkgname='client_server-git'
pkgver=0.1.r1.g891237f
pkgrel=1
pkgdesc="Something of an experiment in network programming"
provides=(${pkgname%-*}=${pkgver})
conflicts=(${pkgname%-*})
arch=('any')
url="https://github.com/chibby0ne/client_server"
license=('MIT')
makedepends=('git')
source=(git+https://github.com/chibby0ne/client_server.git)
md5sums=('SKIP')

pkgver() {
    cd client_server
    git describe --long | sed 's/\([^-]*-g\)/r\1/;s/-/./g'
}

build() {
    mkdir -p client_server/build
    cd client_server/build
    cmake .. -GNinja
    ninja
}

package() {
    cd $srcdir/client_server/build
    DESTDIR="$pkgdir/" ninja install
}

