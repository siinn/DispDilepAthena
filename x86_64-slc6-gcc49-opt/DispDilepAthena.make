#-- start of make_header -----------------

#====================================
#  Library DispDilepAthena
#
#   Generated Tue Jul 25 20:56:47 2017  by che.32
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_DispDilepAthena_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_DispDilepAthena_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_DispDilepAthena

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthena = $(DispDilepAthena_tag)_DispDilepAthena.make
cmt_local_tagfile_DispDilepAthena = $(bin)$(DispDilepAthena_tag)_DispDilepAthena.make

else

tags      = $(tag),$(CMTEXTRATAGS)

DispDilepAthena_tag = $(tag)

#cmt_local_tagfile_DispDilepAthena = $(DispDilepAthena_tag).make
cmt_local_tagfile_DispDilepAthena = $(bin)$(DispDilepAthena_tag).make

endif

include $(cmt_local_tagfile_DispDilepAthena)
#-include $(cmt_local_tagfile_DispDilepAthena)

ifdef cmt_DispDilepAthena_has_target_tag

cmt_final_setup_DispDilepAthena = $(bin)setup_DispDilepAthena.make
cmt_dependencies_in_DispDilepAthena = $(bin)dependencies_DispDilepAthena.in
#cmt_final_setup_DispDilepAthena = $(bin)DispDilepAthena_DispDilepAthenasetup.make
cmt_local_DispDilepAthena_makefile = $(bin)DispDilepAthena.make

else

cmt_final_setup_DispDilepAthena = $(bin)setup.make
cmt_dependencies_in_DispDilepAthena = $(bin)dependencies.in
#cmt_final_setup_DispDilepAthena = $(bin)DispDilepAthenasetup.make
cmt_local_DispDilepAthena_makefile = $(bin)DispDilepAthena.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)DispDilepAthenasetup.make

#DispDilepAthena :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'DispDilepAthena'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = DispDilepAthena/
#DispDilepAthena::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

DispDilepAthenalibname   = $(bin)$(library_prefix)DispDilepAthena$(library_suffix)
DispDilepAthenalib       = $(DispDilepAthenalibname).a
DispDilepAthenastamp     = $(bin)DispDilepAthena.stamp
DispDilepAthenashstamp   = $(bin)DispDilepAthena.shstamp

DispDilepAthena :: dirs  DispDilepAthenaLIB
	$(echo) "DispDilepAthena ok"

#-- end of libary_header ----------------
#-- start of library_no_static ------

#DispDilepAthenaLIB :: $(DispDilepAthenalib) $(DispDilepAthenashstamp)
DispDilepAthenaLIB :: $(DispDilepAthenashstamp)
	$(echo) "DispDilepAthena : library ok"

