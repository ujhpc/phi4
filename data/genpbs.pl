#!/usr/bin/perl -w

use Getopt::Long qw(GetOptionsFromArray);
my $EXEC_DIR="$ENV{'HOME'}/PROJECTS/phi4/bin";


$n_components=2;
$DIM=3;
$OMP=0;

$seed=32165;
$n_term=5000;
$n_sweeps=100000;
$tag="tst";
$size=128;
$float='float';
if($DIM>2) {
    $size=32;
}

$queue='long';


my %options=(
    "dim=i"=> \$DIM,
    "comp=i"=>\$n_components,
    "n-term=i"  => \$n_term,
    "n-sweeps=i" => \$n_sweeps,
    "size=i" =>\$size,
    "tag=s"=>\$tag,
    "omp=i"=>\$OMP,
    "float=s"=>\$float,
    "seed=i"=>\$seed);

GetOptions(
    %options
    );
print STDERR "$OMP $tag\n";

$queue='mp4'  if($OMP > 0);
$queue='mp8'  if($OMP > 7);
$queue='mp16'  if($OMP > 15);
$queue='mp24' if($OMP > 23);

my $omp_tag;
if($OMP>0) {
    $omp_tag='omp';
} else {
   $omp_tag='no-omp';
}

my $exec_name="phi4-${n_components}nc-${DIM}d-${float}-${omp_tag}";

my $executable="$EXEC_DIR/$exec_name";

print STDERR "$executable\n";

my $size_string="--n-x $size --n-y $size";
$size_string.=" --n-z $size" if($DIM>2);

my $pbs_file="pbs.$tag";
open(PBS,">$pbs_file") or die "cannot open file `$pbs_file' for writing\n";

print PBS << "PBS";
#!/bin/sh
#PBS -N $tag
#PBS -q $queue
#PBS -l nodes=1:ppn=$OMP
#PBS -l cput=300:00:00
#PBS -l walltime=300:00:00
export OMP_NUM_THREADS=$OMP
cd \${PBS_O_WORKDIR}
$executable $size_string  -t $n_term -n $n_sweeps --tag $tag  --seed $seed
PBS
;

close(PBS);

(-e $executable) or die " exec `$executable' does not exists\n";
