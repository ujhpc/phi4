BEGIN {hits[0]=1;hits[1]=4;hits[2]=8;hits[3]=16;}
{for(i=0;i<4;i++) printf "%6d %6d %6d %12.8g\n", $2, $1 ,hits[i], $(3+i);}