$(DispDilepAthenalib) :: $(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o
	$(lib_echo) "static library $@"
	$(lib_silent) cd $(bin); \
	  $(ar) $(DispDilepAthenalib) $?
	$(lib_silent) $(ranlib) $(DispDilepAthenalib)
	$(lib_silent) cat /dev/null >$(DispDilepAthenastamp)

#------------------------------------------------------------------
#  Future improvement? to empty the object files after
#  storing in the library
#
##	  for f in $?; do \
##	    rm $${f}; touch $${f}; \
##	  done
#------------------------------------------------------------------

#
# We add one level of dependency upon the true shared library 
# (rather than simply upon the stamp file)
# this is for cases where the shared library has not been built
# while the stamp was created (error??) 
#

$(DispDilepAthenalibname).$(shlibsuffix) :: $(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o $(use_requirements) $(DispDilepAthenastamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) $(shlibbuilder) $(shlibflags) -o $@ $(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o $(DispDilepAthena_shlibflags)
	$(lib_silent) cat /dev/null >$(DispDilepAthenastamp) && \
	  cat /dev/null >$(DispDilepAthenashstamp)

$(DispDilepAthenashstamp) :: $(DispDilepAthenalibname).$(shlibsuffix)
	$(lib_silent) if test -f $(DispDilepAthenalibname).$(shlibsuffix) ; then \
	  cat /dev/null >$(DispDilepAthenastamp) && \
	  cat /dev/null >$(DispDilepAthenashstamp) ; fi

DispDilepAthenaclean ::
	$(cleanup_echo) objects DispDilepAthena
	$(cleanup_silent) /bin/rm -f $(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o) $(patsubst %.o,%.dep,$(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o) $(patsubst %.o,%.d.stamp,$(bin)DispDilepAthenaAlg.o $(bin)CosmicTools.o $(bin)DVUtils.o $(bin)DisplacedDimuonAnalysisAlg.o $(bin)FlipBkgEst.o $(bin)LeptonSelectionTools.o $(bin)SMBkgEst.o $(bin)DispDilepAthena_load.o $(bin)DispDilepAthena_entries.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf DispDilepAthena_deps DispDilepAthena_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
DispDilepAthenainstallname = $(library_prefix)DispDilepAthena$(library_suffix).$(shlibsuffix)

DispDilepAthena :: DispDilepAthenainstall ;

install :: DispDilepAthenainstall ;

DispDilepAthenainstall :: $(install_dir)/$(DispDilepAthenainstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(DispDilepAthenainstallname) :: $(bin)$(DispDilepAthenainstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(DispDilepAthenainstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##DispDilepAthenaclean :: DispDilepAthenauninstall

uninstall :: DispDilepAthenauninstall ;

DispDilepAthenauninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(DispDilepAthenainstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of library_no_static ------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DispDilepAthenaAlg.d

$(bin)$(binobj)DispDilepAthenaAlg.d :

$(bin)$(binobj)DispDilepAthenaAlg.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)DispDilepAthenaAlg.o : $(src)DispDilepAthenaAlg.cxx
	$(cpp_echo) $(src)DispDilepAthenaAlg.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthenaAlg_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthenaAlg_cppflags) $(DispDilepAthenaAlg_cxx_cppflags)  $(src)DispDilepAthenaAlg.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(DispDilepAthenaAlg_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)DispDilepAthenaAlg.cxx

$(bin)$(binobj)DispDilepAthenaAlg.o : $(DispDilepAthenaAlg_cxx_dependencies)
	$(cpp_echo) $(src)DispDilepAthenaAlg.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthenaAlg_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthenaAlg_cppflags) $(DispDilepAthenaAlg_cxx_cppflags)  $(src)DispDilepAthenaAlg.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)CosmicTools.d

$(bin)$(binobj)CosmicTools.d :

$(bin)$(binobj)CosmicTools.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)CosmicTools.o : $(src)CosmicTools.cxx
	$(cpp_echo) $(src)CosmicTools.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(CosmicTools_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(CosmicTools_cppflags) $(CosmicTools_cxx_cppflags)  $(src)CosmicTools.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(CosmicTools_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)CosmicTools.cxx

$(bin)$(binobj)CosmicTools.o : $(CosmicTools_cxx_dependencies)
	$(cpp_echo) $(src)CosmicTools.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(CosmicTools_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(CosmicTools_cppflags) $(CosmicTools_cxx_cppflags)  $(src)CosmicTools.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DVUtils.d

$(bin)$(binobj)DVUtils.d :

$(bin)$(binobj)DVUtils.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)DVUtils.o : $(src)DVUtils.cxx
	$(cpp_echo) $(src)DVUtils.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DVUtils_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DVUtils_cppflags) $(DVUtils_cxx_cppflags)  $(src)DVUtils.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(DVUtils_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)DVUtils.cxx

$(bin)$(binobj)DVUtils.o : $(DVUtils_cxx_dependencies)
	$(cpp_echo) $(src)DVUtils.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DVUtils_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DVUtils_cppflags) $(DVUtils_cxx_cppflags)  $(src)DVUtils.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DisplacedDimuonAnalysisAlg.d

$(bin)$(binobj)DisplacedDimuonAnalysisAlg.d :

$(bin)$(binobj)DisplacedDimuonAnalysisAlg.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)DisplacedDimuonAnalysisAlg.o : $(src)DisplacedDimuonAnalysisAlg.cxx
	$(cpp_echo) $(src)DisplacedDimuonAnalysisAlg.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DisplacedDimuonAnalysisAlg_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DisplacedDimuonAnalysisAlg_cppflags) $(DisplacedDimuonAnalysisAlg_cxx_cppflags)  $(src)DisplacedDimuonAnalysisAlg.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(DisplacedDimuonAnalysisAlg_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)DisplacedDimuonAnalysisAlg.cxx

$(bin)$(binobj)DisplacedDimuonAnalysisAlg.o : $(DisplacedDimuonAnalysisAlg_cxx_dependencies)
	$(cpp_echo) $(src)DisplacedDimuonAnalysisAlg.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DisplacedDimuonAnalysisAlg_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DisplacedDimuonAnalysisAlg_cppflags) $(DisplacedDimuonAnalysisAlg_cxx_cppflags)  $(src)DisplacedDimuonAnalysisAlg.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)FlipBkgEst.d

$(bin)$(binobj)FlipBkgEst.d :

