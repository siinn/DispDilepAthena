#-- start of make_header -----------------

#====================================
#  Document DispDilepAthenaCLIDDB
#
#   Generated Tue Jul 25 20:59:39 2017  by che.32
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_DispDilepAthenaCLIDDB_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthenaCLIDDB_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthenaCLIDDB

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaCLIDDB = $(DispDilepAthena_tag)_DispDilepAthenaCLIDDB.make
cmt_local_tagfile_DispDilepAthenaCLIDDB = $(bin)$(DispDilepAthena_tag)_DispDilepAthenaCLIDDB.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaCLIDDB = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthenaCLIDDB = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthenaCLIDDB)
#-include $(cmt_local_tagfile_DispDilepAthenaCLIDDB)

ifdef cmt_DispDilepAthenaCLIDDB_has_target_tag

cmt_final_setup_DispDilepAthenaCLIDDB = $(bin)setup_DispDilepAthenaCLIDDB.make
cmt_dependencies_in_DispDilepAthenaCLIDDB = $(bin)dependencies_DispDilepAthenaCLIDDB.in
#cmt_final_setup_DispDilepAthenaCLIDDB = $(bin)DispDilepAthena_DispDilepAthenaCLIDDBsetup.make
cmt_local_DispDilepAthenaCLIDDB_makefile = $(bin)DispDilepAthenaCLIDDB.make

else

cmt_final_setup_DispDilepAthenaCLIDDB = $(bin)setup.make
cmt_dependencies_in_DispDilepAthenaCLIDDB = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthenaCLIDDB = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthenaCLIDDB_makefile = $(bin)DispDilepAthenaCLIDDB.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthenaCLIDDB :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthenaCLIDDB'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthenaCLIDDB/
#DispDilepAthenaCLIDDB::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/genCLIDDB_header
# Author: Paolo Calafiura
# derived from genconf_header

# Use genCLIDDB_cmd to create package clid.db files

.PHONY: DispDilepAthenaCLIDDB DispDilepAthenaCLIDDBclean

outname := clid.db
cliddb  := DispDilepAthena_$(outname)
instdir := $(CMTINSTALLAREA)/share
result  := $(instdir)/$(cliddb)
product := $(instdir)/$(outname)
conflib := $(bin)$(library_prefix)DispDilepAthena.$(shlibsuffix)

DispDilepAthenaCLIDDB :: $(result)

$(instdir) :
	$(mkdir) -p $(instdir)

$(result) : $(conflib) $(product)
	@$(genCLIDDB_cmd) -p DispDilepAthena -i$(product) -o $(result)

$(product) : $(instdir)
	touch $(product)

DispDilepAthenaCLIDDBclean ::
	$(cleanup_silent) $(uninstall_command) $(product) $(result)
	$(cleanup_silent) $(cmt_uninstallarea_command) $(product) $(result)

#-- start of cleanup_header --------------

clean :: DispDilepAthenaCLIDDBclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthenaCLIDDB.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaCLIDDBclean ::
#-- end of cleanup_header ---------------
