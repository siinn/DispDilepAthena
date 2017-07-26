#-- start of make_header -----------------

#====================================
#  Document DispDilepAthenaConf
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

cmt_DispDilepAthenaConf_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthenaConf_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthenaConf

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaConf = $(DispDilepAthena_tag)_DispDilepAthenaConf.make
cmt_local_tagfile_DispDilepAthenaConf = $(bin)$(DispDilepAthena_tag)_DispDilepAthenaConf.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthenaConf = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthenaConf = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthenaConf)
#-include $(cmt_local_tagfile_DispDilepAthenaConf)

ifdef cmt_DispDilepAthenaConf_has_target_tag

cmt_final_setup_DispDilepAthenaConf = $(bin)setup_DispDilepAthenaConf.make
cmt_dependencies_in_DispDilepAthenaConf = $(bin)dependencies_DispDilepAthenaConf.in
#cmt_final_setup_DispDilepAthenaConf = $(bin)DispDilepAthena_DispDilepAthenaConfsetup.make
cmt_local_DispDilepAthenaConf_makefile = $(bin)DispDilepAthenaConf.make

else

cmt_final_setup_DispDilepAthenaConf = $(bin)setup.make
cmt_dependencies_in_DispDilepAthenaConf = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthenaConf = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthenaConf_makefile = $(bin)DispDilepAthenaConf.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthenaConf :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthenaConf'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthenaConf/
#DispDilepAthenaConf::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
# File: cmt/fragments/genconfig_header
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

# Use genconf.exe to create configurables python modules, then have the
# normal python install procedure take over.

.PHONY: DispDilepAthenaConf DispDilepAthenaConfclean

confpy  := DispDilepAthenaConf.py
conflib := $(bin)$(library_prefix)DispDilepAthena.$(shlibsuffix)
confdb  := DispDilepAthena.confdb
instdir := $(CMTINSTALLAREA)$(shared_install_subdir)/python/$(package)
product := $(instdir)/$(confpy)
initpy  := $(instdir)/__init__.py

ifdef GENCONF_ECHO
genconf_silent =
else
genconf_silent = $(silent)
endif

DispDilepAthenaConf :: DispDilepAthenaConfinstall

install :: DispDilepAthenaConfinstall

DispDilepAthenaConfinstall : /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena/$(confpy)
	@echo "Installing /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena in /n/atlas05/userdata/sche/testalg/InstallArea/python" ; \
	 $(install_command) --exclude="*.py?" --exclude="__init__.py" --exclude="*.confdb" /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena /n/atlas05/userdata/sche/testalg/InstallArea/python ; \

/n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena/$(confpy) : $(conflib) /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena
	$(genconf_silent) $(genconfig_cmd)   -o /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena -p $(package) \
	  --configurable-module=GaudiKernel.Proxy \
	  --configurable-default-name=Configurable.DefaultName \
	  --configurable-algorithm=ConfigurableAlgorithm \
	  --configurable-algtool=ConfigurableAlgTool \
	  --configurable-auditor=ConfigurableAuditor \
          --configurable-service=ConfigurableService \
	  -i ../$(tag)/$(library_prefix)DispDilepAthena.$(shlibsuffix)

/n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena:
	@ if [ ! -d /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena ] ; then mkdir -p /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena ; fi ;

DispDilepAthenaConfclean :: DispDilepAthenaConfuninstall
	$(cleanup_silent) $(remove_command) /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena/$(confpy) /n/atlas05/userdata/sche/testalg/DispDilepAthena/genConf/DispDilepAthena/$(confdb)

uninstall :: DispDilepAthenaConfuninstall

DispDilepAthenaConfuninstall ::
	@$(uninstall_command) /n/atlas05/userdata/sche/testalg/InstallArea/python
libDispDilepAthena_so_dependencies = ../x86_64-slc6-gcc49-opt/libDispDilepAthena.so
#-- start of cleanup_header --------------

clean :: DispDilepAthenaConfclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthenaConf.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaConfclean ::
#-- end of cleanup_header ---------------