$(bin)$(binobj)FlipBkgEst.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)FlipBkgEst.o : $(src)FlipBkgEst.cxx
	$(cpp_echo) $(src)FlipBkgEst.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(FlipBkgEst_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(FlipBkgEst_cppflags) $(FlipBkgEst_cxx_cppflags)  $(src)FlipBkgEst.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(FlipBkgEst_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)FlipBkgEst.cxx

$(bin)$(binobj)FlipBkgEst.o : $(FlipBkgEst_cxx_dependencies)
	$(cpp_echo) $(src)FlipBkgEst.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(FlipBkgEst_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(FlipBkgEst_cppflags) $(FlipBkgEst_cxx_cppflags)  $(src)FlipBkgEst.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)LeptonSelectionTools.d

$(bin)$(binobj)LeptonSelectionTools.d :

$(bin)$(binobj)LeptonSelectionTools.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)LeptonSelectionTools.o : $(src)LeptonSelectionTools.cxx
	$(cpp_echo) $(src)LeptonSelectionTools.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(LeptonSelectionTools_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(LeptonSelectionTools_cppflags) $(LeptonSelectionTools_cxx_cppflags)  $(src)LeptonSelectionTools.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(LeptonSelectionTools_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)LeptonSelectionTools.cxx

$(bin)$(binobj)LeptonSelectionTools.o : $(LeptonSelectionTools_cxx_dependencies)
	$(cpp_echo) $(src)LeptonSelectionTools.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(LeptonSelectionTools_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(LeptonSelectionTools_cppflags) $(LeptonSelectionTools_cxx_cppflags)  $(src)LeptonSelectionTools.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)SMBkgEst.d

$(bin)$(binobj)SMBkgEst.d :

$(bin)$(binobj)SMBkgEst.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)SMBkgEst.o : $(src)SMBkgEst.cxx
	$(cpp_echo) $(src)SMBkgEst.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(SMBkgEst_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(SMBkgEst_cppflags) $(SMBkgEst_cxx_cppflags)  $(src)SMBkgEst.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(SMBkgEst_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)SMBkgEst.cxx

$(bin)$(binobj)SMBkgEst.o : $(SMBkgEst_cxx_dependencies)
	$(cpp_echo) $(src)SMBkgEst.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(SMBkgEst_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(SMBkgEst_cppflags) $(SMBkgEst_cxx_cppflags)  $(src)SMBkgEst.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DispDilepAthena_load.d

$(bin)$(binobj)DispDilepAthena_load.d :

$(bin)$(binobj)DispDilepAthena_load.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)DispDilepAthena_load.o : $(src)components/DispDilepAthena_load.cxx
	$(cpp_echo) $(src)components/DispDilepAthena_load.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthena_load_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthena_load_cppflags) $(DispDilepAthena_load_cxx_cppflags) -I../src/components $(src)components/DispDilepAthena_load.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(DispDilepAthena_load_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)components/DispDilepAthena_load.cxx

$(bin)$(binobj)DispDilepAthena_load.o : $(DispDilepAthena_load_cxx_dependencies)
	$(cpp_echo) $(src)components/DispDilepAthena_load.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthena_load_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthena_load_cppflags) $(DispDilepAthena_load_cxx_cppflags) -I../src/components $(src)components/DispDilepAthena_load.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (-MMD -MP -MF $*.d -MQ $@,)

ifneq ($(MAKECMDGOALS),DispDilepAthenaclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)DispDilepAthena_entries.d

$(bin)$(binobj)DispDilepAthena_entries.d :

$(bin)$(binobj)DispDilepAthena_entries.o : $(cmt_final_setup_DispDilepAthena)

$(bin)$(binobj)DispDilepAthena_entries.o : $(src)components/DispDilepAthena_entries.cxx
	$(cpp_echo) $(src)components/DispDilepAthena_entries.cxx
	$(cpp_silent) $(cppcomp) -MMD -MP -MF $*.d -MQ $@ -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthena_entries_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthena_entries_cppflags) $(DispDilepAthena_entries_cxx_cppflags) -I../src/components $(src)components/DispDilepAthena_entries.cxx
endif
endif

else
$(bin)DispDilepAthena_dependencies.make : $(DispDilepAthena_entries_cxx_dependencies)

$(bin)DispDilepAthena_dependencies.make : $(src)components/DispDilepAthena_entries.cxx

$(bin)$(binobj)DispDilepAthena_entries.o : $(DispDilepAthena_entries_cxx_dependencies)
	$(cpp_echo) $(src)components/DispDilepAthena_entries.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(DispDilepAthena_pp_cppflags) $(lib_DispDilepAthena_pp_cppflags) $(DispDilepAthena_entries_pp_cppflags) $(use_cppflags) $(DispDilepAthena_cppflags) $(lib_DispDilepAthena_cppflags) $(DispDilepAthena_entries_cppflags) $(DispDilepAthena_entries_cxx_cppflags) -I../src/components $(src)components/DispDilepAthena_entries.cxx

endif

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: DispDilepAthenaclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(DispDilepAthena.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

DispDilepAthenaclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library DispDilepAthena
	-$(cleanup_silent) cd $(bin) && \rm -f $(library_prefix)DispDilepAthena$(library_suffix).a $(library_prefix)DispDilepAthena$(library_suffix).$(shlibsuffix) DispDilepAthena.stamp DispDilepAthena.shstamp
#-- end of cleanup_library ---------------
