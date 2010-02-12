# TenDRA make build infrastructure
#
# $Id$

.if !defined(_TENDRA_WORK_LIB_MK_)
_TENDRA_WORK_LIB_MK_=1

.include <tendra.base.mk>
.include <tendra.functions.mk>

.if !defined(OBJS)
.BEGIN:
	@${ECHO} '$${OBJS} must be set'
	@${EXIT} 1;
.endif

.if !defined(LIB)
.BEGIN:
	@${ECHO} '$${LIB} must be set'
	@${EXIT} 1;
.endif



${OBJ_SDIR}/lib${LIB}.a: ${OBJ_SDIR} ${OBJS}
	@${ECHO} "==> Archiving ${WRKDIR}/${LIB}"
	${AR} cr ${.TARGET} ${OBJS}
	${RANLIB} ${.TARGET}



#
# User-facing targets
#

all:: ${OBJ_SDIR}/lib${LIB}.a


clean::
	${REMOVE} ${OBJ_SDIR}/lib${LIB}.a


install:: all
	@${ECHO} "==> Installing ${WRKDIR}/lib${LIB}"
	${CONDCREATE} "${COMMON_DIR}/sys"
	${INSTALL} -m 755 ${OBJ_SDIR}/lib${LIB}.a ${COMMON_DIR}/sys/lib${LIB}.a



.endif	# !defined(_TENDRA_WORK_LIB_MK_)