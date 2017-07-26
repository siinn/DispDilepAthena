# echo "setup DispDilepAthena DispDilepAthena-00-00-00 in /n/atlas05/userdata/sche/testalg"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.9/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtDispDilepAthenatempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtDispDilepAthenatempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=DispDilepAthena -version=DispDilepAthena-00-00-00 -path=/n/atlas05/userdata/sche/testalg  -no_cleanup $* >${cmtDispDilepAthenatempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=DispDilepAthena -version=DispDilepAthena-00-00-00 -path=/n/atlas05/userdata/sche/testalg  -no_cleanup $* >${cmtDispDilepAthenatempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtDispDilepAthenatempfile}
  unset cmtDispDilepAthenatempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtDispDilepAthenatempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtDispDilepAthenatempfile}
unset cmtDispDilepAthenatempfile
return $cmtsetupstatus

