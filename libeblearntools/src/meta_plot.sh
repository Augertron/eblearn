EMAIL=pierre.sermanet@gmail.com
TGTDIR=$1/
MPLOTSH=`pwd`/$0
MPLOT=`pwd`/`dirname $0`/meta_plot
BASENAME=`basename $TGTDIR`
SUBJECT="Plots_${BASENAME}"
################################################################################
echo "Recursively analyzing *.log files in ${TGTDIR}..."
DATE=`/bin/date '+%Y.%m.%d.%H.%M.%S'`
DIRNAME="plots.$DATE"
rm -Rf $DIRNAME && mkdir $DIRNAME > /dev/null && cd $DIRNAME
################################################################################
# copy metaconf and log files into tarball
METACONF=${TGTDIR}/metaconf
LOGS=`find ${TGTDIR} -name out*.log`
LOGS=`echo $LOGS | sed ':a;N;$!ba;s/\n/ /g'`
ALOGS=`echo $LOGS | sed 's/ / -a /g'`
ALOGS="-a $ALOGS"
touch $METACONF
cp $METACONF .
cp $LOGS .
################################################################################
$MPLOT $TGTDIR
if [ $? -eq 0 ]
then
    plots=`ls *.p`
    all=""
    allpdf=""
    echo "________________________________________________________________________________"
    echo "Plotting *.p"
    for p in $plots
      do
      echo "plotting $p..."
      cat $p | gnuplot
      sleep .3
      PDFNAME=${DIRNAME}_${p}df
      mv ${p}df $PDFNAME
      allpdf="$allpdf -a ${PDFNAME}"
    done
    echo "________________________________________________________________________________"
    echo "Taring plot data:"
    cd .. && tar czvf $DIRNAME.tgz $DIRNAME && cd $DIRNAME
    allpdf="$allpdf -a ../$DIRNAME.tgz"
    echo "________________________________________________________________________________"
    echo "Mailing pdf plots and tgz to ${EMAIL}:"
    echo "cat $METACONF | mutt $EMAIL -s ${SUBJECT} $allpdf -a $METACONF $ALOGS"
    cat $METACONF | mutt $EMAIL -s $SUBJECT $allpdf -a $METACONF $ALOGS
fi
cd .. && rm -Rf $DIRNAME
