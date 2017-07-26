# echo "setup DispDilepAthena DispDilepAthena-00-00-00 in /n/atlas05/userdata/sche/testalg"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.9/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtDispDilepAthenatempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtDispDilepAthenatempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=DispDilepAthena -version=DispDilepAthena-00-00-00 -path=/n/atlas05/userdata/sche/testalg  -no_cleanup $* >${cmtDispDilepAthenatempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=DispDilepAthena -version=DispDilepAthena-00-00-00 -path=/n/atlas05/userdata/sche/testalg  -no_cleanup $* >${cmtDispDilepAthenatempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtDispDilepAthenatempfile}
  unset cmtDispDilepAthenatempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtDispDilepAthenatempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtDispDilepAthenatempfile}
unset cmtDispDilepAthenatempfile
exit $cmtsetupstatus

