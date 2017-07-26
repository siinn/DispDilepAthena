#-- start of make_header -----------------

#====================================
#  Document DispDilepAthenaComponentsList
#
#   Generated Tue Jul 25 20:59:46 2017  by che.32
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_DispDilepAthenaComponentsList_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthenaComponentsList_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthenaComponentsList

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaComponentsList = $(DispDilepAthena_tag)_DispDilepAthenaComponentsList.make
cmt_local_tagfile_DispDilepAthenaComponentsList = $(bin)$(DispDilepAthena_tag)_DispDilepAthenaComponentsList.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaComponentsList = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthenaComponentsList = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthenaComponentsList)
#-include $(cmt_local_tagfile_DispDilepAthenaComponentsList)

ifdef cmt_DispDilepAthenaComponentsList_has_target_tag

cmt_final_setup_DispDilepAthenaComponentsList = $(bin)setup_DispDilepAthenaComponentsList.make
cmt_dependencies_in_DispDilepAthenaComponentsList = $(bin)dependencies_DispDilepAthenaComponentsList.in
#cmt_final_setup_DispDilepAthenaComponentsList = $(bin)DispDilepAthena_DispDilepAthenaComponentsListsetup.make
cmt_local_DispDilepAthenaComponentsList_makefile = $(bin)DispDilepAthenaComponentsList.make

else

cmt_final_setup_DispDilepAthenaComponentsList = $(bin)setup.make
cmt_dependencies_in_DispDilepAthenaComponentsList = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthenaComponentsList = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthenaComponentsList_makefile = $(bin)DispDilepAthenaComponentsList.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthenaComponentsList :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthenaComponentsList'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthenaComponentsList/
#DispDilepAthenaComponentsList::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
##
componentslistfile = DispDilepAthena.components
COMPONENTSLIST_DIR = ../$(tag)
fulllibname = libDispDilepAthena.$(shlibsuffix)

DispDilepAthenaComponentsList :: ${COMPONENTSLIST_DIR}/$(componentslistfile)
	@:

${COMPONENTSLIST_DIR}/$(componentslistfile) :: $(bin)$(fulllibname)
	@echo 'Generating componentslist file for $(fulllibname)'
	cd ../$(tag);$(listcomponents_cmd) --output ${COMPONENTSLIST_DIR}/$(componentslistfile) $(fulllibname)

install :: DispDilepAthenaComponentsListinstall
DispDilepAthenaComponentsListinstall :: DispDilepAthenaComponentsList

uninstall :: DispDilepAthenaComponentsListuninstall
DispDilepAthenaComponentsListuninstall :: DispDilepAthenaComponentsListclean

DispDilepAthenaComponentsListclean ::
	@echo 'Deleting $(componentslistfile)'
	@rm -f ${COMPONENTSLIST_DIR}/$(componentslistfile)

#-- start of cleanup_header --------------

clean :: DispDilepAthenaComponentsListclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthenaComponentsList.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaComponentsListclean ::
#-- end of cleanup_header ---------------
