## main definishing part to include in several Makefiles
## $Id: env_inc.nt.mak,v 1.27 2006-05-05 19:13:40 rssh Exp $

##
# User editable part
##
############################################
## Main
PROJECT_ROOT=C:\home\rssh\work\COS\CosQuery.1
############################################
## install
INSTALL_DIR=C:\local\ndb
INSTALL_IDL_DIR=$(INSTALL_DIR)\idl
INSTALL_INC_DIR=$(INSTALL_DIR)\include
INSTALL_LIB_DIR=$(INSTALL_DIR)\lib
INSTALL_BIN_DIR=$(INSTALL_DIR)\bin

#############################################
## ORB

# Uncomment one of these lines regarding to the ORB you has:

#ORB=Mico

ORB=ORBacus

#ORB=OmniORB

#ORB=TAO

########################
# ORB depended settings:

# TAO: set ACE root folder (optionally):

#ACE_ROOT=D:\ORB\ACE_wrappers

#DEBUG_MARK=d

# Set your ORB root folder:

# TAO (optionally):
TAO_ROOT=$(ACE_ROOT)\TAO

# OmniORB:
OMNI_ROOT=D:\ORB\omni

# ORBacus:
OOC_ROOT=C:\local\ndb\OOC

# Mico
MICO_ROOT=C:\home\rssh\packages\mico
#PTHREADS_ROOT=c:\home\rssh\packages\pthreads
#ORB_USE_PTHREADS
#ORB_USE_DEBUG_LIB=1

###################################
# GradSoft C++ ToolBox

GRAD_TOOLBOX_DIR=C:\local\ndb
GRAD_CORBA_TOOLBOX_LIBDIR=$(GRAD_TOOLBOX_DIR)\lib
                                                 
###################################
# Databases
#                           
#uncomment next line if you have Interbase 
#HAVE_INTERBASE=1

#IBASE_HOME="C:\Program Files\Borland\InterBase" 

#uncomment next line if you have Oracle
HAVE_ORACLE=1

OCI_HOME=c:\oracle\ora92\oci