#
# Part of Grad-Soft Query Service
#


!if "$(IDL_CLN_CPP_SUFFIX)" != "$(IDL_SRV_CPP_SUFFIX)"
CosQuery_idl_SRC= \
    $(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX) \
    $(CosQuery_idl_DIR)\UAKGQuery2$(IDL_SRV_CPP_SUFFIX)
!else
CosQuery_idl_SRC= \
    $(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX) 
!endif



CosQuery_idl_OBJS=$(IDL_SRV_ALL_OBJS)
#
!if 0

!if "$(IDL_CLN_CPP_SUFFIX)" != "$(IDL_SRV_CPP_SUFFIX)"
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX): UAKGQuery2$(IDL_CLN_OBJ_SUFFIX)
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_SRV_CPP_SUFFIX): UAKGQuery2$(IDL_SRV_OBJ_SUFFIX)
!else
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX): UAKGQuery2$(IDL_CLN_OBJ_SUFFIX)
!endif

!else

!if "$(IDL_CLN_CPP_SUFFIX)" != "$(IDL_SRV_CPP_SUFFIX)"
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_OBJ_SUFFIX): UAKGQuery2$(IDL_CLN_CPP_SUFFIX)
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_SRV_OBJ_SUFFIX): UAKGQuery2$(IDL_SRV_CPP_SUFFIX)
!else
$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_OBJ_SUFFIX): UAKGQuery2$(IDL_CLN_CPP_SUFFIX)
!endif

!endif

CosQuery_generate_SRC= \
    $(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX)


$(CosQuery_idl_DIR)\UAKGQuery2$(IDL_CLN_CPP_SUFFIX): UAKGQuery2.idl
