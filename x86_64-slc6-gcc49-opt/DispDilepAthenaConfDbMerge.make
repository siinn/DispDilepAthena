#-- start of make_header -----------------

#====================================
#  Document DispDilepAthenaConfDbMerge
#
#   Generated Tue Jul 25 20:59:47 2017  by che.32
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_DispDilepAthenaConfDbMerge_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthenaConfDbMerge_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthenaConfDbMerge

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaConfDbMerge = $(DispDilepAthena_tag)_DispDilepAthenaConfDbMerge.make
cmt_local_tagfile_DispDilepAthenaConfDbMerge = $(bin)$(DispDilepAthena_tag)_DispDilepAthenaConfDbMerge.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaConfDbMerge = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthenaConfDbMerge = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthenaConfDbMerge)
#-include $(cmt_local_tagfile_DispDilepAthenaConfDbMerge)

ifdef cmt_DispDilepAthenaConfDbMerge_has_target_tag

cmt_final_setup_DispDilepAthenaConfDbMerge = $(bin)setup_DispDilepAthenaConfDbMerge.make
cmt_dependencies_in_DispDilepAthenaConfDbMerge = $(bin)dependencies_DispDilepAthenaConfDbMerge.in
#cmt_final_setup_DispDilepAthenaConfDbMerge = $(bin)DispDilepAthena_DispDilepAthenaConfDbMergesetup.make
cmt_local_DispDilepAthenaConfDbMerge_makefile = $(bin)DispDilepAthenaConfDbMerge.make

else

cmt_final_setup_DispDilepAthenaConfDbMerge = $(bin)setup.make
cmt_dependencies_in_DispDilepAthenaConfDbMerge = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthenaConfDbMerge = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthenaConfDbMerge_makefile = $(bin)DispDilepAthenaConfDbMerge.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthenaConfDbMerge :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthenaConfDbMerge'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthenaConfDbMerge/
#DispDilepAthenaConfDbMerge::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_genconfDb_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.confdb file into a single
# <project>.confdb file in the (lib) install area

.PHONY: DispDilepAthenaConfDbMerge DispDilepAthenaConfDbMergeclean

# default is already '#'
#genconfDb_comment_char := "'#'"

instdir      := ${CMTINSTALLAREA}/$(tag)
confDbRef    := /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena/DispDilepAthena.confdb
stampConfDb  := $(confDbRef).stamp
mergedConfDb := $(instdir)/lib/$(project).confdb

DispDilepAthenaConfDbMerge :: $(stampConfDb) $(mergedConfDb)
	@:

.NOTPARALLEL : $(stampConfDb) $(mergedConfDb)

$(stampConfDb) $(mergedConfDb) :: $(confDbRef)
	@echo "Running merge_genconfDb  DispDilepAthenaConfDbMerge"
	$(merge_genconfDb_cmd) \
          --do-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)

DispDilepAthenaConfDbMergeclean ::
	$(cleanup_silent) $(merge_genconfDb_cmd) \
          --un-merge \
          --input-file $(confDbRef) \
          --merged-file $(mergedConfDb)	;
	$(cleanup_silent) $(remove_command) $(stampConfDb)
libDispDilepAthena_so_dependencies = ../x86_64-slc6-gcc49-opt/libDispDilepAthena.so
#-- start of cleanup_header --------------

clean :: DispDilepAthenaConfDbMergeclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthenaConfDbMerge.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaConfDbMergeclean ::
#-- end of cleanup_header ---------------
