# Created by: Corey Smith <corsmith@gmail.com>
# $FreeBSD: head/net/zerotier/Makefile 407383 2016-01-27 20:27:31Z amdmi3 $

PORTNAME=	zerotier
PORTVERSION=	1.1.4
CATEGORIES=	net

MAINTAINER=	corsmith@gmail.com
COMMENT=	Network virtualization everywhere

LICENSE=	GPLv3
LICENSE_FILE=	${WRKSRC}/LICENSE.txt

USES=		gmake
USE_RC_SUBR=	zerotier

USE_GITHUB=	yes
GH_ACCOUNT=	zerotier
GH_PROJECT=	ZeroTierOne
CFLAGS+=	-flax-vector-conversions

OPTIONS_DEFINE=	CONTROLLER
CONTROLLER_DESC= Enable controller microservice to create and manage networks
CONTROLLER_MAKE_ARGS= ZT_ENABLE_NETWORK_CONTROLLER=1
CONTROLLER_BUILD_DEPENDS= sqlite3:databases/sqlite3

PLIST_FILES=	bin/zerotier-cli bin/zerotier-idtool sbin/zerotier-one

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/zerotier-one ${STAGEDIR}${PREFIX}/sbin/
.for l in zerotier-cli zerotier-idtool
	${LN} -sf ../sbin/zerotier-one ${STAGEDIR}${PREFIX}/bin/${l}
.endfor

.include <bsd.port.mk>
