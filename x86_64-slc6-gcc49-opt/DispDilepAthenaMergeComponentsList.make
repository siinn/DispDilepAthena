#-- start of make_header -----------------

#====================================
#  Document DispDilepAthenaMergeComponentsList
#
#   Generated Tue Jul 25 20:59:51 2017  by che.32
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_DispDilepAthenaMergeComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthenaMergeComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthenaMergeComponentsList

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaMergeComponentsList = $(DispDilepAthena_tag)_DispDilepAthenaMergeComponentsList.make
cmt_local_tagfile_DispDilepAthenaMergeComponentsList = $(bin)$(DispDilepAthena_tag)_DispDilepAthenaMergeComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaMergeComponentsList = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthenaMergeComponentsList = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthenaMergeComponentsList)
#-include $(cmt_local_tagfile_DispDilepAthenaMergeComponentsList)

ifdef cmt_DispDilepAthenaMergeComponentsList_has_target_tag

cmt_final_setup_DispDilepAthenaMergeComponentsList = $(bin)setup_DispDilepAthenaMergeComponentsList.make
cmt_dependencies_in_DispDilepAthenaMergeComponentsList = $(bin)dependencies_DispDilepAthenaMergeComponentsList.in
#cmt_final_setup_DispDilepAthenaMergeComponentsList = $(bin)DispDilepAthena_DispDilepAthenaMergeComponentsListsetup.make
cmt_local_DispDilepAthenaMergeComponentsList_makefile = $(bin)DispDilepAthenaMergeComponentsList.make

else

cmt_final_setup_DispDilepAthenaMergeComponentsList = $(bin)setup.make
cmt_dependencies_in_DispDilepAthenaMergeComponentsList = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthenaMergeComponentsList = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthenaMergeComponentsList_makefile = $(bin)DispDilepAthenaMergeComponentsList.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthenaMergeComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthenaMergeComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthenaMergeComponentsList/
#DispDilepAthenaMergeComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/merge_componentslist_header
# Author: Sebastien Binet (binet@cern.ch)

# Makefile fragment to merge a <library>.components file into a single
# <project>.components file in the (lib) install area
# If no InstallArea is present the fragment is dummy


.PHONY: DispDilepAthenaMergeComponentsList DispDilepAthenaMergeComponentsListclean

# default is already '#'
#genmap_comment_char := "'#'"

componentsListRef    := ../$(tag)/DispDilepAthena.components

ifdef CMTINSTALLAREA
componentsListDir    := ${CMTINSTALLAREA}/$(tag)/lib
mergedComponentsList := $(componentsListDir)/$(project).components
stampComponentsList  := $(componentsListRef).stamp
else
componentsListDir    := ../$(tag)
mergedComponentsList :=
stampComponentsList  :=
endif

DispDilepAthenaMergeComponentsList :: $(stampComponentsList) $(mergedComponentsList)
	@:

.NOTPARALLEL : $(stampComponentsList) $(mergedComponentsList)

$(stampComponentsList) $(mergedComponentsList) :: $(componentsListRef)
	@echo "Running merge_componentslist  DispDilepAthenaMergeComponentsList"
	$(merge_componentslist_cmd) --do-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList)

DispDilepAthenaMergeComponentsListclean ::
	$(cleanup_silent) $(merge_componentslist_cmd) --un-merge \
         --input-file $(componentsListRef) --merged-file $(mergedComponentsList) ;
	$(cleanup_silent) $(remove_command) $(stampComponentsList)
libDispDilepAthena_so_dependencies = ../x86_64-slc6-gcc49-opt/libDispDilepAthena.so
#-- start of cleanup_header --------------

clean :: DispDilepAthenaMergeComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthenaMergeComponentsList.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaMergeComponentsListclean ::
#-- end of cleanup_header ---------------
